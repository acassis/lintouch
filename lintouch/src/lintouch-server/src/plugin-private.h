/* $Id: plugin-private.h 593 2005-04-27 09:46:33Z mman $
 *
 *   FILE: plugin-private.h --
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

#ifndef _LT_PLUGIN_PRIVATE_H
#define _LT_PLUGIN_PRIVATE_H

#include <apr_thread_proc.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct lt_server_plugin_info_t
    {
        /** author */
        const char * author;
        /** version */
        const char * version;
        /** date */
        const char * date;
        /** short description */
        const char * shortdesc;
        /** long description */
        const char * longdesc;

        /** default property values as defined by plugin itself */
        lt_server_plugin_config_t default_config;

        /** plugin specific data */
        void * data;
    };

    struct lt_server_plugin_t
    {
        apr_pool_t * pool;
        apr_thread_t * thread;

        char stoprequested;

        /** plugin metainfo including plugin specific data */
        lt_server_plugin_info_t info;
        /** properties for given plugin instance */
        lt_server_plugin_config_t config;

        /** variables */
        lt_var_set_t * varset;

        /** method callbacks */
        lt_server_plugin_run_t method_init;
        lt_server_plugin_run_t method_run;

        /** project.ltp - to allow plugins to access resources */
        const char * project;
    };

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_PLUGIN_PRIVATE_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: plugin-private.h 593 2005-04-27 09:46:33Z mman $
 */

