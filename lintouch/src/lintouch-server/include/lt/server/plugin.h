/* $Id: plugin.h 1576 2006-04-24 10:21:26Z modesto $
 *
 *   FILE: plugin.h --
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

#ifndef _LT_PLUGIN_H
#define _LT_PLUGIN_H

#include <apr_general.h>
#include <apr_version.h>
#include <apr_file_io.h>

#include <lt/variables/variables.h>

#include "lt/server/project.h"

#include "lt/server/plugin_common.h"

/**
 * @file plugin.h
 * @brief Lintouch Server Plugin Handling Library
 */

/**
 * @defgroup LTSP Lintouch Server Plugin Handling Library
 * @{
 *
 * Lintouch Server Plugin Handling Library.
 *
 * Methods in this module allow you to load/unload given lintouch server
 * plugin and invoke its methods. It also contains methods that make the life
 * of plugin developserverer easier.
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Place this macro on top of your c plugin implementation file to have
 * the extern C support functions automatically created.
 */
#define LT_SERVER_PLUGIN_EXPORT \
\
    static const char * version_info = \
    "interface=lt::ServerPlugin," \
    "apr_version="APR_VERSION_STRING"," \
    "ltserver_major="LT_SERVER_MAJOR_STR"," \
    "ltserver_minor="LT_SERVER_MINOR_STR"," \
    "ltserver_patchlevel="LT_SERVER_PATCH_STR"\0"; \
\
    PLUGIN_EXPORT const char * lt_version_info() { \
        return version_info; \
    }

/** The default name of plugin's create method */
#define LT_SERVER_PLUGIN_CREATE create
/** The default name of plugin's init method */
#define LT_SERVER_PLUGIN_INIT   init
/** The default name of plugin's run method */
#define LT_SERVER_PLUGIN_RUN    run

    /**
     * Lintouch Server Plugin.
     *
     * Contains plugin specific data, varset, plugin properties, properties
     * for variables created in varset.
     */
    typedef struct lt_server_plugin_t lt_server_plugin_t;

    /**
     * Lintouch Server Plugin Config.
     *
     * Contains plugin properties and properties for all plugin exported
     * variables.
     */
    typedef struct lt_server_plugin_config_t
    {
        /** Connection properties */
        apr_hash_t * properties;
        /** Variable properties */
        apr_hash_t * varproperties;
    } lt_server_plugin_config_t;


    /**
     * Lintouch Server Plugin Meta info.
     *
     * Contains information about author, description, plus default values
     * for plugin properties and properties of variables of given type.
     */
    typedef struct lt_server_plugin_info_t lt_server_plugin_info_t;

    /**
     * Type definition for plugin's create routine.
     */
    typedef void *
        (PLUGIN_EXPORT *lt_server_plugin_create_t)(apr_pool_t*);

    /**
     * Type definition for plugin's init routine.
     */
    typedef lt_server_status_t
        (PLUGIN_EXPORT *lt_server_plugin_init_t)(lt_server_plugin_t*);

    /**
     * Type definition for plugin's run routine.
     */
    typedef int
        (PLUGIN_EXPORT *lt_server_plugin_run_t)(lt_server_plugin_t*);

    /**
     * Return pool out of which the given plugin has been allocated.
     *
     * @param src The plugin whose pool should be returned.
     * @return The plugin memory pool.
     */
    apr_pool_t * lt_server_plugin_pool_get( const lt_server_plugin_t * src );

    /**
     * Return varset containing all variables exported by plugin.
     *
     * @param src The plugin whose varset should be returned.
     * @return The plugin varset.
     */
    lt_var_set_t * lt_server_plugin_varset_get(
            const lt_server_plugin_t * src );

    /**
     * Return plugin config.
     *
     * The ``plugin config`` is the real configuration passed by Lintouch
     * Server to the plugin instance. The configuration information is built
     * by parsing connection descriptor from the project.ltp.
     *
     * @param src The plugin whose config should be returned.
     * @return The plugin config.
     */
    const lt_server_plugin_config_t * lt_server_plugin_config_get(
            const lt_server_plugin_t * src );

    /**
     * Return plugin specific data.
     *
     * @param src The plugin whose data should be returned.
     * @return The plugin varset.
     */
    void * lt_server_plugin_data_get( const lt_server_plugin_t * src );

    /**
     * Extract and open given project resource.
     *
     * @param src The plugin whose resource should be returned.
     * @param name The resource name.
     * @param pool The pool to allocate resource related data out of.
     * @return Opened resource file or NULL.
     */
    apr_file_t * lt_server_plugin_resource_get(
            const lt_server_plugin_t * src,
            const char * name, apr_pool_t * pool );

    /**
     * Return default plugin config.
     *
     * The ``default config`` is the config provided by the plugin to the
     * Lintouch Editor. Lintouch Editor determines what properties the
     * plugin supports and what are their default values.
     *
     * @param src The plugin whose default config should be returned.
     * @return The default plugin config.
     */
    const lt_server_plugin_config_t * lt_server_plugin_default_config_get(
            const lt_server_plugin_t * src );

    /**
     * Return plugin author.
     *
     * @param src The plugin whose author should be returned.
     * @return The plugin author.
     */
    const char * lt_server_plugin_author_get( const lt_server_plugin_t * src );

    /**
     * Return plugin version.
     *
     * @param src The plugin whose version should be returned.
     * @return The plugin version.
     */
    const char * lt_server_plugin_version_get( const lt_server_plugin_t * src );

    /**
     * Return plugin date.
     *
     * @param src The plugin whose date should be returned.
     * @return The plugin date.
     */
    const char * lt_server_plugin_date_get( const lt_server_plugin_t * src );

    /**
     * Return plugin shortdesc.
     *
     * @param src The plugin whose shortdesc should be returned.
     * @return The plugin shortdesc.
     */
    const char * lt_server_plugin_shortdesc_get(
            const lt_server_plugin_t * src );

    /**
     * Return plugin longdesc.
     *
     * @param src The plugin whose longdesc should be returned.
     * @return The plugin longdesc.
     */
    const char * lt_server_plugin_longdesc_get(
            const lt_server_plugin_t * src );

    /**
     * Create plugin info.
     *
     * Create plugin info that can be returned from plugin's create method.
     *
     * All arguments passed to this method should be already allocated from
     * a safe memory pool. No copies are made.
     *
     * @param author The plugin author.
     * @param version The plugin version.
     * @param date The plugin date of creation/last modification. In ISO format
     * (for ex. "2005-04-01").
     * @param shortdesc The plugin short description.
     * @param longdesc The plugin long description.
     * @param default_properties The default properties of a plugin.
     * @param default_varproperties The default properties of a variable
     * handled by plugin.
     * @param data The plugin specific data.
     * @param pool The pool to allocate info out of.
     * @return The newly created plugin info.
     */
    lt_server_plugin_info_t * lt_server_plugin_info_create(
            const char * author,
            const char * version,
            const char * date,
            const char * shortdesc,
            const char * longdesc,
            apr_hash_t * default_properties,
            apr_hash_t * default_varproperties,
            void * data, apr_pool_t * pool );

    /**
     * Should the plugin run?
     *
     * @param src The plugin whose run status is to be determined.
     * @return 1 if the plugin should be running, 0 otherwise.
     */
    char lt_server_plugin_should_run( const lt_server_plugin_t * src );

    /**
     * Load a given plugin.
     *
     * @param pluginfile The file that should be loaded.
     * @param projectfile The project.ltp that the plugin might need to
     * access resources.
     * @param connection The connection this plugin will serve.
     * @param pool The pool to allocate plugin out of, when NULL is passed
     * in, new pool will be allocated.
     * @return Loaded plugin or NULL in case of problem.
     */
    lt_server_plugin_t * lt_server_plugin_load( const char * pluginfile,
            const char * projectfile,
            lt_project_connection_t * connection,
            apr_pool_t * pool );

    /**
     * Initialize given plugin.
     *
     * @param plugin The plugin to be initialized.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_plugin_init(
            lt_server_plugin_t * plugin );

    /**
     * Start given plugin.
     *
     * This call will start the given plugin's run method in a separate
     * thread.
     *
     * @param plugin The plugin to be started.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_plugin_start(
            lt_server_plugin_t * plugin );

    /**
     * Stop the plugin.
     *
     * This call signals the given plugin to be stopped. The plugin should
     * behave correctly and terminates as soon as possible after being
     * signaled. Chances are that it will be killed after some time.
     *
     * @param plugin The plugin to be stopped.
     * @return LT_SERVER_SUCCESS or LT_SERVER_INVALID_ARG.
     */
    lt_server_status_t lt_server_plugin_stop(
            lt_server_plugin_t * plugin );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_PLUGIN_H */
/* vim: set et ts=4 sw=4 tw=76:
 * $Id: plugin.h 1576 2006-04-24 10:21:26Z modesto $
 */

