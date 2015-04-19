/* $Id:$
 *
 *   FILE: ltapi.c --
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

#include "lt/server/logger.h"

#include "ltapi.h"

/**
 * Sleep for given amount of time (in microseconds) and return whether
 * the plugin should still be running.
 *
 * INPUTS:
 * - time: LUA_TNUMBER
 *
 * OUTPUTS:
 * - should_run: LUA_TBOOLEAN
 */
static int _ltapi_sleep( lua_State * L );

/**
 * Determine whether the plugin should still be running.
 *
 * INPUTS:
 *
 * OUTPUTS:
 * - should_run: LUA_TBOOLEAN
 */
static int _ltapi_should_run( lua_State * L );

/**
 * Lock/Unlock/Trylock requested values.
 *
 * INPUTS:
 *
 * OUTPUTS:
 */
static int _ltapi_lock_requested( lua_State * L );
static int _ltapi_unlock_requested( lua_State * L );
static int _ltapi_trylock_requested( lua_State * L );

/**
 * Lock/Unlock/Trylock real values.
 *
 * INPUTS:
 *
 * OUTPUTS:
 */
static int _ltapi_lock_real( lua_State * L );
static int _ltapi_unlock_real( lua_State * L );
static int _ltapi_trylock_real( lua_State * L );

/**
 * Wait for new requested values for given time (in microseconds).
 *
 * INPUTS:
 * - time: LUA_TNUMBER
 *
 * OUTPUTS:
 * - new_requested_available: LUA_TBOOLEAN
 */
static int _ltapi_wait_requested( lua_State * L );

/**
 * Sync real/requested values.
 *
 * INPUTS:
 *
 * OUTPUTS:
 */
static int _ltapi_sync_requested( lua_State * L );
static int _ltapi_sync_real( lua_State * L );

/**
 * get real/requested value.
 *
 * INPUTS:
 * - variable: LUA_TLIGHTUSERDATA
 *
 * OUTPUTS:
 * - value: LUA_TBOOLEAN, LUA_TNUMBER, LUA_TSTRING
 */
static int _ltapi_get_requested( lua_State * L );
static int _ltapi_get_real( lua_State * L );

/**
 * is real/requested value dirty?.
 *
 * INPUTS:
 * - variable: LUA_TLIGHTUSERDATA
 *
 * OUTPUTS:
 * - value: LUA_TBOOLEAN
 */
static int _ltapi_is_requested_dirty( lua_State * L );
static int _ltapi_is_real_dirty( lua_State * L );

/**
 * set real/requested value.
 *
 * INPUTS:
 * - variable: LUA_TLIGHTUSERDATA
 * - value: LUA_TBOOLEAN, LUA_TNUMBER, LUA_TSTRING
 *
 * OUTPUTS:
 */
static int _ltapi_set_requested( lua_State * L );
static int _ltapi_set_real( lua_State * L );

/**
 * Structure used to pass apr_file_t * and reading buffer to the chunkreader
 */
typedef struct {
    apr_file_t * file;
    char buffer[4096];
} _ltapi_apr_file_chunkreader_data;

/**
 * Chunk reader reading from an apr_file_t.
 * Read a chunk of data from an apr_file_t * (extracted from `void * data`
 * that is retyped to _ltapi_apr_file_chunkreader_data) and return it, its
 * size will be reported via `size`. At EOF NULL is returned.
 */
static const char * _ltapi_apr_file_chunkreader( lua_State * L,
        void * data, size_t * size );

/* METHOD BODIES */

int ltapi_libraries_load( lua_State * L )
{
    /* load required library and discard any possible results remaining at
     * the stack */

    luaopen_base( L );
    lua_settop( L, 0 );

    luaopen_table( L );
    lua_settop( L, 0 );

    luaopen_io( L );
    lua_settop( L, 0 );

    luaopen_string( L );
    lua_settop( L, 0 );

    luaopen_math( L );
    lua_settop( L, 0 );

    luaopen_debug( L );
    lua_settop( L, 0 );

    luaopen_loadlib( L );
    lua_settop( L, 0 );

    return 0;
}

int ltapi_methods_register( lua_State * L )
{
    /* register C functions as Lua methods */

    lua_register( L, "sleep", _ltapi_sleep );
    lua_register( L, "should_run", _ltapi_should_run );

    lua_register( L, "lock_requested_values", _ltapi_lock_requested );
    lua_register( L, "trylock_requested_values", _ltapi_trylock_requested );
    lua_register( L, "unlock_requested_values", _ltapi_unlock_requested );

    lua_register( L, "lock_real_values", _ltapi_lock_real );
    lua_register( L, "trylock_real_values", _ltapi_trylock_real );
    lua_register( L, "unlock_real_values", _ltapi_unlock_real );

    lua_register( L, "wait_for_new_requested_values", _ltapi_wait_requested );

    lua_register( L, "sync_requested_values", _ltapi_sync_requested );
    lua_register( L, "sync_real_values", _ltapi_sync_real );

    lua_register( L, "get_requested_value", _ltapi_get_requested );
    lua_register( L, "get_real_value", _ltapi_get_real );

    lua_register( L, "set_requested_value", _ltapi_set_requested );
    lua_register( L, "set_real_value", _ltapi_set_real );

    lua_register( L, "is_requested_value_dirty", _ltapi_is_requested_dirty );
    lua_register( L, "is_real_value_dirty", _ltapi_is_real_dirty );

    return 0;
}

int ltapi_globals_register( lua_State * L, lt_server_plugin_t * plugin,
        lt_var_set_t * varset )
{
    /* create new empty table and set it under the key "variables" into the
     * table of globals */
    lua_pushstring( L, "variables" );
    lua_newtable( L );

    /* for each variable in a varset, create light user data object and push
     * it under the key to the table at -3 in the stack */
    {
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;
        const char * key = NULL;
        lt_var_t * var = NULL;
        apr_pool_t * pool = NULL;
        apr_pool_create( &pool, NULL );

        h = lt_var_set_variables_get( varset );
        hi = apr_hash_first( pool, h );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
            apr_hash_this( hi, (const void**)&key, NULL, (void**)&var );

#if 0
            fprintf( stderr, "DEBUG: creating lightuserdata for variable %s"
                    " of lintouch type %d\n", key, lt_var_type_get( var ) );
#endif

            lua_pushstring( L, key );
            lua_pushlightuserdata( L, (void*) var );
            lua_settable( L, -3 );
        }

        apr_pool_destroy( pool );
    }

    lua_settable( L, LUA_GLOBALSINDEX );

    /* create new light user data and set it under the key "plugin" into the
     * table of globals */
    lua_pushstring( L, "plugin" );
    lua_pushlightuserdata( L, (void*) plugin );
    lua_settable( L, LUA_GLOBALSINDEX );

    return 0;
}

int ltapi_file_execute( lua_State * L, apr_file_t * file, const char * name )
{
    _ltapi_apr_file_chunkreader_data d;
    d.file = file;

    if( 0 != lua_load( L, _ltapi_apr_file_chunkreader,
                (void*) &d, name ) ) {
        /* an error occured while loading/parsing lua script */
        lt_server_logger_print( PRIO_DEFAULT,
                "ERROR while loading/parsing Lua script %s\n", name );
        lt_server_logger_print( PRIO_DEFAULT,
                "%s\n", lua_tostring( L, -1 ) );
        return 1;
    }

    if( 0 != lua_pcall( L, 0, 0, 0 ) ) {
        /* an error occured while executing lua script */
        lt_server_logger_print( PRIO_DEFAULT,
                "ERROR while executing Lua script %s\n", name );
        lt_server_logger_print( PRIO_DEFAULT,
                "%s\n", lua_tostring( L, -1 ) );
        return 1;
    }

    return 0;
}

const char * _ltapi_apr_file_chunkreader( lua_State * L, void * data,
        size_t * size )
{
    _ltapi_apr_file_chunkreader_data * d =
        (_ltapi_apr_file_chunkreader_data*) data;

    *size = 4096;
    if( APR_SUCCESS ==
                apr_file_read( d->file, d->buffer, size ) ) {
        return d->buffer;
    }
    return NULL;
}

#define LTAPI_GET_PLUGIN \
    lt_server_plugin_t * plugin = NULL; \
    lua_pushstring( L, "plugin" ); \
    lua_gettable( L, LUA_GLOBALSINDEX ); \
    plugin = (lt_server_plugin_t*) lua_touserdata( L, -1 ); \
    lua_pop( L, 1 );

int _ltapi_sleep( lua_State * L )
{
    apr_interval_time_t total = 0, partial = 0;
    unsigned steps = 1, i;

    LTAPI_GET_PLUGIN;

    total = partial = (apr_interval_time_t) lua_tonumber( L, 1 );

    /* when sleeping more than one second, check exit flag every second */
    if( total > 1 * 1000 * 1000 ) {
        partial = 1 * 1000 * 1000;
        steps = total / partial;
    }

    for( i = 0; i < steps; i ++ ) {
        apr_sleep( partial );
        if( ! lt_server_plugin_should_run( plugin ) ) {
            break;
        }
    }

    /* push the result to the stack */
    lua_pushboolean( L, lt_server_plugin_should_run( plugin ) );

    return 1;
}

int _ltapi_should_run( lua_State * L )
{
    LTAPI_GET_PLUGIN;

    /* push the result to the stack */
    lua_pushboolean( L, lt_server_plugin_should_run( plugin ) );

    return 1;
}

int _ltapi_lock_requested( lua_State * L )
{
    lt_var_set_t * varset;
    LTAPI_GET_PLUGIN;
    varset = lt_server_plugin_varset_get( plugin );

    lt_var_set_requested_values_lock( varset );

    return 0;
}

int _ltapi_unlock_requested( lua_State * L )
{
    lt_var_set_t * varset;
    LTAPI_GET_PLUGIN;
    varset = lt_server_plugin_varset_get( plugin );

    lt_var_set_requested_values_unlock( varset );

    return 0;
}

int _ltapi_trylock_requested( lua_State * L )
{
    lt_var_set_t * varset;
    LTAPI_GET_PLUGIN;
    varset = lt_server_plugin_varset_get( plugin );

    lt_var_set_requested_values_try_lock( varset );

    return 0;
}

int _ltapi_lock_real( lua_State * L )
{
    lt_var_set_t * varset;
    LTAPI_GET_PLUGIN;
    varset = lt_server_plugin_varset_get( plugin );

    lt_var_set_real_values_lock( varset );

    return 0;
}

int _ltapi_unlock_real( lua_State * L )
{
    lt_var_set_t * varset;
    LTAPI_GET_PLUGIN;
    varset = lt_server_plugin_varset_get( plugin );

    lt_var_set_real_values_unlock( varset );

    return 0;
}

int _ltapi_trylock_real( lua_State * L )
{
    lt_var_set_t * varset;
    LTAPI_GET_PLUGIN;
    varset = lt_server_plugin_varset_get( plugin );

    lt_var_set_real_values_try_lock( varset );

    return 0;
}

int _ltapi_wait_requested( lua_State * L )
{
    apr_interval_time_t total = 0;
    lt_var_set_t * varset;
    LTAPI_GET_PLUGIN;
    varset = lt_server_plugin_varset_get( plugin );

    total = (apr_interval_time_t) lua_tonumber( L, 1 );

    lua_pushboolean( L,
            LT_VAR_SUCCESS ==
            lt_var_set_wait_for_new_requested_values( varset, total ) );

    return 1;
}

int _ltapi_sync_requested( lua_State * L )
{
    lt_var_set_t * varset;
    LTAPI_GET_PLUGIN;
    varset = lt_server_plugin_varset_get( plugin );

    lt_var_set_requested_values_sync( varset, -1 );

    return 0;
}

int _ltapi_sync_real( lua_State * L )
{
    lt_var_set_t * varset;
    LTAPI_GET_PLUGIN;
    varset = lt_server_plugin_varset_get( plugin );

    lt_var_set_real_values_sync( varset, -1 );

    return 0;
}

int _ltapi_get_requested( lua_State * L )
{
    const char * tmp = NULL;
    lt_var_t * var = NULL;
    var = (lt_var_t*) lua_touserdata( L, 1 );

#if 0
    fprintf( stderr, "DEBUG: getting req value of Lua type %d\n",
            lua_type( L, 2 ) );
#endif

    switch( lt_var_type_get( var ) ) {
        case LT_IO_TYPE_BIT:
            lua_pushboolean( L,
                    lt_var_requested_value_bit_get( var ) );
            break;
        case LT_IO_TYPE_NUMBER:
            lua_pushnumber( L,
                    lt_var_requested_value_number_get( var ) );
            break;
        case LT_IO_TYPE_STRING:
            tmp = (const char *) lt_var_requested_value_string_get( var );
            if( tmp == NULL ) tmp = "";
            lua_pushstring( L, tmp );
            break;
        default:
            lua_pushnil( L );
    }

    return 1;
}

int _ltapi_get_real( lua_State * L )
{
    const char * tmp = NULL;
    lt_var_t * var = NULL;
    var = (lt_var_t*) lua_touserdata( L, 1 );

#if 0
    fprintf( stderr, "DEBUG: getting real value of Lua type %d\n",
            lua_type( L, 2 ) );
#endif

    switch( lt_var_type_get( var ) ) {
        case LT_IO_TYPE_BIT:
            lua_pushboolean( L,
                    lt_var_real_value_bit_get( var ) );
            break;
        case LT_IO_TYPE_NUMBER:
            lua_pushnumber( L,
                    lt_var_real_value_number_get( var ) );
            break;
        case LT_IO_TYPE_STRING:
            tmp = (const char *) lt_var_real_value_string_get( var );
            if( tmp == NULL ) tmp = "";
            lua_pushstring( L, tmp );
            break;
        default:
            lua_pushnil( L );
    }

    return 1;
}

int _ltapi_is_requested_dirty( lua_State * L )
{
    lt_var_t * var = NULL;
    var = (lt_var_t*) lua_touserdata( L, 1 );

    lua_pushboolean( L, lt_var_requested_value_is_dirty( var ) );

    return 1;
}

int _ltapi_is_real_dirty( lua_State * L )
{
    lt_var_t * var = NULL;
    var = (lt_var_t*) lua_touserdata( L, 1 );

    lua_pushboolean( L, lt_var_real_value_is_dirty( var ) );

    return 1;
}

int _ltapi_set_requested( lua_State * L )
{
    lt_var_t * var = NULL;
    var = (lt_var_t*) lua_touserdata( L, 1 );

#if 0
    fprintf( stderr, "DEBUG: setting req value of Lua type %d\n",
            lua_type( L, 2 ) );
#endif

    switch( lua_type( L, 2 ) ) {
        case LUA_TBOOLEAN:
            lt_var_requested_value_bit_set( var,
                    lua_toboolean( L, 2 ) );
            break;
        case LUA_TNUMBER:
            lt_var_requested_value_number_set( var,
                    (unsigned) lua_tonumber( L, 2 ) );
            break;
        case LUA_TSTRING:
            lt_var_requested_value_string_set2( var,
                    (const uint8_t*) lua_tostring( L, 2 ) );
            break;
    }

    return 0;
}

int _ltapi_set_real( lua_State * L )
{
    lt_var_t * var = NULL;
    var = (lt_var_t*) lua_touserdata( L, 1 );

#if 0
    fprintf( stderr, "DEBUG: setting real value of Lua type %d\n",
            lua_type( L, 2 ) );
#endif

    switch( lua_type( L, 2 ) ) {
        case LUA_TBOOLEAN:
            lt_var_real_value_bit_set( var,
                    lua_toboolean( L, 2 ) );
            break;
        case LUA_TNUMBER:
            lt_var_real_value_number_set( var,
                    (unsigned) lua_tonumber( L, 2 ) );
            break;
        case LUA_TSTRING:
            lt_var_real_value_string_set2( var,
                    (const uint8_t*) lua_tostring( L, 2 ) );
            break;
    }

    return 0;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id:$
 */
