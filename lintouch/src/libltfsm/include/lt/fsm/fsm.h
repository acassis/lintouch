/* $Id: fsm.h 347 2005-02-23 14:56:10Z modesto $
 *
 *   FILE: fsm.h --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 7 July 2003
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

#ifndef _LT_FSM_H
#define _LT_FSM_H

#include <apr_general.h>
#include <apr_hash.h>

/**
 * @file fsm.h
 * @brief Lintouch Finite State Machine Library
 */

/**
 * @defgroup LTFSM Lintouch Finite State Machine Library
 * @{
 */

/**
 * @defgroup LTFSMOverview Overview
 * @{
 *
 * Lintouch Finite State Machine Library.
 *
 * Library for simple finite state machine modeling.
 *
 * This library allows you to create a finite state machine with
 * non-deterministic (probability) based transitions.
 *
 * Every state machine (represented by lt_fsm_t*) is defined as a set of
 * states (represented by const char*).
 *
 * You can create new state machine by invoking lt_fsm_create and add
 * states to it by invoking lt_fsm_state_create.
 *
 * Every state contains zero, one or more transitions defined by input
 * condition (represented by const void* and size_t len), probability
 * (number from 0 to 255) and destination state (represented by const
 * char*).
 *
 * You can create transitions for given state by invoking
 * lt_fsm_transition_create.
 *
 * In order to start simulation you have to create finite state machine
 * session (represented by lt_fsm_session_t*) and set its initial state
 * (represented by const char*) and simulate.
 *
 * You can create finite state machine session by invoking
 * lt_fsm_session_create and set it's initial state by invoking
 * lt_fsm_session_state_set. To obtain current state of the session you
 * can invoke lt_fsm_session_state_get.
 *
 * Yo process given input simply use lt_fsm_session_process.
 *
 * To give you some more flexibility, you can associate with every
 * transition set of pre-hooks and post-hooks.
 *
 * Pre-hooks are functions with one void* argument returning 0 or 1.
 * Pre-hooks are invoked in the same order they were registered before the
 * transition should take a place. If any of the pre-hooks returns 0, the
 * transition is blocked. However all pre-hooks are always invoked.
 *
 * Post-hooks are functions with one void* argument that are invoked in the
 * same order in which they were registered. They are invoked after the
 * transition has been successfully performed.
 *
 * Pre-hooks and post-hooks (when invoked) obtain one void* argument that
 * was passed to lt_fsm_session_process. That way pre-hooks can decide
 * whether ot block/allow given transition.
 *
 * You can register pre-hooks and post-hooks by invoking
 * lt_fsm_transition_prehook_register and
 * lt_fsm_transition_posthook_register respectively.
 *
 * @author Martin Man <mman@swac.cz>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Return value for functions.
     */
    typedef int lt_fsm_status_t;

    /**
     * Successfull termination.
     */
#define LT_FSM_SUCCESS    0
    /**
     * Invalid argument (possibly NULL) was give to the function.
     */
#define LT_FSM_INVALID_ARG    -1
    /**
     * Method has not been implemented yet.
     */
#define LT_FSM_METHOD_UNIMPLEMENTED    -99

    /**
     * The given state already exists.
     */
#define LT_FSM_ESTATEXISTS            -4
    /**
     * The given state does not exists.
     */
#define LT_FSM_ENOSUCHSTATE           -5
    /**
     * The given transition already exists.
     */
#define LT_FSM_ETRANSITIONEXISTS      -6
    /**
     * The given transition does not exists.
     */
#define LT_FSM_ENOSUCHTRANSITION      -7
    /**
     * Transition was blocked by pre-hook.
     */
#define LT_FSM_ETRANSITIONBLOCKED     -8

    /**
     * Pre-hook type.
     *
     * Pre-hooks are functions that are invoked when a transition is to be
     * performed. They obtain user-data via void-pointer as passed to the
     * lt_fsm_process method. They should examine the data and possibly
     * other related info and return 1 if the transition is allowed or 0
     * if it shall be blocked.
     */
    typedef int ( * lt_fsm_prehook_t )( void * );

    /**
     * Post-hook type.
     *
     * Post-hooks are functions that are invoked when a transition has been
     * performed. They obtain user-data via void-pointer as passed to the
     * lt_fsm_process method.
     */
    typedef void ( * lt_fsm_posthook_t )( void * );

    /**
     * Finite state machine transition type.
     */
    typedef struct lt_fsm_transition_t lt_fsm_transition_t;

    /**
     * Finite state machine state type.
     */
    typedef struct lt_fsm_state_t lt_fsm_state_t;

    /**
     * Finite state machine type.
     */
    typedef struct lt_fsm_t lt_fsm_t;

    /**
     * Finite state machine session type.
     */
    typedef struct lt_fsm_session_t lt_fsm_session_t;

    /**
     * Create new finite state machine.
     *
     * @param fsm The newly created fsm.
     * @param pool The pool to allocate the fsm out of.
     *
     * @return LT_FSM_SUCCESS, LT_FSM_INVALID_ARG
     */
    lt_fsm_status_t lt_fsm_create( lt_fsm_t ** fsm, apr_pool_t * pool );

    /**
     * Create new finite state session.
     *
     * @param session The newly created fsm session.
     * @param fsm The fsm associated with this session.
     * @param pool The pool to allocate the fsm session out of.
     *
     * @return LT_FSM_SUCCESS, LT_FSM_INVALID_ARG
     */
    lt_fsm_status_t lt_fsm_session_create( lt_fsm_session_t ** session,
            lt_fsm_t * fsm, apr_pool_t * pool );

    /**
     * Process given input.
     *
     * @param fsms The session that should process given input.
     * @param input The input that will determine which transition has to
     * take place.
     * @param input_len The length of the input.
     * @param userdata The data that will be passed to user defined
     * pre-hooks and post-hooks.
     *
     * @return LT_FSM_SUCCESS, LT_FSM_ENOSUCHTRANSITION,
     * LT_FSM_ETRANSITIONBLOCKED.
     */
    lt_fsm_status_t lt_fsm_session_process( lt_fsm_session_t * fsms,
            const void * input, size_t input_len, void * userdata );

    /**
     * Set current (initial) state.
     *
     * @param fsms The session that should have the new state set.
     * @param state_name The new state name.
     *
     * @return LT_FSM_SUCCESS, LT_FSM_ENOSUCHSTATE
     */
    lt_fsm_status_t lt_fsm_session_state_set( lt_fsm_session_t * fsms,
            const char * state_name );

    /**
     * Return current state.
     *
     * @param fsms The session.
     *
     * @return Current state name associated with the given session or NULL
     * in case of error.
     */
    const char * lt_fsm_session_state_get( lt_fsm_session_t * fsms );

    /**
     * Create new state.
     *
     * @param fsm The fsm.
     * @param state_name The name of the newly created state.
     * @param pool The pool to allocate the state out of.
     *
     * Note: since state_name is used as a key to the internal hash table,
     * you are required to guaranty that it will exist as long as the state
     * diagram exists.
     *
     * @return LT_FSM_SUCCESS, LT_FSM_INVALID_ARG, LT_FSM_ESTATEXISTS
     */
    lt_fsm_status_t lt_fsm_state_create( lt_fsm_t * fsm,
            const char * state_name, apr_pool_t * pool );

    /**
     * Create new transition.
     *
     * @param fsm The fsm.
     * @param from_state The name of the originating state.
     * @param to_state The name of the destination state.
     * @param probability The probability (non-determinism) of this
     * transition (0-255) FIXME: not implemented yet.
     * Behaves as if probability is 1.
     * @param input The input that will trigger this transition.
     * @param input_len The length of the input.
     * @param pool The pool to allocate the transitioon out of.
     *
     * Note: since input is used as a key to the internal hash table,
     * you are required to guaranty that it will exist as long as the state
     * diagram exists.
     *
     * @return LT_FSM_SUCCESS, LT_FSM_INVALID_ARG,
     * LT_FSM_ENOSUCHSTATE, LT_FSM_ETRANSITIONEXISTS
     */
    lt_fsm_status_t lt_fsm_transition_create( lt_fsm_t * fsm,
            const char * from_state, const char * to_state,
            unsigned char probability,
            const void * input, size_t input_len, apr_pool_t * pool );

    /**
     * Register transition pre-hook.
     *
     * pre-hooks are invoked before the transition should be performed. Any
     * of the hooks that returns zero causes transition not to be performed.
     * All pre-hooks are invoked.
     *
     * @param fsm The fsm.
     * @param state_name The state for which the hook should be registered.
     * @param input The transition for which the hook should be registered.
     * @param input_len The length of the input.
     * @param prehook The hook routine.
     *
     * @return LT_FSM_SUCCESS, LT_FSM_INVALID_ARG,
     * LT_FSM_ENOSUCHSTATE, LT_FSM_ENOSUCHTRANSITION
     */
    lt_fsm_status_t lt_fsm_transition_prehook_register( lt_fsm_t * fsm,
            const char * state_name,
            const void * input, size_t input_len,
            lt_fsm_prehook_t prehook );

    /**
     * Register transition post-hook.
     *
     * post-hooks are invoked after the transition has been performed.
     *
     * @param fsm The fsm.
     * @param state_name The state for which the hook should be registered.
     * @param input The transition for which the hook should be registered.
     * @param input_len The length of the input.
     * @param posthook The hook routine.
     *
     * @return LT_FSM_SUCCESS, LT_FSM_INVALID_ARG,
     * LT_FSM_ENOSUCHSTATE, LT_FSM_ENOSUCHTRANSITION
     */
    lt_fsm_status_t lt_fsm_transition_posthook_register( lt_fsm_t * fsm,
            const char * state_name,
            const void * input, size_t input_len,
            lt_fsm_posthook_t posthook );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_IO_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: fsm.h 347 2005-02-23 14:56:10Z modesto $
 */
