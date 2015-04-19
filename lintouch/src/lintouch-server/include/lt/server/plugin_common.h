/* $Id: plugin_common.h 1524 2006-03-10 09:39:58Z modesto $
 *
 *   FILE: plugin_common.h --
 * AUTHOR: Patrik Modesto <modesto@swac.cz>
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

#ifndef _LT_PLUGIN_COMMON_H
#define _LT_PLUGIN_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#   define PLUGIN_EXPORT    __declspec(dllexport)
#else
#   define PLUGIN_EXPORT
#endif

#define LT_SERVER_MAJOR_STR    "0"
#define LT_SERVER_MINOR_STR    "0"
#define LT_SERVER_PATCH_STR    "0"

    /**
     * Return value for functions.
     */
    typedef int lt_server_status_t;

    /**
     * Successfull termination.
     */
#define LT_SERVER_SUCCESS    0
    /**
     * Invalid argument (possibly NULL) was given to the function.
     */
#define LT_SERVER_INVALID_ARG    -1
    /**
     * Method has not been implemented yet.
     */
#define LT_SERVER_METHOD_UNIMPLEMENTED    -99


    /**
     * Type definition for plugin's version_info routine.
     */
    typedef const char *
        (PLUGIN_EXPORT *lt_server_plugin_version_info_t)();


    /**
     * Verify that the version numbers exported by the plugin match our version
     * numbers.
     *
     * @param classnamestr The interface that the plugin should implement.
     * @param versioninfo The version info to be virified.
     * @return 1 if version numbers match, 0 otherwise.
     */
    int verify_lt_plugin( const char * classnamestr,
            const char * versioninfo );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_LOGGING_PLUGIN_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: plugin_common.h 1524 2006-03-10 09:39:58Z modesto $
 */

