/* $Id: demo_plugins.c 593 2005-04-27 09:46:33Z mman $
 *
 *   FILE: plugin.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 14 April 2005
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

#include <apr_strings.h>

#include "lt/server/plugin.h"
#include "lt/server/demo_plugins.h"
#include "plugin-private.h"

/* GENERATOR PLUGIN METHODS */

static lt_server_status_t generator_init( lt_server_plugin_t * plugin );
static lt_server_status_t generator_run( lt_server_plugin_t * plugin );

/* LOOPBACK PLUGIN METHODS */

static lt_server_status_t loopback_init( lt_server_plugin_t * plugin );
static lt_server_status_t loopback_run( lt_server_plugin_t * plugin );

/* PLUGIN SPECIFIC DATA */
typedef struct {
    lt_var_set_t * varset;
    lt_var_t * bit;
    lt_var_t * number;
    lt_var_t * string;
    apr_time_t sleep;
} my_plugin_data;

/* METHOD BODIES */

#define CREATE_POOL_IF_NULL(p)  if((p)==NULL)apr_pool_create(&(p),NULL)

lt_server_plugin_t * lt_server_demo_plugin_generator_create(
        apr_time_t refresh, apr_pool_t * pool )
{
    lt_server_plugin_t * plugin = NULL;
    my_plugin_data * data = NULL;

    CREATE_POOL_IF_NULL( pool );

    /* allocate lt_plugin_t data structure */
    plugin = (lt_server_plugin_t*)
        apr_pcalloc( pool, sizeof( lt_server_plugin_t ) );

    plugin->pool = pool;

    /* allocate plugin specific data */
    data = (my_plugin_data*)
        apr_pcalloc( pool, sizeof( my_plugin_data ) );
    plugin->info.data = (void*)data;
    data->sleep = refresh;

    /* create varset with three typed variables */
    plugin->varset = data->varset = lt_var_set_create( 0, pool );
    lt_var_set_variable_register( plugin->varset, "bit",
            data->bit = lt_var_create( LT_IO_TYPE_BIT, 0, pool ) );
    lt_var_set_variable_register( plugin->varset, "number",
            data->number = lt_var_create( LT_IO_TYPE_NUMBER, 0, pool ) );
    lt_var_set_variable_register( plugin->varset, "string",
            data->string = lt_var_create( LT_IO_TYPE_STRING, 0, pool ) );

    /* leave config empty for now */

    /* prepare method pointers for the plugin */
    plugin->method_init = generator_init;
    plugin->method_run = generator_run;

    /* initialize plugin */
    plugin->method_init( plugin );

    return plugin;
}

lt_server_plugin_t * lt_server_demo_plugin_loopback_create(
        apr_time_t latency, apr_pool_t * pool )
{
    lt_server_plugin_t * plugin = NULL;
    my_plugin_data * data = NULL;

    CREATE_POOL_IF_NULL( pool );

    /* allocate lt_plugin_t data structure */
    plugin = (lt_server_plugin_t*)
        apr_pcalloc( pool, sizeof( lt_server_plugin_t ) );

    plugin->pool = pool;

    /* allocate plugin specific data */
    data = (my_plugin_data*)
        apr_pcalloc( pool, sizeof( my_plugin_data ) );
    plugin->info.data = (void*)data;
    data->sleep = latency;

    /* create varset with three typed variables */
    plugin->varset = data->varset = lt_var_set_create( 0, pool );
    lt_var_set_variable_register( plugin->varset, "bit",
            data->bit = lt_var_create( LT_IO_TYPE_BIT, 0, pool ) );
    lt_var_set_variable_register( plugin->varset, "number",
            data->number = lt_var_create( LT_IO_TYPE_NUMBER, 0, pool ) );
    lt_var_set_variable_register( plugin->varset, "string",
            data->string = lt_var_create( LT_IO_TYPE_STRING, 0, pool ) );

    /* leave config empty for now */

    /* prepare method pointers for the plugin */
    plugin->method_init = loopback_init;
    plugin->method_run = loopback_run;

    /* initialize plugin */
    plugin->method_init( plugin );

    return plugin;
}

/* GENERATOR PLUGIN METHODS */

lt_server_status_t generator_init( lt_server_plugin_t * plugin )
{
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;

    /* initialize our variables to a known state */
    {
        my_plugin_data * data = (my_plugin_data*)
            lt_server_plugin_data_get( plugin );

        /* bit variable */
        lt_var_requested_value_set( data->bit,
                lt_io_value_bit_create( 0, plugin->pool ) );
        lt_var_real_value_set( data->bit,
                lt_io_value_bit_create( 0, plugin->pool ) );

        /* number variable */
        lt_var_requested_value_set( data->number,
                lt_io_value_number_create( 65000, plugin->pool ) );
        lt_var_real_value_set( data->number,
                lt_io_value_number_create( 65000, plugin->pool ) );

        /* string variable */
        lt_var_requested_value_set( data->string,
                lt_io_value_string_create2( "123", plugin->pool ) );
        lt_var_real_value_set( data->string,
                lt_io_value_string_create2( "123", plugin->pool ) );
    }

    return LT_SERVER_SUCCESS;
}

lt_server_status_t generator_run( lt_server_plugin_t * plugin )
{
    my_plugin_data * data = NULL;
    apr_pool_t * pool = NULL;

    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;

    data = (my_plugin_data*)lt_server_plugin_data_get( plugin );
    apr_pool_create( &pool, lt_server_plugin_pool_get( plugin ) );

    while( lt_server_plugin_should_run( plugin ) ) {

        /* clear the pool used to hold temporarily created values */
        apr_pool_clear( pool );

        /* bit variable */
        {
            const lt_io_value_t * v = lt_var_real_value_get( data->bit );
            lt_var_real_value_set( data->bit, lt_io_value_bit_create(
                        ! lt_io_value_bit_get( v ), pool ) );
        }
        /* number variable */
        {
            const lt_io_value_t * v = lt_var_real_value_get( data->number );
            lt_var_real_value_set( data->number, lt_io_value_number_create(
                        lt_io_value_number_get( v ) + 1, pool ) );
        }
        /* string variable */
        {
            const lt_io_value_t * v = lt_var_real_value_get( data->string );
            int num = apr_atoi64( lt_io_value_string_get( v ) );
            lt_var_real_value_set( data->string,
                    lt_io_value_string_create2( apr_itoa( pool, num + 1 ),
                        pool ) );
        }

        /* sync real values back to clients */
        lt_var_set_real_values_sync( data->varset, -1 );

        /* sleep for configured time */
        apr_sleep( data->sleep );
    }

    printf( "generator_run: exiting...\n" );
    return LT_SERVER_SUCCESS;
}

/* LOOPBACK PLUGIN METHODS */

lt_server_status_t loopback_init( lt_server_plugin_t * plugin )
{
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;

    /* initialize our variables to a known state */
    {
        my_plugin_data * data = (my_plugin_data*)
            lt_server_plugin_data_get( plugin );

        /* bit variable */
        lt_var_requested_value_set( data->bit,
                lt_io_value_bit_create( 0, plugin->pool ) );
        lt_var_real_value_set( data->bit,
                lt_io_value_bit_create( 0, plugin->pool ) );

        /* number variable */
        lt_var_requested_value_set( data->number,
                lt_io_value_number_create( 0, plugin->pool ) );
        lt_var_real_value_set( data->number,
                lt_io_value_number_create( 0, plugin->pool ) );

        /* string variable */
        lt_var_requested_value_set( data->string,
                lt_io_value_string_create2( "0", plugin->pool ) );
        lt_var_real_value_set( data->string,
                lt_io_value_string_create2( "0", plugin->pool ) );
    }

    return LT_SERVER_SUCCESS;
}

lt_server_status_t loopback_run( lt_server_plugin_t * plugin )
{
    my_plugin_data * data = NULL;

    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;

    data = (my_plugin_data*)lt_server_plugin_data_get( plugin );

    while( lt_server_plugin_should_run( plugin ) ) {

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

        /* now we can be sure that we have some new requested values in a
         * varset, sleep for the latency time and loop the dirty values back
         * */
        apr_sleep( data->sleep );

        /* bit variable */
        {
            if( lt_var_requested_value_is_dirty( data->bit ) ) {
                lt_var_real_value_set( data->bit,
                        lt_var_requested_value_get( data->bit ) );
            }
        }
        /* number variable */
        {
            if( lt_var_requested_value_is_dirty( data->number ) ) {
                lt_var_real_value_set( data->number,
                        lt_var_requested_value_get( data->number ) );
            }
        }
        /* string variable */
        {
            if( lt_var_requested_value_is_dirty( data->string ) ) {
                lt_var_real_value_set( data->string,
                        lt_var_requested_value_get( data->string ) );
            }
        }

        /* clear dirty flags on requested values in our varset */
        lt_var_set_requested_values_sync( data->varset, -1 );
        /* and unlock our requested values so that new values can arrive
         * from the clients */
        lt_var_set_requested_values_unlock( data->varset );

        /* sync real values back to clients */
        lt_var_set_real_values_sync( data->varset, -1 );

    }

    printf( "loopback_run: exiting...\n" );
    return LT_SERVER_SUCCESS;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: demo_plugins.c 593 2005-04-27 09:46:33Z mman $
 */
