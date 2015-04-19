/* $Id: demo_plugins.h 593 2005-04-27 09:46:33Z mman $
 *
 *   FILE: demo_plugins.h --
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

#ifndef _LT_DEMO_PLUGINS_H
#define _LT_DEMO_PLUGINS_H

#include <apr_general.h>

#include "lt/server/plugin.h"

/**
 * @file demo_plugins.h
 * @brief Lintouch Server Demonstration Plugins
 */

/**
 * @defgroup LTDPL Lintouch Server Demonstration Plugins
 * @{
 *
 * Lintouch Server Demonstration Plugins
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Instantiate demonstration generator plugin.
     *
     * The plugin will export the following variables:
     *
     * "bit"    - generating 0..1 sequence
     * "number" - generating 0..65535 sequence
     * "string" - generating "0".."65535" sequence
     *
     * @param refresh How often to generate new value.
     * @param pool The pool to allocate plugin out of. When null, new pool
     * will be allocated.
     * @return Initialized plugin ready to be registered with the server.
     */
    lt_server_plugin_t * lt_server_demo_plugin_generator_create(
            apr_time_t refresh, apr_pool_t * pool );

    /**
     * Instantiate demonstration loopback plugin.
     *
     * The plugin will export the following variables:
     *
     * "bit"    - looping back given bit
     * "number" - looping back given number in 0..65535 range
     * "string" - looping back given string
     *
     * @param latency For how long to sleep before looping the changed
     * values back.
     * @param pool The pool to allocate plugin out of. When null, new pool
     * will be allocated.
     * @return Initialized plugin ready to be registered with the server.
     */
    lt_server_plugin_t * lt_server_demo_plugin_loopback_create(
            apr_time_t latency, apr_pool_t * pool );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_DEMO_PLUGINS_H */
/* vim: set et ts=4 sw=4 tw=76:
 * $Id: demo_plugins.h 593 2005-04-27 09:46:33Z mman $
 */

