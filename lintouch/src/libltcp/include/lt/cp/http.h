/* $Id: http.h 593 2005-04-27 09:46:33Z mman $
 *
 *   FILE: http.h --
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

#ifndef _LT_HTTP_H
#define _LT_HTTP_H

#include <apr_general.h>

/**
 * @file http.h
 * @brief Lintouch HTTP Server Routines
 */

/**
 * @defgroup LTHTTP Lintouch HTTP Server Routines
 * @{
 *
 * Lintouch HTTP Server Routines.
 *
 * Very simple implementation of non-rfc-conmpliant HTTP Server that serves
 * one file using HTTP/1.0.
 *
 * Create your http server by using lt_http_server_create.
 *
 * Use either lt_http_server_run (will run inside the curent thread) or
 * lt_http_server_start (will start a new thread).
 *
 * Stop the server whenever you don't need it using lt_http_server_stop.
 *
 * Close the opened files/sockets using lt_http_server_destroy and free the
 * server associated pool to finish the cleanup.
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Return status for functions.
     */
    typedef int lt_http_status_t;

#define LT_HTTP_SUCCESS                          0
#define LT_HTTP_INVALID_ARG                     -1
#define LT_HTTP_METHOD_UNIMPLEMENTED            -99

    /**
     * HTTP Server.
     */
    typedef struct lt_http_server_t lt_http_server_t;

    /**
     * return pool out of which the given http server has been allocated.
     */
    apr_pool_t * lt_http_server_pool_get( const lt_http_server_t * src );

    /**
     * Create new http server.
     *
     * Creates a single-client blocking http server that ignores the headers
     * altogether and and serves only given file.
     *
     * @param server The newly created server.
     * @param filepath The file to serve.
     * @param port The port to bind to (0 to use random port).
     * @param pool The pool to allocate server and related data structures
     * out of, when NULL is passed in, new pool will be allocated.
     * @return LT_HTTP_SUCCESS or LT_HTTP_INVALID_ARG
     */
    lt_http_status_t lt_http_server_create( lt_http_server_t ** server,
            const char * filepath,
            int port,
            apr_pool_t * pool );

    /**
     * Determine the server port.
     *
     * Use this routine to determine the port assigned by OS when creating a
     * server with unspecified port (by specifying port=0 to
     * lt_http_server_create).
     *
     * @param server The server whose port should be determined.
     * @return The assigned server port.
     */
    int lt_http_server_port_get(
            const lt_http_server_t * server );

    /**
     * Start the server in a separate thread.
     *
     * @param server The server to be started.
     * @return LT_HTTP_SUCCESS or LT_HTTP_INVALID_ARG
     */
    lt_http_status_t lt_http_server_start( lt_http_server_t * server );

    /**
     * Signal the server to be stopped.
     *
     * The server will finish as soon as the current HTTP transaction
     * completes.
     *
     * @param server The server to be stopped.
     * @return LT_HTTP_SUCCESS or LT_HTTP_INVALID_ARG
     */
    lt_http_status_t lt_http_server_stop( lt_http_server_t * server );

    /**
     * Start the http server and run it until signaled via
     * lt_http_server_stop.
     *
     * Use this method from within a dedicated thread routine.
     *
     * @param server The server to be started.
     * @return LT_HTTP_SUCCESS or LT_HTTP_INVALID_ARG
     */
    lt_http_status_t lt_http_server_run( lt_http_server_t * server );

    /**
     * Close all files/sockets opened by the server.
     *
     * Use this method before cleaning up the pool used for the http server,
     * but after terminating the thread that served run_forever method. This
     * routine will perform close on all possibly opened sockets and files.
     *
     * Note that the lt_http_server_t structure will be in a broken state
     * after this call and must not be reused afterwards.
     *
     * @param server The server to be destroyed.
     * @return LT_HTTP_SUCCESS or LT_HTTP_INVALID_ARG
     */
    lt_http_status_t lt_http_server_destroy( lt_http_server_t * server );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_HTTP_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: http.h 593 2005-04-27 09:46:33Z mman $
 */
