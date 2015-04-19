/* $Id: server.h 1524 2006-03-10 09:39:58Z modesto $
 *
 *   FILE: server.h --
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

#ifndef _LT_SERVER_H
#define _LT_SERVER_H

#include <apr_network_io.h>

#include "lt/server/plugin.h" /* for lt_server_status_t */
#include "lt/server/logging.h" /* for lt_logging_plugin_t */
#include "lt/server/client.h"

/**
 * @file server.h
 * @brief Lintouch Server Routines
 */

/**
 * @defgroup LTSS Lintouch Server Routines
 * @{
 *
 * Lintouch Server Routines.
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Return pool out of which the given server has been allocated.
     *
     * @param src The server whose pool should be returned.
     * @return The plugin memory pool.
     */
    apr_pool_t * lt_server_pool_get( const lt_server_t * src );

    /**
     * Create new server.
     *
     * @param server The newly created server.
     * @param socket The main server socket, bound and listening and
     * non-blocking.
     * @param project_url The project url to be served to clients.
     * @param pool The pool to allocate plugin out of, when NULL is passed
     * in, new pool will be allocated.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_create( lt_server_t ** server,
            apr_socket_t * socket,
            const char * project_url,
            apr_pool_t * pool );

    /**
     * Return the project URL.
     *
     * The returned project URL might contain %s sequence to be replaced
     * with the local interface IP/hostname where the client is connected
     * to.
     *
     * @param src The server whose project URL should be returned.
     * @return The project URL.
     */
    const char * lt_server_project_url_get( const lt_server_t * src );

    /**
     * Register initialized plugin with the server.
     *
     * @param server The server to register plugin with.
     * @param name Name of the plugin instance.
     * @param plugin The plugin to be registered.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_plugin_register(
            lt_server_t * server, const char * name,
            lt_server_plugin_t * plugin );

    /**
     * Return plugins registered within the server.
     *
     * @param server The server whose plugins should be registered.
     * @return Registered server plugins.
     */
    apr_hash_t * lt_server_plugins_get(
            lt_server_t * server );

    /**
     * Start a separate thread handling master socket.
     *
     * This call will start the routine handling incoming connections.
     *
     * @param server The server to start.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_start(
            lt_server_t * server );

    /**
     * Stop the thread handling master socket.
     *
     * This call signals the given server thread to be stopped.
     *
     * @param server The server to be stopped.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_stop(
            lt_server_t * server );

    /**
     * Register client as being alive.
     *
     * This call will register given client connection with the server, so
     * that it can be stopped when the server dies prior to client
     * disconnect.
     *
     * @param server The server to register client with.
     * @param client The client to be registered.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_client_register(
            lt_server_t * server, lt_server_client_t * client );

    /**
     * Register client as being finished.
     *
     * This call will unregister given client connection with the server, so
     * that it will not be stopped when the server will be shutdown.
     *
     * @param server The server to unregister client from.
     * @param client The client to be unregistered.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_client_unregister(
            lt_server_t * server, lt_server_client_t * client );

    /**
     * Register initialized logging plugin with the server.
     *
     * @param server The server to register plugin with.
     * @param name Name of the plugin instance.
     * @param plugin The plugin to be registered.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_logging_plugin_register(
            lt_server_t * server, const char * name,
            lt_logging_plugin_t * plugin );

    /**
     * Return plugins registered within the server.
     *
     * @param server The server whose plugins should be registered.
     * @return Registered server plugins.
     */
    apr_hash_t * lt_server_logging_plugins_get(
            lt_server_t * server );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_SERVER_H */
/* vim: set et ts=4 sw=4 tw=76:
 * $Id: server.h 1524 2006-03-10 09:39:58Z modesto $
 */

