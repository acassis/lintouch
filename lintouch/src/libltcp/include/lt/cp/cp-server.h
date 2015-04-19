/* $Id: cp-server.h 593 2005-04-27 09:46:33Z mman $
 *
 *   FILE: cp-server.h --
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

#ifndef _LT_CP_SERVER_H
#define _LT_CP_SERVER_H

#include <apr_general.h>
#include <apr_network_io.h>

#include <lt/variables/io.h>
#include <lt/cp/cp.h>

/**
 * @file cp-server.h
 * @brief LTCP - Server Specific Parts of Lintouch Communication Protocol
 */

/**
 * @defgroup LTCPS Server Specific Parts of Lintouch Communication Protocol
 * @{
 *
 * Definitions of structures and functions used when developing Lintouch
 * Servers.
 *
 * For general server writers only the method
 * lt_cp_session_set_project_url is interesting. The rest is for internal
 * use only.
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Set the project URL to be send to the client.
     *
     * @param session The session whose project url should be set.
     * @param project_url The project url that should be served to the client.
     * @return LT_CP_SUCCESS or LT_CP_INVALID_ARG.
     */
    lt_cp_status_t lt_cp_session_project_url_set(
            lt_cp_session_t * session,
            const char * project_url );

    /**
     * Send project URL.
     */
    lt_cp_status_t lt_cp_session_send_msg_project_url(
            lt_cp_session_t * session,
            const lt_cp_message_t * in_response_to );

    /**
     * Send connection refused.
     */
    lt_cp_status_t lt_cp_session_send_msg_connection_refused(
            lt_cp_session_t * session,
            const lt_cp_message_t * in_response_to,
            const char * reason );

    /**
     * Send available variables.
     */
    lt_cp_status_t lt_cp_session_send_msg_avaiable_variables(
            lt_cp_session_t * session,
            const lt_cp_message_t * in_response_to );

    /**
     * Send view not selected.
     */
    lt_cp_status_t lt_cp_session_send_msg_view_not_selected(
            lt_cp_session_t * session,
            const lt_cp_message_t * in_response_to,
            const char * reason );

    /**
     * Send monitoring variables.
     */
    lt_cp_status_t lt_cp_session_send_msg_monitoring_variables(
            lt_cp_session_t * session,
            const lt_cp_message_t * in_response_to,
            const lt_io_value_t * variables );

    /**
     * Send variable values.
     */
    lt_cp_status_t lt_cp_session_send_msg_variable_values(
            lt_cp_session_t * session,
            const lt_cp_message_t * in_response_to,
            const lt_io_value_t * variables_with_values );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_CP_SERVER_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: cp-server.h 593 2005-04-27 09:46:33Z mman $
 */
