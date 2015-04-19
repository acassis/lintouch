/* $Id: cp.h 723 2005-05-24 08:46:16Z mman $
 *
 *   FILE: cp.h --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 16 June 2003
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

#ifndef _LT_CP_H
#define _LT_CP_H

#ifndef _WIN32
#   include <inttypes.h>
#else
#   ifndef _WIN32_INTTYPES_DEFINED
#       define _WIN32_INTTYPES_DEFINED
#       define uint64_t unsigned __int64
#       define uint32_t unsigned __int32
#       define uint16_t unsigned __int16
#       define uint8_t  unsigned __int8
#       define int64_t  __int64
#       define int32_t  __int32
#       define int16_t  __int16
#       define int8_t   __int8
#   endif
#endif

#include <apr_general.h>

#include <lt/variables/variables.h>

/**
 * @file cp.h
 * @brief Lintouch Communication Protocol Library
 */

/**
 * @defgroup LTCP Lintouch Communication Protocol Library
 * @{
 *
 * Lintouch Communication Protocol Library.
 *
 * Definitions of common protocol structures and functions.
 *
 * FIXME: document create, register_variables, set_project_url, process and
 * send_* methods.
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Maximum message payload length, you can fine-tune this parameter
     * before compiling the library.
     */
#define LT_CP_MSG_PAYLOAD_LENGTH_MAX          ( 65536 * 4 )


    /**
     * Return status for functions.
     */
    typedef int lt_cp_status_t;

#define LT_CP_SUCCESS                          0
#define LT_CP_INVALID_ARG                     -1
#define LT_CP_INVALID_MESSAGE_RECEIVED        -5
#define LT_CP_INVALID_MESSAGE_IN_SENDQUEUE    -6
#define LT_CP_METHOD_UNIMPLEMENTED            -99

    /**
     * State of the protocol session.
     */
    typedef enum {
        /**
         * TCP/IP connect done, but connect not yet sent/received.
         */
        LT_CP_STATE_DISCONNECTED,

        /**
         * connect sent, but not yet confirmed by the server.
         */
        LT_CP_STATE_CONNECTION_IN_PROGRESS,

        /**
         * connect confirmed, no view selected.
         */
        LT_CP_STATE_CONNECTED,

        /**
         * select view sent, but not yet confirmed by the server.
         */
        LT_CP_STATE_VIEW_SELECTION_IN_PROGRESS,

        /**
         * view selected(or connect succesfull for streamed server).
         */
        LT_CP_STATE_VIEW_SELECTED,

    } lt_cp_connection_state_e;

    /**
     * Session mode.
     */
    typedef enum {
        LT_CP_SERVER      =   0,
        LT_CP_CLIENT      =   1,
    } lt_cp_session_mode_e;

    /**
     * Message.
     */
    typedef struct lt_cp_message_t lt_cp_message_t;

    /**
     * Client-Server connection.
     */
    typedef struct lt_cp_session_t lt_cp_session_t;

    /**
     * Initialize Lintouch Communication Library.
     *
     * This function must be called before any of the library functions can
     * be used.
     *
     * Note that this function does not initialize APR Library. It's up to
     * you to do it manually before initializing ltcp.
     */
    lt_cp_status_t lt_cp_initialize(void);

    /**
     * Tear down any internal structures that are not torn down
     * automatically.
     *
     * This function must be called before the program using this library
     * exits. It's recomended to use atexit to ensure that this function
     * gets called.
     *
     * Note that this function does not terminate usage of APR Library. It's
     * up to you to do it manually after terminating ltcp.
     */
    void lt_cp_terminate(void);

    /**
     * return pool out of which the given session has been allocated.
     */
    apr_pool_t * lt_cp_session_pool_get( const lt_cp_session_t * src );

    /**
     * Create new session.
     *
     * @param session The newly created session.
     * @param mode The session mode(LT_CP_SERVER or LT_CP_CLIENT)
     * @param pool The pool to allocate session out of, when NULL is passed
     * in, new pool will be allocated.
     * @return LT_CP_SUCCESS or LT_CP_INVALID_ARG
     */
    lt_cp_status_t lt_cp_session_create( lt_cp_session_t ** session,
            lt_cp_session_mode_e mode,
            apr_pool_t * pool );

    /**
     * Process received input and generate output to be sent to the peer.
     *
     * Users are supposed to call this function periodically and pass in
     * octets received from the network peer and take out octets which they
     * are supposed to send. Since this function can generate output even
     * when no new input has been passed in, it's wise idea to call this
     * function even in a case when the other peer is actually not sending
     * us any data.
     *
     * rbuffer will be consumed entirely by one call to
     * lt_cp_session_process, so you can
     * reuse it when calling lt_cp_session_process next time.
     *
     * sbuffer will be reused by lt_cp_session_process so you should copy
     * any octets that has not been sent to the peer to your own storage
     * before calling this function next time.
     *
     * @param session The session which should process given input.
     * @param rbuffer The octets received from the network which should be
     * processed.
     * @param rbufferlen The number of valid octets in rbuffer.
     * @param sbuffer Will be filled with pointer to buffer containing
     * octets to be sent to the peer.
     * @param sbufferlen Will be filled with number of valid octets in
     * sbuffer.
     * @return The return value should be taken seriously. When you get back
     * anything else then LT_CP_SUCCESS, you should close the connection
     * and destroy the session since data received/to be send contain
     * protocol critical errors.
     */
    lt_cp_status_t lt_cp_session_process( lt_cp_session_t * session,
            const void * rbuffer, apr_size_t rbufferlen,
            void ** sbuffer, apr_size_t * sbufferlen );

    /**
     * Destroy sesion after disconnecting from the other peer.
     *
     * This call will bring the session to the state where it was after
     * create. You are free to reuse the session again or destroy it
     * completely by destroyng the pool out of which it was allocated.
     *
     * @param session The session to be destroyed.
     * @return LT_CP_SUCCESS or LT_CP_INVALID_ARG
     */
    lt_cp_status_t lt_cp_session_destroy( lt_cp_session_t * session );

    /**
     * Determine current mode of the session.
     *
     * @param session The session whose mode should be determined.
     * @return LT_CP_SERVER or LT_CP_CLIENT.
     */
    lt_cp_session_mode_e lt_cp_session_mode_get(
            const lt_cp_session_t * session );

    /**
     * Determine current state of the session.
     *
     * @param session The session whose state should be determined.
     * @return The state of the underlying finite state machine.
     */
    lt_cp_connection_state_e lt_cp_session_state_get(
            const lt_cp_session_t * session );

    /**
     * Determine whether the session startup is in progress, e.g., in a
     * state LT_CP_STATE_NOT_CONNECTED or
     * LT_CP_STATE_CONNECTION_IN_PROGRESS
     */
    int lt_cp_session_is_inprogress( const lt_cp_session_t * session );

    /**
     * Determine whether the session is alive, e.g., at least in state
     * LT_CP_STATE_CONNECTED.
     */
    int lt_cp_session_is_alive( const lt_cp_session_t * session );

    /**
     * Determine whether the session is in view selection state, e.g.,
     * at least in state
     * LT_CP_STATE_VIEW_SELECTION_IN_PROGRESS.
     */
    int lt_cp_session_is_view_selection_inprogress(
            const lt_cp_session_t * session );

    /**
     * Determine whether the session has view selected, e.g.,
     * at least in state
     * LT_CP_STATE_VIEW_SELECTED.
     */
    int lt_cp_session_is_view_selected(
            const lt_cp_session_t * session );

    /**
     * Add variables from varset to be used by the session.
     *
     * @param session The session to register variables with.
     * @param connection_name The name under which the given vars should be
     * stored internally, this name represents the connection where
     * these vars come from. Given string is duplicated internally. Must not
     * contain dot ('.').
     * @param vars Variables to register.
     * @return LT_CP_SUCCESS or LT_CP_INVALID_ARG.
     */
    lt_cp_status_t lt_cp_session_variables_register(
            lt_cp_session_t * session,
            const char * connection_name,
            lt_var_set_t * vars );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_CP_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: cp.h 723 2005-05-24 08:46:16Z mman $
 */
