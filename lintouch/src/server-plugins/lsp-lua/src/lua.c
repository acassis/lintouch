/* $Id:$
 *
 *   FILE: lua.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE:  8 September 2005
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

#include "ltapi.h"

#include "config.h"

/* automatically define mandatory plugin symbols */
LT_SERVER_PLUGIN_EXPORT;

/* LUA PLUGIN METHODS */

lt_server_plugin_info_t *
PLUGIN_EXPORT LT_SERVER_PLUGIN_CREATE( apr_pool_t * pool );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin );

/* LUA PLUGIN SPECIFIC DATA */

typedef struct {
    const char * script;
} my_plugin_data;

/* METHOD BODIES */

lt_server_plugin_info_t *
LT_SERVER_PLUGIN_CREATE( apr_pool_t * pool )
{
    apr_hash_t * props = apr_hash_make( pool );
    my_plugin_data * data = apr_pcalloc( pool, sizeof( my_plugin_data ) );

    /* plugin properties */
    apr_hash_set( props, "script", APR_HASH_KEY_STRING, (void*)"script.lua" );

    /* create plugin meta-info and return it back */
    return lt_server_plugin_info_create(
            "Martin Man <mman@swac.cz>",            /* author */
            VERSION,                                /* version */
            TODAY,                                  /* date */
            "Lua",                                  /* shortdesc */
            "Lua Lintouch Server Plugin",           /* longdesc */
            props,                                  /* default props */
            NULL,                                   /* default varprops */
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

    /* parse config and initialize local data structure */
    tmp = apr_hash_get( config->properties,
            "script", APR_HASH_KEY_STRING );
    if( tmp == NULL ) {
        tmp = apr_hash_get( dconfig->properties,
                "script", APR_HASH_KEY_STRING );
    }

    data->script = tmp;

    return LT_SERVER_SUCCESS;
}

lt_server_status_t
LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin )
{
    /* get plugin specific data */
    my_plugin_data * data = (my_plugin_data*)
        lt_server_plugin_data_get( plugin );
    /* get varset */
    lt_var_set_t * varset =
        lt_server_plugin_varset_get( plugin );
    /* file to execute */
    apr_file_t * scriptf = NULL;

    lt_server_logger_print( PRIO_PLUGIN, "Lua: started...\n" );

    /* open resource with the script */
    lt_server_logger_print( PRIO_PLUGIN, "Lua: loading resource %s...\n",
            data->script );

    scriptf = lt_server_plugin_resource_get( plugin,
            data->script, lt_server_plugin_pool_get( plugin ) );

    if( scriptf != NULL ) {

        /* create new lua interpreter state */
        lua_State * l = lua_open();

        /* load standard libraries */
        ltapi_libraries_load( l );
        /* register globals available to the Lua interpreter */
        ltapi_globals_register( l, plugin, varset );
        /* register methods available to the Lua interpreter */
        ltapi_methods_register( l );

        lt_server_logger_print( PRIO_PLUGIN,
                "Lua: executing Lua script %s...\n",
                data->script );

        /* start executing a lua script */
        ltapi_file_execute( l, scriptf, data->script );

        /* close lua interpreter */
        lua_close( l );

        /* close resource with the script */
        apr_file_close( scriptf );

    } else {
        lt_server_logger_print( PRIO_PLUGIN,
                "Lua: unable to open resource %s...\n", data->script );
    }

    lt_server_logger_print( PRIO_PLUGIN, "Lua: exiting...\n" );

    return LT_SERVER_SUCCESS;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id:$
 */
