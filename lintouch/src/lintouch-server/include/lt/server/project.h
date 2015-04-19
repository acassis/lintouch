/* $Id: project.h 1519 2006-03-08 10:31:24Z modesto $
 *
 *   FILE: project.h --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 20 April 2005
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

#ifndef _LT_PROJECT_H
#define _LT_PROJECT_H

#include <apr_general.h>

/**
 * @file project.h
 * @brief Lintouch Project Parsing Routines.
 */

/**
 * @defgroup LTSPR Lintouch Project Parsing Routines.
 * @{
 *
 * Lintouch Server Project Parsing Routines.
 *
 * Methods in this module allow you to parse various files from Lintouch
 * Project.
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    typedef int lt_project_status_t;

#define LT_PROJECT_SUCCESS       0
#define LT_PROJECT_INVALID_ARG  -1

    typedef enum {

        /** Project Info */
        LT_PROJECT_INFO = 0x1,

        /** Project Connections */
        LT_PROJECT_CONNECTIONS = 0x2,

        /** Project Views */
        LT_PROJECT_VIEWS = 0x4,

        /** Project Template Libraries */
        LT_PROJECT_TEMPLATE_LIBRARIES = 0x8,

        /** Project Bindings */
        LT_PROJECT_BINDINGS = 0x10,

        /** Project Bindings */
        LT_PROJECT_LOGGING = 0x20,

        /** All Parts of the Project */
        LT_PROJECT_ALL = 0xFFFF

    } lt_project_members_e;

    /**
     * Lintouch Project.
     */
    typedef struct lt_project_t lt_project_t;

    /**
     * Lintouch Project Connection.
     */
    typedef struct lt_project_connection_t lt_project_connection_t;

    /**
     * Lintouch Project Logging.
     */
    typedef struct lt_project_logging_t lt_project_logging_t;

    /**
     * Return pool out of which the given project has been
     * allocated.
     *
     * @param src The project whose pool should be returned.
     * @return The project memory pool.
     */
    apr_pool_t * lt_project_pool_get( const lt_project_t * src );

    /**
     * Parse the given parts of project.
     *
     * @param project Where to store parsed project.
     * @param archive The lintouch project file.
     * @param what_to_parse lt_project_members_e ored flag that indicates
     * what parts of the project should be parsed.
     * @param pool The pool to allocate project out of. When null is pased
     * in, new pool is created.
     * @return The parsing result.
     */
    lt_project_status_t lt_project_parse( lt_project_t ** project,
            const char * archive, int what_to_parse,
            apr_pool_t * pool );

    /**
     * Return the project author.
     *
     * @param project The project.
     * @return The project author.
     */
    const char * lt_project_author_get( lt_project_t * project );

    /**
     * Return the project version.
     *
     * @param project The project.
     * @return The project version.
     */
    const char * lt_project_version_get( lt_project_t * project );

    /**
     * Return the project date.
     *
     * @param project The project.
     * @return The project date.
     */
    const char * lt_project_date_get( lt_project_t * project );

    /**
     * Return the project shortdesc.
     *
     * @param project The project.
     * @return The project shortdesc.
     */
    const char * lt_project_shortdesc_get( lt_project_t * project );

    /**
     * Return the project longdesc.
     *
     * @param project The project.
     * @return The project longdesc.
     */
    const char * lt_project_longdesc_get( lt_project_t * project );

    /**
     * Return the project connections.
     *
     * The returned hash is indexed by ``const char * connection_name`` and
     * stores ``lt_project_connection_t * connection``.
     *
     * @param project The project.
     * @return The project connections.
     */
    apr_hash_t * lt_project_connections_get( lt_project_t * project );

    /**
     * Return the connection type.
     *
     * @param conn The project connection.
     * @return The connection type.
     */
    const char * lt_project_connection_type_get(
            lt_project_connection_t * conn );

    /**
     * Return the connection properties.
     *
     * The returned hash is indexed by ``const char * property_name`` and
     * stores ``const char * property_value``.
     *
     * @param conn The project connection.
     * @return The connection properties.
     */
    apr_hash_t * lt_project_connection_properties_get(
            lt_project_connection_t * conn );

    /**
     * Return the variable properties of a connection.
     *
     * The returned hash is indexed by ``const char * variable_name`` and
     * stores ``apr_hash_t * variable_properties``.
     *
     * variable_properties hash is indexed by ``const char * property_name``
     * and stores ``const char * property_value``.
     *
     * @param conn The project connection.
     * @return The connection properties.
     */
    apr_hash_t * lt_project_connection_varproperties_get(
            lt_project_connection_t * conn );

    /**
     * Return the connection variables.
     *
     * @param conn The project connection.
     * @return The connection variables.
     */
    lt_var_set_t * lt_project_connection_variables_get(
            lt_project_connection_t * conn );

    /**
     * Return the project logging plugins.
     *
     * The returned hash is indexed by ``const char * logging_name`` and
     * stores ``lt_project_logging_t * logging``.
     *
     * @param project The project.
     * @return The project logging plugins.
     */
    apr_hash_t * lt_project_logging_get( lt_project_t * project );

    /**
     * Return the logging type.
     *
     * @param conn The project logging plugin.
     * @return The loggign type.
     */
    const char * lt_project_logging_type_get(
            lt_project_logging_t * conn );

    /**
     * Return the logging plugin properties.
     *
     * The returned hash is indexed by ``const char * property_name`` and
     * stores ``const char * property_value``.
     *
     * @param conn The project logging plugin.
     * @return The logging plugin properties.
     */
    apr_hash_t * lt_project_logging_properties_get(
            lt_project_logging_t * conn );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_PROJECT_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: project.h 1519 2006-03-08 10:31:24Z modesto $
 */
