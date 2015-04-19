/* $Id: client.h 593 2005-04-27 09:46:33Z mman $
 *
 *   FILE: client.h --
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

#ifndef _LT_CLIENT_H
#define _LT_CLIENT_H

#include <apr_network_io.h>

#include "lt/server/plugin.h" /* for lt_server_status_t */

/**
 * @file client.h
 * @brief Lintouch Server Client Handling Routines
 */

/**
 * @defgroup LTSC Lintouch Server Client Handling Routines
 * @{
 *
 * Lintouch Server Client Handling Routines.
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Lintouch server.
     *
     * This data structure contains references to all running plugins and
     * all connected clients.
     */
    typedef struct lt_server_t lt_server_t;

    /**
     * Lintouch client connected to a server.
     */
    typedef struct lt_server_client_t lt_server_client_t;

    /**
     * Return pool out of which the given client has been allocated.
     *
     * @param src The client whose pool should be returned.
     * @return The plugin memory pool.
     */
    apr_pool_t * lt_server_client_pool_get( const lt_server_client_t * src );

    /**
     * Create new client communicating with the server over given socket.
     *
     * @param client The newly created client.
     * @param socket Accepted socket used to communicate with client.
     * @param server The server data (containing varsets, etc.).
     * @param pool The pool to allocate plugin out of, when NULL is passed
     * in, new pool will be allocated.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_client_create ( lt_server_client_t ** client,
            apr_socket_t * socket, lt_server_t * server, apr_pool_t * pool );

    /**
     * Start a separate thread handling one connected client.
     *
     * This call will start the routine handling network communication with
     * one client in a separate thread.
     *
     * @param client The client to be handled.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_client_start(
            lt_server_client_t * client );

    /**
     * Stop the thread handling one connected client.
     *
     * This call signals the given client thread to be stopped.
     *
     * @param client The client to be stopped.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_client_stop(
            lt_server_client_t * client );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_CLIENT_H */
/* vim: set et ts=4 sw=4 tw=76:
 * $Id: client.h 593 2005-04-27 09:46:33Z mman $
 */

