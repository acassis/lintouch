/* $Id: cp-private.h 347 2005-02-23 14:56:10Z modesto $
 *
 *   FILE: cp-private.h --
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

#ifndef _LT_CP_PRIVATE_H
#define _LT_CP_PRIVATE_H

#include "lt/fsm/fsm.h"

/**
 * Communication protocol message codes.
 */
typedef enum {
    LT_CP_MSG_INVALID                 =   0x00,
    LT_CP_MSG_NONE                    =   0x01,

    LT_CP_MSG_CONNECT                 =   0x10,
    LT_CP_MSG_PROJECT_URL             =   0x11,
    LT_CP_MSG_CONNECTION_REFUSED      =   0x12,
    LT_CP_MSG_DISCONNECT              =   0x13,

    LT_CP_MSG_SELECT_VIEW             =   0x20,
    LT_CP_MSG_AVAILABLE_VARIABLES     =   0x21,
    LT_CP_MSG_VIEW_NOT_SELECTED       =   0x22,
    LT_CP_MSG_UNSELECT_VIEW           =   0x23,

    LT_CP_MSG_MONITOR_VARIABLES       =   0x30,
    LT_CP_MSG_UNMONITOR_VARIABLES     =   0x31,
    LT_CP_MSG_MONITORING_VARIABLES    =   0x32,

    LT_CP_MSG_MODIFY_VARIABLES        =   0x40,
    LT_CP_MSG_READ_VARIABLES          =   0x41,

    LT_CP_MSG_VARIABLE_VALUES         =   0x50,

    LT_CP_MSG_ANY                     =   0xff,
    LT_CP_TOTAL_MESSAGE_TYPES         =   0xff,
} lt_cp_message_type_e;

#if 0 /* the old code needed by msvc */

#ifdef _WIN32
#pragma pack(push, ident, 1)
#define ATTR_PACKED
#else
#define ATTR_PACKED __attribute__((packed))
#endif

#else
#define ATTR_PACKED __attribute__((packed))
#endif

/*
 * message header, all fields stored in network order.
 */
typedef struct ATTR_PACKED lt_cp_message_header_t {
    apr_uint32_t    message_length;
    apr_uint32_t    message_id;
    apr_uint64_t    message_timestamp;
    apr_uint16_t    message_type;   /* see lt_cp_message_type */
    /* here follows the lt_cp_message_payload_header */
} ATTR_PACKED lt_cp_message_header_t;

/*
 * message payload header, all fields stored in network order.
 */
typedef struct ATTR_PACKED lt_cp_message_payload_header_t {
    apr_uint32_t    payload_length;
    apr_uint16_t    payload_type;   /* see lt_cp_message_payload_type */
    /* here follows the payload itself. */
} ATTR_PACKED lt_cp_message_payload_header_t;

struct lt_cp_message_t {
    lt_cp_message_header_t header;
    lt_cp_message_payload_header_t payload_header;
    const lt_io_value_t * payload;
    apr_pool_t * pool;
} ATTR_PACKED;

#if 0 /* the old code needed by msvc */

#ifdef _WIN32
#pragma pack(pop, ident)
#endif
#undef ATTR_PACKED

#else
#undef ATTR_PACKED
#endif

#define LT_CP_MESSAGE_HEADER_LEN \
    ( sizeof( lt_cp_message_header_t ) + \
      sizeof( lt_cp_message_payload_header_t ) )

#define LT_CP_MESSAGE_LENGTH_MAX \
    ( LT_CP_MSG_PAYLOAD_LENGTH_MAX + LT_CP_MESSAGE_HEADER_LEN )

struct lt_cp_session_t {
    /**
     * Pool used to allocate memory related to this session.
     */
    apr_pool_t * pool;

    /**
     * Pool used to allocate memory for project url.
     */
    apr_pool_t * url_pool;

    /**
     * Pool used to allocate memory for enqueued messages.
     * Cleared after the messages are dequeued.
     */
    apr_pool_t * send_pool;

    /**
     * Pool used to allocate memory for decoded messages.
     * Cleared after the messages are processed.
     */
    apr_pool_t * recv_pool;

    /**
     * Pool used to allocate memory for variables.
     * Cleared when the session is destroyed.
     */
    apr_pool_t * conn_pool;

    /**
     * Receive buffer, holding received data that has not yet been
     * processed. Length represents total available size, offset represents
     * how many bytes from the beginning of the buffer are valid(e.g.
     * received but unprocessed).
     */
    void * receive_buffer;
    unsigned receive_buffer_length;
    unsigned receive_buffer_offset;
    apr_pool_t * receive_buffer_pool;

    /**
     * Send buffer, holding encoded data that has not yet been
     * delivered to the client as a result of process().
     * Length represents total available size, offset represents
     * how many bytes from the beginning of the buffer are valid(e.g.
     * encoded but not yet returned to the client).
     */
    void * send_buffer;
    unsigned send_buffer_length;
    unsigned send_buffer_offset;
    apr_pool_t * send_buffer_pool;

    /**
     * Send queue. holding prepared, but not yet encoded messages to be
     * sent. they are transformed from send_queue to send_buffer when
     * calling process().
     */
    apr_array_header_t * send_queue;


    /**
     * Am I server or the client.
     */
    lt_cp_session_mode_e mode;

    /**
     * State of the connection.
     */
    lt_cp_connection_state_e state;

    /**
     * Protocol state flow session.
     */
    lt_fsm_session_t * fsm_session;

    /**
     * All available variables from all connections. indexed by connection
     * name(const char*) and containing lt_var_set_t *
     */
    apr_hash_t * connections;

    /**
     * Number of connections in variables hash (for caching purposes)
     */
    unsigned nconnections;

    /**
     * Number of all variables we have in variables hash (for caching
     * purposes).
     */
    unsigned nvariables;

    /**
     * Project url that
     * - should be served to the client when in server mode,
     * - has been received from theserver when in client mode
     */
    const char * project_url;
};

#endif /* _LT_CP_PRIVATE_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: cp-private.h 347 2005-02-23 14:56:10Z modesto $
 */
