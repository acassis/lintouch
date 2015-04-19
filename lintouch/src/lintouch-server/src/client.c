/* $Id: client.c 1774 2006-05-31 13:17:16Z hynek $
 *
 *   FILE: client.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 14 April 2005
 *
 * Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
 * Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this application; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <apr_strings.h>
#include <apr_thread_proc.h>

#include "lt/cp/cp-server.h"
#include "lt/server/client.h"
#include "lt/server/server.h"
#include "lt/server/logger.h"

struct lt_server_client_t
{
    apr_pool_t * pool;
    apr_socket_t * socket;
    apr_thread_t * thread;

    char stoprequested;

    lt_server_t * server;
    char * project_url;

    lt_cp_session_t * session;
};

/**
 * Thread routine
 *
 * @param thread Thread
 * @param userarg User argument
 */
void * APR_THREAD_FUNC _lt_server_client_run( apr_thread_t * thread,
        void * userarg );

/* METHOD BODIES */

apr_pool_t * lt_server_client_pool_get( const lt_server_client_t * src )
{
    if( src == NULL ) return NULL;
    return src->pool;
}

#define CREATE_POOL_IF_NULL(p)  if((p)==NULL)apr_pool_create(&(p),NULL)

lt_server_status_t lt_server_client_create ( lt_server_client_t ** client,
        apr_socket_t * socket, lt_server_t * server, apr_pool_t * pool )
{
    if( socket == NULL ) return LT_SERVER_INVALID_ARG;
    if( server == NULL ) return LT_SERVER_INVALID_ARG;

    CREATE_POOL_IF_NULL( pool );

    *client = ( lt_server_client_t * ) apr_pcalloc( pool,
            sizeof( lt_server_client_t ) );

    if( *client == NULL ) return LT_SERVER_INVALID_ARG;

    ( *client )->pool = pool;
    ( *client )->socket = socket;
    ( *client )->stoprequested = 0;

    ( *client )->server = server;

    /* determine to which of our addresses the client is connected and
     * reformulate project_url accordingly */
    {
        apr_sockaddr_t * sa = NULL;
        char * ip = NULL;
        if( APR_SUCCESS != apr_socket_addr_get( &sa,
                        APR_LOCAL, socket ) ) {
            lt_server_logger_print_socket( PRIO_DEFAULT, "apr_socket_addr_get",
                    "Unable to determine local address "
                    "of the client socket\n" );
            return LT_SERVER_INVALID_ARG;
        }
        if( APR_SUCCESS != apr_sockaddr_ip_get( &ip, sa ) ) {
            lt_server_logger_print_socket( PRIO_DEFAULT, "apr_sockaddr_ip_get",
                    "Unable to determine local IP address "
                    "of the client socket\n" );
            return LT_SERVER_INVALID_ARG;
        }

        ( *client )->project_url = (char*)apr_pcalloc( pool, 1024 );
        apr_snprintf( ( *client )->project_url, 1023,
                lt_server_project_url_get( server ), ip);
    }

    /* make our socket non-blocking, wait 10ms for incoming data */
    if( APR_SUCCESS != apr_socket_timeout_set(
                    ( *client )->socket, 10 * 1000 ) ) {
        lt_server_logger_print_socket( PRIO_DEFAULT, "apr_socket_timeout_set",
                "Unable to set timeout for client socket\n" );
        return LT_SERVER_INVALID_ARG;
    }

    /* create communication protocol session */
    lt_cp_session_create( &(( *client )->session ), LT_CP_SERVER, pool );
    lt_cp_session_project_url_set( ( *client )->session,
            ( *client )->project_url );

    /* register variables from all plugins */
    {
        apr_hash_t * plugins = lt_server_plugins_get( server );
        apr_hash_index_t * hi = apr_hash_first( pool, plugins );

        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {

            const char * key = NULL;
            lt_server_plugin_t * plugin = NULL;
            apr_hash_this( hi, (const void**)&key, NULL, (void**)&plugin );

            lt_cp_session_variables_register( ( *client )->session,
                    key, lt_server_plugin_varset_get( plugin ) );


        }
    }

    return LT_SERVER_SUCCESS;
}

lt_server_status_t lt_server_client_start(
        lt_server_client_t * client )
{
    if( client == NULL ) return LT_SERVER_INVALID_ARG;

    /* prepare & start client handling thread */
    if( APR_SUCCESS != apr_thread_create(
                    &(client->thread), NULL,
                    (apr_thread_start_t)_lt_server_client_run, client,
                     client->pool ) ) {
        lt_server_logger_print_apr( PRIO_DEFAULT, "apr_thread_create",
                "Unable to create thread to handle client communication\n" );
        return LT_SERVER_INVALID_ARG;
    }

    return LT_SERVER_SUCCESS;
}

lt_server_status_t lt_server_client_stop(
        lt_server_client_t * client )
{
    if( client == NULL ) return LT_SERVER_INVALID_ARG;

    /* bool assignment should be atomic operation so no locking is needed */
    client->stoprequested = 1;

    /* wait for server thread to finish. */
    {
        apr_status_t rv;

        if( APR_SUCCESS != apr_thread_join(
                        &rv, client->thread ) ) {
            lt_server_logger_print_apr( PRIO_DEFAULT, "apr_thread_join",
                    "Unable to wait for client thread to terminate\n" );
        }
    }

    return LT_SERVER_SUCCESS;
}

/* LOCAL METHODS */

void * APR_THREAD_FUNC _lt_server_client_run( 
        apr_thread_t * thread, void * userarg )
{
    char * rbuffer = NULL;
    apr_size_t rbufferlen = 0;

    char * sbuffer = NULL;
    apr_size_t sbufferlen = 0;

    apr_time_t timestart;
    char inprogress = 1;

    /* convert thread routine argument to our server structure */
    lt_server_client_t * client = (lt_server_client_t*) userarg;

    /* register ourselves with the server */
    lt_server_client_register( client->server, client );

    /* prepare receive & send buffers */
    rbuffer = apr_pcalloc( client->pool, 65536 );
    sbuffer = apr_pcalloc( client->pool, 65536 );

    /* remember when we started */
    timestart = apr_time_now();

    lt_server_logger_print( PRIO_THREAD,
            "Thread to handle client communication started...\n" );

    while( 1 ) {
        apr_status_t rv;

        /* bool reading should be atomic operation so no locking is needed */
        if( client->stoprequested ) {
            break;
        }

        /* try to receive data from the network */
        rbufferlen = 65536;
        rv = apr_socket_recv( client->socket, rbuffer, &rbufferlen );

        if( ! APR_STATUS_IS_TIMEUP( rv ) && (APR_SUCCESS != rv )
                && ! APR_STATUS_IS_EAGAIN( rv ) ) {
            /* we are not interested in errors generated by disconnected
             * clients */
            lt_server_logger_print_socket( PRIO_SOCKET, "apr_socket_recv",
                    "A TCP/IP problem while recieving data from client\n" );
            break;
        }

        /* try to process received data */
        sbufferlen = 65536;
        if( LT_CP_SUCCESS != lt_cp_session_process( client->session,
                    rbuffer, rbufferlen, (void**)&sbuffer, &sbufferlen ) ) {
            lt_server_logger_print( PRIO_DEFAULT,
                "Communication protocol error "
                "(while calling lt_cp_session_process)\n" );
            break;
        }

        /* and now send the generated data to the network */
        if( sbufferlen > 0 ) {
            apr_size_t sent = 0;
            apr_size_t sendnow = sbufferlen;

            while( sent < sbufferlen ) {

                rv = apr_socket_send( client->socket,
                        (const char*)(sbuffer + sent), &sendnow );

                if( ! APR_STATUS_IS_TIMEUP( rv ) &&
                        ( APR_SUCCESS != rv) ) {
                    /* we are not interested in errors generated by
                     * disconnected clients */
                    lt_server_logger_print_socket( PRIO_SOCKET,
                            "apr_socket_send",
                            "A TCP/IP problem while sendinf data to client\n" );
                    break;
                }

                sent += sendnow;
                sendnow -= sent;
            }

            /* not eveything has been sent, we are in trouble, so we better
             * exit soon */
            if( sent < sbufferlen ) {
                break;
            }
        }

        /* wait up to 10 seconds for protocol startup */
        if( inprogress ) {
            apr_time_t diff = (apr_time_now() - timestart ) / 1000;
            if( diff > 10 * 1000 ) {
                if( ! lt_cp_session_is_alive( client->session ) ) {
                    lt_server_logger_print( PRIO_DEFAULT,
                            "Timeout while waiting for client "
                            "connection handshake\n" );
                    break;
                }
                inprogress = 0;
            }
        }
    }

    /* close client socket w/o any error reporting */
    apr_socket_shutdown( client->socket, APR_SHUTDOWN_READ );
    apr_socket_shutdown( client->socket, APR_SHUTDOWN_WRITE );
    apr_socket_close( client->socket );

    /* unregister ourselves with the server */
    lt_server_client_unregister( client->server, client );

    lt_server_logger_print( PRIO_THREAD,
            "Thread to handle client communication exiting...\n" );

    apr_thread_exit( thread, 0 );

    /* destroy the memory pool used by this client */
    apr_pool_destroy( client->pool );

    return NULL;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: client.c 1774 2006-05-31 13:17:16Z hynek $
 */
