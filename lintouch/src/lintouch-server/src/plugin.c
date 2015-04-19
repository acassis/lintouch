/* $Id: plugin.c 1774 2006-05-31 13:17:16Z hynek $
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

#include <apr_dso.h>
#include <apr_strings.h>

#include "lt/server/plugin.h"
#include "lt/server/logger.h"
#include "plugin-private.h"
#include "ziparchive.h"

/**
 * Thread routine
 *
 * @param thread Thread
 * @param userarg User argument
 */
static void *
APR_THREAD_FUNC _lt_plugin_run( apr_thread_t * thread, void * userarg );

apr_pool_t * lt_server_plugin_pool_get( const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->pool;
}

lt_var_set_t * lt_server_plugin_varset_get( const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->varset;
}

const lt_server_plugin_config_t * lt_server_plugin_config_get(
        const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return &src->config;
}

void * lt_server_plugin_data_get( const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.data;
}
apr_file_t * lt_server_plugin_resource_get(
        const lt_server_plugin_t * src,
        const char * name, apr_pool_t * pool )
{
    if( src == NULL ) return NULL;
    if( name == NULL ) return NULL;

    const char * filename = apr_pstrcat( pool,
            "resources/", name, NULL );

    return lt_server_zip_extract_file( src->project,
            filename, pool );
}

const lt_server_plugin_config_t * lt_server_plugin_default_config_get(
        const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return &src->info.default_config;
}

const char * lt_server_plugin_author_get( const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.author;
}

const char * lt_server_plugin_version_get( const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.version;
}

const char * lt_server_plugin_date_get( const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.date;
}

const char * lt_server_plugin_shortdesc_get( const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.shortdesc;
}

const char * lt_server_plugin_longdesc_get( const lt_server_plugin_t * src )
{
    if( src == NULL ) return NULL;
    return src->info.longdesc;
}

lt_server_plugin_info_t * lt_server_plugin_info_create(
        const char * author,
        const char * version,
        const char * date,
        const char * shortdesc,
        const char * longdesc,
        apr_hash_t * default_properties,
        apr_hash_t * default_varproperties,
        void * data, apr_pool_t * pool )
{
    lt_server_plugin_info_t * result = NULL;

    result = (lt_server_plugin_info_t*) apr_pcalloc(
            pool, sizeof( lt_server_plugin_info_t ) );

    result->author = author;
    result->version = version;
    result->date = date;
    result->shortdesc = shortdesc;
    result->longdesc = longdesc;

    result->default_config.properties = default_properties;
    result->default_config.varproperties = default_varproperties;
    result->data = data;

    return result;
}

char lt_server_plugin_should_run( const lt_server_plugin_t * src )
{
    if( src == NULL ) return 0;
    return ! src->stoprequested;
}

lt_server_plugin_t * lt_server_plugin_load( const char * pluginfile,
        const char * projectfile,
        lt_project_connection_t * connection,
        apr_pool_t * pool )
{
    apr_dso_handle_t * handle = NULL;
    apr_dso_handle_sym_t sym_vinfo = NULL;
    apr_dso_handle_sym_t sym_create = NULL;
    apr_dso_handle_sym_t sym_init = NULL;
    apr_dso_handle_sym_t sym_run = NULL;

    lt_server_plugin_info_t * info = NULL;

    lt_server_plugin_t * result = NULL;

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
                    &sym_create, handle, "create" ) ) {
        lt_server_logger_print_apr_dso( PRIO_DEFAULT, handle,
                "Unable to resolve symbol ``create`` in %s\n",
                pluginfile );
        return NULL;
    }

    /* get required symbol out of DSO */
    if( APR_SUCCESS != apr_dso_sym(
                    &sym_init, handle, "init" ) ) {
        lt_server_logger_print_apr_dso( PRIO_DEFAULT, handle,
                "Unable to resolve symbol ``init`` in %s\n",
                pluginfile );
        return NULL;
    }

    /* get required symbol out of DSO */
    if( APR_SUCCESS != apr_dso_sym(
                    &sym_run, handle, "run" ) ) {
        lt_server_logger_print_apr_dso( PRIO_DEFAULT, handle,
                "Unable to resolve symbol ``run`` in %s\n",
                pluginfile );
        return NULL;
    }

    /* verify version info */
    {
        const char * vi =
            ( ( lt_server_plugin_version_info_t ) sym_vinfo )();

        if( ! verify_lt_plugin( "lt::ServerPlugin", vi ) ) {
            lt_server_logger_print( PRIO_DEFAULT,
                    "Unable to load plugin %s, plugin does not define "
                    "correct version_info\n",
                    pluginfile );
            return NULL;
        }
    }

    /* call create to get plugin specific data */
    info = ( ( lt_server_plugin_create_t ) sym_create ) ( pool );

    /* allocate output structure */
    result = ( lt_server_plugin_t * ) apr_pcalloc( pool,
            sizeof( lt_server_plugin_t ) );

    result->pool = pool;
    result->method_init = sym_init;
    result->method_run = sym_run;

    /* remember info */
    result->info = *info;

    /* remember varset + config */
    result->varset = lt_project_connection_variables_get( connection );
    result->config.properties =
        lt_project_connection_properties_get( connection );
    result->config.varproperties =
        lt_project_connection_varproperties_get( connection );

    /* remember project filename */
    result->project = projectfile;

    return result;
}

lt_server_status_t lt_server_plugin_init( lt_server_plugin_t * plugin )
{
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;
    if( plugin->method_init == NULL ) return LT_SERVER_INVALID_ARG;

    return plugin->method_init( plugin );
}

lt_server_status_t lt_server_plugin_start( lt_server_plugin_t * plugin )
{
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;

    /* prepare & start plugin thread */
    if( APR_SUCCESS != apr_thread_create(
                    &(plugin->thread), NULL,
                    (apr_thread_start_t)_lt_plugin_run, plugin,
                     plugin->pool ) ) {
        lt_server_logger_print_apr( PRIO_DEFAULT, "apr_thread_create",
                "Unable to create plugin thread\n" );
        return LT_SERVER_INVALID_ARG;
    }

    return LT_SERVER_SUCCESS;
}

lt_server_status_t lt_server_plugin_stop( lt_server_plugin_t * plugin )
{
    if( plugin == NULL ) return LT_SERVER_INVALID_ARG;

    /* bool assignment should be atomic operation so no locking is needed */
    plugin->stoprequested = 1;

    /* wait for plugin thread to finish. */
    {
        apr_status_t rv;

        if( APR_SUCCESS != apr_thread_join(
                        &rv, plugin->thread ) ) {
            lt_server_logger_print_apr( PRIO_DEFAULT, "apr_thread_join",
                    "Unable to wait for plugin thread to terminate\n" );
        }
    }

    return LT_SERVER_SUCCESS;
}

void * APR_THREAD_FUNC _lt_plugin_run( apr_thread_t * thread, void * userarg )
{
    /* convert thread routine argument to our plugin structure */
    lt_server_plugin_t * plugin = (lt_server_plugin_t*) userarg;

    lt_server_logger_print( PRIO_THREAD,
            "Plugin thread started...\n" );

    /* start plugin run routine */
    if( plugin->method_run != NULL ) plugin->method_run( plugin );

    lt_server_logger_print( PRIO_THREAD,
            "Plugin thread exiting...\n" );

    apr_thread_exit( thread, 0 );

    return NULL;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: plugin.c 1774 2006-05-31 13:17:16Z hynek $
 */
