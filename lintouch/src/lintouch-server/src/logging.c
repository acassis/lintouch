/* $Id:$
 *
 *   FILE: logging.c --
 * AUTHOR: Patrik Modesto <modesto@swac.cz>
 *
 * Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
 * Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
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

#include <apr_dso.h>
#include <apr_strings.h>

#include "lt/server/logging.h"
#include "logging-private.h"

#include "lt/server/logger.h"
#include "ziparchive.h"

apr_pool_t * lt_logging_plugin_pool_get( const lt_logging_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->pool;
}

apr_hash_t * lt_logging_plugin_properties_get(
        const lt_logging_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->properties;
}

void * lt_logging_plugin_data_get( const lt_logging_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.data;
}

apr_hash_t * lt_logging_plugin_default_properties_get(
        const lt_logging_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.default_properties;
}

const char * lt_logging_plugin_author_get( const lt_logging_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.author;
}

const char * lt_logging_plugin_version_get( const lt_logging_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.version;
}

const char * lt_logging_plugin_date_get( const lt_logging_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.date;
}

const char * lt_logging_plugin_shortdesc_get( const lt_logging_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.shortdesc;
}

const char * lt_logging_plugin_longdesc_get( const lt_logging_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.longdesc;
}

lt_logging_plugin_info_t * lt_logging_plugin_info_create(
        const char * author,
        const char * version,
        const char * date,
        const char * shortdesc,
        const char * longdesc,
        apr_hash_t * default_properties,
        void * data, apr_pool_t * pool )
{
    lt_logging_plugin_info_t * result = NULL;

    result = (lt_logging_plugin_info_t*) apr_pcalloc(
            pool, sizeof( lt_logging_plugin_info_t ) );

    result->author = author;
    result->version = version;
    result->date = date;
    result->shortdesc = shortdesc;
    result->longdesc = longdesc;

    result->default_properties = default_properties;
    result->data = data;

    return result;
}

lt_logging_plugin_t * lt_logging_plugin_load( const char * pluginfile,
        lt_project_logging_t * log, apr_pool_t * pool )
{
    apr_dso_handle_t * handle = NULL;
    apr_dso_handle_sym_t sym_vinfo = NULL;
    apr_dso_handle_sym_t sym_create = NULL;
    apr_dso_handle_sym_t sym_init = NULL;
    apr_dso_handle_sym_t sym_log = NULL;
    apr_dso_handle_sym_t sym_exit = NULL;

    lt_logging_plugin_info_t * info = NULL;

    lt_logging_plugin_t * result = NULL;

    /* load DSO */
    if( APR_SUCCESS != apr_dso_load(
                    &handle, pluginfile, pool ) ) {
        lt_server_logger_print_apr_dso( PRIO_DEFAULT, handle,
                "Unable to load pluginfile %s\n", pluginfile );
        return NULL;
    }

    /* get required symbol out of DSO */
    if( APR_SUCCESS != apr_dso_sym(
                    &sym_vinfo, handle, "lt_version_info" ) ) {
        lt_server_logger_print_apr_dso( PRIO_DEFAULT, handle,
                "Unable to resolve symbol ``lt_version_info`` in %s\n",
                pluginfile );
        return NULL;
    }

    /* get required symbol out of DSO */
    if( APR_SUCCESS != apr_dso_sym(
                    &sym_create, handle, STR(LT_LOGGING_PLUGIN_CREATE) ) ) {
        lt_server_logger_print_apr_dso( PRIO_DEFAULT, handle,
                "Unable to resolve symbol ``" STR(LT_LOGGING_PLUGIN_CREATE)
                "`` in %s\n", pluginfile );
        return NULL;
    }

    /* get required symbol out of DSO */
    if( APR_SUCCESS != apr_dso_sym(
                    &sym_init, handle,
                    STR(LT_LOGGING_PLUGIN_INIT) ) ) {
        lt_server_logger_print_apr_dso( PRIO_DEFAULT, handle,
                "Unable to resolve symbol ``"
                STR(LT_LOGGING_PLUGIN_INIT) "`` in %s\n",
                pluginfile );
        return NULL;
    }

    /* get required symbol out of DSO */
    if( APR_SUCCESS != apr_dso_sym(
                    &sym_log, handle, STR(LT_LOGGING_PLUGIN_LOG) ) ) {
        lt_server_logger_print_apr_dso( PRIO_DEFAULT, handle,
                "Unable to resolve symbol ``"
                STR(LT_LOGGING_PLUGIN_LOG) "`` in %s\n",
                pluginfile );
        return NULL;
    }

    /* get required symbol out of DSO */
    if( APR_SUCCESS != apr_dso_sym(
                    &sym_exit, handle, STR(LT_LOGGING_PLUGIN_EXIT) ) ) {
        lt_server_logger_print_apr_dso( PRIO_DEFAULT, handle,
                "Unable to resolve symbol ``"
                STR(LT_LOGGING_PLUGIN_EXIT) "`` in %s\n",
                pluginfile );
        return NULL;
    }

    /* verify version info */
    {
        const char * vi =
            ( ( lt_logging_plugin_version_info_t ) sym_vinfo )();

        if( ! verify_lt_plugin( "lt::LoggingPlugin", vi ) ) {
            lt_server_logger_print( PRIO_DEFAULT,
                    "Unable to load plugin %s, plugin does not define "
                    "correct version_info\n",
                    pluginfile );
            return NULL;
        }
    }

    /* call create to get plugin specific data */
    info = ( ( lt_logging_plugin_create_t ) sym_create ) ( pool );

    /* allocate output structure */
    result = ( lt_logging_plugin_t * ) apr_pcalloc( pool,
            sizeof( lt_logging_plugin_t ) );

    result->pool = pool;
    result->method_init = sym_init;
    result->method_log = sym_log;
    result->method_exit = sym_exit;

    /* remember info */
    result->info = *info;

    /* remember properties */
    result->properties =
        lt_project_logging_properties_get( log );

    return result;
}

lt_server_status_t lt_logging_plugin_init( lt_logging_plugin_t * plugin,
        apr_hash_t * connections )
{
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;
    if( plugin->method_init == NULL ) return LT_SERVER_INVALID_ARG;

    return plugin->method_init( plugin, connections );
}

lt_server_status_t lt_logging_plugin_log( lt_logging_plugin_t * plugin,
        apr_time_exp_t* timestamp, lt_var_set_t* vset, int real )
{
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;
    if( plugin->method_log == NULL ) return LT_SERVER_INVALID_ARG;

    return plugin->method_log( plugin, timestamp, vset, real);
}

lt_server_status_t lt_logging_plugin_exit( lt_logging_plugin_t * plugin )
{
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;
    if( plugin->method_exit == NULL ) return LT_SERVER_INVALID_ARG;

    return plugin->method_exit( plugin );
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: plugin.c 989 2005-09-08 14:31:57Z mman $
 */
