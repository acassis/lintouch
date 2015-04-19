/* $Id:$
 *
 *   FILE: null.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 23 August 2005
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

#include <stdio.h>
#include <apr_time.h>

#include "lt/server/plugin.h"
#include "lt/server/logger.h"

#include "config.h"

/* automatically define mandatory plugin symbols */
LT_SERVER_PLUGIN_EXPORT;

/* LOOPBACK PLUGIN METHODS */

lt_server_plugin_info_t *
PLUGIN_EXPORT LT_SERVER_PLUGIN_CREATE( apr_pool_t * pool );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin );

/* LOOPBACK PLUGIN SPECIFIC DATA */

typedef struct {
    apr_interval_time_t sleep;
    lt_var_set_t * varset;
} my_plugin_data;

/**
 * Sleep for given time while monitoring the "should exit" flag. Optionaly
 * report how many seconds of sleep are left to given counter.
 */
static int _my_interruptible_sleep( lt_server_plugin_t * plugin,
        apr_interval_time_t sleep, lt_var_t * counter );

/* METHOD BODIES */

lt_server_plugin_info_t *
LT_SERVER_PLUGIN_CREATE( apr_pool_t * pool )
{
    apr_hash_t * props = apr_hash_make( pool );
    apr_hash_t * varprops = apr_hash_make( pool );
    apr_hash_t * bitprops = apr_hash_make( pool );
    apr_hash_t * numprops = apr_hash_make( pool );
    apr_hash_t * strprops = apr_hash_make( pool );
    my_plugin_data * data = apr_pcalloc( pool, sizeof( my_plugin_data ) );

    /* plugin properties */
    /* none for now */

    /* bit var properties */
    apr_hash_set( bitprops, "address", APR_HASH_KEY_STRING, (void*)"" );
    apr_hash_set( bitprops, "description", APR_HASH_KEY_STRING, (void*)"" );

    /* num var properties */
    apr_hash_set( numprops, "address", APR_HASH_KEY_STRING, (void*)"" );
    apr_hash_set( numprops, "description", APR_HASH_KEY_STRING, (void*)"" );

    /* string var properties */
    /* none for now */

    /* variable properties according to var type */
    apr_hash_set( varprops, "bit", APR_HASH_KEY_STRING, (void*)bitprops );
    apr_hash_set( varprops, "number", APR_HASH_KEY_STRING, (void*)numprops );
    apr_hash_set( varprops, "string", APR_HASH_KEY_STRING, (void*)strprops );

    /* create plugin meta-info and return it back */
    return lt_server_plugin_info_create(
            "Martin Man <mman@swac.cz>",            /* author */
            VERSION,                                /* version */
            TODAY,                                  /* date */
            "Null",                                 /* shortdesc */
            "Null Lintouch Server Plugin",          /* longdesc */
            props,                                  /* default props */
            varprops,                               /* default varprops */
            data,                                   /* plugin data */
            pool );
}

lt_server_status_t
LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin )
{
    /* get plugin specific data */
    my_plugin_data * data = (my_plugin_data*)
        lt_server_plugin_data_get( plugin );
    /* get plugin config */
    const lt_server_plugin_config_t * config =
        lt_server_plugin_config_get( plugin );
    /* get default plugin config ( could be optimized ) */
    const lt_server_plugin_config_t * dconfig =
        lt_server_plugin_default_config_get( plugin );

    /* remember varset to speed up access to it */
    data->varset = lt_server_plugin_varset_get( plugin );

    return LT_SERVER_SUCCESS;
}

lt_server_status_t
LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin )
{
    /* get plugin specific data */
    my_plugin_data * data = (my_plugin_data*)
        lt_server_plugin_data_get( plugin );
    apr_pool_t * pool = NULL;

    /* create local pool used to allocate temporary values */
    apr_pool_create( &pool, lt_server_plugin_pool_get( plugin ) );

    lt_server_logger_print( PRIO_PLUGIN, "Null: started...\n" );

    while( lt_server_plugin_should_run( plugin ) ) {

        /* clear the pool used to hold temporarily created values */
        apr_pool_clear( pool );

        /* first lock requested values and go sleeping, we can be sure that
         * wake up and requested values will be locked again */
        lt_var_set_requested_values_lock( data->varset );

        /* wait one second for new requested values, if no values arrive,
         * check whether we still should be running and if yes, unlock
         * requested values and lock them back to allow lintouch to work
         * with our varset */
        while( lt_var_set_wait_for_new_requested_values(
                    data->varset, 1 * 1000 * 1000 ) != LT_VAR_SUCCESS ) {
            /* when stop was requested, we break our waiting cycle */
            if( ! lt_server_plugin_should_run( plugin ) ) break;
            lt_var_set_requested_values_unlock( data->varset );
            lt_var_set_requested_values_lock( data->varset );
        }

        /* clear dirty flags on requested values in our varset */
        lt_var_set_requested_values_sync( data->varset, -1 );
        /* and unlock our requested values so that new values can arrive
         * from the clients */
        lt_var_set_requested_values_unlock( data->varset );

        lt_server_logger_print( PRIO_PLUGIN, "Null: running...\n" );
    }

    lt_server_logger_print( PRIO_PLUGIN, "Null: exiting...\n" );

    return LT_SERVER_SUCCESS;
}

static int _my_interruptible_sleep( lt_server_plugin_t * plugin,
        apr_interval_time_t sleep, lt_var_t * counter )
{
    int i = 0;
    int total = 0;
    apr_interval_time_t partial = 0;
    int ret = 0;
    apr_pool_t * pool;

    /* sleep required for less than one second */
    if( sleep <= 1 * 1000 * 1000 ) {
        apr_sleep( sleep );
        return ! lt_server_plugin_should_run( plugin );
    }

    /* sleep required for more than one second */
    apr_pool_create( &pool, lt_server_plugin_pool_get( plugin ) );

    partial = 1 * 1000 * 1000;
    total = sleep / partial;

    for( i = 0; i < total; i ++ ) {
        /* sleep and check exit flag */
        apr_sleep( partial );
        if( ! lt_server_plugin_should_run( plugin ) ) {
            ret = 1;
            break;
        }

        /* decrease counter (if defined) */
        if( counter != NULL ) {
            lt_var_real_value_number_set( counter, total - i - 1 );
            lt_var_set_real_values_sync(
                    lt_server_plugin_varset_get( plugin ), -1 );
        }
    }

    apr_pool_destroy( pool );
    return ret;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id:$
 */
