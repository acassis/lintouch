/* $Id:$
 *
 *   FILE: console.c --
 * AUTHOR: Patrik Modesto <modesto@swac.cz>
 *   DATE: 14 March 2006
 *
 * Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
 * Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
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

#include <apr_hash.h>
#include <apr_strings.h>
#include <apr_time.h>
#include <apr_thread_mutex.h>
#include <apr_file_io.h>

#include <lt/variables/variables.h>

#include <lt/server/logging.h>

#ifndef CONSOLE_LOGGING
#include <apr_file_info.h>
#endif

#include <lt/server/logger.h>
#include <lt/server/project.h>

#include "config.h"

/* automatically define mandatory plugin symbols */
LT_LOGGING_PLUGIN_EXPORT;

/* CONSOLE/FILE METHODS */

lt_logging_plugin_info_t *
    PLUGIN_EXPORT LT_LOGGING_PLUGIN_CREATE( apr_pool_t * pool );

lt_server_status_t
    PLUGIN_EXPORT LT_LOGGING_PLUGIN_INIT( lt_logging_plugin_t * plugin,
        apr_hash_t * connections );

lt_server_status_t
    PLUGIN_EXPORT LT_LOGGING_PLUGIN_LOG( lt_logging_plugin_t * plugin,
            apr_time_exp_t* timestamp, lt_var_set_t* vset, int real );

lt_server_status_t
    PLUGIN_EXPORT LT_LOGGING_PLUGIN_EXIT( lt_logging_plugin_t * plugin );

/* PLUGIN SPECIFIC DATA */

typedef struct {
    apr_hash_t* vset2cname;
    apr_thread_mutex_t* mutex;
    apr_file_t* out;
} my_plugin_data;

/* METHOD BODIES */
lt_logging_plugin_info_t * LT_LOGGING_PLUGIN_CREATE( apr_pool_t * pool )
{
    apr_hash_t * props = apr_hash_make( pool );

#ifndef CONSOLE_LOGGING
    /* create propertiy file_name */
    apr_hash_set( props, "file-name", APR_HASH_KEY_STRING,
            (void*)"/tmp/server.log" );
#endif


    /* allocate my_plugin_data and create empty hash */
    my_plugin_data* data = (my_plugin_data*)apr_palloc(pool,
            sizeof(my_plugin_data));
    data->vset2cname = NULL;
    data->mutex = NULL;
    data->out = NULL;

    /* create plugin meta-info and return it back */
    return lt_logging_plugin_info_create(
            "Patrik Modesto <modesto@swac.cz>",     /* author */
            VERSION,                                /* version */
            TODAY,                                  /* date */
#ifdef CONSOLE_LOGGING
            "Console",                              /* shortdesc */
            "Console Lintouch Logging Plugin",      /* longdesc */
#else
            "File",                                 /* shortdesc */
            "File Lintouch Logging Plugin",         /* longdesc */
#endif
            props,                                  /* default props */
            data,                                   /* user data */
            pool );
}

lt_server_status_t LT_LOGGING_PLUGIN_INIT( lt_logging_plugin_t* plugin,
        apr_hash_t * connections )
{
    /* get plugin specific data */
    my_plugin_data * data = (my_plugin_data*)lt_logging_plugin_data_get(
            plugin );
    apr_pool_t* pool = lt_logging_plugin_pool_get(plugin);

    // create the hash
    data->vset2cname = apr_hash_make(pool);

    // create the access mutex
    if(apr_thread_mutex_create(&data->mutex,
                APR_THREAD_MUTEX_DEFAULT, pool) != APR_SUCCESS) {
        // return error if mutex can't be created
        lt_server_logger_print(PRIO_PLUGIN,
                "File Logging: Can't create access mutex!\n");
        return LT_SERVER_INVALID_ARG;
    }

#ifdef CONSOLE_LOGGING
    // open the stdout
    if(apr_file_open_stdout(&data->out, pool) != APR_SUCCESS) {
        // return error if stdout can't be opened
        lt_server_logger_print(PRIO_PLUGIN,
                "File Logging: Can't open stdout!\n");
        return LT_SERVER_INVALID_ARG;
    }
#else
    // open the file
    apr_hash_t* props = lt_logging_plugin_properties_get(plugin);
    if(apr_file_open(&data->out, (const char*)apr_hash_get(
                    props, "file-name", APR_HASH_KEY_STRING),
                APR_CREATE|APR_WRITE|APR_APPEND, 0xfff, pool) != APR_SUCCESS)
    {
         // return error if stdout can't be opened
         lt_server_logger_print(PRIO_PLUGIN,
                "File Logging: Can't open/create the output file: %s!\n",
                (const char*)apr_hash_get(
                    props, "file-name", APR_HASH_KEY_STRING));
         return LT_SERVER_INVALID_ARG;
     }
#endif

    /* create local pool out of the plugin pool */
    apr_pool_t* lpool = NULL;
    apr_pool_create(&lpool, pool);

    const char * name = NULL;
    lt_project_connection_t * conn = NULL;

    /* for each connection */
    apr_hash_index_t* hi = apr_hash_first( lpool, connections );
    for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
        apr_hash_this( hi, (const void**)&name, NULL, (void**)&conn );

        /* store connections names */
        apr_hash_set(data->vset2cname,
                lt_project_connection_variables_get(conn),
                3*sizeof(apr_pool_t*),
                apr_pstrdup(lt_logging_plugin_pool_get(plugin), name));
    }
    apr_pool_destroy(lpool);

    return LT_SERVER_SUCCESS;
}

lt_server_status_t LT_LOGGING_PLUGIN_LOG( lt_logging_plugin_t * plugin,
        apr_time_exp_t* timestamp, lt_var_set_t* vset, int real )
{
    /* get plugin specific data */
    my_plugin_data * data = (my_plugin_data*)lt_logging_plugin_data_get(
            plugin );

    /* create local pool out of the plugin pool */
    apr_pool_t* lpool = NULL;
    apr_pool_create(&lpool, lt_logging_plugin_pool_get(plugin));

    const char* cname =
        apr_hash_get(data->vset2cname, vset, 3*sizeof(apr_pool_t*));

    char* header = apr_psprintf(lpool, "%u-%02u-%02u %02u:%02u:%02u.%06u %s %c",
            timestamp->tm_year+1900,
            timestamp->tm_mon,
            timestamp->tm_mday,
            timestamp->tm_hour,
            timestamp->tm_min,
            timestamp->tm_sec,
            timestamp->tm_usec,
            cname, (real==1)?'R':'Q');

    // lock the mutex. only one thread can write to the stdout at a time
    apr_thread_mutex_lock(data->mutex);

    apr_file_puts(header, data->out);

    /* log the varset data */
    {
        const char * vname = NULL;
        lt_var_t * var = NULL;
        const lt_io_value_t * val = NULL;
        /* lt_io_value_type_e vt = 0; */

       apr_hash_t * dirty = (real==1)?lt_var_set_dirty_real_values_get(vset) :
           lt_var_set_dirty_requested_values_get(vset);

        /* for each dirty variable in the varset */
        apr_hash_index_t* hi = apr_hash_first( lpool, dirty );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
            apr_hash_this( hi, (const void**)&vname, NULL, (void**)&var );

            apr_file_printf(data->out, " %s", vname);

            val = (real==1)?lt_var_real_value_get(var) :
                lt_var_requested_value_get(var);

            if(LT_IO_VALUE_IS_TYPE_NULL(val)) {
                apr_file_puts("( )", data->out);
            } else if(LT_IO_VALUE_IS_TYPE_BIT(val)) {
                apr_file_printf(data->out, "(B)=%d",
                        lt_io_value_bit_get(val));
            } else if(LT_IO_VALUE_IS_TYPE_NUMBER(val)) {
                apr_file_printf(data->out, "(N)=%d",
                        lt_io_value_number_get(val));
            } else if(LT_IO_VALUE_IS_TYPE_STRING(val)) {
                apr_file_printf(data->out, "(S)=%s",
                        lt_io_value_string_get(val));
            } else if(LT_IO_VALUE_IS_TYPE_SEQUENCE(val)) {
                apr_file_printf(data->out, "(Q) cnt=%d",
                        lt_io_value_sequence_length_get(val));
            }
        }
        apr_pool_destroy(lpool);
    }

    apr_file_puts("\n", data->out);
    apr_file_flush(data->out);
    apr_thread_mutex_unlock(data->mutex);

    return LT_SERVER_SUCCESS;
}

lt_server_status_t LT_LOGGING_PLUGIN_EXIT( lt_logging_plugin_t* plugin)
{
    /* get plugin specific data */
    my_plugin_data * data = (my_plugin_data*)lt_logging_plugin_data_get(
            plugin );

    apr_file_close(data->out);

    return LT_SERVER_SUCCESS;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id:$
 */
