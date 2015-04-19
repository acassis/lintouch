/* $Id: http.c 1774 2006-05-31 13:17:16Z hynek $
 *
 *   FILE: http.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 13 April 2005
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <apr_file_info.h>
#include <apr_network_io.h>
#include <apr_strings.h>
#include <apr_thread_proc.h>
#include <apr_version.h>

#include "lt/cp/http.h"

struct lt_http_server_t {
    /* memory pool for all server related data */
    apr_pool_t * pool;

    /* main server thread */
    apr_thread_t * thread;

    /* should the server be running? */
    char stoprequested;

    /* full path + filename of the file we serve */
    const char * filepath;

    /* TCP/IP communication port where we accept HTTP connections */
    int port;

    /* server socket */
    apr_socket_t * socket;

    /* file to serve */
    apr_file_t * file;

    /* file info (for size, filename, etc.) */
    apr_finfo_t finfo;
};

static void my_perror( const char * prefix );
static void * _lt_http_server_run( apr_thread_t * thread, void * userarg );

apr_pool_t * lt_http_server_pool_get( const lt_http_server_t * src )
{
    if( src == NULL ) return NULL;
    return src->pool;
}

#define CREATE_POOL_IF_NULL(p)  if((p)==NULL)apr_pool_create(&(p),NULL)

lt_http_status_t lt_http_server_create( lt_http_server_t ** server,
        const char * filepath,
        int port,
        apr_pool_t * pool )
{
    CREATE_POOL_IF_NULL( pool );

    *server = ( lt_http_server_t * ) apr_pcalloc( pool,
            sizeof( lt_http_server_t ) );

    if( *server == NULL ) return LT_HTTP_INVALID_ARG;

    ( *server )->pool = pool;
    ( *server )->port = port;
    ( *server )->filepath = apr_pstrdup( pool, filepath );
    ( *server )->stoprequested = 0;
    ( *server )->thread = NULL;

    /*
     * TODO: extract filename from filepath and urlencode it to
     * create the only url that we support for GET method. I'm leaving it
     * unimplemented for now because it's not that important
     */

    /* determine file size and name */
    if( APR_SUCCESS != apr_stat( &(( *server )->finfo),
                    ( *server )->filepath,
                    APR_FINFO_SIZE,
                    pool ) ) {
        my_perror( "ERROR: apr_stat failed with: " );
        return LT_HTTP_INVALID_ARG;
    }

    /* open file */
    if( APR_SUCCESS != apr_file_open( &(( *server )->file),
                    ( *server )->filepath,
                    APR_READ | APR_BINARY | APR_SENDFILE_ENABLED,
                    0, pool ) ) {
        my_perror( "ERROR: apr_file_open failed with: " );
        return LT_HTTP_INVALID_ARG;
    }

    /* create socket */
    if( APR_SUCCESS != apr_socket_create( &(( *server )->socket),
                    APR_INET, SOCK_STREAM,
#if (APR_MAJOR_VERSION > 0)
                    APR_PROTO_TCP,
#endif
                    pool ) ) {
        my_perror( "ERROR: apr_socket_create failed with: " );
        return LT_HTTP_INVALID_ARG;
    }

    {
        /* prepare socket address */
        apr_sockaddr_t * sa = NULL;
        if( APR_SUCCESS != apr_sockaddr_info_get( &sa,
                        NULL, APR_UNSPEC, port, 0, pool ) ) {
            my_perror( "ERROR: apr_sockaddr_info_get failed with: " );
            return LT_HTTP_INVALID_ARG;
        }

        /* bind socket to given port or let the OS to give us port */
        if( APR_SUCCESS != apr_socket_bind(
                        ( *server )->socket, sa ) ) {
            my_perror( "ERROR: apr_socket_bind failed with: " );
            return LT_HTTP_INVALID_ARG;
        }
    }

    /* determine where we are really listening */
    {
        apr_sockaddr_t * sa = NULL;
        if( APR_SUCCESS != apr_socket_addr_get( &sa,
                        APR_LOCAL, ( *server )->socket ) ) {
            my_perror( "ERROR: apr_socket_addr_get failed with: " );
            return LT_HTTP_INVALID_ARG;
        }

        ( *server )->port = sa->port;
    }

    /* and make the socket listen */
    if( APR_SUCCESS != apr_socket_listen(
                    ( *server )->socket, 5 ) ) {
        my_perror( "ERROR: apr_socket_listen failed with: " );
        return LT_HTTP_INVALID_ARG;
    }

    return LT_HTTP_SUCCESS;
}

int lt_http_server_port_get( const lt_http_server_t * server )
{
    if( server == NULL ) return -1;
    return server->port;
}

lt_http_status_t lt_http_server_start( lt_http_server_t * server )
{
    if( server == NULL ) return LT_HTTP_INVALID_ARG;

    /* prepare & start thread */
    if( APR_SUCCESS != apr_thread_create(
                    &(server->thread), NULL,
                    (apr_thread_start_t)_lt_http_server_run, server,
                     server->pool ) ) {
        my_perror( "ERROR: apr_thread_create failed with: " );
        return LT_HTTP_INVALID_ARG;
    }

    return LT_HTTP_SUCCESS;
}

lt_http_status_t lt_http_server_stop( lt_http_server_t * server )
{
    if( server == NULL ) return LT_HTTP_INVALID_ARG;

    /* bool assignment should be atomic operation so no locking is needed */
    server->stoprequested = 1;

    /* if we were running in a separate thread we wait for it to finish. */
    if( server->thread != NULL ) {
        apr_status_t rv;

        if( APR_SUCCESS != apr_thread_join(
                        &rv, server->thread ) ) {
            my_perror( "ERROR: apr_thread_join failed with: " );
        }
    }

    return LT_HTTP_SUCCESS;
}

lt_http_status_t lt_http_server_run( lt_http_server_t * server )
{
    apr_pool_t * pool = NULL;
    apr_socket_t * client = NULL;
    char error[1025];
    memset( error, 0, 1025 );

    if( server == NULL ) return LT_HTTP_INVALID_ARG;

    /* prepare connection pool */
    apr_pool_create( &pool, server->pool );

    /* make the socket non-blocking */
    if( APR_SUCCESS != apr_socket_opt_set( server->socket,
                    APR_SO_NONBLOCK, 1 ) ) {
        my_perror( "ERROR: apr_socket_opt_set failed with: " );
        return LT_HTTP_INVALID_ARG;
    }

    while( 1 ) {
        apr_status_t rv;

        /* bool reading should be atomic operation so no locking is needed */
        if( server->stoprequested ) {
            break;
        }

        /* clear pool memory */
        apr_pool_clear( pool );

        /* accept new connection */
        rv = apr_socket_accept( &client, server->socket, pool );

        if( APR_STATUS_IS_EAGAIN( rv ) || APR_STATUS_IS_EINTR( rv ) ) {
            /* sleep for 100ms before accepting new client */
            apr_sleep( 100 * 1000 );
            continue;
        }

        if( APR_SUCCESS != rv ) {
            my_perror( "ERROR: apr_socket_accept failed with: " );
            continue;
        }

        /* determine client address */
        {
            apr_sockaddr_t * sa = NULL;
            char * ip = NULL;
            if( APR_SUCCESS != apr_socket_addr_get( &sa,
                            APR_REMOTE, client ) ) {
                my_perror( "ERROR: apr_socket_addr_get failed with: " );
                apr_socket_close( client );
                continue;
            }
            if( APR_SUCCESS != apr_sockaddr_ip_get( &ip, sa ) ) {
                my_perror( "ERROR: apr_sockaddr_ip_get failed with: " );
                apr_socket_close( client );
                continue;
            }
        }


        /* immediatelly start sending HTTP response headers */
        {
            char * headers = apr_pstrcat( pool,
                    "HTTP/1.0 200 OK\r\n"
                    "Content-Length: ",
                    apr_ltoa( pool, server->finfo.size ),
                    "\r\n",
                    "Content-Type: application/octet-stream;"
                    " charset=utf-8\r\n",
                    "Connection: Close\r\n",
                    "\r\n",
                    NULL );
            apr_size_t headers_size = strlen( headers );
            if( APR_SUCCESS != apr_socket_send(
                            client, headers, &headers_size ) ) {
                my_perror( "ERROR: apr_socket_send failed with: " );
                apr_socket_close( client );
                continue;
            }
        }

        /* send file contents */
        {
            apr_off_t offset = 0;
            apr_size_t len = server->finfo.size;

            if( APR_SUCCESS != apr_socket_sendfile(
                            client, server->file, NULL,
                            &offset, &len, 0 ) ) {
                my_perror( "ERROR: apr_socket_sendfile failed with: " );
                apr_socket_close( client );
                continue;
            }
        }

        /* read and discard all headers */
        {
            apr_status_t rv;

            /* set non-block option on client socket */
            if( APR_SUCCESS != apr_socket_timeout_set( client,
                            2 * 1000 * 1000 ) ) {
                my_perror( "ERROR: apr_socket_timeout_set failed with: " );
                apr_socket_close( client );
                continue;
            }

            /* read all data until 2 sec timeout or eof, then proceed to */
            /* close */
            do {
                char buffer[1024];
                apr_size_t len = 1024;
                rv = apr_socket_recv( client, buffer, &len );

                if( APR_STATUS_IS_TIMEUP( rv ) || APR_STATUS_IS_EOF( rv ) ) {
                    break;
                }
            } while( 1 );
        }

        /* close our side of connection */
        if( APR_SUCCESS !=
                    apr_socket_shutdown( client, APR_SHUTDOWN_WRITE ) ) {
            /* we actually don't care about errors arriving during shutdown
             * phase
             * my_perror( "ERROR: apr_socket_shutdown(WRITE) failed with: " );
             */
            apr_socket_close( client );
            continue;
        }

        /* close other side of connection */
        if( APR_SUCCESS !=
                    apr_socket_shutdown( client, APR_SHUTDOWN_READ ) ) {
            /* we actually don't care about errors arriving during shutdown
             * phase
             * my_perror( "ERROR: apr_socket_shutdown(READ) failed with: " );
             */
            apr_socket_close( client );
            continue;
        }

        /* close socket */
        if( APR_SUCCESS != apr_socket_close( client ) ) {
            /* we actually don't care about errors arriving during shutdown
             * phase
             * my_perror( "ERROR: apr_socket_close failed with: " );
             */
            continue;
        }
    }

    return LT_HTTP_SUCCESS;
}

lt_http_status_t lt_http_server_destroy( lt_http_server_t * server )
{
    if( server == NULL ) return LT_HTTP_INVALID_ARG;

    /* close the master socket */
    if( APR_SUCCESS != apr_socket_close( server->socket ) ) {
        my_perror( "ERROR: apr_socket_close failed with: " );
    }

    /* close the opened file that has been served */
    if( APR_SUCCESS != apr_file_close( server->file ) ) {
        my_perror( "ERROR: apr_file_info failed with: " );
    }

    return LT_HTTP_SUCCESS;
}

void my_perror( const char * prefix )
{
    char error[1025];
    apr_strerror( apr_get_os_error(), error, 1024 );
    fprintf( stderr, "%s(%ld) %s\n", prefix, (long)apr_get_os_error(), error );
}

void * _lt_http_server_run( apr_thread_t * thread, void * userarg )
{
    /* convert thread routine argument to our server structure */
    lt_http_server_t * server = (lt_http_server_t*) userarg;

    /* run the server */
    lt_http_server_run( server );

    apr_thread_exit( thread, 0 );

    return NULL;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: http.c 1774 2006-05-31 13:17:16Z hynek $
 */
