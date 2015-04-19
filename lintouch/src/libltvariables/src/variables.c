/* $Id: variables.c 1774 2006-05-31 13:17:16Z hynek $
 *
 *   FILE: variables.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 18 August 2003
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

#include "lt/variables/variables.h"

#include <apr_hash.h>
#include <apr_strings.h>
#include <apr_thread_mutex.h>
#include <apr_thread_cond.h>

#include <stdio.h>
#include <stdlib.h>

typedef enum _lt_var_dirty_type {
    LT_VAR_DIRTY_REAL_VALUES          =   0,
    LT_VAR_DIRTY_REQUESTED_VALUES,
    LT_VAR_DIRTY_REAL_MONITORS,
    LT_VAR_DIRTY_REQUESTED_MONITORS,
    LT_VAR_DIRTY_TOTAL,
} _lt_var_dirty_type;

/* internaly set parent varset of the variable */
static lt_var_status_t _lt_var_parent_set( lt_var_t * var,
        const lt_var_set_t * parent );
const lt_var_set_t * _lt_var_parent_get( const lt_var_t * var );

/* internaly set source of the variable */
static lt_var_status_t _lt_var_source_set( lt_var_t * var,
        lt_var_t * source );
/* internaly register target_var as a target of the given variable */
static lt_var_status_t _lt_var_target_add( lt_var_t * var,
        lt_var_set_t * target, lt_var_t * target_var );

/* internaly unregister target_var as a target of the given variable */
static lt_var_status_t _lt_var_target_remove( lt_var_t * var,
        lt_var_set_t * target, lt_var_t * target_var );

/* internaly sync real value of a variable to given varset */
static int _lt_var_real_value_sync( lt_var_t * variable,
        const lt_var_set_t * to_varset );
/* internaly clear dirty real flags of a given variable */
static int _lt_var_dirty_real_flags_clear( lt_var_t * variable );
/* internaly sync req value of a variable to its source */
static int _lt_var_requested_value_sync( lt_var_t * variable );

/* internaly set the name of the variable */
static lt_var_status_t _lt_var_name_set( lt_var_t * var,
        const char * name );
static const char * _lt_var_name_get( const lt_var_t * var );

/* register given variable as being dirty within it's parent varset */
static int _lt_var_dirty_register( lt_var_t * var,
        _lt_var_dirty_type type );

/* internaly register varset as being target of another varset */
static lt_var_status_t _lt_var_set_target_add( lt_var_set_t * vset,
        lt_var_set_t * target );

/* internaly unregister varset as being target of another varset */
static lt_var_status_t _lt_var_set_target_remove( lt_var_set_t * vset,
        lt_var_set_t * target );

/* clean out internal pool of a varset which holds dirty real
 * values/monitors. */
static int _lt_var_set_real_pool_recreate( lt_var_set_t * varset );

/* clean out internal pool of a varset which holds dirty requested
 * values/monitors. */
static int _lt_var_set_requested_pool_recreate( lt_var_set_t * varset );

struct lt_var_t {
    apr_pool_t * pool;
    apr_pool_t * rlpool;
    apr_pool_t * rqpool;

    const char * name;
    const lt_var_set_t * parent;

    lt_io_value_type_e type;

    lt_io_value_t * real_value;
    lt_io_value_t * requested_value;

    int real_monitor;
    int requested_monitor;

    lt_var_t * source;
    /* all vars that have us as a source,
     * indexed by their parent var_set_t * */
    apr_hash_t * targets;

    int dirty[ LT_VAR_DIRTY_TOTAL ];

    /* user data associated with the variable */
    void * userdata;
};

/* keep frontend/backend func and its registered user arg. */
typedef struct _lt_fb_func_t {
    void * user_arg;
    union {
        lt_var_set_frontend_func_t frontend;
        lt_var_set_backend_func_t backend;
    } func;
} _lt_fb_func_t;

/* the first 3 * sizeof(pointer) bytes are used as a key to hash table,
 * that's why I use this bloody pool pointers hack */
struct lt_var_set_t {
    apr_pool_t * pool;

    /* used to hold hashes for dirty real values */
    apr_pool_t * rl_pool;
    /* used to hold hashes for dirty requested values */
    apr_pool_t * rq_pool;

    apr_hash_t * variables;

    lt_var_set_t * source;
    /* all varsets that have us as a source, indexed by var_set_t * */
    apr_hash_t * targets;

    _lt_fb_func_t backend;
    unsigned nfrontends;
    unsigned tfrontends;
    _lt_fb_func_t * frontends;

    /* holds dirty variables */
    apr_hash_t * dirty[ LT_VAR_DIRTY_TOTAL ];

    /* mutex associated with real values */
    apr_thread_mutex_t * mutex_real;
    /* mutex associated with requested values */
    apr_thread_mutex_t * mutex_requested;

    /* associated condition_var signaling real_values changed */
    apr_thread_cond_t * cond_real;
    /* associated condition_var signaling requested_values changed */
    apr_thread_cond_t * cond_requested;

    /* logging hook */
    lt_var_set_logging_func_t logging;
    void* logging_data;
};

apr_pool_t * lt_var_pool_get( const lt_var_t * src )
{
    if( src == NULL ) return NULL;
    return src->pool;
}

#define CREATE_POOL_IF_NULL(p)  if((p)==NULL)apr_pool_create(&(p),NULL)

lt_var_t * lt_var_create( lt_io_value_type_e type, int monitored,
        apr_pool_t * pool )
{
    lt_io_value_t * rq = NULL, * rl = NULL;
    lt_var_t * variable = NULL;

    CREATE_POOL_IF_NULL(pool);

    variable =( lt_var_t * )
        apr_pcalloc( pool, sizeof( lt_var_t ) );

    variable->pool = pool;

    variable->type = type;

    apr_pool_create( &( variable )->rlpool, pool );
    apr_pool_create( &( variable )->rqpool, pool );

    variable->name = NULL;

    switch( type ) {
        case LT_IO_TYPE_NULL:
            rq = lt_io_value_create( variable->rqpool );
            rl = lt_io_value_create( variable->rlpool );
            break;
        case LT_IO_TYPE_BIT:
            rq = lt_io_value_bit_create( 0, variable->rqpool );
            rl = lt_io_value_bit_create( 0, variable->rlpool );
            break;
        case LT_IO_TYPE_NUMBER:
            rq = lt_io_value_number_create( 0, variable->rqpool );
            rl = lt_io_value_number_create( 0, variable->rlpool );
            break;
        case LT_IO_TYPE_STRING:
            rq = lt_io_value_string_create2( "", variable->rqpool );
            rl = lt_io_value_string_create2( "", variable->rlpool );
            break;
        case LT_IO_TYPE_SEQUENCE:
            rq = lt_io_value_sequence_create( 0, variable->rqpool );
            rl = lt_io_value_sequence_create( 0, variable->rlpool );
            break;
    }
    variable->real_value = rl;
    variable->requested_value = rq;

    variable->real_monitor = monitored;
    variable->requested_monitor = monitored;

    variable->source = NULL;
    variable->targets = apr_hash_make( pool );

    /* make variable dirty by default */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REAL_VALUES );
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REQUESTED_VALUES );
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REAL_MONITORS );
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REQUESTED_MONITORS );

    return variable;
}

lt_io_value_type_e lt_var_type_get( const lt_var_t * src )
{
    if( src == NULL ) return LT_IO_TYPE_NULL;
    return src->type;
}

lt_var_status_t lt_var_real_value_set( lt_var_t * variable,
        const lt_io_value_t * value )
{
    if( variable == NULL ) return LT_VAR_INVALID_ARG;
    if( value == NULL ) return LT_VAR_INVALID_ARG;

    if( variable->type != lt_io_value_type_get( value ) )
        return LT_VAR_INVALID_ARG;

    if( lt_io_value_equals( value, variable->real_value ) ) {
        return LT_VAR_SUCCESS;
    }

    /* set the real value by dupping the given value */
    apr_pool_clear( variable->rlpool );
    variable->real_value = lt_io_value_deepcopy( value,
            variable->rlpool );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REAL_VALUES );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_requested_value_set(
        lt_var_t * variable,
        const lt_io_value_t * value ) {

    if( variable == NULL ) return LT_VAR_INVALID_ARG;
    if( value == NULL ) return LT_VAR_INVALID_ARG;

    if( variable->type != lt_io_value_type_get( value ) )
        return LT_VAR_INVALID_ARG;

    if( lt_io_value_equals( value, variable->requested_value ) ) {
        return LT_VAR_SUCCESS;
    }

    /* set the requested value by dupping the given value */
    apr_pool_clear( variable->rqpool );
    variable->requested_value = lt_io_value_deepcopy( value,
            variable->rqpool );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REQUESTED_VALUES );

    return LT_VAR_SUCCESS;
}

const lt_io_value_t * lt_var_real_value_get(
        const lt_var_t * variable ) {

    if( variable == NULL ) return 0;

    return variable->real_value;
}

const lt_io_value_t * lt_var_requested_value_get(
        const lt_var_t * variable ) {

    if( variable == NULL ) return 0;

    return variable->requested_value;
}

lt_var_status_t lt_var_real_value_bit_set(
        lt_var_t * variable,
        uint8_t v )
{
    if( variable == NULL || variable->type != LT_IO_TYPE_BIT ) {
        return LT_VAR_INVALID_ARG;
    }
    lt_io_value_bit_set( variable->real_value, v );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REAL_VALUES );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_real_value_number_set(
        lt_var_t * variable,
        int32_t v )
{
    if( variable == NULL || variable->type != LT_IO_TYPE_NUMBER ) {
        return LT_VAR_INVALID_ARG;
    }
    lt_io_value_number_set( variable->real_value, v );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REAL_VALUES );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_real_value_string_set(
        lt_var_t * variable,
        const uint8_t * v, uint32_t len )
{
    if( variable == NULL || variable->type != LT_IO_TYPE_STRING ) {
        return LT_VAR_INVALID_ARG;
    }
    lt_io_value_string_set( variable->real_value, v, len );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REAL_VALUES );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_real_value_string_set2(
        lt_var_t * variable,
        const uint8_t * v )
{
    if( variable == NULL || variable->type != LT_IO_TYPE_STRING ) {
        return LT_VAR_INVALID_ARG;
    }
    lt_io_value_string_set( variable->real_value, v, strlen( v ) );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REAL_VALUES );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_requested_value_bit_set(
        lt_var_t * variable,
        uint8_t v )
{
    if( variable == NULL || variable->type != LT_IO_TYPE_BIT ) {
        return LT_VAR_INVALID_ARG;
    }
    lt_io_value_bit_set( variable->requested_value, v );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REQUESTED_VALUES );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_requested_value_number_set(
        lt_var_t * variable,
        int32_t v )
{
    if( variable == NULL || variable->type != LT_IO_TYPE_NUMBER ) {
        return LT_VAR_INVALID_ARG;
    }
    lt_io_value_number_set( variable->requested_value, v );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REQUESTED_VALUES );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_requested_value_string_set(
        lt_var_t * variable,
        const uint8_t * v, uint32_t len )
{
    if( variable == NULL || variable->type != LT_IO_TYPE_STRING ) {
        return LT_VAR_INVALID_ARG;
    }
    lt_io_value_string_set( variable->requested_value, v, len );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REQUESTED_VALUES );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_requested_value_string_set2(
        lt_var_t * variable,
        const uint8_t * v )
{
    if( variable == NULL || variable->type != LT_IO_TYPE_STRING ) {
        return LT_VAR_INVALID_ARG;
    }
    lt_io_value_string_set( variable->requested_value, v, strlen( v ) );

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REQUESTED_VALUES );

    return LT_VAR_SUCCESS;
}

uint8_t lt_var_real_value_bit_get(
        const lt_var_t * variable )
{
    return lt_io_value_bit_get( variable->real_value );
}

int32_t lt_var_real_value_number_get(
        const lt_var_t * variable )
{
    return lt_io_value_number_get( variable->real_value );
}

uint32_t lt_var_real_value_string_length_get(
        const lt_var_t * variable )
{
    return lt_io_value_string_length_get( variable->real_value );
}

const uint8_t * lt_var_real_value_string_get(
        const lt_var_t * variable )
{
    return lt_io_value_string_get( variable->real_value );
}

uint8_t lt_var_requested_value_bit_get(
        const lt_var_t * variable )
{
    return lt_io_value_bit_get( variable->requested_value );
}

int32_t lt_var_requested_value_number_get(
        const lt_var_t * variable )
{
    return lt_io_value_number_get( variable->requested_value );
}

uint32_t lt_var_requested_value_string_length_get(
        const lt_var_t * variable )
{
    return lt_io_value_string_length_get( variable->requested_value );
}

const uint8_t * lt_var_requested_value_string_get(
        const lt_var_t * variable )
{
    return lt_io_value_string_get( variable->requested_value );
}

lt_var_status_t lt_var_real_monitor_set(
        lt_var_t * variable,
        int monitor ) {

    if( variable == NULL ) return LT_VAR_INVALID_ARG;

    if( monitor ) monitor = 1;

    if( monitor == variable->real_monitor ) return LT_VAR_SUCCESS;

    /* set the real monitor */
    variable->real_monitor = monitor;

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REAL_MONITORS );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_requested_monitor_set(
        lt_var_t * variable,
        int monitor ) {

    if( variable == NULL ) return LT_VAR_INVALID_ARG;

    if( monitor ) monitor = 1;

    if( monitor ) {
        variable->requested_monitor ++;
    } else {
        variable->requested_monitor --;

        if( variable->requested_monitor < 0 ) {
            /* fprintf( stderr,  */
            /*         "libltvariables: requested_monitor_set has " */
            /*         "gone less than zero\n" ); */
            variable->requested_monitor = 0;
        }
    }

    /* add variable to the hash of dirty variables of its all parents */
    _lt_var_dirty_register( variable, LT_VAR_DIRTY_REQUESTED_MONITORS );

    return LT_VAR_SUCCESS;
}

int lt_var_real_monitor_get(
        const lt_var_t * variable ) {

    if( variable == NULL ) return 0;

    return variable->real_monitor;
}

int lt_var_requested_monitor_get(
        const lt_var_t * variable ) {

    if( variable == NULL ) return 0;

    return variable->requested_monitor;
}

lt_var_status_t lt_var_userdata_set(
        lt_var_t * variable, void * userdata )
{
    if( variable == NULL ) return LT_VAR_INVALID_ARG;
    variable->userdata = userdata;
    return LT_VAR_SUCCESS;
}

void * lt_var_userdata_get( const lt_var_t * variable )
{
    if( variable == NULL ) return NULL;
    return variable->userdata;
}

int lt_var_real_value_is_dirty( const lt_var_t * var ) {
    if( var == NULL ) return 0;
    return var->dirty[ LT_VAR_DIRTY_REAL_VALUES ];
}

int lt_var_requested_value_is_dirty( const lt_var_t * var ) {
    if( var == NULL ) return 0;
    return var->dirty[ LT_VAR_DIRTY_REQUESTED_VALUES ];
}

int lt_var_real_monitor_is_dirty( const lt_var_t * var ) {
    if( var == NULL ) return 0;
    return var->dirty[ LT_VAR_DIRTY_REAL_MONITORS ];
}

int lt_var_requested_monitor_is_dirty( const lt_var_t * var ) {
    if( var == NULL ) return 0;
    return var->dirty[ LT_VAR_DIRTY_REQUESTED_MONITORS ];
}

apr_pool_t * lt_var_set_pool_get( const lt_var_set_t * src ) {
    if( src == NULL ) return NULL;
    return src->pool;
}

lt_var_set_t * lt_var_set_create(
        unsigned maxfrontends, apr_pool_t * pool ) {

    lt_var_set_t * varset = NULL;

    CREATE_POOL_IF_NULL(pool);

    varset =( lt_var_set_t * )
        apr_pcalloc( pool, sizeof( lt_var_set_t ) );

    varset->pool = pool;

    apr_pool_create( &varset->rl_pool, pool );
    apr_pool_create( &varset->rq_pool, pool );

    varset->variables = apr_hash_make( pool );

    varset->nfrontends = 0;
    varset->tfrontends = maxfrontends;
    varset->frontends =
        apr_pcalloc( pool,
                maxfrontends * sizeof( _lt_fb_func_t ) );

    varset->logging = NULL;
    varset->logging_data = NULL;

    _lt_var_set_real_pool_recreate( varset );
    _lt_var_set_requested_pool_recreate( varset );

    varset->source = NULL;
    varset->targets = apr_hash_make( pool );

    if( APR_SUCCESS != apr_thread_mutex_create(
                    &(varset->mutex_real),
            APR_THREAD_MUTEX_DEFAULT, pool ) ) {
        return NULL;
    }

    if( APR_SUCCESS != apr_thread_mutex_create(
                    &(varset->mutex_requested),
            APR_THREAD_MUTEX_DEFAULT, pool ) ) {
        return NULL;
    }

    if( APR_SUCCESS != apr_thread_cond_create(
                    &(varset->cond_real), pool ) ) {
        return NULL;
    }

    if( APR_SUCCESS != apr_thread_cond_create(
                    &(varset->cond_requested), pool ) ) {
        return NULL;
    }

    return varset;
  }

int lt_var_set_requested_values_lock( lt_var_set_t * vset ) {
    int rv;
    /* printf(">>>0x%x: lock_requested\n", vset); */
    rv = ( APR_SUCCESS ==
            apr_thread_mutex_lock( vset->mutex_requested ) );
    /* printf("<<<0x%x: locked_requested\n", vset); */
    return rv;
}

int lt_var_set_requested_values_try_lock( lt_var_set_t * vset ) {
    int rv;
    /* printf(">>>0x%x: try_lock_requested\n", vset); */
    rv = !APR_STATUS_IS_EBUSY(
            apr_thread_mutex_trylock( vset->mutex_requested ) );
    /* printf("<<<0x%x: try_locked_requested? %d\n", vset, rv); */
    return rv;
}

int lt_var_set_requested_values_unlock( lt_var_set_t * vset ) {
    int rv;
    /* printf(">>>0x%x: unlock_requested\n", vset); */
    rv = ( APR_SUCCESS ==
            apr_thread_mutex_unlock( vset->mutex_requested ) );
    /* printf("<<<0x%x: unlocked_requested\n", vset); */
    return rv;
}

int lt_var_set_real_values_lock( lt_var_set_t * vset ) {
    int rv;
    /* printf(">>>0x%x: lock_real\n", vset); */
    rv =  ( APR_SUCCESS ==
            apr_thread_mutex_lock( vset->mutex_real ) );
    /* printf("<<<0x%x: locked_real\n", vset); */
    return rv;
}

int lt_var_set_real_values_try_lock( lt_var_set_t * vset ) {
    int rv;
    /* printf(">>>0x%x: try_lock_real\n", vset); */
    rv = !APR_STATUS_IS_EBUSY(
            apr_thread_mutex_trylock( vset->mutex_real ) );
    /* printf("<<<0x%x: try_locked_real? %d\n", vset, rv); */
    return rv;
}

int lt_var_set_real_values_unlock( lt_var_set_t * vset ) {
    int rv;
    /* printf(">>>0x%x: unlock_real\n", vset); */
    rv = ( APR_SUCCESS ==
            apr_thread_mutex_unlock( vset->mutex_real ) );
    /* printf("<<<0x%x: unlocked_real\n", vset); */
    return rv;
}

lt_var_status_t lt_var_set_source_set( lt_var_set_t * vset,
        lt_var_set_t * source ) {

    if( vset == NULL ) return LT_VAR_INVALID_ARG;

    /* printf("debug: >>> lt_var_set_source_set( 0x%x, 0x%x)...\n",
            (unsigned)vset, (unsigned)source ); */

    /* if we had a source, do safe disconnect */
    if( vset->source != NULL ) {

        /* remove us as a target of previous source */
        _lt_var_set_target_remove( vset->source, vset );

        /* and now for all variables do the same */
        {
            apr_pool_t * pool = NULL;
            apr_hash_index_t * ht = NULL;
            lt_var_t * var = NULL;
            lt_var_t * svar = NULL;
            const char * key;

            apr_pool_create( &pool, vset->pool );

            for( ht = apr_hash_first( pool, vset->variables ); ht != NULL;
                    ht = apr_hash_next( ht ) ) {

                /* for each of our variables, try to find svar with the same
                 * name in source varset */
                apr_hash_this( ht, (const void**)&key, NULL, (void **)&var );
                svar = (lt_var_t*) lt_var_set_variable_get(
                        vset->source, key );

                /* printf("debug: got myvar %s\n", key ); */

                /* and if both exist, disconnect them */
                if( var != NULL && svar != NULL ) {
                    /* printf("debug: resetting source of myvar %s\n", key ); */
                    _lt_var_source_set( var, NULL );
                    _lt_var_target_remove( svar, vset, var );
                }

            }

            apr_pool_destroy( pool );
        }
    }

    /* if we have a new source, do safe connect */
    if( source != NULL ) {

        /* register us as a target of source */
        _lt_var_set_target_add( source, vset );

        /* and now for all variables do the same */
        {
            apr_pool_t * pool = NULL;
            apr_hash_index_t * ht = NULL;
            lt_var_t * var = NULL;
            lt_var_t * svar = NULL;
            const char * key;

            apr_pool_create( &pool, vset->pool );

            for( ht = apr_hash_first( pool, vset->variables ); ht != NULL;
                    ht = apr_hash_next( ht ) ) {

                /* for each of our variables, try to find svar with the same
                 * name in source varset */
                apr_hash_this( ht, (const void**)&key, NULL, (void **)&var );
                svar = (lt_var_t*) lt_var_set_variable_get( source, key );

                /* printf("debug: got myvar %s\n", key ); */

                /* and if both exist, interconnect them */
                if( var != NULL && svar != NULL ) {
                    /* printf("debug: setting source of myvar %s\n", key ); */
                    _lt_var_source_set( var, svar );
                    _lt_var_target_add( svar, vset, var );
                }

            }

            apr_pool_destroy( pool );
        }
    }

    /* set our source */
    vset->source = source;

    /* printf("debug: <<< lt_var_set_source_set...\n"); */

    return LT_VAR_SUCCESS;
}

lt_var_set_t * lt_var_set_source_get( const lt_var_set_t * vset ) {

    if( vset == NULL ) return NULL;

    return vset->source;
}

lt_var_status_t lt_var_set_real_values_sync( lt_var_set_t * vset,
        apr_interval_time_t timeout ) {

    unsigned i = 0;

    if( vset == NULL ) return LT_VAR_INVALID_ARG;

    /* printf("debug: >>> lt_var_set_real_values_sync( 0x%x )...\n",
            (unsigned)vset ); */

#if 0   /*DEBUG output */
    {
        apr_pool_t * ipool = NULL;
        apr_hash_index_t * ht = NULL;
        void * key = NULL;
        int keylen = 0;
        void * value = NULL;

        apr_pool_create( &ipool, NULL );
        /* printf("debug: varset(0x%x)->targets contents\n", (unsigned)vset); */
        for( ht = apr_hash_first( ipool, vset->targets ); ht != NULL;
                ht = apr_hash_next( ht ) ) {
            apr_hash_this( ht, (const void**)&key, &keylen,
                    (void **)&value );
            /* printf("debug: key=0x%x, keylen=%d, value=0x%x\n",
                    key, keylen, value); */
        }
        apr_pool_destroy( ipool );
    }
#endif

    /* for each of our target varsets or at least once to clear dirty flags */
    {
        apr_pool_t * pool = NULL;
        apr_hash_index_t * tht = NULL;
        lt_var_set_t * target = NULL;
        lt_var_t * var = NULL;
        const char * key = NULL;
        int first_pass = 1;

        apr_pool_create( &pool, vset->pool );

        // call logging_hook
        if( vset->logging != NULL ) {
            vset->logging( vset, 1, vset->logging_data );
        }

        /* transfer dirty real values of our variables to their targets */
        for( tht = apr_hash_first( pool, vset->targets );
                tht != NULL || first_pass;
                tht = apr_hash_next( tht ) ) {

            apr_hash_index_t * ht = NULL;

            /* only lock target varset if we actually have a target varset,
             * otherwise let the sync pass at least once to clear dirty_real
             * flags */
            if( tht != NULL ) {
                apr_hash_this( tht, NULL, NULL, (void **) &target );
                /* printf("debug: going to sync_real "
                   "all variables with varset 0x%x\n",
                   (unsigned)target ); */

                lt_var_set_real_values_lock( target );
            }

            for( ht = apr_hash_first( pool, vset->variables ); ht != NULL;
                    ht = apr_hash_next( ht ) ) {

                apr_hash_this( ht, (const void **)&key, NULL, (void **) &var );

                /* printf("debug: going to sync_real "
                        "variable %s with varset 0x%x\n",
                        key, (unsigned)target ); */

                _lt_var_real_value_sync( var, target );

            }

            if( tht ) {
                lt_var_set_real_values_unlock( target );
                /*  signal the threads waiting on target to wake up */
                apr_thread_cond_broadcast( target->cond_real );
           }

            /*  when we have no targets, simply end */
            if( tht == NULL ) break;

            /*  when we have targets, clear our pass-at-least-once indicator
             *  */
            first_pass = 0;
        }

        /* and now clear all dirty flags on all our variables */
        for( tht = apr_hash_first( pool, vset->variables ); tht != NULL;
                tht = apr_hash_next( tht ) ) {

            apr_hash_this( tht, (const void **)&key, NULL, (void **) &var );

            /* printf("debug: going to dirty_clear "
               "variable %s \n",
               key ); */

            _lt_var_dirty_real_flags_clear( var );

        }

        apr_pool_destroy( pool );
    }

    /* invoke frontend callbacks */
    for( i = 0; i < vset->nfrontends; i ++ ) {

        if( vset->frontends[ i ].func.frontend != NULL ) {

            vset->frontends[ i ].func.frontend(
                    vset,
                    vset->frontends[ i ].user_arg,
                    timeout );
        }
    }

    /* clear dirty values, they have been synced */
    _lt_var_set_real_pool_recreate( vset );

    /* printf("debug: <<< lt_var_set_real_values_sync...\n"); */

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_set_requested_values_sync(
        lt_var_set_t * vset, apr_interval_time_t timeout ) {

    if( vset == NULL ) return LT_VAR_INVALID_ARG;

    /* transfer our requested values to the possible source */
    {
        apr_pool_t * pool = NULL;
        apr_hash_index_t * ht = NULL;
        lt_var_t * var = NULL;
        const char * key = NULL;

        apr_pool_create( &pool, vset->pool );

        if( vset->source != NULL ) {
            lt_var_set_requested_values_lock( vset->source );
        }

        /* logging */
        if( vset->logging != NULL ) {
            vset->logging( vset, 0, vset->logging_data );
        }

        for( ht = apr_hash_first( pool, vset->variables ); ht != NULL;
                ht = apr_hash_next( ht ) ) {

            apr_hash_this( ht, (const void **) &key, NULL, (void **) &var );

            /* printf("debug: going to sync variable %s\n", key ); */

            _lt_var_requested_value_sync( var );

        }

        if( vset->source != NULL ) {

            lt_var_set_requested_values_unlock( vset->source );
            /*  signal the threads waiting on source to wake up */
            apr_thread_cond_broadcast( vset->source->cond_requested );
        }

        apr_pool_destroy( pool );
    }

    /* invoke backend callback */
    if( vset->backend.func.backend != NULL ) {
        vset->backend.func.backend( vset,
                vset->backend.user_arg, timeout );
    }

    /* clear dirty values, they have been synced */
    _lt_var_set_requested_pool_recreate( vset );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_set_wait_for_new_real_values(
        lt_var_set_t * vset, apr_interval_time_t timeout ) {

    if( vset == NULL ) return LT_VAR_INVALID_ARG;

    /* if we don't have a source, we simply timeout... */
    if( vset->source == NULL ) {
        apr_thread_mutex_unlock( vset->mutex_real );
        apr_sleep( timeout );
        apr_thread_mutex_lock( vset->mutex_real );
        return LT_VAR_TIMEUP;
    }

    /* if something is dirty, don't wait */
    if( apr_hash_count( vset->dirty[ LT_VAR_DIRTY_REAL_VALUES ] ) > 0 ) {
        return LT_VAR_SUCCESS;
    }

    /* otherwise wait for signal or timeout */
    if( APR_SUCCESS == apr_thread_cond_timedwait(
                        vset->cond_real,
                        vset->mutex_real, timeout) ) {
        return LT_VAR_SUCCESS;
    }

    /* if something is dirty, return success instead of timeup */
    if( apr_hash_count( vset->dirty[ LT_VAR_DIRTY_REAL_VALUES ] ) > 0 ) {
        return LT_VAR_SUCCESS;
    }

    return LT_VAR_TIMEUP;
}

lt_var_status_t lt_var_set_wait_for_new_requested_values(
        lt_var_set_t * vset, apr_interval_time_t timeout ) {

    if( vset == NULL ) return LT_VAR_INVALID_ARG;

    /* if we don't have a target, we simply timeout... */
    if( apr_hash_count( vset->targets ) == 0 ) {
        apr_thread_mutex_unlock( vset->mutex_requested );
        apr_sleep( timeout );
        apr_thread_mutex_lock( vset->mutex_requested );
        return LT_VAR_TIMEUP;
    }

    /* if something is dirty, don't wait */
    if( apr_hash_count(
                vset->dirty[ LT_VAR_DIRTY_REQUESTED_VALUES ] ) > 0 ) {
        return LT_VAR_SUCCESS;
    }

    /* otherwise wait for signal or timeout */
    if( APR_SUCCESS == apr_thread_cond_timedwait(
                        vset->cond_requested,
                        vset->mutex_requested, timeout) ) {
        return LT_VAR_SUCCESS;
    }

    /* if something is dirty, return success instead of timeout */
    if( apr_hash_count(
                vset->dirty[ LT_VAR_DIRTY_REQUESTED_VALUES ] ) > 0 ) {
        return LT_VAR_SUCCESS;
    }

    return LT_VAR_TIMEUP;
}

apr_hash_t * lt_var_set_dirty_real_values_get(
        const lt_var_set_t * vset ) {

    if( vset == NULL ) return NULL;

    return vset->dirty[ LT_VAR_DIRTY_REAL_VALUES ];
}

apr_hash_t * lt_var_set_dirty_requested_values_get(
        const lt_var_set_t * vset ) {

    if( vset == NULL ) return NULL;

    return vset->dirty[ LT_VAR_DIRTY_REQUESTED_VALUES ];
}

apr_hash_t * lt_var_set_dirty_real_monitors_get(
        const lt_var_set_t * vset ) {

    if( vset == NULL ) return NULL;

    return vset->dirty[ LT_VAR_DIRTY_REAL_MONITORS ];
}

apr_hash_t * lt_var_set_dirty_requested_monitors_get(
        const lt_var_set_t * vset ) {

    if( vset == NULL ) return NULL;

    return vset->dirty[ LT_VAR_DIRTY_REQUESTED_MONITORS ];
}

apr_hash_t * lt_var_set_variables_get(
        const lt_var_set_t * vset ) {

    if( vset == NULL ) return NULL;

    return vset->variables;
}

lt_var_t * lt_var_set_variable_get(
        const lt_var_set_t * vset, const char * name ) {

    if( vset == NULL ) return NULL;
    if( name == NULL ) return NULL;

    return( lt_var_t * ) apr_hash_get( vset->variables,
            ( name ), APR_HASH_KEY_STRING );
}

lt_var_status_t lt_var_set_variable_register( lt_var_set_t * vset,
        const char * name,
        lt_var_t * variable ) {

    char * key = NULL;

    if( vset == NULL ) return LT_VAR_INVALID_ARG;
    if( variable == NULL ) return LT_VAR_INVALID_ARG;
    if( name == NULL ) return LT_VAR_INVALID_ARG;

    key = apr_pstrdup( vset->pool, name );

    /* register this variable */
    apr_hash_set( vset->variables,
            ( key ), APR_HASH_KEY_STRING,
            ( void * ) variable );

    /* assign name to the variable */
    _lt_var_name_set( variable, key );

    /* register varset as a parent of this variable */
    _lt_var_parent_set( variable, vset );

    /* if we have source set, try to find name in source set and
     * interconnect variable with source */
    if( vset->source != NULL ) {
        lt_var_t * svar = lt_var_set_variable_get( vset->source, key );
        if( svar != NULL ) {
            _lt_var_source_set( variable, svar );
            _lt_var_target_add( svar, vset, variable );
        }
    }

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_set_variable_unregister( lt_var_set_t * vset,
        const char * name ) {

    lt_var_t * variable = NULL;

    if( vset == NULL ) return LT_VAR_INVALID_ARG;
    if( name == NULL ) return LT_VAR_INVALID_ARG;

    /* retrieve the variable from the varset using the variable name */
    variable = lt_var_set_variable_get( vset, name );
    if( variable == NULL ) return LT_VAR_INVALID_ARG;

    /* if we have source set, try to find name in source set and
     * disconnect the variable from its source */
    if( vset->source != NULL ) {
        lt_var_t * svar = lt_var_set_variable_get( vset->source, name );
        if( svar != NULL ) {
            _lt_var_source_set( variable, NULL );
            _lt_var_target_remove( svar, vset, variable );
        }
    }

    /* unregister varset as a parent of this variable */
    _lt_var_parent_set( variable, NULL );

    /* anonymize the variable */
    variable->name = NULL;

    /* remove the variable from the hash */
    apr_hash_set( vset->variables,
            name, APR_HASH_KEY_STRING,
            ( void * ) NULL );

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_set_backend_set( lt_var_set_t * vset,
        lt_var_set_backend_func_t backend, void * user_arg ) {

    if( vset == NULL ) return LT_VAR_INVALID_ARG;
    if( backend == NULL ) return LT_VAR_INVALID_ARG;

    vset->backend.func.backend = backend;
    vset->backend.user_arg = user_arg;

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_set_frontend_register( lt_var_set_t * vset,
        lt_var_set_frontend_func_t frontend, void * user_arg ) {

    if( vset == NULL ) return LT_VAR_INVALID_ARG;
    if( frontend == NULL ) return LT_VAR_INVALID_ARG;

    /* do we have a space for a frontend */
    if( vset->nfrontends + 1 >= vset->tfrontends )
        return LT_VAR_INVALID_ARG;

    vset->nfrontends ++;

    /* register frontend */
    vset->frontends[ vset->nfrontends ].func.frontend = frontend;
    vset->frontends[ vset->nfrontends ].user_arg = user_arg;

    return LT_VAR_SUCCESS;
}

lt_var_status_t lt_var_set_logging_set( lt_var_set_t * vset,
        lt_var_set_logging_func_t lf, void* user_data ) {

    if( vset == NULL ) return LT_VAR_INVALID_ARG;
    if( lf == NULL ) return LT_VAR_INVALID_ARG;

    /* set logging hook */
    vset->logging = lf;
    vset->logging_data = user_data;

    return LT_VAR_SUCCESS;
}

int _lt_var_dirty_register( lt_var_t * var,
        _lt_var_dirty_type type ) {

    const char * name = NULL;

    if( var == NULL ) return 0;

    /* make me dirty */
    var->dirty[ type ] = 1;

    name = _lt_var_name_get( var );

    /* fprintf( stderr,
     * "_lt_var_dirty_register: making var %s dirty type %d\n",
            name, type ); */

    if( name != NULL && var->parent != NULL ) {

        /* fprintf( stderr, "_lt_var_dirty_register: telling parent 0x%x\n",
            (unsigned)var->parent); */

        /* tell my parent varset I'm dirty */
        apr_hash_set( var->parent->dirty[ type ],
                ( name ), APR_HASH_KEY_STRING, var );
    }

    return 1;
}

lt_var_status_t _lt_var_set_target_add( lt_var_set_t * vset,
        lt_var_set_t * target ) {

    if( vset == NULL ) return LT_VAR_INVALID_ARG;
    if( target == NULL ) return LT_VAR_INVALID_ARG;

    apr_hash_set( vset->targets, target, 3*sizeof( target ), target );

    return LT_VAR_SUCCESS;
}

lt_var_status_t _lt_var_set_target_remove( lt_var_set_t * vset,
        lt_var_set_t * target ) {

    if( vset == NULL ) return LT_VAR_INVALID_ARG;
    if( target == NULL ) return LT_VAR_INVALID_ARG;

    /* null placed in a hash removes the entry */
    apr_hash_set( vset->targets, target, 3*sizeof( target ), NULL );

    return LT_VAR_SUCCESS;
}

int _lt_var_set_real_pool_recreate( lt_var_set_t * varset ) {

    if( varset == NULL ) return 0;

    apr_pool_clear( varset->rl_pool );

    ( varset )->dirty[ LT_VAR_DIRTY_REAL_VALUES ] =
        apr_hash_make( varset->rl_pool );
    ( varset )->dirty[ LT_VAR_DIRTY_REAL_MONITORS ] =
        apr_hash_make( varset->rl_pool );

    return 1;
}

int _lt_var_set_requested_pool_recreate( lt_var_set_t * varset ) {

    if( varset == NULL ) return 0;

    apr_pool_clear( varset->rq_pool );

    ( varset )->dirty[ LT_VAR_DIRTY_REQUESTED_VALUES ] =
        apr_hash_make( varset->rq_pool );
    ( varset )->dirty[ LT_VAR_DIRTY_REQUESTED_MONITORS ] =
        apr_hash_make( varset->rq_pool );

    return 1;
}

lt_var_status_t _lt_var_source_set(
        lt_var_t * var, lt_var_t * source ) {

    if( var == NULL ) return LT_VAR_INVALID_ARG;

    var->source = source;

    return LT_VAR_SUCCESS;
}

lt_var_status_t _lt_var_target_add( lt_var_t * var,
        lt_var_set_t * target, lt_var_t * target_var ) {

    if( var == NULL ) return LT_VAR_INVALID_ARG;
    if( target == NULL ) return LT_VAR_INVALID_ARG;

    /* printf("debug: >>> _lt_var_target_add( var=0x%x, target_set=0x%x, "
            " target_var=0x%x )\n",
            (unsigned)var, (unsigned)target, (unsigned)target_var); */

    apr_hash_set( var->targets, target, 3*sizeof( target ), target_var );

    return LT_VAR_SUCCESS;
}

lt_var_status_t _lt_var_target_remove( lt_var_t * var,
        lt_var_set_t * target, lt_var_t * target_var ) {

    if( var == NULL ) return LT_VAR_INVALID_ARG;
    if( target == NULL ) return LT_VAR_INVALID_ARG;

    /* printf("debug: >>> _lt_var_target_remove( var=0x%x, target_set=0x%x, "
            " target_var=0x%x )\n",
            (unsigned)var, (unsigned)target, (unsigned)target_var); */

    /* null removes the entry from hash table */
    apr_hash_set( var->targets, target, 3*sizeof( target ), NULL );

    return LT_VAR_SUCCESS;
}

lt_var_status_t _lt_var_parent_set(
        lt_var_t * var, const lt_var_set_t * parent ) {

    if( var == NULL ) return LT_VAR_INVALID_ARG;
    if( parent == NULL ) return LT_VAR_INVALID_ARG;

    var->parent = parent;

    return LT_VAR_SUCCESS;
}

const lt_var_set_t * _lt_var_parent_get(
        const lt_var_t * var ) {

    if( var == NULL ) return NULL;

    return var->parent;
}

lt_var_status_t _lt_var_name_set(
        lt_var_t * var, const char * name ) {

    if( var == NULL ) return LT_VAR_INVALID_ARG;
    if( name == NULL ) return LT_VAR_INVALID_ARG;

    var->name = name;

    return LT_VAR_SUCCESS;
}

const char * _lt_var_name_get(
        const lt_var_t * var ) {

    if( var == NULL ) return NULL;

    return var->name;
}

int _lt_var_real_value_sync( lt_var_t * variable,
        const lt_var_set_t * to_varset ) {

    lt_var_t * target = NULL;

    if( variable == NULL ) return 0;

    /* only sync if the variable is dirty */
    if( lt_var_real_value_is_dirty( variable ) == 0 ) return 0;

    /* printf("debug: >>> _lt_var_real_value_sync( var=0x%x, "
     * "to_vset=0x%x )\n",
            (unsigned)variable, (unsigned)to_varset); */

    if( to_varset != NULL ) {
#if 0   /*DEBUG output */
        {
            apr_pool_t * ipool = NULL;
            apr_hash_index_t * ht = NULL;
            void * key = NULL;
            int keylen = 0;
            void * value = NULL;

            apr_pool_create( &ipool, NULL );
            /* printf("debug: var(0x%x)->targets contents\n",
             * (unsigned)variable); */
            for( ht = apr_hash_first( ipool, variable->targets ); ht != NULL;
                    ht = apr_hash_next( ht ) ) {
                apr_hash_this( ht, (const void**)&key, &keylen,
                        (void **)&value );
                /* printf("debug: key=0x%x, keylen=%d, value=0x%x\n",
                        key, keylen, value); */
            }
            apr_pool_destroy( ipool );
        }
#endif

        target = ( lt_var_t * ) apr_hash_get( variable->targets,
                to_varset, 3*sizeof( to_varset ) );

        /* printf("debug: lt_var_real_value_set to target var=0x%x\n",
                (unsigned)target); */

        if( target == NULL ) return 0;

        lt_var_real_value_set( target,
                lt_var_real_value_get( variable ) );

        /* if our target monitors, give him the real monitor status */
        if( lt_var_requested_monitor_get( target ) ) {
            lt_var_real_monitor_set( target,
                    lt_var_real_monitor_get( variable ) );
        } else {
            /* otherwise drop target monitor to zero,
             * regardless the real monitor status */
            lt_var_real_monitor_set( target, 0 );
        }

    }

    /* printf("debug: <<< _lt_var_real_value_sync"); */

    return 1;

}

int _lt_var_dirty_real_flags_clear( lt_var_t * variable ) {

    if( variable == NULL ) return 0;

    /* unset dirty flags */
    variable->dirty[ LT_VAR_DIRTY_REAL_VALUES ] = 0;
    variable->dirty[ LT_VAR_DIRTY_REAL_MONITORS ] = 0;

    return 0;
}

int _lt_var_requested_value_sync( lt_var_t * variable ) {

    if( variable == NULL ) return 0;

    /* only sync if the variable is dirty */
    if( lt_var_requested_value_is_dirty( variable ) == 0 ) return 0;

    /* unset dirty flags */
    variable->dirty[ LT_VAR_DIRTY_REQUESTED_VALUES ] = 0;
    variable->dirty[ LT_VAR_DIRTY_REQUESTED_MONITORS ] = 0;

    if( variable->source == NULL ) return 0;

    lt_var_requested_value_set( variable->source,
            lt_var_requested_value_get( variable ) );

    /* if our monitor status has changed, forward it upstream */
    if( variable->dirty[ LT_VAR_DIRTY_REQUESTED_MONITORS ] ) {
        lt_var_requested_monitor_set( variable->source,
                lt_var_requested_monitor_get( variable ) );
    }

    return 1;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: variables.c 1774 2006-05-31 13:17:16Z hynek $
 */
