/* $Id: variables.h 1541 2006-03-21 13:04:52Z modesto $
 *
 *   FILE: vars.h --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 18 July 2003
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

#ifndef _LT_VARIABLES_H
#define _LT_VARIABLES_H

#include <apr_general.h>
#include <apr_hash.h>
#include <apr_time.h>

#include <lt/variables/io.h>

/**
 * @file variables.h
 * @brief Lintouch Variables Library
 */

/**
 * @defgroup LTVARS Lintouch Variables Library
 * @{
 *
 * Lintouch Variables Library.
 *
 * Definitions of common structures and functions used when dealing with
 * shared variables and shared variable sets.
 *
 * A Shared variable is a variable shared between exactly one backend and
 * one or more frontends (called also modules).
 *
 * Every variable has a real value and requested value, real monitor
 * flag and requested monitor flag.
 *
 * Shared Variables, represented by opaque lt_var_t datatype are organized
 * within sets represented by opaque lt_var_set_t datatype.
 *
 * Every set has to be associated with one backend module and one or more
 * frontend modules. Modules are represented as a method of either
 * lt_var_set_backend_func_t or lt_var_set_frontend_func_t datatype.
 *
 * Backend is responsible for the "real" variable's value, whilst frontend
 * modules can request the change of the variable's value by setting the
 * requested value.
 *
 * Once the real or requested values/monitor flags are modified, they have
 * to be synced to the other side. You can use lt_var_set_real_values_sync
 * and lt_var_set_requested_values_sync methods.
 *
 * When real values are being synced, all frontend modules get invoked with
 * the varset in question as the first arg. They can determine what has
 * changed (by comparing real/requested values and checking dirty real
 * values) and react appropriately.
 *
 * When requested values are being synced, the backend module gets invoked
 * with the varset in question as the first arg. Backend can examine which
 * changes are required (by comparing requested values with real values, and
 * by checking dirty requested values) and react appropriatelly.
 *
 * Note that the *_sync routines should not be invoked by the backend a/o
 * frontend modules itself but rather by the third party independent
 * arbiter. When called directly from the backend/frontend routines they can
 * cause infinite loops.
 *
 * Additionally one can chain variable sets (of type lt_var_set_t *)
 * together to create a one way linked list or tree. Root of the tree is a
 * var set without source.
 *
 * Childs of the tree are represented by varsets which specified their
 * parent varsets as their source.
 *
 * When the real values of certain varaible presented in certain varset are
 * being synced, they propagate one level deep within the tree (from parents
 * to leafes).
 *
 * When the requested values of certain variable in certain varset are being
 * synced, they propagate one level up the tree (from leafes to parents).
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Return status for functions.
     */
    typedef int lt_var_status_t;

#define LT_VAR_SUCCESS                          0
#define LT_VAR_INVALID_ARG                     -1
#define LT_VAR_TIMEUP                          -2
#define LT_VAR_METHOD_UNIMPLEMENTED            -99

    /**
     * Shared variable.
     */
    typedef struct lt_var_t lt_var_t;

    /**
     * Set of shared variables.
     */
    typedef struct lt_var_set_t lt_var_set_t;

    /**
     * Backend func.
     *
     * @see lt_var_set_backend_set
     */
    typedef int( * lt_var_set_backend_func_t )(
            lt_var_set_t * vset, void * user_arg,
            apr_interval_time_t timeout );

    /**
     * Frontend func.
     *
     * @see lt_var_set_frontend_register
     */
    typedef int( * lt_var_set_frontend_func_t )(
            lt_var_set_t * vset, void * user_arg,
            apr_interval_time_t timeout );

    /**
     * Logging hook func.
     *
     * @param vset The varset which is synced
     * @param real Flag; true when syncing real values false for requested
     * @param user_data Any data the logging hook may need. They are passed
     * to the lt_var_set_logging_func_t when called.
     */
    typedef void( * lt_var_set_logging_func_t )(
           lt_var_set_t * vset, int real, void* user_data );

    /**
     * return pool out of which the given var has been allocated.
     */
    apr_pool_t * lt_var_pool_get( const lt_var_t * src );

    /**
     * Create new variable.
     *
     * Will create new variable of given type with initial requested and
     * real value set to 0 or empty string (according to given type).
     *
     * @param type The variable's type.
     * value.
     * @param monitored The initial value of the variable's real and
     * requested monitor state.
     * @param pool The pool to allocate variable out of, when NULL new one
     * will be allocated.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_t * lt_var_create(
            lt_io_value_type_e type,
            int monitored,
            apr_pool_t * pool );

    /**
     * return type of the variable.
     */
    lt_io_value_type_e lt_var_type_get( const lt_var_t * src );

    /**
     * Set value of a variable from the backend side (the real value).
     *
     * The variable will make a deepcopy of the given value out of its
     * internal pool to make this value independent.
     *
     * @param variable The variable whose real value should be set.
     * @param value New value.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_real_value_set(
            lt_var_t * variable,
            const lt_io_value_t * value );

    /**
     * Set value of a variable from the frontend side (the requested value).
     *
     * The variable will make a deepcopy of the given value out of its
     * internal pool to make this value independent.
     *
     * @param variable The variable whose requested value should be set.
     * @param value New value.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_requested_value_set(
            lt_var_t * variable,
            const lt_io_value_t * value );

    /**
     * Get real value of a variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return value of the variable.
     */
    const lt_io_value_t * lt_var_real_value_get(
            const lt_var_t * variable );

    /**
     * Get requested value of a variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return value of the variable.
     */
    const lt_io_value_t * lt_var_requested_value_get(
            const lt_var_t * variable );

    /**
     * Set real value of a variable.
     *
     * @param variable The variable whose real value should be set.
     * @param v New value.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_real_value_bit_set(
            lt_var_t * variable,
            uint8_t v );

    /**
     * Set real value of a variable.
     *
     * @param variable The variable whose real value should be set.
     * @param v New value.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_real_value_number_set(
            lt_var_t * variable,
            int32_t v );

    /**
     * Set real value of a variable.
     *
     * @param variable The variable whose real value should be set.
     * @param v New value.
     * @param len The length of new value.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_real_value_string_set(
            lt_var_t * variable,
            const uint8_t * v, uint32_t len );

    /**
     * Set real value of a variable.
     *
     * @param variable The variable whose real value should be set.
     * @param v New value. Null terminated.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_real_value_string_set2(
            lt_var_t * variable,
            const uint8_t * v );

    /**
     * Set requested value of a variable.
     *
     * @param variable The variable whose requested value should be set.
     * @param v New value.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_requested_value_bit_set(
            lt_var_t * variable,
            uint8_t v );

    /**
     * Set requested value of a variable.
     *
     * @param variable The variable whose requested value should be set.
     * @param v New value.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_requested_value_number_set(
            lt_var_t * variable,
            int32_t v );

    /**
     * Set requested value of a variable.
     *
     * @param variable The variable whose requested value should be set.
     * @param v New value.
     * @param len The length of new value.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_requested_value_string_set(
            lt_var_t * variable,
            const uint8_t * v, uint32_t len );

    /**
     * Set requested value of a variable.
     *
     * @param variable The variable whose requested value should be set.
     * @param v New value. Null terminated.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_requested_value_string_set2(
            lt_var_t * variable,
            const uint8_t * v );

    /**
     * Get real value of a variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return value of the variable.
     */
    uint8_t lt_var_real_value_bit_get(
            const lt_var_t * variable );

    /**
     * Get real value of a variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return value of the variable.
     */
    int32_t lt_var_real_value_number_get(
            const lt_var_t * variable );

    /**
     * Get length of real value of string typed variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return length of a string typed variable.
     */
    uint32_t lt_var_real_value_string_length_get(
            const lt_var_t * variable );

    /**
     * Get real value of a variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return string data of the string typed variable.
     */
    const uint8_t * lt_var_real_value_string_get(
            const lt_var_t * variable );

    /**
     * Get requested value of a variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return value of the variable.
     */
    uint8_t lt_var_requested_value_bit_get(
            const lt_var_t * variable );

    /**
     * Get requested value of a variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return value of the variable.
     */
    int32_t lt_var_requested_value_number_get(
            const lt_var_t * variable );

    /**
     * Get length of requested value of string typed variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return length of a string typed variable.
     */
    uint32_t lt_var_requested_value_string_length_get(
            const lt_var_t * variable );

    /**
     * Get requested value of a variable.
     *
     * @param variable The variable whose value should be obtained.
     * @return string data of the string typed variable.
     */
    const uint8_t * lt_var_requested_value_string_get(
            const lt_var_t * variable );

    /**
     * Set monitor status of a variable from the backend side (the real
     * monitor status).
     *
     * @param variable The variable whose monitor status shall be changed.
     * @param monitor Monitor/Unmonitor the variable.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_real_monitor_set(
            lt_var_t * variable,
            int monitor );

    /**
     * Set monitor status of a variable from the frontend side (the
     * requested monitor state).
     *
     * Note that the monitor value different from zero will increment
     * internal counter of requested monitor flag, value equal to zero will
     * decement it. This way an application can trace how many clients of
     * this variable required the monitoring of the variable and the
     * variable will be still monitored as long as at least one client will
     * request monitoring.
     *
     * Note also that you can't get the requested monitor value to be less
     * than zero by repetitively calling requested_monitor_set( 0 ).
     *
     * @param variable The variable whose monitor status shall be changed.
     * @param monitor Monitor/Unmonitor the variable.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_requested_monitor_set(
            lt_var_t * variable,
            int monitor );

    /**
     * Get real monitor status of a variable (whether the variable is really
     * monitored).
     *
     * @param variable The variable whose monitor status should be obtained.
     * @return monitor status of the variable.
     */
    int lt_var_real_monitor_get(
            const lt_var_t * variable );

    /**
     * Get requested monitor status of a variable (whether the
     * variable should be monitored).
     *
     * Note that the returned number will represent how many times
     * requested_monitor_set( 1 ) has been invoked for this variable which
     * means how many clients requested the variable to be monitored.
     *
     * @param variable The variable whose monitor status should be obtained.
     * @return monitor status of the variable.
     */
    int lt_var_requested_monitor_get(
            const lt_var_t * variable );

    /**
     * Associate user data with a variable.
     *
     * @param variable The variable.
     * @param userdata Whatever you need.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_userdata_set(
            lt_var_t * variable,
            void * userdata );

    /**
     * Get user data associated with a variable.
     *
     * @param variable The variable.
     * @return User data of the variable.
     */
    void * lt_var_userdata_get(
            const lt_var_t * variable );

    /**
     * Has the real value been changed since the last call to
     * lt_var_real_value_sync?
     *
     * @param var The variable whose real value should be examined.
     * @return positive int when the variable's real value has been changed
     * synce the last call to lt_var_real_value_sync.
     */
    int lt_var_real_value_is_dirty( const lt_var_t * var );

    /**
     * Has the requested value been changed since the last call to
     * lt_var_requested_value_sync?
     *
     * @param var The variable whose requested value should be examined.
     * @return positive int when the variable's requested value has been
     * changed synce the last call to lt_var_requested_value_sync.
     */
    int lt_var_requested_value_is_dirty( const lt_var_t * var );

    /**
     * Has the real monitor been changed since the last call to
     * lt_var_real_value_sync?
     *
     * @param var The variable whose real monitor should be examined.
     * @return positive int when the variable's real monitor has been changed
     * synce the last call to lt_var_real_value_sync.
     */
    int lt_var_real_monitor_is_dirty( const lt_var_t * var );

    /**
     * Has the requested monitor been changed since the last call to
     * lt_var_requested_value_sync?
     *
     * @param var The variable whose requested monitor should be examined.
     * @return positive int when the variable's requested monitor has been
     * changed synce the last call to lt_var_requested_value_sync.
     */
    int lt_var_requested_monitor_is_dirty( const lt_var_t * var );

    /**
     * return pool out of which the given varset has been allocated.
     */
    apr_pool_t * lt_var_set_pool_get( const lt_var_set_t * src );

    /**
     * Create new var_set.
     *
     * @param maxfrontends The maximum number of frontends that will be
     * allowed to be registered with this var_set.
     * @param pool The pool to allocate varset out of, when NULL new one
     * will be allocated.
     * @return The newly created set or null.
     */
    lt_var_set_t * lt_var_set_create(
            unsigned maxfrontends,
            apr_pool_t * pool );

    /**
     * Lock the given varset.
     *
     * This function will lock mutex associated with given varset. Note that
     * no function in this library checks whether the mutex is locked, it's
     * up to the user to lock/unlock varsets appropriately when concurrent
     * access to varset is needed. This function therefore serves as a
     * helper not requiring you to allocate external mutex for each varset
     * explicitely.
     *
     * Note: This function can put its invoking thread to sleep.
     *
     * @param vset Varset whose mutex should be locked.
     * @return 1 if the lock has been obtained, 0 otherwise.
     */
    int lt_var_set_requested_values_lock( lt_var_set_t * vset );

    /**
     * Unlock the given varset.
     *
     * This function will unlock the mutex associated with given varset.
     * Note that no function in this library checks whether the mutex is
     * locked, it's up to the user to lock/unlock varsets appropriately when
     * concurrent access to varset is needed. This function therefore serves
     * as a helper not requiring you to allocate external mutex for each
     * varset explicitely.
     *
     * Note: This function will never put its invoking thread to sleep.
     *
     * @param vset Varset whose mutex should be unlocked.
     * @return 1 if the lock has been released, 0 otherwise.
     */
    int lt_var_set_requested_values_unlock( lt_var_set_t * vset );

    /**
    * Lock the given varset.
    *
    * This function will lock mutex associated with given varset. Note that
    * no function in this library checks whether the mutex is locked, it's
    * up to the user to lock/unlock varsets appropriately when concurrent
    * access to varset is needed. This function therefore serves as a
    * helper not requiring you to allocate external mutex for each varset
    * explicitely.
    *
    * Note: This function can put its invoking thread to sleep.
    *
    * @param vset Varset whose mutex should be locked.
    * @return 1 if the lock has been obtained, 0 otherwise.
    */
    int lt_var_set_real_values_lock( lt_var_set_t * vset );

    /**
    * Try to Lock the given varset.
    *
    * This function will try to lock mutex associated with given varset.
    * Note that no function in this library checks whether the mutex is
    * locked, it's up to the user to lock/unlock varsets appropriately when
    * concurrent access to varset is needed. This function therefore serves
    * as a helper not requiring you to allocate external mutex for each
    * varset explicitely.
    *
    * Note: This function will never put its invoking thread to sleep.
    *
    * @param vset Varset whose mutex should be locked.
    * @return 1 if the lock has been obtained, 0 otherwise.
    */
    int lt_var_set_real_values_try_lock( lt_var_set_t * vset );

    /**
    * Try to Lock the given varset.
    *
    * This function will try to lock mutex associated with given varset.
    * Note that no function in this library checks whether the mutex is
    * locked, it's up to the user to lock/unlock varsets appropriately when
    * concurrent access to varset is needed. This function therefore serves
    * as a helper not requiring you to allocate external mutex for each
    * varset explicitely.
    *
    * Note: This function will never put its invoking thread to sleep.
    *
    * @param vset Varset whose mutex should be locked.
    * @return 1 if the lock has been obtained, 0 otherwise.
    */
    int lt_var_set_requested_values_try_lock( lt_var_set_t * vset );

    /**
    * Unlock the given varset.
    *
    * This function will unlock the mutex associated with given varset.
    * Note that no function in this library checks whether the mutex is
    * locked, it's up to the user to lock/unlock varsets appropriately when
    * concurrent access to varset is needed. This function therefore serves
    * as a helper not requiring you to allocate external mutex for each
    * varset explicitely.
    *
    * Note: This function will never put its invoking thread to sleep.
    *
    * @param vset Varset whose mutex should be unlocked.
    * @return 1 if the lock has been released, 0 otherwise.
    */
    int lt_var_set_real_values_unlock( lt_var_set_t * vset );

    /**
     * Chain varsets together by setting source varset of this varset.
     *
     * Will connect all variables from the given varset with source varset
     * according to their name, so that real will be synced from source to
     * given var and requested from given var to source.
     *
     * When you pass in NULL source, vset will be disconnected from the
     * previously connected one. When you call this function two or more
     * times, the previous source will be forgotten and the latest given
     * used.
     *
     * @param vset The varset whose source varset should be set.
     * @param source Source varset.
     */
    lt_var_status_t lt_var_set_source_set( lt_var_set_t * vset,
            lt_var_set_t * source );

    /**
     * Return source varset of the given varset.
     *
     * @param vset The varset whose source varset should be set.
     */
    lt_var_set_t * lt_var_set_source_get( const lt_var_set_t * vset );

    /**
     * Sync the real values generated by the backend with the frontend
     * modules.
     *
     * All real values will propagate to all varsets that have the given
     * varset as a source (if any).
     *
     * note: this function can put its invoking thread to sleep.
     *
     * @param vset The varset whose frontend modules should be invoked.
     * @param timeout Timeout that will be passed to each frontend module func.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_set_real_values_sync(
            lt_var_set_t * vset, apr_interval_time_t timeout );

    /**
     * Sync the requested values generated by the frontend modules
     * with the backend.
     *
     * All requested values will propagate to the source varset of the given
     * varset (if any).
     *
     * note: this function can put its invoking thread to sleep.
     *
     * @param vset The varset whose backend module should be invoked.
     * @param timeout Timeout that will be passed to backend func.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_set_requested_values_sync(
            lt_var_set_t * vset, apr_interval_time_t timeout );

    /**
     * Wait till one of our targets synces requested values to our varset.
     *
     * Will put the invoking thread on sleep and wakes it up after someone
     * synces the requested values with us or timeout expires.
     *
     * Note that if we have no targets and therefore none can wake us up
     * this function will return immediately for safety reasons.
     *
     * Also note that you have to call lt_var_set_requested_values_lock()
     * before invoking this function. when this function returns
     * LT_CP_SUCCESS the varset has been signaled and is locked for you
     * again. otherwise the timeout expired.
     *
     * @param vset The varset we want to wait for.
     * @param timeout How log should we wait.
     */
    lt_var_status_t lt_var_set_wait_for_new_requested_values(
        lt_var_set_t * vset, apr_interval_time_t timeout );

    /**
     * Wait till our source synces its real values to our varset.
     *
     * Will put the invoking thread on sleep and wakes it up after the
     * source synces the real values with us or timeout expires.
     *
     * Note that if we have no source and therefore none can wake us up
     * this function will return immediately for safety reasons.
     *
     * Also note that you have to call lt_var_set_real_values_lock()
     * before invoking this function. when this function returns
     * LT_CP_SUCCESS the varset has been signaled and is locked for you
     * again. otherwise the timeout expired.
     *
     * @param vset The varset we want to wait for.
     * @param timeout How log should we wait.
     */
    lt_var_status_t lt_var_set_wait_for_new_real_values(
        lt_var_set_t * vset, apr_interval_time_t timeout );

    /**
     * Return the hash of variables whose real value has
     * changed since the last call to lt_var_set_real_values_sync.
     *
     * The hash is indexed by the string key as obtained from the variable's
     * name and contains pointer to the respective lt_var_t.
     *
     * @param vset The set of variables.
     * @return hash of varaibles with dirty real value.
     */
    apr_hash_t * lt_var_set_dirty_real_values_get(
            const lt_var_set_t * vset );

    /**
     * Return the hash of variables whose requested value has
     * changed since the last call to lt_var_set_requested_values_sync.
     *
     * The hash is indexed by the string key as obtained from the variable's
     * name and contains pointer to the respective lt_var_t.
     *
     * @param vset The set of variables.
     * @return hash of varaibles with dirty requested value.
     */
    apr_hash_t * lt_var_set_dirty_requested_values_get(
            const lt_var_set_t * vset );

    /**
     * Return the hash of variables whose real monitor state has
     * changed since the last call to lt_var_set_real_values_sync.
     *
     * The hash is indexed by the string key as obtained from the variable's
     * name and contains pointer to the respective lt_var_t.
     *
     * @param vset The set of variables.
     * @return hash of varaibles with dirty real monitor state.
     */
    apr_hash_t * lt_var_set_dirty_real_monitors_get(
            const lt_var_set_t * vset );

    /**
     * Return the hash of variables whose requested monitor state has
     * changed since the last call to lt_var_set_requested_values_sync.
     *
     * The hash is indexed by the string key as obtained from the variable's
     * name and contains pointer to the respective lt_var_t.
     *
     * @param vset The set of variables.
     * @return hash of varaibles with dirty requested monitor state.
     */
    apr_hash_t * lt_var_set_dirty_requested_monitors_get(
            const lt_var_set_t * vset );

    /**
     * Return the variables registered within the given set.
     *
     * The hash is indexed by the string key as obtained from the variable's
     * name and contains pointer to the respective lt_var_t.
     *
     * @param vset The set of variables.
     * @return hash of the variables.
     */
    apr_hash_t * lt_var_set_variables_get(
            const lt_var_set_t * vset );

    /**
     * Return the variable registered within the given set under the given
     * name.
     *
     * @param vset The set of variables.
     * @param name The name of the variable.
     * @return requested variable or null.
     */
    lt_var_t * lt_var_set_variable_get(
            const lt_var_set_t * vset, const char * name );

    /**
     * Put a variable to the set.
     *
     * @param vset The varset to register the variable with.
     * @param name The name of the given variable. Will be duplicated
     * internally.
     * @param variable The variable to be registered.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_set_variable_register( lt_var_set_t * vset,
            const char * name,
            lt_var_t * variable );

    /**
     * Remove a variable from the set.
     *
     * @param vset The varset to unregister the variable with.
     * @param name The name of the given variable. The variable must
     * exist within the varset or else LT_VAR_INVALID_ARG is returned.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_set_variable_unregister(
            lt_var_set_t * vset, const char * name );

    /**
     * Set the backend of the given varset.
     *
     * There can always be only one backend responsible for a varset,
     * therefore this method always overrides the previously set backend.
     *
     * @param vset The varset to register the backend with.
     * @param backend The backend to be used by the varset.
     * @param user_arg The user defined argument that will be passed to the
     * backend func when syncing requested values.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_set_backend_set( lt_var_set_t * vset,
            lt_var_set_backend_func_t backend, void * user_arg );

    /**
     * Register new frontend module with the given set.
     *
     * There can always be more than one fronted associated with a varset,
     * therefore this method always adds the given frontend module to the
     * FIFO list of modules. Modules are invoked in the order in which they
     * were registered.
     *
     * @param vset The varset to register the frontend with.
     * @param frontend The frontend module to be used by the varset.
     * @param user_arg The user defined argument that will be passed to the
     * frontend func when syncing real values.
     * @return LT_VAR_SUCCESS or LT_VAR_INVALID_ARG.
     */
    lt_var_status_t lt_var_set_frontend_register( lt_var_set_t * vset,
            lt_var_set_frontend_func_t frontend, void * user_arg );

    /**
     * Set the logging hook of the given varset.
     *
     * There can always be only one loggin hook in a varset, therefore this
     * method always overrides the previously set loggin hook.
     *
     * @param vset The varset to register the loggin hook with.
     * @param lf The logging hook to be used by the varset.
     * @param user_data Any data the logging hook may need. They are passed
     * to the lt_var_set_logging_func_t when called.
     */
    lt_var_status_t lt_var_set_logging_set( lt_var_set_t * vset,
            lt_var_set_logging_func_t lf, void* user_data );

#ifdef __cplusplus
}
#endif

#endif /* _LT_VARIABLES_H */

/** @} */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: variables.h 1541 2006-03-21 13:04:52Z modesto $
 */
