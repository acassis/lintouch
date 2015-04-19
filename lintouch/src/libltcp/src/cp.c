/* $Id: cp.c 1774 2006-05-31 13:17:16Z hynek $
 *
 *   FILE: cp.c --
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <apr_strings.h>

#include "lt/cp/cp.h"
#include "lt/cp/cp-client.h"
#include "lt/cp/cp-server.h"

#include "cp-private.h"

/* define to enable lot of fprintfs */
#undef FLOOD_CONSOLE_WITH_DEBUG

/**
 * Hook related structures and routines
 */

/*
 * This structure is passed to hooks, they can extract session, and
 * request/response message as they wish
 */
typedef struct lt_cp_fsm_hook_arg_t {
    lt_cp_session_t * session;
    const lt_cp_message_t * message;
} lt_cp_fsm_hook_arg_t;

/**
 * Extract session from hook arg.
 *
 */
lt_cp_session_t * _lt_cp_hook_arg_session_get(
        lt_cp_fsm_hook_arg_t * hook_arg );

/**
 * Extract message from hook arg
 */
const lt_cp_message_t * _lt_cp_hook_arg_message_get(
        lt_cp_fsm_hook_arg_t * hook_arg );

/**
 * Message related routines.
 */

/**
 * Return type of the message.
 */
lt_cp_message_type_e _lt_cp_message_type_get(
        const lt_cp_message_t * msg );

/**
 * Return id of the message.
 */
uint32_t _lt_cp_message_id_get(
        const lt_cp_message_t * msg );

/**
 * Return payload of the message.
 */
const lt_io_value_t * _lt_cp_message_payload_get(
        const lt_cp_message_t * msg );


/*
 * INTERNAL STRUCTURES, VARIABLES AND METHODS
 */

/**
 * Global APR Pool used by CP Library.
 */
apr_pool_t * _lt_cp_pool = NULL;

/*
 * Type of a message payload.
 */
typedef enum {
    /**
     * Payload is encoded as a BER SEQUENCE with tag 0x16 - constructed
     * finite-length sequence.
     */
    LT_MESSAGE_PAYLOAD_BER_SEQUENCE   =   0x0001,

    /**
     * Payload is encoded as a Simplified BER SEQUENCE.
     * see io.h
     */
    LT_MESSAGE_PAYLOAD_SBER_SEQUENCE  =   0x0002

} lt_cp_message_payload_type_e;

/**
 * INTERNAL: Create empty message without payload, if id is zero, random one
 * will be generated. FIXME: for now id is always 0.
 */
lt_cp_message_t * _lt_cp_message_create(
        lt_cp_message_type_e type,
        uint32_t id,
        const lt_io_value_t * payload,
        apr_pool_t * pool );

/**
 * INTERNAL: Enqueue the message to the session's send_queue.
 */
lt_cp_status_t _lt_cp_enqueue_msg(
        lt_cp_session_t * session,
        const lt_cp_message_t * msg );

/**
 * INTERNAL: Dequeue the message from the session's send_queue.
 */
const lt_cp_message_t * _lt_cp_dequeue_msg(
        lt_cp_session_t * session );

/**
 * INTERNAL: Encode the message to the session's send_buffer. Should always
 * return success, since automatic resizing of send_buffer will be
 * performed.
 */
void _lt_cp_encode_msg(
        lt_cp_session_t * session,
        const lt_cp_message_t * msg );

/**
 * INTERNAL: Decode the message from the session's receive_buffer.
 * returns 0 when no more messages can be decoded. the returned message will
 * be allocated out of session's recv_pool.
 */
lt_cp_message_t * _lt_cp_decode_msg(
        lt_cp_session_t * session );

/**
 * INTERNAL: Dump message to stderr
 */
void _lt_cp_dump_msg(
        const lt_cp_message_t * msg );

/**
 * INTERNAL: Dump variable to stderr
 */
void _lt_cp_dump_var(
        const lt_io_value_t * name, const lt_io_value_t * value );

/**
 * INTERNAL: client specific process routine.
 */
lt_cp_status_t _lt_cp_session_process_client(
        lt_cp_session_t * session );

/**
 * INTERNAL: server specific process routine.
 */
lt_cp_status_t _lt_cp_session_process_server(
        lt_cp_session_t * session );

/*
 * Finite state machine representing our protocol.
 */
lt_fsm_t * _lt_cp_fsm = NULL;
lt_fsm_t * _lt_cp_fsm_create( apr_pool_t * pool );

/*
 * Array indexed by lt_cp_message_type_e and containing one-byte value
 * used as input for transitions.
 */
lt_cp_message_type_e
_lt_cp_fsm_transition_inputs [ LT_CP_TOTAL_MESSAGE_TYPES ];

/*
 * length of a transition input
 */
#define LT_CP_FSM_TRANSITION_INPUT_LEN \
    ( sizeof( lt_cp_message_type_e ) )

/*
 * transition input for given message type
 */
#define LT_CP_FSM_TRANSITION_INPUT(msg_type) \
    ( &_lt_cp_fsm_transition_inputs [ msg_type ] )

/*
 * SERVER AND CLIENT specific post-hooks that either
 * change the state of cp_session or simply enqueue response.
 */

/*
 * server side responses that the client will react to
 */
void _lt_cp_fsm_post_hook_after_connection_refused( void * arg );
void _lt_cp_fsm_post_hook_after_project_url( void * arg );
void _lt_cp_fsm_post_hook_after_view_not_selected( void * arg );
void _lt_cp_fsm_post_hook_after_available_variables( void * arg );
void _lt_cp_fsm_post_hook_after_variable_values( void * arg );
void _lt_cp_fsm_post_hook_after_monitoring_variables( void * arg );

/*
 * client side requests that the server will react to
 */
void _lt_cp_fsm_post_hook_after_connect( void * arg );
void _lt_cp_fsm_post_hook_after_disconnect( void * arg );
void _lt_cp_fsm_post_hook_after_select_view( void * arg );
void _lt_cp_fsm_post_hook_after_unselect_view( void * arg );
void _lt_cp_fsm_post_hook_after_modify_variables( void * arg );
void _lt_cp_fsm_post_hook_after_read_variables( void * arg );
void _lt_cp_fsm_post_hook_after_monitor_variables( void * arg );
void _lt_cp_fsm_post_hook_after_unmonitor_variables( void * arg );

/*
 * ************************************************************************
 */

/*
 * FUCTION BODIES
 */
lt_cp_status_t lt_cp_initialize(void) {

    /* FIXME: return some menaingfull errorcode here */

    /* create global allocation pool */
    if( APR_SUCCESS !=
                apr_pool_create( &_lt_cp_pool, NULL ) ) {
        return LT_CP_INVALID_ARG;
    }

    /* initialize transition inputs with proper values */
    {
        unsigned i = 0;
        for( i = 0; i < LT_CP_TOTAL_MESSAGE_TYPES; i ++ ) {
            _lt_cp_fsm_transition_inputs [ i ] =
                ( lt_cp_message_type_e ) i;
        }
    }

    _lt_cp_fsm = _lt_cp_fsm_create( _lt_cp_pool );

    /* FIXME: return some menaingfull errorcode here */

    if( _lt_cp_fsm == NULL ) return LT_CP_INVALID_ARG;

    return LT_CP_SUCCESS;
}

void lt_cp_terminate(void) {
    /* destroy global allocation pool */
    apr_pool_destroy( _lt_cp_pool );
}

apr_pool_t * lt_cp_session_pool_get( const lt_cp_session_t * session ) {
    if( session == NULL ) return NULL;
    return session->pool;
}

#define CREATE_POOL_IF_NULL(p)  if((p)==NULL)apr_pool_create(&(p),NULL)

lt_cp_status_t lt_cp_session_create( lt_cp_session_t ** session,
        lt_cp_session_mode_e mode,
        apr_pool_t * pool ) {

    CREATE_POOL_IF_NULL( pool );

    *session = ( lt_cp_session_t * ) apr_pcalloc( pool,
            sizeof( lt_cp_session_t ) );

    if( *session == NULL ) return LT_CP_INVALID_ARG;

    ( *session )->pool = pool;
    ( *session )->mode = mode;

    ( *session )->project_url = NULL;

    /**
     * We put the session here automagically to the state
     * CONNECTION_IN_PROGRESS, even if we can not be sure that the client
     * will really send connect,
     *
     * but anyway, tcp/ip connection is already ready, so we are in fact in
     * progress.
     */
    ( *session )->state = LT_CP_STATE_CONNECTION_IN_PROGRESS;

    lt_fsm_session_create( &( *session )->fsm_session,
            _lt_cp_fsm, pool );

    lt_fsm_session_state_set( ( *session )->fsm_session,
            "disconnected" );

    /* create pool to allocate duplicates of variables when
     * register_variables is called */
    apr_pool_create( &( *session )->url_pool, pool );

    /* create pool to allocate duplicates of variables when
     * register_variables is called */
    apr_pool_create( &( *session )->conn_pool, pool );

    /* allocate hash table that stores variables */
    ( *session )->connections = apr_hash_make( ( *session )->conn_pool );
    ( *session )->nconnections = 0;
    ( *session )->nvariables = 0;

    /* allocate pools for receive/send buffer and receive/send queue */
    apr_pool_create( &( *session )->send_pool, pool );
    apr_pool_create( &( *session )->recv_pool, pool );

    /* send_queue will be automatically created by enqueue_msg */
    ( *session )->send_queue = NULL;

    apr_pool_create( &( *session )->send_buffer_pool, pool );
    apr_pool_create( &( *session )->receive_buffer_pool, pool );

    /* allocate receive_buffer, start at 5 times max message len */
    ( *session )->receive_buffer_offset = 0;
    ( *session )->receive_buffer_length = 5 * LT_CP_MESSAGE_LENGTH_MAX;
    ( *session )->receive_buffer = apr_palloc(
        ( *session )->receive_buffer_pool,
        ( *session )->receive_buffer_length );

    /* allocate send_buffer, start at 5 times max message len */
    ( *session )->send_buffer_offset = 0;
    ( *session )->send_buffer_length = 5 * LT_CP_MESSAGE_LENGTH_MAX;
    ( *session )->send_buffer = apr_palloc(
        ( *session )->send_buffer_pool,
        ( *session )->send_buffer_length );


    return LT_CP_SUCCESS;
}

lt_cp_status_t lt_cp_session_process( lt_cp_session_t * session,
        const void * rbuffer, apr_size_t rbufferlen,
        void ** sbuffer, apr_size_t * sbufferlen ) {

    lt_cp_status_t cprv = LT_CP_SUCCESS;
    lt_fsm_status_t fsmrv = LT_FSM_SUCCESS;

    if( session == NULL ) return LT_CP_INVALID_ARG;

#ifdef APR_POOL_DEBUG
#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf( stderr,
            "debug: session: %d, send: %d, recv: %d, vars: %d, url: %d,"
            " recvb: %d, sendb: %d\n",
            apr_pool_num_bytes( session->pool, 1 ),
            apr_pool_num_bytes( session->send_pool, 1 ),
            apr_pool_num_bytes( session->recv_pool, 1 ),
            apr_pool_num_bytes( session->conn_pool, 1 ),
            apr_pool_num_bytes( session->url_pool, 1 ),
            apr_pool_num_bytes( session->receive_buffer_pool, 1 ),
            apr_pool_num_bytes( session->send_buffer_pool, 1 ) );
#endif
#endif

    /* nullify the output params */
    if( sbuffer && sbufferlen ) {
        *sbuffer = NULL;
        *sbufferlen = 0;
    }

    /* the first thing to do here is to append rbuffer to our
     * internal receive_buffer reallocing it if necessary
     */
    if( rbuffer && rbufferlen > 0 &&
            ( session->receive_buffer_offset + rbufferlen >=
              session->receive_buffer_length ) ) {
        /* FIXME: reallocate receive_buffer while increasing its size */
        fprintf(stderr, "no space in receive buffer, have %d, need %ld\n",
                session->receive_buffer_length - session->receive_buffer_offset,
                rbufferlen );
        return LT_CP_INVALID_ARG;
    }
    if( rbuffer && rbufferlen > 0 ) {
        memcpy( (char *)session->receive_buffer + session->receive_buffer_offset,
                rbuffer, rbufferlen );
        session->receive_buffer_offset += rbufferlen;
    }

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf(stderr,
            "lt_cp_session_process: going to examine what we received\n");
#endif

    /* then we can check whether we are able to decode one or more
     * messages from receive_buffer */
    {
        lt_cp_message_t * msg = NULL;
        while( ( msg = _lt_cp_decode_msg( session ) ) != NULL ) {
            /* decode & check */

            /* check transition diagram that we are actually in a
             * state where we can handle received message */

            lt_cp_fsm_hook_arg_t ha;

            ha.session = session;
            ha.message = msg;

#ifdef FLOOD_CONSOLE_WITH_DEBUG
            fprintf( stderr, "%s: received 0x%x\n",
                    ( session->mode == LT_CP_CLIENT ? "CLIENT" : "SERVER" ),
                    _lt_cp_message_type_get( msg ) );
            fprintf( stderr, "%s: instate %s\n",
                    ( session->mode == LT_CP_CLIENT ? "CLIENT" : "SERVER" ),
                    lt_fsm_session_state_get( session->fsm_session ) );
#endif

            fsmrv = lt_fsm_session_process( session->fsm_session,
                    LT_CP_FSM_TRANSITION_INPUT(
                        _lt_cp_message_type_get( msg ) ),
                    LT_CP_FSM_TRANSITION_INPUT_LEN, &ha );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
            fprintf( stderr, "%s: after process instate %s\n",
                    ( session->mode == LT_CP_CLIENT ? "CLIENT" : "SERVER" ),
                    lt_fsm_session_state_get( session->fsm_session ) );
#endif

            /* LT_FSM_ENOSUCHTRANSITION: we can't receive given message,
             * protocol error, bail out */
            if( fsmrv == LT_FSM_ENOSUCHTRANSITION ) {
                fprintf( stderr, "%s: received invalid message 0x%x"
                        " in state %s\n",
                    ( session->mode == LT_CP_CLIENT ? "CLIENT" : "SERVER" ),
                    msg->header.message_type,
                    lt_fsm_session_state_get( session->fsm_session )
                        );
                return LT_CP_INVALID_ARG;
            }
        }
    }

    /* clear pool holding decoded message which has already been processed
     * */
    apr_pool_clear( session->recv_pool );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf(stderr,
            "lt_cp_session_process: going to check dirty vars\n");
#endif

    /* then we call process_server and process_client to examine
     * which values are dirty and compose
     * monitor, unmonitor, variable_values, etc.
     *
     * but we do this only when in view_selected state
     */
    if( lt_cp_session_is_view_selected( session ) ) {
        switch( lt_cp_session_mode_get( session ) ) {
            case LT_CP_SERVER:
                cprv = _lt_cp_session_process_server( session );
                break;
            case LT_CP_CLIENT:
                cprv = _lt_cp_session_process_client( session );
                break;
            default:
                return LT_CP_INVALID_ARG;
        }
    }

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf(stderr,
            "lt_cp_session_process: going to send what we have in queue\n");
#endif

    /* then for every enqueued message we encode it into the internal
     * send_buffer resizing it if necessary
     */
    {
        const lt_cp_message_t * msg = NULL;
        while( ( msg = _lt_cp_dequeue_msg( session ) ) != NULL ) {

            /* check transition diagram that we are actually in a
             * state where we can send the given message */
            /* dequee & encode to send_buffer */

            lt_cp_fsm_hook_arg_t ha;

            ha.session = session;
            ha.message = msg;

#ifdef FLOOD_CONSOLE_WITH_DEBUG
            fprintf( stderr, "%s: sending 0x%x\n",
                    ( session->mode == LT_CP_CLIENT ? "CLIENT" : "SERVER" ),
                    _lt_cp_message_type_get( msg ) );
            fprintf( stderr, "%s: instate %s\n",
                    ( session->mode == LT_CP_CLIENT ? "CLIENT" : "SERVER" ),
                    lt_fsm_session_state_get( session->fsm_session ) );
#endif

            fsmrv = lt_fsm_session_process( session->fsm_session,
                    LT_CP_FSM_TRANSITION_INPUT(
                        _lt_cp_message_type_get( msg ) ),
                    LT_CP_FSM_TRANSITION_INPUT_LEN, &ha );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
            fprintf( stderr, "%s: after process instate %s\n",
                    ( session->mode == LT_CP_CLIENT ? "CLIENT" : "SERVER" ),
                    lt_fsm_session_state_get( session->fsm_session ) );
#endif

            /* LT_FSM_ENOSUCHTRANSITION: we can't send requested message,
             * protocol error, bail out */
            if( fsmrv == LT_FSM_ENOSUCHTRANSITION ) {
                fprintf( stderr, "%s: tried to send invalid message 0x%x "
                        "in state %s\n",
                    ( session->mode == LT_CP_CLIENT ? "CLIENT" : "SERVER" ),
                    msg->header.message_type,
                    lt_fsm_session_state_get( session->fsm_session )
                        );
                return LT_CP_INVALID_ARG;
            }

            _lt_cp_encode_msg( session, msg );
        }
        /* clear send pool which was holding messages that we just encoded
         * to the send_buffer */
        apr_pool_clear( session->send_pool );
        session->send_queue = NULL;
    }

    /* then we simply return our internal send_buffer pointer and
     * sbuffer len sent to total number of octets encoded and clear our
     * pointers. when used did pass null in, we buffer it for him for the
     * next time.
     */
    if( sbuffer && sbufferlen ) {
        *sbuffer = session->send_buffer;
        *sbufferlen = session->send_buffer_offset;
        /* clear our offset in a send buffer */
        session->send_buffer_offset = 0;
    }

    return LT_CP_SUCCESS;
}

lt_cp_status_t lt_cp_session_destroy( lt_cp_session_t * session ) {

    if( session == NULL ) return LT_CP_INVALID_ARG;

    /* walk through variables and set source of all varsets to NULL */
    {
        apr_pool_t * pool;
        apr_hash_index_t * hi = NULL;
        const char * connkey = NULL;
        lt_var_set_t * vset = NULL;

        apr_pool_create( &pool, session->conn_pool );

        for( hi = apr_hash_first( pool, session->connections ); hi != NULL;
                hi = apr_hash_next( hi ) ) {

            apr_hash_this( hi, ( const void ** ) &connkey, NULL,
                    ( void ** ) &vset );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
            fprintf( stderr, "lt_cp_session_destroy:"
                    " disconnecting connction %s\n",
                    connkey );
#endif
            {
                lt_var_set_t * orig_source =
                    lt_var_set_source_get( vset );

                if( orig_source != NULL ) {
                    lt_var_set_real_values_lock( orig_source );
                }
                lt_var_set_real_values_lock( vset );
                if( orig_source != NULL ) {
                    lt_var_set_requested_values_lock( orig_source );
                }
                lt_var_set_requested_values_lock( vset );

                lt_var_set_source_set( vset, NULL );

                lt_var_set_requested_values_unlock( vset );
                if( orig_source != NULL ) {
                    lt_var_set_requested_values_unlock( orig_source );
                }
                lt_var_set_real_values_unlock( vset );
                if( orig_source != NULL ) {
                    lt_var_set_real_values_unlock( orig_source );
                }
            }
        }

        apr_pool_destroy( pool );

    }

    /* clear pool that holds variables and recreate hash table */
    apr_pool_clear( session->conn_pool );
    session->connections = apr_hash_make( session->conn_pool );

    /* clear pool that holds url */
    apr_pool_clear( session->url_pool );
    session->project_url = NULL;

    /* FIXME: shrink recv_buffer and send_buffer to the original size */
    session->send_buffer_offset = 0;
    session->receive_buffer_offset = 0;

    /* clear send_pool */
    apr_pool_clear( session->send_pool );
    session->send_queue = NULL;

    /* clear recv_pool */
    apr_pool_clear( session->recv_pool );

    /* reset diagrams to the first state */
    lt_fsm_session_state_set( session->fsm_session,
            "disconnected" );

    /* reset state and protocol to neutral */

    session->state = LT_CP_STATE_CONNECTION_IN_PROGRESS;

    return LT_CP_SUCCESS;
}

lt_cp_session_mode_e lt_cp_session_mode_get(
        const lt_cp_session_t * session ) {
    if( session == NULL ) return LT_CP_SERVER;
    return session->mode;
}

lt_cp_connection_state_e lt_cp_session_state_get(
        const lt_cp_session_t * session ) {
    if( session == NULL ) return LT_CP_STATE_DISCONNECTED;
    return session->state;
}

int lt_cp_session_is_inprogress( const lt_cp_session_t * session ) {

    if( session == NULL ) return 0;

    if( lt_cp_session_state_get( session ) ==
            LT_CP_STATE_CONNECTION_IN_PROGRESS ) {

        return 1;
    }

    return 0;
}

int lt_cp_session_is_alive( const lt_cp_session_t * session ) {

    if( session == NULL ) return 0;

    if( lt_cp_session_state_get( session ) >= LT_CP_STATE_CONNECTED ) {

        return 1;
    }

    return 0;
}

int lt_cp_session_is_view_selection_inprogress(
        const lt_cp_session_t * session ) {

    if( session == NULL ) return 0;

    if( ( lt_cp_session_state_get( session ) ==
                LT_CP_STATE_VIEW_SELECTION_IN_PROGRESS ) ) {

        return 1;
    }

    return 0;
}

int lt_cp_session_is_view_selected( const lt_cp_session_t * session ) {

    if( session == NULL ) return 0;

    if( lt_cp_session_state_get( session ) >=
            LT_CP_STATE_VIEW_SELECTED ) {

        return 1;
    }

    return 0;
}

lt_cp_status_t lt_cp_session_variables_register(
        lt_cp_session_t * session,
        const char * connection_name,
        lt_var_set_t * vars ) {

    lt_var_set_t * newvars;

    if( session == NULL ) return LT_CP_INVALID_ARG;
    if( connection_name == NULL ) return LT_CP_INVALID_ARG;
    if( vars == NULL ) return LT_CP_INVALID_ARG;

    /* create new varset out of our variable pool */
    newvars = lt_var_set_create( 0, session->conn_pool );
    if( newvars == NULL ) return LT_CP_INVALID_ARG;

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf( stderr, "lt_cp_session_variables_register: registering "
            "variables for connection %s\n", connection_name );
#endif

    /* for each variable within vars, do create new variable and
     * register it under the same name to our varset. */
    {
        apr_pool_t * pool = NULL;
        apr_hash_index_t * hi = NULL;
        const char * key;
        lt_var_t * variable;
        lt_var_t * newvar;

        apr_pool_create( &pool, session->conn_pool );

        for( hi = apr_hash_first( pool, lt_var_set_variables_get( vars ) );
                hi != NULL; hi = apr_hash_next( hi ) ) {

            apr_hash_this( hi, ( const void ** ) &key, NULL,
                    ( void ** ) &variable );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
            fprintf( stderr, "lt_cp_session_variables_register: "
                    "duplicating variable %s\n",
                    key );
#endif

            newvar = lt_var_create( lt_var_type_get( variable ), 1,
                    session->conn_pool );

            // copy real & requested value from origin
            lt_var_real_value_set( newvar,
                    lt_var_real_value_get( variable ) );
            lt_var_requested_value_set( newvar,
                    lt_var_requested_value_get( variable ) );

            lt_var_set_variable_register( newvars, key, newvar );

            session->nvariables ++;
        }

        session->nconnections ++;

        apr_pool_destroy( pool );
    }

    /* if on the server, vars will be source for our new varset. */
    /* on the client, our new varset will be the source for given varset */
    switch( lt_cp_session_mode_get( session ) ) {
        case LT_CP_SERVER:
            /* lock both varsets before setting source */
            /* fprintf(stderr,"locking...\n"); */
            lt_var_set_real_values_lock( newvars );
            lt_var_set_real_values_lock( vars );
            lt_var_set_requested_values_lock( newvars );
            lt_var_set_requested_values_lock( vars );
            /* fprintf(stderr,"locked...\n"); */

            lt_var_set_source_set( newvars, vars );

            /* unlock both varsets after setting source */
            /* fprintf(stderr,"unlocking...\n"); */
            lt_var_set_requested_values_unlock( vars );
            lt_var_set_requested_values_unlock( newvars );
            lt_var_set_real_values_unlock( vars );
            lt_var_set_real_values_unlock( newvars );
            /* fprintf(stderr,"unlocked...\n"); */
            break;
        case LT_CP_CLIENT:
            /* lock both varsets before setting source */
            /* fprintf(stderr,"locking...\n"); */
            lt_var_set_real_values_lock( newvars );
            lt_var_set_real_values_lock( vars );
            lt_var_set_requested_values_lock( newvars );
            lt_var_set_requested_values_lock( vars );
            /* fprintf(stderr,"locked...\n"); */

            lt_var_set_source_set( vars, newvars );

            /* unlock both varsets after setting source */
            /* fprintf(stderr,"unlocking...\n"); */
            lt_var_set_requested_values_unlock( vars );
            lt_var_set_requested_values_unlock( newvars );
            lt_var_set_real_values_unlock( vars );
            lt_var_set_real_values_unlock( newvars );
            /* fprintf(stderr,"unlocked...\n"); */
            break;
    }
    {
        char * name = apr_pstrdup( session->conn_pool, connection_name );

        /* and put the newly created varset into session->connections under
         * the key connection_name */
        apr_hash_set( session->connections,
                name, APR_HASH_KEY_STRING,
                ( void * ) newvars );
    }

    return LT_CP_SUCCESS;
}

const char * lt_cp_session_project_url_get(
        lt_cp_session_t * session ) {
    if( session == NULL ) return NULL;
    return session->project_url;
}

lt_cp_status_t lt_cp_session_project_url_set(
        lt_cp_session_t * session,
        const char * project_url ) {

    if( session == NULL) return LT_CP_INVALID_ARG;

    /* clear old url */
    apr_pool_clear( session->url_pool );

    /* dup the given string */
    session->project_url = apr_pstrdup( session->url_pool, project_url );

    return LT_CP_SUCCESS;
}

lt_cp_status_t _lt_cp_session_process_client(
        lt_cp_session_t * session ) {

    apr_pool_t * pool;
    apr_hash_index_t * hi = NULL;
    const char * connkey = NULL;
    lt_var_set_t * vset = NULL;

    assert( session != NULL );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf( stderr, "_lt_cp_session_process_client\n" );
#endif

    /* examine state of variables and which requested values and
     * monitors are dirty and sent them across the wire */

    apr_pool_create( &pool, session->conn_pool );

    for( hi = apr_hash_first( pool, session->connections ); hi != NULL;
            hi = apr_hash_next( hi ) ) {

        apr_hash_this( hi, ( const void ** ) &connkey, NULL,
                ( void ** ) &vset );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
        fprintf( stderr, "_lt_cp_session_process_client:"
                " examining dirty requested values of connction %s\n",
                connkey );
#endif
        {
            /* FIXME: process requested monitors */
            /* FIXME: and then walk only through monitored variables */

            /* FIXME: rework this into
             * _encode_variables_to_payload( real_or_requested,
             * hash_with_vars )
             */
            apr_hash_t * dirty = NULL;
            apr_hash_index_t * i = NULL;
            unsigned ndirty = 0;

            /* {{{ */
            lt_var_set_requested_values_lock( vset );
            dirty = lt_var_set_dirty_requested_values_get( vset );

            /* FIXME: do not send more than 100 dirty values within one
             * message */

            if( ( ndirty = apr_hash_count( dirty ) ) > 0 ) {

                int idx = 0;
                const char * key = NULL;
                lt_var_t * variable = NULL;
                lt_io_value_t * vars_with_values =
                    lt_io_value_sequence_create( ndirty * 2,
                            session->send_pool );

                for( i = apr_hash_first( pool, dirty );
                        i != NULL; i = apr_hash_next( i ), idx += 2 ) {

                    char varkey[2048] = "";

                    apr_hash_this( i, ( const void ** ) &key, NULL,
                            ( void ** ) &variable );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                    fprintf( stderr, "_lt_cp_session_process_client:"
                            " seems like %s is dirty\n",
                        key );

                    _lt_cp_dump_var( NULL,
                            lt_var_requested_value_get(variable) );
                    _lt_cp_dump_var( NULL,
                            lt_var_real_value_get(variable) );
#endif

                    //FIXME: MMAN: here key must be connkey + key
                    //make this algo safe
                    strncpy( varkey, connkey, 2048 );
                    strncat( varkey, ".", 2048 - strlen( connkey ) - 1 );
                    strncat( varkey, key, 2048 - strlen( connkey ) - 2 );

                    lt_io_value_sequence_item_set(
                            vars_with_values, idx,
                            lt_io_value_string_create2( varkey,
                                session->send_pool ) );

                    lt_io_value_sequence_item_set(
                            vars_with_values, idx + 1,
                            lt_io_value_deepcopy(
                                lt_var_requested_value_get( variable ),
                                session->send_pool ) );
                }

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                fprintf( stderr, "_lt_cp_session_process_client:"
                        " enqueueing modify_variables for connection %s\n",
                        connkey );
#endif

                /* enqueue modify_variables */
                lt_cp_session_send_msg_modify_variables( session,
                        vars_with_values );

                /* and sync_requested to clear dirty flags */
                lt_var_set_requested_values_sync( vset, -1 );

            } else {
#ifdef FLOOD_CONSOLE_WITH_DEBUG
                fprintf( stderr, "_lt_cp_session_process_client:"
                        " no dirty requested values in connction %s\n",
                        connkey );
#endif
            }

            /* }}} */
            lt_var_set_requested_values_unlock( vset );
        }

    }

    apr_pool_destroy( pool );

    return LT_CP_SUCCESS;
}

lt_cp_status_t _lt_cp_session_process_server(
        lt_cp_session_t * session ) {

    apr_pool_t * pool;
    apr_hash_index_t * hi = NULL;
    const char * connkey = NULL;
    lt_var_set_t * vset = NULL;

    assert( session != NULL );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf( stderr, "_lt_cp_session_process_server\n" );
#endif

    /* examine state of variables and which real values and
     * monitors are dirty and sent them across the wire */

    apr_pool_create( &pool, session->conn_pool );

    for( hi = apr_hash_first( pool, session->connections ); hi != NULL;
            hi = apr_hash_next( hi ) ) {

        apr_hash_this( hi, ( const void ** ) &connkey, NULL,
                ( void ** ) &vset );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
        fprintf( stderr, "_lt_cp_session_process_server:"
                " examining dirty real values of connction %s\n",
                connkey );
#endif
        {
            /* FIXME: process real monitors */
            /* FIXME: and then walk only through monitored variables */

            /* FIXME: rework this into
             * _encode_variables_to_payload( real_or_requested,
             * hash_with_vars )
             */
            apr_hash_t * dirty = NULL;
            apr_hash_index_t * i = NULL;
            unsigned ndirty = 0;

            /* {{{ */
            lt_var_set_real_values_lock( vset );

            dirty = lt_var_set_dirty_real_values_get( vset );

            /* FIXME: do not send more than 100 dirty values within one
             * message */

            if( ( ndirty = apr_hash_count( dirty ) ) > 0 ) {

                int idx = 0;
                const char * key = NULL;
                lt_var_t * variable = NULL;
                lt_io_value_t * vars_with_values =
                    lt_io_value_sequence_create( ndirty * 2,
                            session->send_pool );

                for( i = apr_hash_first( pool, dirty );
                        i != NULL; i = apr_hash_next( i ), idx += 2 ) {

                    char varkey[2048] = "";

                    apr_hash_this( i, ( const void ** ) &key, NULL,
                            ( void ** ) &variable );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                    fprintf( stderr, "_lt_cp_session_process_server:"
                            " seems like %s(0x%x) is dirty\n",
                        key,(unsigned)variable );

                    _lt_cp_dump_var( NULL,
                            lt_var_requested_value_get(variable) );
                    _lt_cp_dump_var( NULL,
                            lt_var_real_value_get(variable) );
#endif

                    //FIXME: MMAN: here key must be connkey + key
                    //make this algo safe
                    strncpy( varkey, connkey, 2048 );
                    strncat( varkey, ".", 2048 - strlen( connkey ) - 1 );
                    strncat( varkey, key, 2048 - strlen( connkey ) - 2 );

                    lt_io_value_sequence_item_set(
                            vars_with_values, idx,
                            lt_io_value_string_create2( varkey,
                                session->send_pool ) );

                    lt_io_value_sequence_item_set(
                            vars_with_values, idx + 1,
                            lt_io_value_deepcopy(
                                lt_var_real_value_get( variable ),
                                session->send_pool ) );
                }

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                fprintf( stderr, "_lt_cp_session_process_server:"
                        " enqueueing variable_values for connection %s\n",
                        connkey );
#endif

                /* enqueue variable_values without passing in original
                 * request that we obviously don't know anymore */
                lt_cp_session_send_msg_variable_values( session,
                        NULL, vars_with_values );

                /* and sync_real to clear dirty flags */
                lt_var_set_real_values_sync( vset, -1 );

            } else {
#ifdef FLOOD_CONSOLE_WITH_DEBUG
                fprintf( stderr, "_lt_cp_session_process_server:"
                        " no dirty real values in connction %s\n",
                        connkey );
#endif
            }

            /* }}} */
            lt_var_set_real_values_unlock( vset );

        }

    }

    apr_pool_destroy( pool );

    return LT_CP_SUCCESS;
}

/*
 * Message related function bodies
 */
lt_cp_message_type_e _lt_cp_message_type_get(
        const lt_cp_message_t * msg ) {
    if( msg != NULL )  {
        return msg->header.message_type;
    }
    return LT_CP_MSG_INVALID;
}

uint32_t _lt_cp_message_id_get(
        const lt_cp_message_t * msg ) {
    if( msg != NULL )  {
            return msg->header.message_id;
    }
    return 0;
}

const lt_io_value_t * _lt_cp_message_payload_get(
        const lt_cp_message_t * msg ) {
    if( msg != NULL )  {
            return msg->payload;
    }
    return NULL;
}

lt_cp_message_t * _lt_cp_message_create(
        lt_cp_message_type_e type,
        uint32_t id,
        const lt_io_value_t * payload, apr_pool_t * pool ) {

    lt_cp_message_t * m = ( lt_cp_message_t * )
        apr_pcalloc( pool, sizeof( lt_cp_message_t ) );

    if( id == 0 ) {

#if 0
        /* FIXME: disable for now, since it seems to be very slow */
        apr_generate_random_bytes( ( unsigned char * ) &id, sizeof( id ) );
#endif
    }

    m->header.message_id = id;
    m->header.message_timestamp = 0x2222 /* FIXME: put timestamp here */;
    m->header.message_type = type;

    m->payload = payload;
    m->pool = pool;

    return m;
}

lt_cp_status_t _lt_cp_enqueue_msg( lt_cp_session_t * session,
        const lt_cp_message_t * msg ) {

    const lt_cp_message_t ** slot = NULL;

    if( session == NULL ) return LT_CP_INVALID_ARG;

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf(stderr, "debug: going to enqueue msg type 0x%x\n",
            msg->header.message_type );
#endif

    /* create new array with only one element allocated out of send_pool */
    {
        apr_array_header_t * new = apr_array_make( session->send_pool,
                1, sizeof( lt_cp_message_t * ) );

        slot = ( const lt_cp_message_t ** )
            apr_array_push( new );

        *slot = msg;

        if( session->send_queue != NULL ) {
            /* cat newly created array and  send_queue thus simulating append */
            session->send_queue = apr_array_append( session->send_pool,
                    new, session->send_queue );
        } else {
            session->send_queue = new;
        }

    }

    return LT_CP_SUCCESS;
}

const lt_cp_message_t * _lt_cp_dequeue_msg(
        lt_cp_session_t * session ) {

    const lt_cp_message_t ** slot = NULL;

    if( session == NULL ) return NULL;
    if( session->send_queue == NULL ) return NULL;

    slot = ( const lt_cp_message_t ** )
        apr_array_pop( session->send_queue );

    if( slot != NULL ) {
#ifdef FLOOD_CONSOLE_WITH_DEBUG
        fprintf(stderr, "debug: going to dequeue msg type 0x%x\n",
                (*slot)->header.message_type );
#endif
        return *slot;
    }

    return NULL;
}

void _lt_cp_encode_msg( lt_cp_session_t * session,
        const lt_cp_message_t * msg ) {

    /* try to encode message to a char array and see whether it fits,
     * to the send_buffer, if so, copy it there, if not, resize the
     * send_buffer and repeat.
     */

    lt_io_encode_buffer_t ebuf;
    lt_cp_message_t * emsg = NULL;
    unsigned len = 0, paylen = 0;

    assert( session != NULL );
    assert( msg != NULL );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf( stderr, "_lt_cp_encode_msg: going to encode"
            " message of type 0x%x"
            " into send_buffer\n",
            msg->header.message_type );
#endif

    /* check that our header will fit in send_queue */
    if( session->send_buffer_offset + LT_CP_MESSAGE_HEADER_LEN >=
            session->send_buffer_length ) {
        /* FIXME: here we need to reallocate the send_buffer while
         * increasing its size */
        fprintf( stderr, "_lt_cp_encode_msg: not enough space in send_buffer"
                " to encode message header\n" );
        return;
    }

    /* initialize emsg so that it will start in first free byte in a
     * send_buffer */
    emsg = (lt_cp_message_t *)
        ((char*)session->send_buffer + session->send_buffer_offset );

    /* initialize encode buffer so that it will start right behind the
     * header */
    ebuf.length = session->send_buffer_length;
    ebuf.offset = session->send_buffer_offset + LT_CP_MESSAGE_HEADER_LEN;
    ebuf.data = session->send_buffer;

    if( msg->payload != NULL ) {
#ifdef FLOOD_CONSOLE_WITH_DEBUG
        fprintf( stderr, "_lt_cp_encode_msg: going to encode payload"
                " of type=0x%x with nitems=%d\n",
                lt_io_value_type_get( msg->payload ),
                lt_io_value_sequence_length_get( msg->payload ) );
#endif
        if( lt_io_value_encode( &ebuf, msg->payload ) == -1 ) {
            fprintf( stderr, "_lt_cp_encode_msg: not enough space in "
                    "send_buffer"
                    " to encode message payload\n" );
            return;
        }
    }

    /* len should contain total encoded message len in octets */
    paylen = ebuf.offset - session->send_buffer_offset
        - LT_CP_MESSAGE_HEADER_LEN;
    len = LT_CP_MESSAGE_HEADER_LEN + paylen;

    /* encode message header, i.e., htons, htonl it */
    emsg->header.message_length = htonl( len );
    emsg->header.message_id = htonl( msg->header.message_id );
    /* FIXME: conversion from 64 bits to 32 bits and back to 64 */
    emsg->header.message_timestamp = htonl( msg->header.message_timestamp );
    emsg->header.message_type = htons( msg->header.message_type );

    emsg->payload_header.payload_length = htonl( paylen );
    emsg->payload_header.payload_type =
        htons( LT_MESSAGE_PAYLOAD_SBER_SEQUENCE );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf( stderr, "_lt_cp_encode_msg: encoded message of type 0x%x"
            " with total_length=%d into send_buffer\n",
            msg->header.message_type, len );
#endif

    /* encoded successfully, shift pointer of our send_buffer */
    session->send_buffer_offset += len;
}

lt_cp_message_t * _lt_cp_decode_msg(
        lt_cp_session_t * session ) {
    /* look into receive_buffer whether we can take a message out, if
     * so, decode it and shift the buffer to the left using memmove which
     * can do overlapping memcpy
     */

    lt_cp_message_t * emsg = NULL;
    lt_cp_message_t * msg = NULL;
    unsigned long len = 0, paylen = 0;

    lt_io_decode_buffer_t dbuf;
    lt_io_value_t * payload = NULL;

    assert( session != NULL );

    /* check whether we have at least a full header in receive_buffer */
    if( session->receive_buffer_offset < LT_CP_MESSAGE_HEADER_LEN ) {
        /* not enough data to start decoding */
#ifdef FLOOD_CONSOLE_WITH_DEBUG
        fprintf( stderr, "_lt_cp_decode_msg: not enough data in "
                "receive_buffer"
                " to decode message header\n" );
#endif
        return NULL;
    }

    emsg = ( lt_cp_message_t * ) session->receive_buffer;
    len = ntohl( emsg->header.message_length );
    paylen = ntohl( emsg->payload_header.payload_length );

    /* check whether we have whole encoded message in receive_buffer */
    if( session->receive_buffer_offset < len ) {
#ifdef FLOOD_CONSOLE_WITH_DEBUG
        fprintf( stderr, "_lt_cp_decode_msg: not enough data in "
                "receive_buffer"
                " to decoded whole message of reported len=%d\n",
                len );
#endif
        return NULL;
    }

    /* check the integrity of a message */
    if( paylen + LT_CP_MESSAGE_HEADER_LEN != len ) {
        fprintf( stderr, "_lt_cp_decode_msg: integrity error"
                " message with fixed header len %ld and"
                " reported payload len %ld should have a size %ld"
                " (%ld reported)\n",
                LT_CP_MESSAGE_HEADER_LEN, paylen,
                LT_CP_MESSAGE_HEADER_LEN + paylen, len );
        return NULL;
    }

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf( stderr, "_lt_cp_decode_msg: going to decode "
            "message of type 0x%x with reported len=%d and "
            "payload len=%d from receive_buffer\n",
            ntohs( emsg->header.message_type ), len, paylen );
#endif

    /* check the payload encoding type, pass on when unable to decode */
    if( ntohs( emsg->payload_header.payload_type ) !=
                LT_MESSAGE_PAYLOAD_SBER_SEQUENCE ) {
        fprintf( stderr, "_lt_cp_decode_msg: message payload decoding error"
                " reported payload encoding type=%d, I can decode only %d\n",
                ntohs( emsg->payload_header.payload_type ),
                LT_MESSAGE_PAYLOAD_SBER_SEQUENCE );
        payload = NULL;
    } else {
        if( paylen == 0 ) {
            /* when message has got no payload, decode dummy null payload */
            payload = lt_io_value_create( session->recv_pool );
        } else {
            /* otherwise decode from buffer */
            dbuf.length = session->receive_buffer_length;
            dbuf.offset = LT_CP_MESSAGE_HEADER_LEN;
            dbuf.data = session->receive_buffer;

            payload = lt_io_value_decode( &dbuf, session->recv_pool );

            if( payload == NULL ) {
                fprintf( stderr,
                        "_lt_cp_decode_msg: unable to decode payload\n" );
            }
        }
#ifdef FLOOD_CONSOLE_WITH_DEBUG
        fprintf( stderr, "_lt_cp_decode_msg: decoded "
                "payload of type 0x%x with nitems=%d from receive_buffer\n",
                lt_io_value_type_get( payload ),
                lt_io_value_sequence_length_get( payload ) );
#endif
    }

    msg = _lt_cp_message_create(
            ntohs( emsg->header.message_type ),
            ntohl( emsg->header.message_id ),
            payload, session->recv_pool );

    /* FIXME: this does not look that nice, moreover it generates warning. */
    /* FIXME: conversion from 64 bits to 32 bits and back to 64 */
    msg->header.message_timestamp =
        ntohl( emsg->header.message_timestamp );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
    fprintf( stderr, "_lt_cp_decode_msg: decoded "
            "message of type 0x%x from receive_buffer\n",
            msg->header.message_type );
#endif

    /* and now shift receive_buffer to the left */
    memmove( session->receive_buffer,
            (char *)session->receive_buffer + len,
            session->receive_buffer_offset - len );
    session->receive_buffer_offset -= len;

    return msg;
}

void _lt_cp_dump_msg( const lt_cp_message_t * msg ) {
    fprintf( stderr, "Message dump: " );
    if( msg == NULL ) {
        fprintf( stderr, "NULL\n" );
        return;
    }
    fprintf( stderr, "type 0x%x\n", msg->header.message_type );
    fprintf( stderr, "message_length 0x%x\n",
            msg->header.message_length );
    fprintf( stderr, "message_id 0x%x\n", msg->header.message_id );
    fprintf( stderr, "message_timestamp 0x%x\n",
            (unsigned)msg->header.message_timestamp );
    fprintf( stderr, "payload_length 0x%x\n",
            msg->payload_header.payload_length );
    fprintf( stderr, "payload_type 0x%x\n",
            msg->payload_header.payload_type );
    if( msg->payload != NULL ) {
        fprintf( stderr, "payload_items 0x%x\n",
                lt_io_value_sequence_length_get(
                    msg->payload ) );
    }
}

void _lt_cp_dump_var(
        const lt_io_value_t * name, const lt_io_value_t * value ) {

    fprintf( stderr, "Variable dump: name=" );

    if( name == NULL ) {
        fprintf( stderr, "NULL, " );
    } else {
        fprintf( stderr, "(type=%d,value=%s), ",
                lt_io_value_type_get( name ),
                lt_io_value_2_strkey( name ) );
    }

    if( value == NULL ) {
        fprintf( stderr, "NULL, " );
    } else {
        fprintf( stderr, "(type=%d,value=%s), ",
                lt_io_value_type_get( value ),
                lt_io_value_2_strkey( value ) );
    }
    fprintf( stderr, "\n" );
}

/*
 * Some more macros to save some typing
 */

#ifdef CHECK_THAT_I_AM_SERVER
#undef CHECK_THAT_I_AM_SERVER
#endif
#define CHECK_THAT_I_AM_SERVER(session) \
    if( (session) == NULL || \
            lt_cp_session_mode_get( (session) ) != LT_CP_SERVER ) { \
        return LT_CP_INVALID_ARG; \
    }

#ifdef CHECK_THAT_I_AM_CLIENT
#undef CHECK_THAT_I_AM_CLIENT
#endif
#define CHECK_THAT_I_AM_CLIENT(session) \
    if( (session) == NULL || \
            lt_cp_session_mode_get( (session) ) != LT_CP_CLIENT ) { \
        return LT_CP_INVALID_ARG; \
    }

#ifdef PREPARE_RESPONSE_ID
#undef PREPARE_RESPONSE_ID
#endif
#define PREPARE_RESPONSE_ID(name,request) \
    uint32_t name = 0; \
    if( (request) != NULL ) { \
        name = ntohl( request->header.message_id ); \
    }

/*
 * SERVER SPECIFIC METHODS
 */

lt_cp_status_t lt_cp_session_send_msg_project_url(
        lt_cp_session_t * session,
        const lt_cp_message_t * in_response_to ) {

    lt_cp_message_t * msg;
    lt_io_value_t * payload = NULL;

    PREPARE_RESPONSE_ID( rid, in_response_to );

    CHECK_THAT_I_AM_SERVER( session );

    /* encode project url as the only parameter of this message */
    payload = lt_io_value_sequence_create( 1, session->send_pool );
    lt_io_value_sequence_item_set( payload, 0,
            lt_io_value_string_create2(
                session->project_url, session->send_pool ) );

    msg = _lt_cp_message_create( LT_CP_MSG_PROJECT_URL, rid,
            payload,
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_connection_refused(
        lt_cp_session_t * session,
        const lt_cp_message_t * in_response_to,
        const char * reason ) {

    lt_cp_message_t * msg;
    lt_io_value_t * payload = NULL;

    PREPARE_RESPONSE_ID( rid, in_response_to );

    CHECK_THAT_I_AM_SERVER( session );

    /* encode reason as the only parameter of this message */
    payload = lt_io_value_sequence_create( 1, session->send_pool );
    lt_io_value_sequence_item_set( payload, 0,
            lt_io_value_string_create2(
                reason, session->send_pool ) );

    msg = _lt_cp_message_create( LT_CP_MSG_CONNECTION_REFUSED, rid,
            payload,
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_avaiable_variables(
        lt_cp_session_t * session,
        const lt_cp_message_t * in_response_to ) {

    lt_cp_message_t * msg;
    lt_io_value_t * payload = NULL;
    unsigned i = 0;

    PREPARE_RESPONSE_ID( rid, in_response_to );

    CHECK_THAT_I_AM_SERVER( session );

    /* we have session->nvariables variables, let's create sequence for them
     */
    payload = lt_io_value_sequence_create( session->nvariables * 2,
            session->send_pool );


    /* now walk through all connections */
    {
        apr_pool_t * pool;
        apr_hash_index_t * hi = NULL;
        lt_var_set_t * vars = NULL;
        apr_hash_index_t * hv = NULL;
        const char * connname = NULL;
        lt_var_t * var = NULL;
        const char * varname = NULL;

        apr_pool_create( &pool, session->send_pool );

        for( hi = apr_hash_first( pool, session->connections ); hi != NULL;
                hi = apr_hash_next( hi ) ) {

            apr_hash_this( hi, ( const void ** ) &connname, NULL,
                    ( void** ) &vars );

            /* and for every variable in a connection */
            for( hv = apr_hash_first( pool,
                        lt_var_set_variables_get( vars ) ); hv != NULL;
                    hv = apr_hash_next( hv ), i += 2 ) {

                char varkey[2048] = "";

                apr_hash_this( hv, ( const void** ) &varname, NULL,
                        ( void ** ) &var );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                fprintf( stderr, "got var %s in connection %s\n",
                        varname, connname );
#endif

                /* prepare variable name to be sent connection + . +
                 * variable */
                strncpy( varkey, connname, 2048 );
                strncat( varkey, ".", 2048 );
                strncat( varkey, varname, 2048 );

                lt_io_value_sequence_item_set( payload, i,
                        lt_io_value_string_create2( varkey,
                            session->send_pool ) );
                lt_io_value_sequence_item_set( payload, i + 1,
                        lt_io_value_deepcopy(
                            lt_var_real_value_get( var ),
                            session->send_pool ) );
            }
        }

        apr_pool_clear( pool );
    }

    msg = _lt_cp_message_create( LT_CP_MSG_AVAILABLE_VARIABLES, rid,
            payload,
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_view_not_selected(
        lt_cp_session_t * session,
        const lt_cp_message_t * in_response_to,
        const char * reason ) {

    lt_cp_message_t * msg;
    lt_io_value_t * payload = NULL;

    PREPARE_RESPONSE_ID( rid, in_response_to );

    CHECK_THAT_I_AM_SERVER( session );

    /* encode reason as the only parameter of this message */
    payload = lt_io_value_sequence_create( 1, session->send_pool );
    lt_io_value_sequence_item_set( payload, 0,
            lt_io_value_string_create2(
                reason, session->send_pool ) );

    msg = _lt_cp_message_create( LT_CP_MSG_VIEW_NOT_SELECTED,
            rid, payload,
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_monitoring_variables(
        lt_cp_session_t * session,
        const lt_cp_message_t * in_response_to,
        const lt_io_value_t * variables ) {

    lt_cp_message_t * msg;

    PREPARE_RESPONSE_ID( rid, in_response_to );

    CHECK_THAT_I_AM_SERVER( session );

    /* FIXME: check from which pool comes lt_io_value_t arg and possibly
     * don't do deepcopy here */
    msg = _lt_cp_message_create( LT_CP_MSG_MONITORING_VARIABLES,
            rid, lt_io_value_deepcopy( variables,
                session->send_pool ),
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_variable_values(
        lt_cp_session_t * session,
        const lt_cp_message_t * in_response_to,
        const lt_io_value_t * variables_with_values ) {

    lt_cp_message_t * msg;

    PREPARE_RESPONSE_ID( rid, in_response_to );

    CHECK_THAT_I_AM_SERVER( session );

    msg = _lt_cp_message_create( LT_CP_MSG_VARIABLE_VALUES,
            rid, lt_io_value_deepcopy( variables_with_values,
                session->send_pool ),
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

/*
 * CLIENT SPECIFIC METHODS
 */

lt_cp_status_t lt_cp_session_send_msg_connect(
        lt_cp_session_t * session )
{
    lt_cp_message_t * msg;
    lt_io_value_t * payload;

    CHECK_THAT_I_AM_CLIENT( session );

    payload = lt_io_value_sequence_create( 1, session->send_pool );
    lt_io_value_sequence_item_set( payload, 0,
            lt_io_value_number_create( 0, session->send_pool ) );

    msg = _lt_cp_message_create( LT_CP_MSG_CONNECT, 0, payload,
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_disconnect(
        lt_cp_session_t * session ) {

    lt_cp_message_t * msg;

    CHECK_THAT_I_AM_CLIENT( session );

    msg = _lt_cp_message_create( LT_CP_MSG_DISCONNECT, 0, NULL,
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_select_view(
        lt_cp_session_t * session,
        const char * view ) {

    lt_cp_message_t * msg = NULL;
    lt_io_value_t * payload = NULL;

    CHECK_THAT_I_AM_CLIENT( session );

    payload = lt_io_value_sequence_create( 1, session->send_pool );
    lt_io_value_sequence_item_set( payload, 0,
            lt_io_value_string_create2( view, session->send_pool ) );

    msg = _lt_cp_message_create( LT_CP_MSG_SELECT_VIEW, 0, payload,
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_unselect_view(
        lt_cp_session_t * session ) {

    lt_cp_message_t * msg;

    CHECK_THAT_I_AM_CLIENT( session );

    msg = _lt_cp_message_create( LT_CP_MSG_UNSELECT_VIEW, 0, NULL,
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_monitor_variables(
        lt_cp_session_t * session,
        const lt_io_value_t * variables ) {

    lt_cp_message_t * msg;

    CHECK_THAT_I_AM_CLIENT( session );

    /* FIXME: check from which pool comes lt_io_value_t arg and possibly
     * don't do deepcopy here */
    msg = _lt_cp_message_create( LT_CP_MSG_MONITOR_VARIABLES, 0,
            lt_io_value_deepcopy( variables,
                session->send_pool ),
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_unmonitor_variables(
        lt_cp_session_t * session,
        const lt_io_value_t * variables ) {

    lt_cp_message_t * msg;

    CHECK_THAT_I_AM_CLIENT( session );

    /* FIXME: check from which pool comes lt_io_value_t arg and possibly
     * don't do deepcopy here */
    msg = _lt_cp_message_create( LT_CP_MSG_UNMONITOR_VARIABLES, 0,
            lt_io_value_deepcopy( variables,
                session->send_pool ),
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_modify_variables(
        lt_cp_session_t * session,
        const lt_io_value_t * variables_with_values ) {

    lt_cp_message_t * msg;

    CHECK_THAT_I_AM_CLIENT( session );

    /* FIXME: check from which pool comes lt_io_value_t arg and possibly
     * don't do deepcopy here */
    msg = _lt_cp_message_create( LT_CP_MSG_MODIFY_VARIABLES, 0,
            lt_io_value_deepcopy( variables_with_values,
                session->send_pool ),
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

lt_cp_status_t lt_cp_session_send_msg_read_variables(
        lt_cp_session_t * session,
        const lt_io_value_t * variables ) {

    lt_cp_message_t * msg;

    CHECK_THAT_I_AM_CLIENT( session );

    /* FIXME: check from which pool comes lt_io_value_t arg and possibly
     * don't do deepcopy here */
    msg = _lt_cp_message_create( LT_CP_MSG_READ_VARIABLES, 0,
            lt_io_value_deepcopy( variables,
                session->send_pool ),
            session->send_pool );

    return _lt_cp_enqueue_msg( session, msg );
}

/*
 * PRE-HOOK STUFF
 */
#ifdef EXTRACT_HOOK_ARG
#undef EXTRACT_HOOK_ARG
#endif
#define EXTRACT_HOOK_ARG(name,arg) \
    lt_cp_fsm_hook_arg_t * name = \
        ( lt_cp_fsm_hook_arg_t * ) ( arg ); \

/**
 * client work, server usually do not need to do anything here
 */

void _lt_cp_fsm_post_hook_after_connection_refused( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* now we are disconnected again */
    ha->session->state = LT_CP_STATE_DISCONNECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully received
                 * connection_refused
                 */
                /* const lt_cp_message_t * response = ha->message; */

                /* FIXME: server does not want us, really.
                 * invoke connection_refused_callback( reason )
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully sent out
                 * connection_refused
                 */
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_project_url( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* now we are connected */
    ha->session->state = LT_CP_STATE_CONNECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully received
                 * project_url,
                 */
                const lt_cp_message_t * response = ha->message;
                const lt_io_value_t * payload =
                    _lt_cp_message_payload_get( response );
                lt_io_value_t * arg =
                    lt_io_value_sequence_item_get( payload, 0 );

                lt_cp_session_project_url_set( ha->session,
                        lt_io_value_string_get( arg ) );
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully sent out
                 * project_url
                 */
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_view_not_selected( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* we are still only connected */
    ha->session->state = LT_CP_STATE_CONNECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully received
                 * view_not_selected
                 */
                /* const lt_cp_message_t * response = ha->message; */

                /* FIXME: server does not want us
                 * invoke view_not_selected_callback( reason )
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully sent out
                 * view_not_selected
                 */
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_available_variables( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* now we are connected and we have selected view. */
    ha->session->state = LT_CP_STATE_VIEW_SELECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully received
                 * available_variables
                 */
                /* FIXME: rework this into
                 * _decode_payload_to_variables( real_or_requested )
                 */
                const lt_cp_message_t * response = ha->message;
                const lt_io_value_t * payload =
                    _lt_cp_message_payload_get( response );

                /* varset which should contain the given variables */
                lt_var_set_t * vset = NULL;

                unsigned i = 0;
                unsigned count = lt_io_value_sequence_length_get( payload );
                int locked = 0;
                unsigned shift = 0;

                lt_var_t * variable = NULL;

                /* walk through the list of received variables and values */
                for( i = 0; i < count; i += 2 ) {
                    const lt_io_value_t * vnam;
                    const char * vname;
                    const lt_io_value_t * vval;

                    /* determine to which connection this variable belongs
                     **/
                    const char * varname = lt_io_value_string_get(
                            lt_io_value_sequence_item_get( payload, i ) );
                    const char * dot = strchr( varname, '.' );

                    /* when not able to find ., skip entirely */
                    if( dot == NULL ) {
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_available_variables: "
                                "unable to determine connection name "
                                "from variable name %s\n",
                                varname );
                        continue;
                    }

                    shift = dot - varname + 1;

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                    fprintf( stderr, "_lt_cp_fsm_post_hook_"
                            "after_available_variables: "
                            "looking up varset for connection "
                            "%*s\n",
                            dot - varname, varname );
#endif

                    /* lookup varset according to figured name */
                    /* use this bloody hack to compute key into apr_hash
                    */
                    vset = ( lt_var_set_t * ) apr_hash_get(
                            ha->session->connections,
                            varname, dot - varname );

                    if( vset == NULL ) {
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_available_variables: "
                                "unable to find varset for connection "
                                "%*s\n",
                                (int)(dot - varname), varname );
                        continue;
                    }

                    /* lock the given varset and go ahead */
                    lt_var_set_real_values_lock( vset );
                    locked = 1;

                    /* process variable's name and value */
                    vnam = lt_io_value_sequence_item_get( payload, i );
                    vname = lt_io_value_string_get( vnam );
                    vval = lt_io_value_sequence_item_get( payload, i + 1 );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                    _lt_cp_dump_var( vnam, vval );
#endif

                    variable = lt_var_set_variable_get( vset,
                            vname + shift );

                    if( variable == NULL ) {
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_available_variables: "
                                "unable to find variable %s\n", vname );
                    } else {
#ifdef FLOOD_CONSOLE_WITH_DEBUG
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_available_variables: "
                                "setting real value of %s\n",
                                vname + shift );
#endif
                        lt_var_real_value_set( variable, vval );
                    }

                    if( vset != NULL && locked ) {
                        lt_var_set_real_values_sync( vset, -1 );
                        lt_var_set_real_values_unlock( vset );
                    }
                }

            }

            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully sent out
                 * available_variables
                 */
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_variable_values( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* we are still ok */
    ha->session->state = LT_CP_STATE_VIEW_SELECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully received
                 * variable_values
                 */
                /* FIXME: rework this into
                 * _decode_payload_to_variables( real_or_requested )
                 */
                const lt_cp_message_t * response = ha->message;
                const lt_io_value_t * payload =
                    _lt_cp_message_payload_get( response );

                /* varset which should contain the given variables */
                lt_var_set_t * vset = NULL;

                unsigned i = 0;
                unsigned count = lt_io_value_sequence_length_get( payload );
                int locked = 0;
                unsigned shift = 0;

                lt_var_t * variable = NULL;


                /* walk through the list of received variables and values */
                for( i = 0; i < count; i += 2 ) {
                    const lt_io_value_t * vnam;
                    const char * vname;
                    const lt_io_value_t * vval;
                    /* when at the first item, determine to which connection
                     * the variables belong, lookup varset and lock it */
                    if( i == 0 ) {
                        /* determine the first variable's name */
                        const char * varname = lt_io_value_string_get(
                                lt_io_value_sequence_item_get( payload, 0 ) );
                        const char * dot = strchr( varname, '.' );

                        /* when not able to find ., skip entirely */
                        if( dot == NULL ) {
                            fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                    "after_variable_values: "
                                    "unable to determine connection name "
                                    "from variable name %s\n",
                                    varname );
                            break;
                        }

                        shift = dot - varname + 1;

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_variable_values: "
                                "looking up varset for connection "
                                "%*s\n",
                                dot - varname, varname );
#endif

                        /* lookup varset according to figured name */
                        /* use this bloody hack to compute key into apr_hash
                         */
                        vset = ( lt_var_set_t * ) apr_hash_get(
                                ha->session->connections,
                                varname, dot - varname );

                        if( vset == NULL ) {
                            fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                    "after_variable_values: "
                                    "unable to find varset for connection "
                                    "%*s\n",
                                    (int)(dot - varname), varname );
                            break;
                        }

                        /* lock the given varset and go ahead */
                        lt_var_set_real_values_lock( vset );
                        locked = 1;
                    }

                    /* process variable's name and value */
                    vnam = lt_io_value_sequence_item_get( payload, i );
                    vname = lt_io_value_string_get( vnam );
                    vval = lt_io_value_sequence_item_get( payload, i + 1 );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                    _lt_cp_dump_var( vnam, vval );
#endif

                    variable = lt_var_set_variable_get( vset,
                            vname + shift );

                    if( variable == NULL ) {
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_variable_values: "
                                "unable to find variable %s\n", vname );
                    } else {
#ifdef FLOOD_CONSOLE_WITH_DEBUG
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_variable_values: "
                                "setting real value of %s\n",
                                vname + shift );
#endif
                        lt_var_real_value_set( variable, vval );
                    }

                }

                if( vset != NULL && locked ) {
                    lt_var_set_real_values_sync( vset, -1 );
                    lt_var_set_real_values_unlock( vset );
                }
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully sent out
                 * variable_values
                 */
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_monitoring_variables( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* we are still ok */
    ha->session->state = LT_CP_STATE_VIEW_SELECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully received
                 * monitoring_variables
                 */
                /* const lt_cp_message_t * response = ha->message; */

                /* FIXME: the same as after_variable_values */
                /* FIXME: ok, we know the server is now monitoring given set
                 * of variables.
                 * walk through received list and set
                 * ha->session->connections
                 * appropriately
                 */
                /* FIXME: check whether we need to sync real values here
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully sent out
                 * monitoring_variables
                 */
            }
            break;
    }
}

/**
 * server work, client usually do not need to do anything here
 */

void _lt_cp_fsm_post_hook_after_connect( void * arg ) {
    const lt_cp_message_t * request;
    const lt_io_value_t * payload;

    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* we are now trying to connect */
    ha->session->state = LT_CP_STATE_CONNECTION_IN_PROGRESS;

    /* extract protocol from the message */
    request = ha->message;
    payload = _lt_cp_message_payload_get( ha->message );

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully sent out
                 * connect
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully received
                 * connect
                 */
                lt_cp_session_send_msg_project_url(
                        ha->session, request );
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_disconnect( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* hmm, disconnected again */
    ha->session->state = LT_CP_STATE_DISCONNECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully sent out
                 * disconnect
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully received
                 * disconnect
                 */
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_select_view( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* hmm, only connected */
    ha->session->state = LT_CP_STATE_VIEW_SELECTION_IN_PROGRESS;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully sent out
                 * select_view
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully received
                 * select_view
                 */
                const lt_cp_message_t * request = ha->message;

                /* send available variables */
                lt_cp_session_send_msg_avaiable_variables(
                        ha->session, request );

                /* FIXME: if we ever need to cancel the connection due to
                 * invalid view name, we do it here
                 */
                /*
                 * lt_cp_session_send_msg_view_not_selected( ha->session,
                 *        request, NULL );
                 */
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_unselect_view( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* hmm, only connected */
    ha->session->state = LT_CP_STATE_CONNECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully sent out
                 * unselect_view
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully received
                 * unselect_view
                 */
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_modify_variables( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* hmm, we have view, dont we ? */
    ha->session->state = LT_CP_STATE_VIEW_SELECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully sent out
                 * modify_variables
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully received
                 * modify_variables
                 */
                /* FIXME: rework this into
                 * _decode_payload_to_variables( real_or_requested )
                 */
                const lt_cp_message_t * request = ha->message;
                const lt_io_value_t * payload =
                    _lt_cp_message_payload_get( request );

                /* varset which should contain the given variables */
                lt_var_set_t * vset = NULL;

                unsigned i = 0;
                unsigned count = lt_io_value_sequence_length_get( payload );
                int locked = 0;
                unsigned shift = 0;

                lt_var_t * variable = NULL;


                /* walk through the list of received variables and values */
                for( i = 0; i < count; i += 2 ) {
                    const lt_io_value_t * vnam;
                    const char * vname;
                    const lt_io_value_t * vval;
                    /* when at the first item, determine to which connection
                     * the variables belong, lookup varset and lock it */
                    if( i == 0 ) {
                        /* determine the first variable's name */
                        const char * varname = lt_io_value_string_get(
                                lt_io_value_sequence_item_get( payload, 0 ) );
                        const char * dot = strchr( varname, '.' );

                        /* when not able to find ., skip entirely */
                        if( dot == NULL ) {
                            fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                    "after_modify_variables: "
                                    "unable to determine connection name "
                                    "from variable name %s\n",
                                    varname );
                            break;
                        }

                        shift = dot - varname + 1;

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_modify_variables: "
                                "looking up varset for connection "
                                "%*s\n",
                                dot - varname, varname );
#endif

                        /* lookup varset according to figured name */
                        /* use this bloody hack to compute key into apr_hash
                         */
                        vset = ( lt_var_set_t * ) apr_hash_get(
                                ha->session->connections,
                                varname, dot - varname );

                        if( vset == NULL ) {
                            fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                    "after_modify_variables: "
                                    "unable to find varset for connection "
                                    "%*s\n",
                                    (int)(dot - varname), varname );
                            break;
                        }

                        /* lock the given varset and go ahead */
                        lt_var_set_requested_values_lock( vset );
                        locked = 1;
                    }

                    /* process variable's name and value */
                    vnam = lt_io_value_sequence_item_get( payload, i );
                    vname = lt_io_value_string_get( vnam );
                    vval = lt_io_value_sequence_item_get( payload, i + 1 );

#ifdef FLOOD_CONSOLE_WITH_DEBUG
                    _lt_cp_dump_var( vnam, vval );
#endif

                    variable = lt_var_set_variable_get( vset,
                            vname + shift );

                    if( variable == NULL ) {
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_modify_variables: "
                                "unable to find variable %s\n", vname );
                    } else {
#ifdef FLOOD_CONSOLE_WITH_DEBUG
                        fprintf( stderr, "_lt_cp_fsm_post_hook_"
                                "after_modify_variables: "
                                "setting requested value of %s\n",
                                vname + shift );
#endif
                        lt_var_requested_value_set( variable, vval );
                    }

                }

                if( vset != NULL && locked ) {
                    lt_var_set_requested_values_sync( vset, -1 );
                    lt_var_set_requested_values_unlock( vset );
                }
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_read_variables( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* hmm, boring comments */
    ha->session->state = LT_CP_STATE_VIEW_SELECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully sent out
                 * read_variables
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully received
                 * read_variables
                 */
                const lt_cp_message_t * request = ha->message;

                /* FIXME: assemble the list of requested variables and their
                 * values and send it out with:
                 *
                 * lt_cp_session_send_msg_variable_values
                 *
                 * */
                lt_cp_session_send_msg_variable_values(
                        ha->session, request, NULL );
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_monitor_variables( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* hmm, boring comments */
    ha->session->state = LT_CP_STATE_VIEW_SELECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully sent out
                 * monitor_variables
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully received
                 * monitor_variables
                 */
                /* const lt_cp_message_t * request = ha->message; */

                /* FIXME: set monitor status on requested variables and sync
                 * requested
                 */
            }
            break;
    }
}

void _lt_cp_fsm_post_hook_after_unmonitor_variables( void * arg ) {
    EXTRACT_HOOK_ARG( ha, arg );
    if( ha == NULL || ha->session == NULL ) return;

    /* hmm, boring comments */
    ha->session->state = LT_CP_STATE_VIEW_SELECTED;

    switch( lt_cp_session_mode_get( ha->session ) ) {

        case LT_CP_CLIENT:
            {
                /* client has just successfully sent out
                 * unmonitor_variables
                 */
            }
            break;

        case LT_CP_SERVER:
            {
                /* server has just successfully received
                 * unmonitor_variables
                 */
                /* const lt_cp_message_t * request = ha->message; */

                /* FIXME: set monitor status on requested variables and sync
                 * requested
                 */
            }
            break;
    }
}

/*
 * helper macro to save some typing
 */
#define CREATE_TRANSITION(fsm,from,to,msg_type,prehook,posthook) \
    lt_fsm_transition_create( fsm, \
            (from), (to), 255, \
            LT_CP_FSM_TRANSITION_INPUT( (msg_type) ), \
            LT_CP_FSM_TRANSITION_INPUT_LEN, \
            _lt_cp_pool ); \
    if( (prehook) != NULL ) { \
        lt_fsm_transition_prehook_register( fsm, \
                (from), \
                LT_CP_FSM_TRANSITION_INPUT( (msg_type) ), \
                LT_CP_FSM_TRANSITION_INPUT_LEN, \
                (prehook) ); \
    } \
    if( (posthook) != NULL ) { \
        lt_fsm_transition_posthook_register( fsm, \
                (from), \
                LT_CP_FSM_TRANSITION_INPUT( (msg_type) ), \
                LT_CP_FSM_TRANSITION_INPUT_LEN, \
                (posthook) ); \
    }

lt_fsm_t * _lt_cp_fsm_create( apr_pool_t * pool )
{
    /* FIXME: catch errors when creating states and transitions */

    lt_fsm_t * f = NULL;

    if( lt_fsm_create( &f, pool )
            != LT_FSM_SUCCESS ) {
        return NULL;
    }

    /* create state machine for connected protocol */
    lt_fsm_state_create(
            f, "disconnected", pool );
    lt_fsm_state_create(
            f, "connection_in_progress", pool );
    lt_fsm_state_create(
            f, "connected", pool );
    lt_fsm_state_create(
            f, "view_selection_in_progress", pool );
    lt_fsm_state_create(
            f, "view_selected", pool );
    lt_fsm_state_create(
            f, "change_set_of_monitored_variables", pool );

    CREATE_TRANSITION( f, "disconnected", "connection_in_progress",
            LT_CP_MSG_CONNECT,
            NULL,
            _lt_cp_fsm_post_hook_after_connect );

    CREATE_TRANSITION( f, "connection_in_progress", "disconnected",
            LT_CP_MSG_CONNECTION_REFUSED,
            NULL,
            _lt_cp_fsm_post_hook_after_connection_refused );

    CREATE_TRANSITION( f, "connection_in_progress", "disconnected",
            LT_CP_MSG_DISCONNECT,
            NULL,
            _lt_cp_fsm_post_hook_after_disconnect );

    CREATE_TRANSITION( f, "connection_in_progress", "connected",
            LT_CP_MSG_PROJECT_URL,
            NULL,
            _lt_cp_fsm_post_hook_after_project_url );

    CREATE_TRANSITION( f, "connected", "disconnected",
            LT_CP_MSG_DISCONNECT,
            NULL,
            _lt_cp_fsm_post_hook_after_disconnect );

    CREATE_TRANSITION( f, "connected", "view_selection_in_progress",
            LT_CP_MSG_SELECT_VIEW,
            NULL,
            _lt_cp_fsm_post_hook_after_select_view );

    CREATE_TRANSITION( f, "view_selection_in_progress", "connected",
            LT_CP_MSG_VIEW_NOT_SELECTED,
            NULL,
            _lt_cp_fsm_post_hook_after_view_not_selected );

    CREATE_TRANSITION( f, "view_selection_in_progress", "disconnected",
            LT_CP_MSG_DISCONNECT,
            NULL,
            _lt_cp_fsm_post_hook_after_disconnect );

    CREATE_TRANSITION( f, "view_selection_in_progress", "view_selected",
            LT_CP_MSG_AVAILABLE_VARIABLES,
            NULL,
            _lt_cp_fsm_post_hook_after_available_variables );

    CREATE_TRANSITION( f, "view_selected", "connected",
            LT_CP_MSG_UNSELECT_VIEW,
            NULL,
            _lt_cp_fsm_post_hook_after_unselect_view );

    CREATE_TRANSITION( f, "view_selected", "disconnected",
            LT_CP_MSG_DISCONNECT,
            NULL,
            _lt_cp_fsm_post_hook_after_disconnect );

    CREATE_TRANSITION( f, "view_selected",
            "change_set_of_monitored_variables",
            LT_CP_MSG_MONITOR_VARIABLES,
            NULL,
            _lt_cp_fsm_post_hook_after_monitor_variables );

    CREATE_TRANSITION( f, "view_selected",
            "change_set_of_monitored_variables",
            LT_CP_MSG_UNMONITOR_VARIABLES,
            NULL,
            _lt_cp_fsm_post_hook_after_unmonitor_variables );

    CREATE_TRANSITION( f, "change_set_of_monitored_variables",
            "change_set_of_monitored_variables",
            LT_CP_MSG_VARIABLE_VALUES,
            NULL,
            _lt_cp_fsm_post_hook_after_variable_values );

    CREATE_TRANSITION( f, "change_set_of_monitored_variables",
            "view_selected",
            LT_CP_MSG_MONITORING_VARIABLES,
            NULL,
            _lt_cp_fsm_post_hook_after_monitoring_variables );

    CREATE_TRANSITION( f, "view_selected", "view_selected",
            LT_CP_MSG_READ_VARIABLES,
            NULL,
            _lt_cp_fsm_post_hook_after_read_variables );

    CREATE_TRANSITION( f, "view_selected", "view_selected",
            LT_CP_MSG_MODIFY_VARIABLES,
            NULL,
            _lt_cp_fsm_post_hook_after_modify_variables );

    CREATE_TRANSITION( f, "view_selected", "view_selected",
            LT_CP_MSG_VARIABLE_VALUES,
            NULL,
            _lt_cp_fsm_post_hook_after_variable_values );

    /* NOTE: allow variable values to come even in connected,
     * disconnected and view_selection_in_progress state, but
     * simply ignore them, can happen that the server will send variable
     * values while the client sends unselect_view/disconnect
     * and goes sleeping */

    CREATE_TRANSITION( f, "disconnected", "disconnected",
            LT_CP_MSG_VARIABLE_VALUES,
            NULL,
            NULL );

    CREATE_TRANSITION( f, "connected", "connected",
            LT_CP_MSG_VARIABLE_VALUES,
            NULL,
            NULL );

    CREATE_TRANSITION( f, "view_selection_in_progress",
            "view_selection_in_progress",
            LT_CP_MSG_VARIABLE_VALUES,
            NULL,
            NULL );

    return f;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: cp.c 1774 2006-05-31 13:17:16Z hynek $
 */
