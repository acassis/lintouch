/* $Id: server.c 1774 2006-05-31 13:17:16Z hynek $
 *
 *   FILE: server.c --
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

#include "lt/server/server.h"
#include "lt/server/client.h"
#include "lt/server/logger.h"

struct lt_server_t
{
    apr_pool_t * pool;
    apr_socket_t * socket;
    apr_thread_mutex_t * mutex;
    apr_thread_t * thread;

    char stoprequested;

    const char * project_url;

    apr_hash_t * plugins;
    apr_hash_t * clients;
    apr_hash_t * logging;
};

/**
 * Start all registered server plugins.
 *
 * @param server The server whose plugins should be started.
 * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
 */
static lt_server_status_t _lt_server_plugins_start(
        lt_server_t * server );

/**
 * Stop all registered server plugins.
 *
 * @param server The server whose plugins should be stopped.
 * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
 */
static lt_server_status_t _lt_server_plugins_stop(
        lt_server_t * server );

/**
 * Stop all running client threads.
 *
 * @param server The server whose client threads should be stopped.
 * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
 */
static lt_server_status_t _lt_server_clients_stop(
        lt_server_t * server );

/**
 * Thread routine
 *
 * @param thread Thread
 * @param userarg User argument
 */
void * APR_THREAD_FUNC _lt_server_run( apr_thread_t * thread, void * userarg );

/* LOCAL FUNTIONS */

/* lt_var_set_t logging hook */
static void _lt_server_logging_hook(lt_var_set_t * vset, int real,
        void* user_data );

/* METHOD BODIES */

apr_pool_t * lt_server_pool_get( const lt_server_t * src )
{
    if( src == NULL ) return NULL;
    return src->pool;
}

#define CREATE_POOL_IF_NULL(p)  if((p)==NULL)apr_pool_create(&(p),NULL)

lt_server_status_t lt_server_create( lt_server_t ** server,
        apr_socket_t * socket,
        const char * project_url,
        apr_pool_t * pool )
{
    if( socket == NULL ) return LT_SERVER_INVALID_ARG;
    if( project_url == NULL ) return LT_SERVER_INVALID_ARG;

    CREATE_POOL_IF_NULL( pool );

    *server = ( lt_server_t * ) apr_pcalloc( pool,
            sizeof( lt_server_t ) );

    if( *server == NULL ) return LT_SERVER_INVALID_ARG;

    ( *server )->pool = pool;
    ( *server )->socket = socket;
    ( *server )->project_url = apr_pstrdup( pool, project_url );
    ( *server )->stoprequested = 0;

    ( *server )->plugins = apr_hash_make( pool );
    ( *server )->clients = apr_hash_make( pool );
    ( *server )->logging = apr_hash_make( pool );

    apr_thread_mutex_create( &(( *server )->mutex), 0, pool );

    return LT_SERVER_SUCCESS;
}

const char * lt_server_project_url_get( const lt_server_t * src )
{
    if( src == NULL ) return NULL;
    return src->project_url;
}

lt_server_status_t lt_server_plugin_register( lt_server_t * server,
        const char * name,
        lt_server_plugin_t * plugin )
{
    if( server == NULL ) return LT_SERVER_INVALID_ARG;
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;
    if( name == NULL ) return LT_SERVER_INVALID_ARG;

    /* lock server mutex */
    apr_thread_mutex_lock( server->mutex );

    /* add plugin to the hash table */
    apr_hash_set( server->plugins,
            (const void*)name, APR_HASH_KEY_STRING, (void*)plugin );

    /* hook in the logging but only if we have a logging plugin loaded */
    if(apr_hash_count(server->logging) > 0) {
        lt_var_set_logging_set(lt_server_plugin_varset_get(plugin),
                _lt_server_logging_hook, (void*)server);
    }

    /* unlock server mutex */
    apr_thread_mutex_unlock( server->mutex );

    return LT_SERVER_SUCCESS;
}

apr_hash_t * lt_server_plugins_get( lt_server_t * server )
{
    if( server == NULL ) return NULL;
    return server->plugins;
}

lt_server_status_t lt_server_logging_plugin_register(
        lt_server_t * server, const char * name,
        lt_logging_plugin_t * plugin )
{
    if( server == NULL ) return LT_SERVER_INVALID_ARG;
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;
    if( name == NULL ) return LT_SERVER_INVALID_ARG;

    /* lock server mutex */
    apr_thread_mutex_lock( server->mutex );

    /* add plugin to the hash table */
    apr_hash_set( server->logging,
            (const void*)name, APR_HASH_KEY_STRING, (void*)plugin );

    /* unlock server mutex */
    apr_thread_mutex_unlock( server->mutex );

    return LT_SERVER_SUCCESS;
}

apr_hash_t * lt_server_logging_plugins_get( lt_server_t * server )
{
    if( server == NULL ) return NULL;
    return server->logging;
}

lt_server_status_t lt_server_start(
        lt_server_t * server )
{
    if( server == NULL ) return LT_SERVER_INVALID_ARG;

    /* start plugins */
    _lt_server_plugins_start( server );

    /* prepare & start main server thread */
    if( APR_SUCCESS != apr_thread_create(
                    &(server->thread), NULL,
                    (apr_thread_start_t)_lt_server_run, server,
                     server->pool ) ) {
        lt_server_logger_print_apr( PRIO_DEFAULT, "apr_thread_create",
                "Unable to create server thread\n" );
        return LT_SERVER_INVALID_ARG;
    }

    return LT_SERVER_SUCCESS;
}

lt_server_status_t lt_server_stop(
        lt_server_t * server )
{
    if( server == NULL ) return LT_SERVER_INVALID_ARG;

    /* bool assignment should be atomic operation so no locking is needed */
    server->stoprequested = 1;

    /* wait for server thread to finish. */
    {
        apr_status_t rv;

        if( APR_SUCCESS != apr_thread_join(
                        &rv, server->thread ) ) {
            lt_server_logger_print_apr( PRIO_DEFAULT, "apr_thread_join",
                    "Unable to wait for server thread to terminate\n" );
        }
    }

    /* disconnect clients */
    _lt_server_clients_stop( server );

    /* stop plugins */
    _lt_server_plugins_stop( server );

    return LT_SERVER_SUCCESS;
}

lt_server_status_t lt_server_client_register(
        lt_server_t * server, lt_server_client_t * client )
{
    if( server == NULL ) return LT_SERVER_INVALID_ARG;
    if( client == NULL ) return LT_SERVER_INVALID_ARG;

    /* lock server mutex */
    apr_thread_mutex_lock( server->mutex );

    /* add client to the hash table */
    apr_hash_set( server->clients,
            (const void*)client, sizeof( client ), (void*)client );

    /* unlock server mutex */
    apr_thread_mutex_unlock( server->mutex );

    return LT_SERVER_SUCCESS;
}

lt_server_status_t lt_server_client_unregister(
        lt_server_t * server, lt_server_client_t * client )
{
    if( server == NULL ) return LT_SERVER_INVALID_ARG;
    if( client == NULL ) return LT_SERVER_INVALID_ARG;

    /* try to lock server mutex */
    if( ! APR_STATUS_IS_EBUSY( apr_thread_mutex_trylock( server->mutex ) ) ) {

        /* add client to the hash table */
        apr_hash_set( server->clients,
                (const void*)client, sizeof( client ), NULL );

        /* unlock server mutex */
        apr_thread_mutex_unlock( server->mutex );
    }

    return LT_SERVER_SUCCESS;
}

/* LOCAL METHODS */

lt_server_status_t _lt_server_plugins_start(
        lt_server_t * server )
{
    if( server == NULL ) return LT_SERVER_INVALID_ARG;

    /* lock server mutex */
    apr_thread_mutex_lock( server->mutex );

    /* start all registered clients */
    {
        apr_hash_index_t * hi = NULL;

        hi = apr_hash_first( server->pool, server->plugins );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {

            const char * key = NULL;
            lt_server_plugin_t * plugin = NULL;
            apr_hash_this( hi, (const void**)&key, NULL, (void**)&plugin );

            lt_server_logger_print( PRIO_THREAD,
                    "Starting plugin thread for connection %s...\n", key );

            lt_server_plugin_start( plugin );
        }
    }

    /* unlock server mutex */
    apr_thread_mutex_unlock( server->mutex );

    return LT_SERVER_SUCCESS;
}

lt_server_status_t _lt_server_plugins_stop(
        lt_server_t * server )
{
    if( server == NULL ) return LT_SERVER_INVALID_ARG;

    /* lock server mutex */
    apr_thread_mutex_lock( server->mutex );

    /* start all registered clients */
    {
        apr_hash_index_t * hi = NULL;

        hi = apr_hash_first( server->pool, server->plugins );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {

            const char * key = NULL;
            lt_server_plugin_t * plugin = NULL;
            apr_hash_this( hi, (const void**)&key, NULL, (void**)&plugin );

            lt_server_logger_print( PRIO_THREAD,
                    "Stopping plugin thread for connection %s...\n", key );

            lt_server_plugin_stop( plugin );
        }
    }

    /* unlock server mutex */
    apr_thread_mutex_unlock( server->mutex );

    return LT_SERVER_SUCCESS;
}

lt_server_status_t _lt_server_clients_stop(
        lt_server_t * server )
{
    if( server == NULL ) return LT_SERVER_INVALID_ARG;

    /* lock server mutex */
    apr_thread_mutex_lock( server->mutex );

    /* stop all connected clients */
    {
        apr_hash_index_t * hi = NULL;

        hi = apr_hash_first( server->pool, server->clients );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {

            lt_server_client_t * client = NULL;
            apr_hash_this( hi, NULL, NULL, (void**)&client );

            lt_server_logger_print( PRIO_THREAD,
                    "Stopping client thread...\n" );

            lt_server_client_stop( client );
        }
    }

    /* unlock server mutex */
    apr_thread_mutex_unlock( server->mutex );

    return LT_SERVER_SUCCESS;
}

void * APR_THREAD_FUNC _lt_server_run( apr_thread_t * thread, void * userarg )
{
    /* convert thread routine argument to our server structure */
    lt_server_t * server = (lt_server_t*) userarg;

    lt_server_logger_print( PRIO_THREAD,
            "Server thread started...\n" );

    while( 1 ) {
        apr_status_t rv;
        apr_pool_t * clientpool = NULL;
        apr_socket_t * clientsocket = NULL;
        lt_server_client_t * client = NULL;

        /* bool reading should be atomic operation so no locking is needed */
        if( server->stoprequested ) {
            break;
        }

        /* debugging messages showing that the thread is alive */
        apr_sleep( 500 * 1000 );

        apr_pool_create( &clientpool, server->pool );

        /* accept new connection */
        rv = apr_socket_accept( &clientsocket, server->socket, clientpool );

        if( APR_STATUS_IS_EAGAIN( rv ) || APR_STATUS_IS_EINTR( rv ) ) {
            /* sleep for 100ms before accepting new client */
            apr_sleep( 100 * 1000 );
            apr_pool_destroy( clientpool );
            continue;
        }

        if( APR_SUCCESS != rv ) {
            lt_server_logger_print_socket( PRIO_DEFAULT, "apr_socket_accept",
                    "Unable to accept new client connection\n" );
            apr_pool_destroy( clientpool );
            continue;
        }

        /* determine client address */
        {
            apr_sockaddr_t * sa = NULL;
            char * ip = NULL;
            apr_sockaddr_t * sal = NULL;
            char * ipl = NULL;

            /* determine remote address and port */
            if( APR_SUCCESS != apr_socket_addr_get( &sa,
                            APR_REMOTE, clientsocket ) ) {
                lt_server_logger_print_socket( PRIO_DEFAULT,
                        "apr_socket_addr_get",
                        "Unable to determine remote address of the "
                        "client socket\n" );
                apr_socket_close( clientsocket );
                apr_pool_destroy( clientpool );
                continue;
            }
            if( APR_SUCCESS != apr_sockaddr_ip_get( &ip, sa ) ) {
                lt_server_logger_print_socket( PRIO_DEFAULT,
                        "apr_sockaddr_ip_get",
                        "Unable to determine remote IP address of the "
                        "client socket\n" );
                apr_socket_close( clientsocket );
                apr_pool_destroy( clientpool );
                continue;
            }

            /* determine local address and port */
            if( APR_SUCCESS != apr_socket_addr_get( &sal,
                            APR_LOCAL, clientsocket ) ) {
                lt_server_logger_print_socket( PRIO_DEFAULT,
                        "apr_socket_addr_get",
                        "Unable to determine local address of the "
                        "client socket\n" );
                apr_socket_close( clientsocket );
                apr_pool_destroy( clientpool );
                continue;
            }
            if( APR_SUCCESS != apr_sockaddr_ip_get( &ipl, sal ) ) {
                lt_server_logger_print_socket( PRIO_DEFAULT,
                        "apr_sockaddr_ip_get",
                        "Unable to determine local IP address of the "
                        "client socket\n" );
                apr_socket_close( clientsocket );
                apr_pool_destroy( clientpool );
                continue;
            }

            lt_server_logger_print( PRIO_DEFAULT, "Accepting client from %s:%d "
                    "(connected to %s:%d)\n",
                    ip, sa->port, ipl, sal->port );
        }

        /* create client */
        if( lt_server_client_create(
                    &client, clientsocket, server, clientpool ) !=
                LT_SERVER_SUCCESS ) {
            lt_server_logger_print( PRIO_DEFAULT,
                    "Unable to create client handling data structures\n" );
            apr_socket_close( clientsocket );
            apr_pool_destroy( clientpool );
            continue;
        }

        /* and fork a thread for him */
        if( lt_server_client_start( client ) !=
                LT_SERVER_SUCCESS ) {
            lt_server_logger_print( PRIO_DEFAULT,
                    "Unable to start client handling thread\n" );
            apr_socket_close( clientsocket );
            apr_pool_destroy( clientpool );
            continue;
        }
    }

    lt_server_logger_print( PRIO_THREAD,
            "Server thread exiting...\n" );

    apr_thread_exit( thread, 0 );

    return NULL;
}

void _lt_server_logging_hook(lt_var_set_t * vset, int real, void* user_data)
{
    lt_server_t* server = (lt_server_t*)user_data;

    apr_time_exp_t timestamp;
    apr_time_exp_lt(&timestamp, apr_time_now());

    // walk the hash of all logging plugins and call their log()
    {
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;
        apr_pool_t * lpool = NULL;

        apr_pool_create( &lpool, lt_server_pool_get(server) );

        /* for each logging plugin */
        lt_logging_plugin_t * plugin = NULL;
        h = server->logging;
        hi = apr_hash_first( lpool, h );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
            apr_hash_this( hi, NULL, NULL, (void**)&plugin );

            /* call the log function */
            lt_logging_plugin_log(plugin, &timestamp, vset, real);
        }

        apr_pool_destroy( lpool );
    }
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: server.c 1774 2006-05-31 13:17:16Z hynek $
 */
