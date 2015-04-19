/* $Id: exec.c 577 2005-04-22 12:33:25Z mman $
 *
 *   FILE: exec.c --
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
#include <apr_thread_proc.h>

#include "lt/server/plugin.h"
#include "lt/server/logger.h"

#include "config.h"

/**
 * Exec Lintouch Server Plugin.
 *
 * This is a very simple, I'd say demonstration plugin that you might use to
 * periodically execute one or more commands at the Lintouch Server machine.
 *
 * Each numerical typed variable corresponds to one executed command. The
 * value of the variable represents return code of the command.
 *
 * Additionally if you define string typed variables of the same name with
 * .stdout and .stderr suffix, they will contain the output of the command.
 *
 * For example:
 *
 * Define in your connection these variables:
 *
 * mycommand : number (cmdline property set to "/bin/ls")
 * mycommand.stdout : string
 * mycommand.stderr : string
 *
 * The plugin can be configured with two global properties:
 *
 * - refresh: defines how often to execute all commands to refresh the
 *   values of variables
 * - sleep: how long to sleep before executing next command in a sequence to
 *   not overflow the CPU of the server machine.
 *
 * The plugin will also report some of its activities via the following
 * global variables (if defined)
 *
 * .timer: number - to see how many seconds are remaining till the next
 *                  check
 *
 * @author Martin Man <mman@swac.cz>
 */

/* automatically define mandatory plugin symbols */
LT_SERVER_PLUGIN_EXPORT;

/* EXEC PLUGIN METHODS */

lt_server_plugin_info_t *
PLUGIN_EXPORT LT_SERVER_PLUGIN_CREATE( apr_pool_t * pool );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin );

/* EXEC PLUGIN SPECIFIC DATA */

typedef struct {
    /* commandline as specified by the user */
    const char * cmdline;
    /* tokenized argv */
    char ** argv;
    /* retcode of the process */
    int retcode;
    /* lintouch variable for retcode, stdout, and stderr */
    lt_var_t * varret, * varout, * varerr;
} my_vardata;

typedef struct {
    /* how long to sleep before next refresh */
    apr_interval_time_t refresh;
    /* how long to sleep before each exec */
    apr_interval_time_t sleep;
    /* varset used to communicate with the clients */
    lt_var_set_t * varset;
    /* my_vardata associated with each variable */
    apr_hash_t * vardata;
    /* lintouch variable for .timer */
    lt_var_t * vartimer;
} my_plugin_data;

static float _property_as_float( apr_hash_t * hash,
        const char * name, float dflt );
static int _property_as_int( apr_hash_t * hash,
        const char * name, int dflt );
static const char * _property_as_string( apr_hash_t * hash,
        const char * name, const char * dflt );

/**
 * execute command as precofigured in command structure and capture its
 * stdout, stderr, and return value
 */
static void _my_exec( my_vardata * command );

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
    apr_hash_set( props, "refresh", APR_HASH_KEY_STRING, (void*)"10" );
    apr_hash_set( props, "sleep", APR_HASH_KEY_STRING, (void*)"0.5" );

    /* bit var properties */
    /* num var properties */
    apr_hash_set( numprops, "cmdline", APR_HASH_KEY_STRING,
            (void*)"/bin/true" );
    /* string var properties */

    /* variable properties according to var type */
    apr_hash_set( varprops, "bit", APR_HASH_KEY_STRING, (void*)bitprops );
    apr_hash_set( varprops, "number", APR_HASH_KEY_STRING, (void*)numprops );
    apr_hash_set( varprops, "string", APR_HASH_KEY_STRING, (void*)strprops );

    /* create plugin meta-info and return it back */
    return lt_server_plugin_info_create(
            "Martin Man <mman@swac.cz>",            /* author */
            VERSION,                                /* version */
            TODAY,                                  /* date */
            "Exec",                                 /* shortdesc */
            "Exec Lintouch Server Plugin",          /* longdesc */
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

    /* parse config and initialize local data structure */
    apr_hash_t * props = config->properties;
    if( props == NULL ) props = dconfig->properties;
    data->refresh = (apr_interval_time_t)
        ( _property_as_float( props, "refresh", 10 ) * 1000 * 1000);
    data->sleep = (apr_interval_time_t)
        ( _property_as_float( props, "sleep", 0.5 ) * 1000 * 1000);

    /* remember varset to speed up access to it */
    data->varset = lt_server_plugin_varset_get( plugin );

    /* remember global lintouch variables (if defined) */
    data->vartimer = lt_var_set_variable_get( data->varset, ".timer" );

    /* remember data for numerical variables */
    data->vardata = apr_hash_make(
            lt_server_plugin_pool_get( plugin ) );

    /* now for each numerical variable that does not start with '.',
     * set up defaults + props */
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

            if( lt_var_type_get( var ) == LT_IO_TYPE_NUMBER &&
                    name[ 0 ] != '.' ) {
                lt_var_real_value_number_set( var, 255 );

                /* try to get variable properties */
                varprops = apr_hash_get( config->varproperties,
                        name, APR_HASH_KEY_STRING );
                if( varprops == NULL ) {
                    varprops = apr_hash_get( dconfig->varproperties,
                            name, APR_HASH_KEY_STRING );
                }

                /* allocate varable data structure and fill it with defaults */
                vardata = (my_vardata*) apr_pcalloc( pool,
                        sizeof( my_vardata ) );
                vardata->cmdline = _property_as_string(
                        varprops, "cmdline", "" );
                apr_tokenize_to_argv( vardata->cmdline,
                        &vardata->argv, pool );

                /* remember (as shortcuts) where to store return value +
                 * stdout + stderr */
                vardata->varret = var;
                vardata->varout =
                    lt_var_set_variable_get( data->varset,
                            apr_pstrcat( pool, name, ".stdout", NULL ) );
                vardata->varerr =
                    lt_var_set_variable_get( data->varset,
                            apr_pstrcat( pool, name, ".stderr", NULL ) );

                /* remember the variable related data */
                apr_hash_set( data->vardata,
                        name, APR_HASH_KEY_STRING, vardata );
            }

        }

        apr_pool_destroy( lpool );
    }

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

    lt_server_logger_print( PRIO_PLUGIN, "Exec: started...\n" );

    /* initialize the timer reporting variable to a known state */
    if( data->vartimer != NULL ) {
        lt_var_real_value_number_set( data->vartimer, 0 );
    }

    while( lt_server_plugin_should_run( plugin ) ) {
        /* clear the pool used to hold temporarily created values */
        apr_pool_clear( pool );

        /* for each variable in our varset */
        {
            apr_hash_t * rel = data->vardata;
            apr_hash_index_t * hi =
                apr_hash_first( pool, rel );

            for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
                const char * varname = NULL;
                my_vardata * vardata = NULL;

                apr_hash_this( hi,
                        (const void**)&varname, NULL, (void**)&vardata );

                lt_server_logger_print( PRIO_PLUGIN,
                        "Exec: going to exec '%s'\n", vardata->cmdline );

                /* try to exec command and grab its output */
                _my_exec( vardata );

                /* sync real values back to clients after each
                 * execution of a command */
                lt_var_set_real_values_sync( data->varset, -1 );

                if( ! lt_server_plugin_should_run( plugin ) ) break;

                lt_server_logger_print( PRIO_PLUGIN,
                        "Exec: waiting before executing"
                        " next command...\n" );

                /* sleep for configured time while allowing plugin to exit */
                if( _my_interruptible_sleep( plugin,
                            data->sleep, NULL ) ) {
                    lt_server_logger_print( PRIO_PLUGIN,
                            "Exec: exit requested while sleeping...\n" );
                    break;
                }
            }

            if( ! lt_server_plugin_should_run( plugin ) ) break;
        }

        lt_server_logger_print( PRIO_PLUGIN,
                "Exec: waiting until next refresh is needed...\n" );

        /* sleep for configured time while allowing plugin to exit */
        if( _my_interruptible_sleep( plugin,
                    data->refresh, data->vartimer ) ) {
            lt_server_logger_print( PRIO_PLUGIN,
                    "Exec: exit requested while sleeping...\n" );
            break;
        }

        lt_server_logger_print( PRIO_PLUGIN, "Exec: running...\n" );
    }

    lt_server_logger_print( PRIO_PLUGIN, "Exec: exiting...\n" );
    return LT_SERVER_SUCCESS;
}

float _property_as_float( apr_hash_t * hash, const char * name, float dflt )
{
    float result;
    const char * val = apr_hash_get( hash, name, APR_HASH_KEY_STRING );
    if( val == NULL ) return dflt;

    if( sscanf( val, "%f", &result ) != 1 ) return dflt;
    return result;
}

int _property_as_int( apr_hash_t * hash, const char * name, int dflt )
{
    int result;
    const char * val = apr_hash_get( hash, name, APR_HASH_KEY_STRING );
    if( val == NULL ) return dflt;

    if( sscanf( val, "%d", &result ) != 1 ) return dflt;
    return result;
}

const char * _property_as_string( apr_hash_t * hash,
        const char * name, const char * dflt )
{
    const char * val = apr_hash_get( hash, name, APR_HASH_KEY_STRING );
    if( val == NULL ) return dflt;
    return val;
}

static void _my_exec( my_vardata * command )
{
    apr_status_t rv = 0;
    apr_proc_t handle;
    apr_procattr_t * attr = NULL;
    apr_file_t * pout, * chout, * perr, * cherr;
    int retcode = 255;
    apr_exit_why_e why;
    apr_pool_t * pool;

    /* create temporary pool */
    apr_pool_create( &pool, NULL );

    /* create stdout & stderr pipes */
    apr_file_pipe_create( &pout, &chout, pool );
    apr_file_pipe_timeout_set( pout, 0 );
    apr_file_pipe_create( &perr, &cherr, pool );
    apr_file_pipe_timeout_set( perr, 0 );

    /* create procattr, set type to be shell command and redirect stdout and
     * stderr */
    rv = apr_procattr_create( &attr, pool );
    apr_procattr_child_out_set( attr, chout, pout );
    apr_procattr_child_err_set( attr, cherr, perr );
    apr_procattr_cmdtype_set( attr, APR_SHELLCMD );

    /* create & exec process */
    rv = apr_proc_create( &handle,
            command->argv[ 0 ],
            (const char * const *) command->argv,
            NULL,
            attr, pool );

    if( ! APR_STATUS_IS_SUCCESS( rv ) ) {
        lt_server_logger_print_apr( PRIO_PLUGIN, "apr_proc_create",
                "Exec: failed to exec '%s'\n", command->cmdline );
    }

    /* wait for process to exit */
    rv = apr_proc_wait( &handle, &retcode, &why,
            APR_WAIT );
    lt_server_logger_print( PRIO_PLUGIN, "Exec: command '%s'"
            " exited with retcode %d...\n", command->cmdline, retcode );

    /* push return code to lintouch */
    lt_var_real_value_number_set( command->varret, retcode );

    /* push stdout code to lintouch */
    if( command->varout != NULL ) {
        char buffer[16000];
        apr_size_t len;
        char * ret = "";

        do {
            len = 16000 - 1;
            rv = apr_file_read( pout, buffer, &len );
            buffer[ len ] = 0;
            ret = apr_pstrcat( pool, ret, buffer, NULL );
            lt_server_logger_print_apr( PRIO_PLUGIN + 2, "apr_file_read",
                    "Exec: read %d bytes from stdout\n", len );
        } while( len > 0 );

        lt_var_real_value_string_set2( command->varout, ret );

        lt_server_logger_print( PRIO_PLUGIN + 1, "Exec: command '%s'"
                " produced the following stdout\n%s\n",
                command->cmdline, ret );
    }

    apr_file_close( pout );
    apr_file_close( chout );

    /* push stderr code to lintouch */
    if( command->varerr != NULL ) {
        char buffer[16000];
        apr_size_t len;
        char * ret = "";

        do {
            len = 16000 - 1;
            rv = apr_file_read( perr, buffer, &len );
            buffer[ len ] = 0;
            ret = apr_pstrcat( pool, ret, buffer, NULL );
            lt_server_logger_print_apr( PRIO_PLUGIN + 2, "apr_file_read",
                    "Exec: read %d bytes from stderr\n", len );
        } while( len > 0 );

        lt_var_real_value_string_set2( command->varerr, ret );

        lt_server_logger_print( PRIO_PLUGIN + 1, "Exec: command '%s'"
                " produced the following stderr\n%s\n",
                command->cmdline, ret );
    }

    apr_file_close( perr );
    apr_file_close( cherr );

    /* destroy temporary pool */
    apr_pool_destroy( pool );
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
 * $Id: exec.c 577 2005-04-22 12:33:25Z mman $
 */
