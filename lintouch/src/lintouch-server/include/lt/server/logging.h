/* $Id: logging.h 1559 2006-04-06 16:18:54Z modesto $
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

#ifndef _LT_LOGGING_PLUGIN_H
#define _LT_LOGGING_PLUGIN_H

#include <apr_general.h>
#include <apr_version.h>
#include <apr_file_io.h>
#include <apr_hash.h>

#include <lt/variables/variables.h>

#include "lt/server/project.h"

#include "lt/server/plugin_common.h"

/**
 * @file plugin.h
 * @brief Lintouch Server Logging Plugin Handling Library
 */

/**
 * @defgroup LLP Lintouch Logging Plugin Handling Library
 * @{
 *
 * Lintouch Logging Plugin Handling Library.
 *
 * Methods in this module allow you to load/unload given lintouch logging
 * plugin and invoke its methods. It also contains methods that make the life
 * of logging plugin developserverer easier.
 *
 * @author Patrik Modesto <modesto@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* to convert #defined fucntions name to a string */
#define MAKE_STRING(a) #a
#define STR(x) MAKE_STRING(x)

#ifdef _WIN32
#   define PLUGIN_EXPORT    __declspec(dllexport)
#else
#   define PLUGIN_EXPORT
#endif

/**
 * Place this macro on top of your c plugin implementation file to have
 * the extern C support functions automatically created.
 */
#define LT_LOGGING_PLUGIN_EXPORT \
\
    static const char * version_info = \
    "interface=lt::LoggingPlugin," \
    "apr_version="APR_VERSION_STRING"," \
    "ltserver_major="LT_SERVER_MAJOR_STR"," \
    "ltserver_minor="LT_SERVER_MINOR_STR"," \
    "ltserver_patchlevel="LT_SERVER_PATCH_STR"\0"; \
\
    PLUGIN_EXPORT const char * lt_version_info() { \
        return version_info; \
    }

/** The default name of plugin's create method */
#define LT_LOGGING_PLUGIN_CREATE logging_create
/** The default name of plugin's init method */
#define LT_LOGGING_PLUGIN_INIT logging_init
/** The default name of plugin's log method */
#define LT_LOGGING_PLUGIN_LOG logging_log
/** The default name of plugin's exit method */
#define LT_LOGGING_PLUGIN_EXIT logging_exit

    /**
     * Lintouch Logging Plugin.
     *
     * Contains plugin specific data and plugin properties.
     */
    typedef struct lt_logging_plugin_t lt_logging_plugin_t;

    /**
     * Lintouch Logging Plugin Meta info.
     *
     * Contains information about author, description, plus default values
     * for plugin properties and properties of variables of given type.
     */
    typedef struct lt_logging_plugin_info_t lt_logging_plugin_info_t;

    /**
     * Type definition for plugin's version_info routine.
     */
    typedef const char *
        (PLUGIN_EXPORT *lt_logging_plugin_version_info_t)();

    /**
     * Type definition for plugin's create routine.
     */
    typedef void *
        (PLUGIN_EXPORT *lt_logging_plugin_create_t)(apr_pool_t *);

    /**
     * Type definition for plugin's init routine.
     */
    typedef lt_server_status_t
        (PLUGIN_EXPORT *lt_logging_plugin_init_t)(
                                                  lt_logging_plugin_t*,
                                                  apr_hash_t* );

    /**
     * Type definition for plugin's log routine.
     */
    typedef lt_server_status_t
        (PLUGIN_EXPORT *lt_logging_plugin_log_t)(lt_logging_plugin_t*,
                                                 apr_time_exp_t*,
                                                 lt_var_set_t*,
                                                 int real);

    /**
     * Type definition for plugin's exit routine.
     */
    typedef lt_server_status_t
        (PLUGIN_EXPORT *lt_logging_plugin_exit_t)(lt_logging_plugin_t*);

    /**
     * Return pool out of which the given plugin has been allocated.
     *
     * @param src The plugin whose pool should be returned.
     * @return The plugin memory pool.
     */
    apr_pool_t * lt_logging_plugin_pool_get( const lt_logging_plugin_t * src );

    /**
     * Return plugin config.
     *
     * The ``plugin config`` is the real configuration passed by Lintouch
     * logging to the plugin instance. The configuration information is built
     * by parsing connection descriptor from the project.ltp.
     *
     * @param src The plugin whose config should be returned.
     * @return The plugin config.
     */
    apr_hash_t * lt_logging_plugin_properties_get(
            const lt_logging_plugin_t * src );

    /**
     * Return plugin specific data.
     *
     * @param src The plugin whose data should be returned.
     * @return The plugin varset.
     */
    void * lt_logging_plugin_data_get( const lt_logging_plugin_t * src );

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
    apr_hash_t * lt_logging_plugin_default_properties_get(
            const lt_logging_plugin_t * src );

    /**
     * Return plugin author.
     *
     * @param src The plugin whose author should be returned.
     * @return The plugin author.
     */
    const char * lt_logging_plugin_author_get( const lt_logging_plugin_t * src );

    /**
     * Return plugin version.
     *
     * @param src The plugin whose version should be returned.
     * @return The plugin version.
     */
    const char * lt_logging_plugin_version_get( const lt_logging_plugin_t * src );

    /**
     * Return plugin date.
     *
     * @param src The plugin whose date should be returned.
     * @return The plugin date.
     */
    const char * lt_logging_plugin_date_get( const lt_logging_plugin_t * src );

    /**
     * Return plugin shortdesc.
     *
     * @param src The plugin whose shortdesc should be returned.
     * @return The plugin shortdesc.
     */
    const char * lt_logging_plugin_shortdesc_get(
            const lt_logging_plugin_t * src );

    /**
     * Return plugin longdesc.
     *
     * @param src The plugin whose longdesc should be returned.
     * @return The plugin longdesc.
     */
    const char * lt_logging_plugin_longdesc_get(
            const lt_logging_plugin_t * src );

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
     * @param data The plugin specific data.
     * @param pool The pool to allocate info out of.
     * @return The newly created plugin info.
     */
    lt_logging_plugin_info_t * lt_logging_plugin_info_create(
            const char * author,
            const char * version,
            const char * date,
            const char * shortdesc,
            const char * longdesc,
            apr_hash_t * default_properties,
            void * data, apr_pool_t * pool );

    /**
     * Load a given plugin.
     *
     * @param pluginfile The file that should be loaded.
     * @param connection The connection this plugin will serve.
     * @param pool The pool to allocate plugin out of, when NULL is passed
     * in, new pool will be allocated.
     * @return Loaded plugin or NULL in case of problem.
     */
    lt_logging_plugin_t * lt_logging_plugin_load( const char * pluginfile,
            lt_project_logging_t * log,
            apr_pool_t * pool );

    /**
     * Initialize given plugin.
     *
     * @param plugin The plugin to init.
     * @param connections List of all loaded lsps.
     * @return LT_logging_SUCCESS or LT_logging_INVALID_ARG.
     */
    lt_server_status_t lt_logging_plugin_init( lt_logging_plugin_t * plugin,
            apr_hash_t * connections );

    /**
     * Log given varset with the given logging plugin.
     *
     * @param plugin The plugin to init.
     * @param timestamp The timestamp of this log.
     * @param vset The varset to log.
     * @param real A flag that is true if real values should be logged,
     * false for requested.
     * @return LT_logging_SUCCESS or LT_logging_INVALID_ARG.
     */
    lt_server_status_t lt_logging_plugin_log( lt_logging_plugin_t * plugin,
            apr_time_exp_t* timestamp, lt_var_set_t* vset, int real );

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
    lt_server_status_t lt_logging_plugin_exit(
            lt_logging_plugin_t * plugin );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_LOGGING_PLUGIN_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: logging.h 1559 2006-04-06 16:18:54Z modesto $
 */

