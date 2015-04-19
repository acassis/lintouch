/* $Id: generator.c 772 2005-06-07 15:16:37Z mman $
 *
 *   FILE: generator.c --
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
#include <stdlib.h>
#include <time.h>
#include <apr_time.h>
#include <apr_strings.h>

#include "lt/server/plugin.h"
#include "lt/server/logger.h"

#include "config.h"

/* automatically define mandatory plugin symbols */
LT_SERVER_PLUGIN_EXPORT;

/* GENERATOR PLUGIN METHODS */

lt_server_plugin_info_t *
PLUGIN_EXPORT LT_SERVER_PLUGIN_CREATE( apr_pool_t * pool );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin );

/* GENERATOR PLUGIN SPECIFIC DATA */

typedef struct {
    char random;
    int min;
    int max;
    int step;
    double scale;
} my_vardata;

typedef struct {
    apr_interval_time_t sleep;
    lt_var_set_t * varset;
    apr_hash_t * values;
} my_plugin_data;

static int _property_as_int( apr_hash_t * hash, const char * name, int dflt );

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
    apr_hash_set( props, "refresh", APR_HASH_KEY_STRING, (void*)"0.5" );

    /* bit var properties */
    apr_hash_set( numprops, "random", APR_HASH_KEY_STRING, (void*)"0" );
    /* num var properties */
    apr_hash_set( numprops, "min", APR_HASH_KEY_STRING, (void*)"-10" );
    apr_hash_set( numprops, "max", APR_HASH_KEY_STRING, (void*)"10" );
    apr_hash_set( numprops, "step", APR_HASH_KEY_STRING, (void*)"1" );
    apr_hash_set( numprops, "random", APR_HASH_KEY_STRING, (void*)"0" );
    /* string var properties */
    apr_hash_set( strprops, "min", APR_HASH_KEY_STRING, (void*)"-10" );
    apr_hash_set( strprops, "max", APR_HASH_KEY_STRING, (void*)"10" );
    apr_hash_set( strprops, "step", APR_HASH_KEY_STRING, (void*)"1" );
    apr_hash_set( numprops, "random", APR_HASH_KEY_STRING, (void*)"0" );

    /* variable properties according to var type */
    apr_hash_set( varprops, "bit", APR_HASH_KEY_STRING, (void*)bitprops );
    apr_hash_set( varprops, "number", APR_HASH_KEY_STRING, (void*)numprops );
    apr_hash_set( varprops, "string", APR_HASH_KEY_STRING, (void*)strprops );

    /* create plugin meta-info and return it back */
    return lt_server_plugin_info_create(
            "Martin Man <mman@swac.cz>",            /* author */
            VERSION,                                /* version */
            TODAY,                                  /* date */
            "Generator",                            /* shortdesc */
            "Generator Lintouch Server Plugin",     /* longdesc */
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

    const char * tmp = NULL;
    double tmpd = 0.5;

    /* parse config and initialize local data structure */
    tmp = apr_hash_get( config->properties,
            "refresh", APR_HASH_KEY_STRING );
    if( tmp == NULL ) {
        tmp = apr_hash_get( dconfig->properties,
                "refresh", APR_HASH_KEY_STRING );
    }
    if( tmp == NULL ) {
        tmp = "0.5";
    }

    /* scan number in seconds */
    sscanf( tmp, "%lf", &tmpd );
    /* and convert it to microseconds */
    data->sleep = (apr_interval_time_t) (tmpd * 1000 * 1000);

    /* remember varset to speed up access to it */
    data->varset = lt_server_plugin_varset_get( plugin );

    /* remember current value and limit for all variables */
    data->values = apr_hash_make(
            lt_server_plugin_pool_get( plugin ) );

    /* now for each variable, set up defaults + props */
    {
        apr_pool_t * pool = NULL;
        apr_pool_t * lpool = NULL;
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;

        pool = lt_server_plugin_pool_get( plugin );
        apr_pool_create( &lpool, pool );
        h = lt_var_set_variables_get( data->varset );
        hi = apr_hash_first( lpool, h );

        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
            const char * name = NULL;
            lt_var_t * var = NULL;
            apr_hash_t * varprops = NULL;
            my_vardata * vardata = NULL;

            apr_hash_this( hi, (const void**)&name, NULL, (void**)&var );

            /* try to get variable properties */
            varprops = apr_hash_get( config->varproperties,
                    name, APR_HASH_KEY_STRING );
            if( varprops == NULL ) {
                varprops = apr_hash_get( dconfig->varproperties,
                        name, APR_HASH_KEY_STRING );
            }

            /* allocate varable limits structure and fill it with defaults */
            vardata = (my_vardata*) apr_pcalloc( pool,
                    sizeof( my_vardata ) );
            vardata->min = _property_as_int( varprops, "min", -10 );
            vardata->max = _property_as_int( varprops, "max", 10 );
            vardata->step = _property_as_int( varprops, "step", 1 );
            vardata->random = _property_as_int( varprops, "random", 0 );

            /* adjust var limits according to a type and initialize each
             * variable to the min value */
            switch( lt_var_type_get( var ) ) {
                case LT_IO_TYPE_BIT:
                    if( vardata->min < 0 ) vardata->min = 0;
                    if( vardata->max > 1 ) vardata->max = 1;
                    if( vardata->step > 1 ) vardata->step = 1;
                    if( vardata->step < 1 ) vardata->step = -1;
                    lt_var_real_value_bit_set( var, vardata->min );
                    break;
                case LT_IO_TYPE_NUMBER:
                    vardata->scale = ( vardata->max - vardata->min ) /
                        (double)RAND_MAX;
                    lt_var_real_value_number_set( var, vardata->min );
                    break;
                case LT_IO_TYPE_STRING:
                    vardata->scale = ( vardata->max - vardata->min ) /
                        (double)RAND_MAX;
                    lt_var_real_value_string_set2( var,
                                apr_itoa( lpool, vardata->min ) );
                    break;
                default:
                    break;
            }

            apr_hash_set( data->values, name, APR_HASH_KEY_STRING, vardata );
        }

        apr_pool_destroy( lpool );
    }

    /* initialize random engine */
    srand( time( NULL ) );

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

    lt_server_logger_print( PRIO_PLUGIN, "Generator: started...\n" );

    while( lt_server_plugin_should_run( plugin ) ) {

        /* clear the pool used to hold temporarily created values */
        apr_pool_clear( pool );

        /* for each variable in our varset */
        {
            apr_hash_t * rel =
                lt_var_set_variables_get( data->varset );
            apr_hash_index_t * hi =
                apr_hash_first( pool, rel );

            for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
                const char * varname = NULL;
                lt_var_t * var = NULL;
                my_vardata * vardata = NULL;

                apr_hash_this( hi,
                        (const void**)&varname, NULL, (void**)&var );
                vardata = apr_hash_get( data->values,
                        varname, APR_HASH_KEY_STRING );

                switch( lt_var_type_get( var ) ) {
                    case LT_IO_TYPE_BIT:
                        lt_server_logger_print( PRIO_PLUGIN + 1,
                                "Generator: changing bit variable %s (%s)...\n",
                                varname, ( vardata->random ? "random" : "sequenced" ));
                        if( vardata->random ) {
                            int r = rand();
                            int s = ( r > ( RAND_MAX / 2 ) );
                            lt_var_real_value_bit_set( var, s );
                        } else {
                            lt_var_real_value_bit_set( var,
                                    ! lt_var_real_value_bit_get( var ) );
                        }
                        break;
                    case LT_IO_TYPE_NUMBER:
                        lt_server_logger_print( PRIO_PLUGIN + 1,
                                "Generator: changing number variable %s (%s)...\n",
                                varname, ( vardata->random ? "random" : "sequenced" ));
                        if( vardata->random ) {
                            int r = rand();
                            int s = ( r * vardata->scale ) + vardata->min;
                            lt_var_real_value_number_set( var, s );
                        } else {
                            int r = lt_io_value_number_get(
                                    lt_var_real_value_get( var ) );
                            r += vardata->step;
                            if( r > vardata->max ) r = vardata->min;
                            if( r < vardata->min ) r = vardata->max;
                            lt_var_real_value_number_set( var, r );
                        }
                        break;
                    case LT_IO_TYPE_STRING:
                        lt_server_logger_print( PRIO_PLUGIN + 1,
                                "Generator: changing string variable %s (%s)...\n",
                                varname, ( vardata->random ? "random" : "sequenced" ));
                        if( vardata->random ) {
                            int r = rand();
                            int s = ( r * vardata->scale ) + vardata->min;
                            lt_var_real_value_string_set2( var,
                                    apr_itoa( pool, s ) );
                        } else {
                            int r = lt_io_value_number_get(
                                    lt_var_real_value_get( var ) );
                            r += vardata->step;
                            if( r > vardata->max ) r = vardata->min;
                            if( r < vardata->min ) r = vardata->max;
                            lt_var_real_value_string_set2( var,
                                    apr_itoa( pool, r ) );
                        }
                        break;
                    default:
                        break;
                }
            }

        }

        /* sync real values back to clients */
        lt_var_set_real_values_sync( data->varset, -1 );

        /* sleep for configured time while allowing plugin to exit */
        if( _my_interruptible_sleep( plugin,
                    data->sleep, NULL ) ) {
            lt_server_logger_print( PRIO_PLUGIN,
                    "Generator: exit requested while sleeping...\n" );
            break;
        }

        lt_server_logger_print( PRIO_PLUGIN, "Generator: running...\n" );
    }

    lt_server_logger_print( PRIO_PLUGIN, "Generator: exiting...\n" );

    return LT_SERVER_SUCCESS;
}

int _property_as_int( apr_hash_t * hash, const char * name, int dflt )
{
    int result;
    const char * val = apr_hash_get( hash, name, APR_HASH_KEY_STRING );
    if( val == NULL ) return dflt;

    if( sscanf( val, "%d", &result ) != 1 ) return dflt;
    return result;
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
 * $Id: generator.c 772 2005-06-07 15:16:37Z mman $
 */
