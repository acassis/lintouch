/* $Id: ltapi.h 787 2005-06-13 13:21:41Z mman $
 *
 *   FILE: ltapi.h --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE:  9 September 2005
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

#ifndef _LTAPI_H
#define _LTAPI_H

#include <lt/variables/variables.h>
#include <lt/server/plugin.h>

#include <lua.h>
#include <lualib.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Load standard Lua libraries to the Lua interpreter.
     */
    int ltapi_libraries_load( lua_State * L );

    /**
     * Register Lintouch exported functions to the Lua interpreter.
     */
    int ltapi_methods_register( lua_State * L );

    /**
     * Register Lintouch exported globals to the Lua interpreter. At this
     * moment the following tables are exported:
     *
     * - variables: LUA_TTABLE
     * - plugin: LUA_TLIGHTUSERDATA
     */
    int ltapi_globals_register( lua_State * L,
            lt_server_plugin_t * plugin,
            lt_var_set_t * varset );

    /**
     * Read contents of the given file and execute it.
     */
    int ltapi_file_execute( lua_State * L, apr_file_t * file,
            const char * name );

#ifdef __cplusplus
}
#endif

#endif /* _LTAPI_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: ltapi.h 787 2005-06-13 13:21:41Z mman $
 */
