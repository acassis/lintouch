/* $Id:$
 *
 *   FILE: plugin-private.c --
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

#include <stdio.h>

#include <apr_version.h>
#include <apr_dso.h>
#include <apr_strings.h>
#include <apr_hash.h>

#include "lt/server/plugin_common.h"
#include "lt/server/logger.h"

int verify_lt_plugin( const char * classnamestr,
        const char * versioninfo )
{
    char * str = NULL;
    char * tok = NULL;
    char * strtokstate = NULL;
    apr_pool_t * lpool = NULL;
    apr_hash_t * fields = NULL;

    apr_pool_create( &lpool, NULL );
    str = apr_pstrdup( lpool, versioninfo );
    fields = apr_hash_make( lpool );

    /* convert coma separated list of = assignments to a hash table */
    tok = apr_strtok( str, ",", &strtokstate );
    while( tok != NULL ) {
        char * key = NULL, * val = NULL;
        char * state = NULL;

        /* printf( "got token: %s\n", tok ); */

        key = apr_strtok( tok, "=", &state );
        val = apr_strtok( NULL, "=", &state );

        /* printf( "key %s, val %s\n", key, val ); */
        apr_hash_set( fields, key, APR_HASH_KEY_STRING, val );

        tok = apr_strtok( NULL, ",", &strtokstate );
    }


    /* check exported interface */
    {
        char * val = apr_hash_get( fields, "interface", APR_HASH_KEY_STRING );
        if( val == NULL || classnamestr == NULL ||
                apr_strnatcmp( classnamestr, val ) != 0 ) {
            lt_server_logger_print( PRIO_DEFAULT,
                    "Error when loading plugin,"
                    " plugin implements interface '%s'\n"
                    " you wanted '%s'", val, classnamestr );
            apr_pool_destroy( lpool );
            return 0;
        }
    }

    /* check APR version */
    {
        char * val = apr_hash_get( fields, "apr_version", APR_HASH_KEY_STRING );
        if( val == NULL || apr_strnatcmp( APR_VERSION_STRING, val ) != 0 ) {
            lt_server_logger_print( PRIO_DEFAULT,
                    "Error when loading plugin,"
                    " plugin was compiled with APR version '%s'"
                    " your application is compiled with APR version '%s'\n",
                    val, APR_VERSION_STRING );
            apr_pool_destroy( lpool );
            return 0;
        }
    }

    /* check ltserver major version only */
    {
        char * val = apr_hash_get( fields,
                "ltserver_major", APR_HASH_KEY_STRING );
        if( val == NULL || apr_strnatcmp( LT_SERVER_MAJOR_STR, val ) != 0 ) {
            lt_server_logger_print( PRIO_DEFAULT,
                    "Error when loading plugin,"
                    " plugin was compiled with ltserver major version '%s'"
                    " your application is compiled with ltserver"
                    " major version '%s'\n",
                    val, LT_SERVER_MAJOR_STR );
            apr_pool_destroy( lpool );
            return 0;
        }
    }

    apr_pool_destroy( lpool );
    return 1;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id:$
 */
