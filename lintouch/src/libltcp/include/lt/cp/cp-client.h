/* $Id: cp-client.h 347 2005-02-23 14:56:10Z modesto $
 *
 *   FILE: cp-client.h --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 19 June 2003
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

#ifndef _LT_CP_CLIENT_H
#define _LT_CP_CLIENT_H

#include <lt/cp/cp.h>

/**
 * @file cp-client.h
 * @brief LTCP - Client Specific Parts of Lintouch Communication Protocol
 */

/**
 * @defgroup LTCPC Client Specific Parts of Lintouch Communication Protocol
 * @{
 *
 * Definitions of structures and functions used when developing Lintouch
 * Clients.
 *
 * For general client writers only
 * lt_cp_session_send_msg_connect,
 * lt_cp_session_send_msg_disconnect,
 * lt_cp_session_send_msg_select_view,
 * lt_cp_session_send_msg_unselect_view,
 * lt_cp_session_project_url_get
 *
 * are interested, the rest is used internally.
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Return the project URL returned from the server.
     *
     * You can use this call after lt_cp_session_send_msg_connect and
     * as soon as lt_cp_session_is_alive()
     *
     * @param session The session whose project url should be obtained.
     * @return The project URL.
     */
    const char * lt_cp_session_project_url_get(
            lt_cp_session_t * session );

    /**
     * Send connect.
     */
    int lt_cp_session_send_msg_connect(
            lt_cp_session_t * session );

    /**
     * Send disconnect.
     */
    int lt_cp_session_send_msg_disconnect(
            lt_cp_session_t * session );

    /**
     * Send select view.
     *
     * view should be LT_IO_TYPE_STRING.
     */
    int lt_cp_session_send_msg_select_view(
            lt_cp_session_t * session,
            const char * view );

    /**
     * Send unselect view.
     */
    int lt_cp_session_send_msg_unselect_view(
            lt_cp_session_t * session );

    /**
     * Send monitor variables.
     *
     * variables should be LT_IO_TYPE_SEQUENCE of type LT_IO_TYPE_STRING.
     */
    int lt_cp_session_send_msg_monitor_variables(
            lt_cp_session_t * session,
            const lt_io_value_t * variables );

    /**
     * Send unmonitor variables.
     *
     * variables should be LT_IO_TYPE_SEQUENCE of type LT_IO_TYPE_STRING.
     */
    int lt_cp_session_send_msg_unmonitor_variables(
            lt_cp_session_t * session,
            const lt_io_value_t * variables );

    /**
     * Send modify variables.
     *
     * variables_with_values should be LT_IO_TYPE_SEQUENCE with
     * [variable, value, variable, value, ...].
     */
    int lt_cp_session_send_msg_modify_variables(
            lt_cp_session_t * session,
            const lt_io_value_t * variables_with_values );

    /**
     * Send read variables.
     *
     * variables should be LT_IO_TYPE_SEQUENCE of type LT_IO_TYPE_STRING.
     */
    int lt_cp_session_send_msg_read_variables(
            lt_cp_session_t * session,
            const lt_io_value_t * variables );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_CP_CLIENT_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: cp-client.h 347 2005-02-23 14:56:10Z modesto $
 */
