/* $Id: fsm.c 353 2005-02-23 15:40:36Z modesto $
 *
 *   FILE: fsm.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 7 July 2003
 *
 * Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
 * Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or(at
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

#include <stdlib.h>
#include <string.h>

#include <apr_strings.h>

#include "lt/fsm/fsm.h"

#define LINUX 2
#define _LARGEFILE64_SOURCE

struct lt_fsm_transition_t {
    apr_pool_t * pool;
    const void * transition_input;
    size_t transition_input_len;
    const char * dest_state_name;
    unsigned char transition_probability;

    unsigned latest_prehook_id;
    apr_hash_t * pre_hooks;

    unsigned latest_posthook_id;
    apr_hash_t * post_hooks;
};

struct lt_fsm_state_t {
    apr_pool_t * pool;
    const char * state_name;
    apr_hash_t * transitions;
};

struct lt_fsm_t {
    apr_pool_t * pool;
    apr_hash_t * states;
};

struct lt_fsm_session_t {
    apr_pool_t * pool;
    const char * current_state_name;
    lt_fsm_state_t * current_state;
    lt_fsm_t * fsm;
};

/**
 * Do we have a given state ?
 */
lt_fsm_state_t * lt_fsm_has_state(
        lt_fsm_t * fsm, const char * state );

/**
 * Do we have a given transition ?
 */
lt_fsm_transition_t * lt_fsm_has_transition(
        lt_fsm_t * fsm, const char * state,
        const void * input, size_t input_len );

/**
 * Invoke pre-hooks of given transition and return 0 or 1
 */
int lt_fsm_transition_prehooks_invoke( lt_fsm_transition_t * t,
        void * userdata );

/**
 * Invoke post-hooks of given transition.
 */
void lt_fsm_transition_posthooks_invoke( lt_fsm_transition_t * t,
        void * userdata );

#define CHECK_STRING_ARG(s) \
    if( (s) == NULL ) return LT_FSM_INVALID_ARG; \
    if( strcmp( "", (s) ) == 0 ) return LT_FSM_INVALID_ARG;

lt_fsm_status_t lt_fsm_create( lt_fsm_t ** fsm, apr_pool_t * pool ) {

    if( fsm == NULL ) return LT_FSM_INVALID_ARG;
    *fsm = NULL;
    if( pool == NULL ) return LT_FSM_INVALID_ARG;

    *fsm = ( lt_fsm_t * ) apr_pcalloc( pool, sizeof( lt_fsm_t ) );

    ( *fsm )->pool = pool;
    ( *fsm )->states = apr_hash_make( pool );
   
    return LT_FSM_SUCCESS;
}

lt_fsm_status_t lt_fsm_session_create( lt_fsm_session_t ** session,
        lt_fsm_t * fsm, apr_pool_t * pool ) {

    if( session == NULL ) return LT_FSM_INVALID_ARG;
    *session = NULL;
    if( fsm == NULL ) return LT_FSM_INVALID_ARG;
    if( pool == NULL ) return LT_FSM_INVALID_ARG;

    *session = ( lt_fsm_session_t * )
        apr_pcalloc( pool, sizeof( lt_fsm_session_t ) );

    ( *session )->pool = pool;
    ( *session )->fsm = fsm;

    return LT_FSM_SUCCESS;
}

lt_fsm_status_t lt_fsm_session_process( lt_fsm_session_t * fsms,
        const void * input, size_t input_len, void * userdata ) {

    lt_fsm_transition_t * t = NULL;

    if( fsms == NULL ) return LT_FSM_INVALID_ARG;
    if( fsms->fsm == NULL ) return LT_FSM_INVALID_ARG;
    if( fsms->current_state == NULL ) return LT_FSM_ENOSUCHSTATE;

    t = apr_hash_get( fsms->current_state->transitions, input, input_len );

    if( t == NULL ) return LT_FSM_ENOSUCHTRANSITION;

    if( ! lt_fsm_transition_prehooks_invoke( t, userdata ) )
        return LT_FSM_ETRANSITIONBLOCKED;

    lt_fsm_session_state_set( fsms, t->dest_state_name );

    lt_fsm_transition_posthooks_invoke( t, userdata );

    return LT_FSM_SUCCESS;
}

lt_fsm_status_t lt_fsm_session_state_set( lt_fsm_session_t * fsms,
        const char * state_name ) {

    if( fsms == NULL ) return LT_FSM_INVALID_ARG;
    if( fsms->fsm == NULL ) return LT_FSM_INVALID_ARG;

    if( NULL == lt_fsm_has_state( fsms->fsm, state_name ) )
        return LT_FSM_ENOSUCHSTATE;

    fsms->current_state_name = state_name;

    fsms->current_state = ( lt_fsm_state_t * )
        apr_hash_get( fsms->fsm->states, state_name, APR_HASH_KEY_STRING );

    if( fsms->current_state == NULL ) return LT_FSM_ENOSUCHSTATE;

    return LT_FSM_SUCCESS;
}

const char * lt_fsm_session_state_get( lt_fsm_session_t * fsms ) {

    if( fsms == NULL ) return NULL;

    return fsms->current_state_name;
}

lt_fsm_status_t lt_fsm_state_create( lt_fsm_t * fsm,
        const char * state_name, apr_pool_t * pool ) {

    lt_fsm_state_t * s = NULL;

    if( fsm == NULL ) return LT_FSM_INVALID_ARG;

    CHECK_STRING_ARG( state_name );

    if( NULL != lt_fsm_has_state( fsm, state_name ) )
        return LT_FSM_ESTATEXISTS;

    s = apr_pcalloc( pool, sizeof( lt_fsm_state_t ) );

    s->pool = pool;
    s->state_name = state_name;

    s->transitions = apr_hash_make( pool );

    apr_hash_set( fsm->states, state_name, APR_HASH_KEY_STRING, s );

    return LT_FSM_SUCCESS;
}

lt_fsm_status_t lt_fsm_transition_create( lt_fsm_t * fsm,
        const char * from_state, const char * to_state,
        unsigned char probability,
        const void * input, size_t input_len, apr_pool_t * pool ) {

    lt_fsm_state_t * s = NULL;
    lt_fsm_transition_t * t = NULL;

    if( fsm == NULL ) return LT_FSM_INVALID_ARG;

    CHECK_STRING_ARG( from_state );
    CHECK_STRING_ARG( to_state );

    if( NULL == ( s = lt_fsm_has_state( fsm, from_state ) ) )
        return LT_FSM_ENOSUCHSTATE;
    if( NULL == lt_fsm_has_state( fsm, to_state ) )
        return LT_FSM_ENOSUCHSTATE;

    if( input == NULL || input_len == 0 ) return LT_FSM_INVALID_ARG;

    if( lt_fsm_has_transition( fsm, from_state, input, input_len ) )
        return LT_FSM_ETRANSITIONEXISTS;

    t = apr_pcalloc( pool, sizeof( lt_fsm_transition_t ) );

    t->pool = pool;
    t->dest_state_name = to_state;
    t->transition_input = input;
    t->transition_input_len = input_len;

    t->transition_probability = probability;

    t->pre_hooks = apr_hash_make( pool );
    t->latest_prehook_id = 0;
    t->post_hooks = apr_hash_make( pool );
    t->latest_posthook_id = 0;

    apr_hash_set( s->transitions, input, input_len, t );

    return LT_FSM_SUCCESS;
}

lt_fsm_status_t lt_fsm_transition_prehook_register( lt_fsm_t * fsm,
        const char * state_name,
        const void * input, size_t input_len,
        lt_fsm_prehook_t prehook ) {

    lt_fsm_state_t * s = NULL;
    lt_fsm_transition_t * t = NULL;
    char * id = NULL;

    if( fsm == NULL ) return LT_FSM_INVALID_ARG;

    if( prehook == NULL ) return LT_FSM_INVALID_ARG;

    if( NULL == ( s = lt_fsm_has_state( fsm, state_name ) ) )
        return LT_FSM_ENOSUCHSTATE;

    if( NULL == ( t = lt_fsm_has_transition(
                    fsm, state_name, input, input_len ) ) )
        return LT_FSM_ENOSUCHTRANSITION;

    t->latest_prehook_id ++;
    id = apr_itoa( t->pool, t->latest_prehook_id );

    apr_hash_set( t->pre_hooks, id, APR_HASH_KEY_STRING, prehook );

    return LT_FSM_SUCCESS;
}

lt_fsm_status_t lt_fsm_transition_posthook_register( lt_fsm_t * fsm,
        const char * state_name,
        const void * input, size_t input_len,
        lt_fsm_posthook_t posthook ) {

    lt_fsm_state_t * s = NULL;
    lt_fsm_transition_t * t = NULL;
    char * id = NULL;

    if( fsm == NULL ) return LT_FSM_INVALID_ARG;

    if( posthook == NULL ) return LT_FSM_INVALID_ARG;

    if( NULL == ( s = lt_fsm_has_state( fsm, state_name ) ) )
        return LT_FSM_ENOSUCHSTATE;

    if( NULL == ( t = lt_fsm_has_transition(
                    fsm, state_name, input, input_len ) ) )
        return LT_FSM_ENOSUCHTRANSITION;

    t->latest_posthook_id ++;
    id = apr_itoa( t->pool, t->latest_posthook_id );

    apr_hash_set( t->post_hooks, id, APR_HASH_KEY_STRING, posthook );

    return LT_FSM_SUCCESS;
}

lt_fsm_state_t * lt_fsm_has_state(
        lt_fsm_t * fsm, const char * state ) {

    if( fsm == NULL ) return 0;

    return apr_hash_get( fsm->states, state, APR_HASH_KEY_STRING );
}

lt_fsm_transition_t * lt_fsm_has_transition(
        lt_fsm_t * fsm, const char * state,
        const void * input, size_t input_len ) {

    lt_fsm_state_t * s = NULL;

    if( fsm == NULL ) return 0;

    s = apr_hash_get(
            fsm->states, state, APR_HASH_KEY_STRING );

    if( s == NULL ) return 0;

    return apr_hash_get( s->transitions, input, input_len );
}

int lt_fsm_transition_prehooks_invoke( lt_fsm_transition_t * t,
        void * userdata ) {

    int result = 1;

    apr_pool_t * pool = NULL;
    apr_hash_index_t * i = NULL;
    lt_fsm_prehook_t prehook = NULL;

    const void * key = NULL;
    size_t key_len = 0;

    apr_pool_create( &pool, t->pool );

    for( i = apr_hash_first( pool, t->pre_hooks ); i;
            i = apr_hash_next( i ) ) {
        apr_hash_this( i, &key, &key_len, ( void ** ) &prehook );

        if( prehook != NULL ) {
            if( ! prehook( userdata ) ) {
                result = 0;
            }
        }
    }

    apr_pool_destroy( pool );

    return result;
}

void lt_fsm_transition_posthooks_invoke( lt_fsm_transition_t * t,
        void * userdata ) {

    apr_pool_t * pool = NULL;
    apr_hash_index_t * i = NULL;
    lt_fsm_posthook_t posthook = NULL;

    const void * key = NULL;
    size_t key_len = 0;

    apr_pool_create( &pool, t->pool );

    for( i = apr_hash_first( pool, t->post_hooks ); i;
            i = apr_hash_next( i ) ) {
        apr_hash_this( i, &key, &key_len, ( void ** ) &posthook );

        if( posthook != NULL ) {
            posthook( userdata );
        }
    }

    apr_pool_destroy( pool );

}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: fsm.c 353 2005-02-23 15:40:36Z modesto $
 */
