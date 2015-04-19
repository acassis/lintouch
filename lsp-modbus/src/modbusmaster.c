/* $Id: modbusmaster.c 1550 2006-03-27 07:50:50Z hynek $
 *
 *   FILE: modbusmaster.c --
 * AUTHOR: Jiri Barton <jbar@swac.cz>
 *   DATE: 24 May 2005
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
#include <stdlib.h>
#include <time.h>
#include <apr_time.h>
#include <apr_strings.h>
#define APR_WANT_BYTEFUNC /* htons, ntohs */
#include "apr_want.h"

#include <lt/server/plugin.h>
#include <lt/server/logger.h>

#include "config.h"
#include "modbus.h"

/* automatically define mandatory plugin symbols */
LT_SERVER_PLUGIN_EXPORT;

/* MODBUS SLAVE PLUGIN METHODS */

lt_server_plugin_info_t *
PLUGIN_EXPORT LT_SERVER_PLUGIN_CREATE( apr_pool_t * pool );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin );

/* MODBUS SLAVE PLUGIN SPECIFIC DATA */

typedef struct {
    /* starting address --> list of variables (for each data type)
    The variable addresses in each list form a sequence starting at the
    starting address. Data types: 0 - coils, 1 - discrete inputs,
    2 - holding registers, 3 - input registers */
    apr_hash_t *responses[4];

    /* the list of the real requests (frames) for polling all the real
    values of all the variables */
    apr_array_header_t *requests;

    /* the plugin's varset */
    lt_var_set_t *varset;

    /* port for Modbus to listen on */
    int port_no;

    /* target unit (slave) identifier */
    apr_byte_t unit_id;

    /* the poll interval expressed in micro-seconds */
    apr_interval_time_t refresh;
    
    /* timeout interval for a socket expressed in micro-seconds */
    apr_interval_time_t timeout;

    /* the modbus slave hostname */
    char *host;
    
    /* whether to wait for response before we send the next request */
    int serializerequests;

} my_plugin_data;

/* METHOD BODIES */

lt_server_plugin_info_t *
LT_SERVER_PLUGIN_CREATE( apr_pool_t * pool )
{
    apr_hash_t * props = apr_hash_make( pool );
    apr_hash_t * varprops = apr_hash_make( pool );
    apr_hash_t * bitprops = apr_hash_make( pool );
    apr_hash_t * numprops = apr_hash_make( pool );
    apr_hash_t * strprops = apr_hash_make( pool );
    my_plugin_data * data = apr_pcalloc( pool, sizeof( my_plugin_data ) );

    /* plugin properties */
    apr_hash_set( props, "host", APR_HASH_KEY_STRING, (void*)"localhost" );
    apr_hash_set( props, "port", APR_HASH_KEY_STRING, (void*)"502" );
    apr_hash_set( props, "refresh", APR_HASH_KEY_STRING, (void*)"200" );
    apr_hash_set( props, "unitid", APR_HASH_KEY_STRING, (void*)"0" );
    apr_hash_set( props, "serialize", APR_HASH_KEY_STRING, (void*)"0" );
    apr_hash_set( props, "timeout", APR_HASH_KEY_STRING, (void*)"100" );

    /* bit var properties */
    apr_hash_set( bitprops, "address", APR_HASH_KEY_STRING, (void*)"" );
    /* num var properties */
    apr_hash_set( numprops, "address", APR_HASH_KEY_STRING, (void*)"" );
    /* string var properties */
    /* none */

    /* variable properties according to var type */
    apr_hash_set(varprops, "bit", APR_HASH_KEY_STRING, (void*)bitprops);
    apr_hash_set(varprops, "number", APR_HASH_KEY_STRING, (void*)numprops);
    apr_hash_set(varprops, "string", APR_HASH_KEY_STRING, (void*)strprops);

    /* create plugin meta-info and return it back */
    return lt_server_plugin_info_create(
            "Jiri Barton <jbar@swac.cz>",          /* author */
            VERSION,                               /* version */
            TODAY,                                 /* date */
            "ModbusTCP Master",                    /* shortdesc */
            "ModbusTCP Master (Client) Lintouch Server Plugin",/*longdesc*/
            props,                                 /* default props */
            varprops,                              /* default varprops */
            data,                                  /* plugin data */
            pool );
}

int _config_read_int(const lt_server_plugin_config_t *config,
    const char *key, int default_)
{
    const char *tmp = NULL;
    int tmpd = default_;

    tmp = apr_hash_get(config->properties, key, APR_HASH_KEY_STRING);
    if(tmp == NULL) return default_;
    sscanf(tmp, "%d", &tmpd);
    return tmpd;
}

/* The following helper routines are used primarly to help working with
the temporary array of continuous blocks.*/

int _varaddr_size_get()
{
    return sizeof(void *) + sizeof(sa_t);
}

void _varaddr_make(void *item, lt_var_t *var, sa_t addr)
{
    *(lt_var_t **) item = var;
    *(sa_t *) ((lt_var_t **) item + 1) = addr;
}

lt_var_t *_varaddr_var_get(void *item)
{
    return *(lt_var_t **) item;
}

sa_t _varaddr_addr_get(void *item)
{
    return *(sa_t *) ((lt_var_t **) item + 1);
}

int _varaddr_compare (void *a, void *b)
{
    return _varaddr_addr_get(a) < _varaddr_addr_get(b) ? -1 : 1;
}

/* Create the frame that sends the requested value to the modbus server.
Leave the last two bytes unset for the requested value. */
void *_writable_frame(apr_byte_t unit_id, apr_byte_t function, sa_t addr,
    apr_pool_t *pool)
{
    apr_byte_t *tel = apr_palloc(pool, DATAOFFSET + 4);
    memset(tel, 0, 7);
    tel[LENOFFSET] = DATAOFFSET + 4 - LENOFFSET - 1;
    tel[FUNCTIONOFFSET] = function;
    tel[SLAVEOFFSET] = unit_id;
    *(sa_t *) (tel + DATAOFFSET) = htons(addr);
    return tel;
}

/* Create the frame that requests the real values from the modbus server. */
void _request_make(void *item, apr_uint16_t transaction, apr_byte_t unit_id,
    sa_t startchunk, sa_t blocksize, apr_byte_t function, apr_pool_t *pool)
{
    apr_byte_t *tel = apr_palloc(pool, DATAOFFSET + 4);
    memset(tel, 0, 7);
    *(apr_uint16_t *) (tel + TRANSACTIONIDOFFSET) = htons(transaction);
    tel[LENOFFSET] = DATAOFFSET + 4 - LENOFFSET - 1;
    tel[FUNCTIONOFFSET] = function;
    tel[SLAVEOFFSET] = unit_id;
    *(sa_t *) (tel + DATAOFFSET) = htons(startchunk);
    *(sa_t *) (tel + DATAOFFSET + 2) = htons(blocksize);
    *(apr_byte_t **) item = tel;
}

/* Store the list of variables of a block. */
void _response_make(apr_hash_t *responses, apr_uint16_t transaction,
    apr_array_header_t *varlist, apr_pool_t *pool)
{
    sa_t *t = apr_palloc(pool, sizeof(transaction));
    *t = transaction;
    apr_hash_set(responses, t, sizeof(*t), varlist);
}

apr_status_t process_response(my_plugin_data *data, apr_socket_t *lsock)  
{
    /* receive the real values from the modbus server */
    apr_byte_t function, bufferlen;
    apr_byte_t buffer[FRAMELEN];
    apr_uint16_t transaction;
    apr_status_t s;
    lt_var_t **var;
    int j;

    if ((s = modbus_response(lsock, &transaction, &function, buffer,
            &bufferlen)) != APR_SUCCESS) {
        return s;
    }

    /* okay, continue with parsing the response */
    switch (function) {
        apr_array_header_t *varlist;
        case READ_COILS:
        case READ_DISCRETE_INPUTS:
        case READ_HOLDING_REGISTERS:
        case READ_INPUT_REGISTERS:
        varlist = apr_hash_get(data->responses[function - 1],
                &transaction, sizeof(transaction));
        if (varlist == NULL) {
            lt_server_logger_print(PRIO_PLUGIN,
                    "modbus sync error: the response %d not "
                    "expected\n", (int) function);
            return APR_EGENERAL;
        }

        if (function == READ_COILS
                || function == READ_DISCRETE_INPUTS) {
            if ((varlist->nelts + 7) / 8 != bufferlen) {
                lt_server_logger_print(PRIO_PLUGIN,
                        "modbus sync error: the response contains"
                        " %d values, expected %d values\n",
                        bufferlen, (int) (varlist->nelts + 7) / 8);
                return APR_EGENERAL;
            }

            var = (lt_var_t **) varlist->elts;
            for (j = 0; j < varlist->nelts; j++, var++) {
                lt_var_real_value_bit_set(*var,
                        buffer[j / 8] >> j % 8 & 1);
            }
        } else {
            if (varlist->nelts * 2 != bufferlen) {
                lt_server_logger_print(PRIO_PLUGIN,
                        "modbus sync error: the response contains"
                        " %d values, expected %d values\n",
                        bufferlen, (int) (varlist->nelts * 2));
                return APR_EGENERAL;
            }

            var = (lt_var_t **) varlist->elts;
            for (j = 0; j < varlist->nelts; j++, var++) {
                lt_var_real_value_number_set(*var,
                        ntohs (*(apr_uint16_t *)&buffer[j << 1]));
            }
        }
        break;
        case WRITE_SINGLE_COIL:
        case WRITE_SINGLE_REGISTER:
        break;
        default:
        if (function & 128) {
            lt_server_logger_print(PRIO_PLUGIN,
                    "modbus exception received fn: %d code: %d\n",
                    (int) (function ^ 128), (int) buffer[0]);
        } else {
            lt_server_logger_print(PRIO_PLUGIN,
                    "unexpected modbus response %d\n",
                    (int) function);
            return APR_EGENERAL;
        }
        break;
    }
    return APR_SUCCESS;
}

lt_server_status_t
LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin )
{
    apr_pool_t *pluginpool = lt_server_plugin_pool_get(plugin);

    lt_server_logger_print(PRIO_DEFAULT, "Modbus/TCP Master plugin $Rev: 1550 $\n");

    /* plugin specific data */
    my_plugin_data * data = (my_plugin_data*)
        lt_server_plugin_data_get( plugin );

    /* get the plugin properties*/
    const lt_server_plugin_config_t * config =
        lt_server_plugin_config_get( plugin );

    data->port_no = _config_read_int(config, "port", 502);
    data->refresh = _config_read_int(config, "refresh", 200) * 1000;
    data->unit_id = _config_read_int(config, "unitid", 0);
    data->serializerequests = _config_read_int(config, "serialize", 0);
    data->timeout = _config_read_int(config, "timeout", 200) * 1000;

    /* the modbus hostname */
    data->host = apr_hash_get(config->properties, "host",
        APR_HASH_KEY_STRING);
    if (data->host == NULL) data->host = "localhost";
    
    lt_server_logger_print(PRIO_PLUGIN, 
            "Modbus host='%s'\n", data->host);
    lt_server_logger_print(PRIO_PLUGIN, 
            "Modbus port=%d\n", data->port_no);
    lt_server_logger_print(PRIO_PLUGIN, 
            "Modbus refresh=%d\n", data->refresh/1000);
    lt_server_logger_print(PRIO_PLUGIN, 
            "Modbus unitid=%d\n", data->unit_id);
    lt_server_logger_print(PRIO_PLUGIN, 
            "Modbus serialize=%d\n", data->serializerequests);
    lt_server_logger_print(PRIO_PLUGIN, 
            "Modbus timeout=%d\n", data->timeout/1000);

    /* cache the varset */
    data->varset = lt_server_plugin_varset_get(plugin);

    /* hash all the variables into CO, DI, IR, or HR category */
    {
        apr_pool_t * lpool = NULL;
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;

        apr_pool_create( &lpool, pluginpool );
        h = lt_var_set_variables_get(lt_server_plugin_varset_get(plugin));
        hi = apr_hash_first( lpool, h );

        /* initialize the data structures */
        apr_array_header_t *tables[4];
        int i, j;
        for (i = 0; i < 4; i++)
        {
            tables[i] = apr_array_make(lpool, 1, _varaddr_size_get());
            data->responses[i] = apr_hash_make(pluginpool);
        }
        data->requests = apr_array_make(pluginpool, 0, sizeof(apr_byte_t*));

        for( ; hi != NULL; hi = apr_hash_next( hi ) )
        {
            const char * name = NULL;
            const char * address = NULL;
            lt_var_t * var = NULL;
            apr_hash_t * varprops = NULL;

            apr_hash_this( hi, (const void**)&name, NULL, (void**)&var );

            /* retrieve the variable properties */
            varprops = apr_hash_get( config->varproperties,
                    name, APR_HASH_KEY_STRING );
            if( varprops == NULL ) {
                lt_server_logger_print(PRIO_PLUGIN, "the variable %s"
                    " has no properties", name);
                continue;
            }

            /* look for the address property */
            address = apr_hash_get( varprops, "address",
                APR_HASH_KEY_STRING );
            if( address == NULL ) {
                lt_server_logger_print(PRIO_PLUGIN, "the variable %s"
                    " has no address property", name);
                continue;
            }

            /* parse the address */
            if (strlen(address) < 3)
            {
                lt_server_logger_print(PRIO_PLUGIN, "the variable %s"
                    " is of incorrect format", name);
                continue;
            }

            /* parse the variable type */
            int datatype = -1;
            switch (address[0])
            {
                case 'D':
                case 'd':
                    datatype = DISCRETE_INPUTS;
                    break;
                case 'C':
                case 'c':
                    datatype = COILS;
                    break;
                case 'I':
                case 'i':
                    datatype = INPUT_REGISTERS;
                    break;
                case 'H':
                case 'h':
                    datatype = HOLDINGREGISTERS;
                    break;
            }
            if (datatype == -1)
            {
                lt_server_logger_print(PRIO_PLUGIN, "the variable %s"
                    " is of incorrect format", name);
                continue;
            }
            if (datatype == DISCRETE_INPUTS || datatype == COILS)
            {
                if (lt_var_type_get(var) != LT_IO_TYPE_BIT)
                {
                    lt_server_logger_print(PRIO_PLUGIN, "the variable "
                        "type of %s is must be bit", name);
                    continue;
                }
            }
            else
            {
                if (lt_var_type_get(var) != LT_IO_TYPE_NUMBER)
                {
                    lt_server_logger_print(PRIO_PLUGIN, "the variable "
                        "type of %s is must be number", name);
                    continue;
                }
            }

            /* parse the starting address */
            sa_t *startingaddress = apr_palloc(pluginpool, sizeof(sa_t));
            *startingaddress = (sa_t) atoi(address + 2);

            /* add the pair (var, address) to the array */
            _varaddr_make(apr_array_push(tables[datatype]), var,
                *startingaddress);

            /* attach the write request to the variable if applicable */
            if (datatype == COILS)
            {
                lt_var_userdata_set(var, _writable_frame(data->unit_id,
                    WRITE_SINGLE_COIL, *startingaddress, pluginpool));

            }
            else if (datatype == HOLDINGREGISTERS)
            {
                lt_var_userdata_set(var, _writable_frame(data->unit_id,
                    WRITE_SINGLE_REGISTER, *startingaddress, pluginpool));
            }
        }

        /* the largest count of bits and registers in a PDU */
        sa_t maxblocksize[4] = {2000, 2000, 125, 125};
        apr_byte_t requesttype[4] = {READ_COILS, READ_DISCRETE_INPUTS,
            READ_HOLDING_REGISTERS, READ_INPUT_REGISTERS};
        sa_t transaction = 0;
        for (i = 0; i < 4; i++)
        {
            apr_array_header_t *table = tables[i];

            /* sort the array */
            qsort(table->elts, table->nelts, table->elt_size,
                (int (*)(const void *, const void *))
                _varaddr_compare);
            /* Cut the array to continuous blocks. The variables addresses
            in each block constitute a succession; the first address is
            blockstart and there are blocksize variables in the succession.

            Store each such block (list of variables, or varlist) in the
            hash table responses[i]
            under the key blockstart so that the modbus response can be
            written to the real values of the corresponding variables.

            In addition, create the modbus request (store the whole frame)
            to read the variable values for the block.*/
            sa_t blockstart = 0;
            sa_t blocksize = 0;

            lt_var_t *var;
            sa_t addr;
            apr_array_header_t *varlist =
                apr_array_make(pluginpool, 1, sizeof(var));

            for (j = 0; j < table->nelts; j++)
            {
                var = _varaddr_var_get(table->elts + table->elt_size*j);
                addr = _varaddr_addr_get(table->elts + table->elt_size*j);

                if (blocksize)
                {
                    if (blockstart + blocksize < addr ||
                        blocksize == maxblocksize[i])
                    {
                        _request_make(apr_array_push(data->requests),
                            transaction, data->unit_id, blockstart,
                            blocksize, requesttype[i], pluginpool);

                        _response_make(data->responses[i], transaction,
                            varlist, pluginpool);

                        transaction++;
                        varlist = apr_array_make(pluginpool,1, sizeof(var));

                        blockstart = addr;
                        blocksize = 0;
                    }
                }
                else
                {
                    blockstart = addr;
                }

                blocksize++;
                *(lt_var_t **) apr_array_push(varlist) = var;
            }

            if (blocksize)
            {
                _request_make(apr_array_push(data->requests), transaction,
                    data->unit_id, blockstart, blocksize, requesttype[i],
                    pluginpool);

                _response_make(data->responses[i], transaction, varlist,
                    pluginpool);
            }

        }

        apr_pool_destroy( lpool );
    }

    return LT_SERVER_SUCCESS;
}

lt_server_status_t
LT_SERVER_PLUGIN_RUN(lt_server_plugin_t * plugin)
{
    my_plugin_data *data = (my_plugin_data *)
        lt_server_plugin_data_get(plugin);
    apr_socket_t *lsock;
    apr_status_t status;
    apr_pool_t *lsock_pool;

#define FAILMESSAGE "Error in the MODBUS Master plugin: "
#define FAILMESSAGE2 "modbusmaster.c @ %d: ", __LINE__

    while (lt_server_plugin_should_run(plugin)) {
        /* create local pool */
        apr_pool_create(&lsock_pool, lt_server_plugin_pool_get(plugin));

        /* connect to the TCP server */
        status = apr_socket_create_ex(&lsock, APR_INET, SOCK_STREAM,
                                      APR_PROTO_TCP, lsock_pool);
        if (status != APR_SUCCESS) {
            lt_server_logger_print_apr(PRIO_DEFAULT,
                                       "apr_socket_create_ex",
                                       FAILMESSAGE);
            goto destroy_pool;
        }
        status = apr_socket_opt_set(lsock, APR_SO_REUSEADDR, 1);
        if (status != APR_SUCCESS) {
            /* Hynek: non-fatal error */
            lt_server_logger_print_apr(PRIO_DEFAULT, "setsockopt",
                                       FAILMESSAGE);
        }
        status = apr_socket_opt_set(lsock, APR_TCP_NODELAY, 1);
        if (status != APR_SUCCESS) {
            /* Hynek: non-fatal error */
            lt_server_logger_print_apr(PRIO_DEFAULT, "setsockopt",
                                       FAILMESSAGE);
        }
        /* 15s timeout for connect */
        status = apr_socket_timeout_set(lsock, 15000000 );
        if (status != APR_SUCCESS) {
            lt_server_logger_print_apr(PRIO_DEFAULT,
                    "apr_socket_timeout_set", FAILMESSAGE);
        }

        apr_sockaddr_t *server_addr;
        apr_sockaddr_info_get(&server_addr, data->host, APR_INET,
                              data->port_no, 0, lsock_pool);

        lt_server_logger_print(PRIO_PLUGIN,
                "MODBUS client connecting to "
                "%s:%d\n", data->host, data->port_no);
        status = apr_socket_connect(lsock, server_addr);
        if (status == APR_SUCCESS) {
            lt_server_logger_print(PRIO_PLUGIN,
                    "MODBUS client succesfuly connected.\n");
        } else if ( status == APR_TIMEUP ) {
            lt_server_logger_print(PRIO_PLUGIN,
                    FAILMESSAGE "Timeout occured while calling connect().\n");
            goto close_socket;
        } else {        /* failed but try to connect again when polling */
            lt_server_logger_print_apr(PRIO_PLUGIN, "connect",
                                       FAILMESSAGE);
            goto close_socket;
        }
        /* if the server doesn't respond within 100 miliseconds 
         * then let him go to hell ...*/
        status = apr_socket_timeout_set(lsock, data->timeout);
        if (status != APR_SUCCESS) {
            /* if the timeout is not set properly, there is a danger
             * of a dead lock of the plugin. The dead lock may ocur if the
             * server doesn't give back the same number of responses as
             * requested. Hynek.
             */
            lt_server_logger_print_apr(PRIO_PLUGIN,
                    "apr_socket_timeout_set", FAILMESSAGE);
        }


        while (lt_server_plugin_should_run(plugin)) {

            int framessent = 0;
            apr_size_t len;
            apr_byte_t j;
            lt_var_t **var;

            /* receive the requested values from the runtime and send the
               write requests to the modbus server */
            lt_var_set_requested_values_lock(data->varset);
            if (apr_hash_count
                (lt_var_set_dirty_requested_values_get(data->varset))) {
                apr_hash_t *dirty_req =
                    lt_var_set_dirty_requested_values_get(data->varset);
                apr_hash_index_t *hi =
                    apr_hash_first(lsock_pool, dirty_req);

                for (; hi != NULL; hi = apr_hash_next(hi)) {
                    const char *varname = NULL;
                    lt_var_t *var = NULL;

                    apr_hash_this(hi, (const void **) &varname, NULL,
                                  (void **) &var);

                    /* retrieve the modbus frame */
                    apr_byte_t *frame = lt_var_userdata_get(var);
                    if (frame == NULL)
                        continue;

                    /* encode the requested value into the modbus frame */
                    const lt_io_value_t *v =
                        lt_var_requested_value_get(var);
                    switch (lt_io_value_type_get(v)) {
                    case LT_IO_TYPE_BIT:
                        *(sa_t *) (frame + DATAOFFSET + 2) =
                            htons((sa_t) (lt_io_value_bit_get(v) ? 0xFF00
                                            : 0));
                        break;
                    case LT_IO_TYPE_NUMBER:
                        *(sa_t *) (frame + DATAOFFSET + 2) =
                            htons((sa_t) lt_io_value_number_get(v));
                        break;
                    default:
                        continue;
                    }

                    /* send out the request */
                    len = DATAOFFSET + 4;
                    if (apr_socket_send(lsock, frame, &len) != APR_SUCCESS) {
                        lt_server_logger_print_apr(PRIO_PLUGIN,
                                "apr_socket_send" , FAILMESSAGE2);
                        goto close_socket;
                    }

                    if (data->serializerequests) {
                        status = process_response(data, lsock);
                        if (status != APR_SUCCESS) {
                            lt_server_logger_print_apr(PRIO_PLUGIN,
                                    "process_response" , FAILMESSAGE2);
                            goto close_socket;
                        }
                    } else {
                        framessent++;
                    }
                    if (!lt_server_plugin_should_run(plugin))
                        break;
                }
            }
            lt_var_set_requested_values_sync(data->varset, -1);
            lt_var_set_requested_values_unlock(data->varset);

            /* should we continue? */
            if (!lt_server_plugin_should_run(plugin))
                break;

            /* request the real values from the modbus server */
            apr_byte_t **req = (apr_byte_t **) data->requests->elts;
            apr_byte_t **tail = req + data->requests->nelts;
            for (; req < tail; req++) {
                len = LENOFFSET + 1 + (*req)[LENOFFSET];
                if (apr_socket_send(lsock, *req, &len) != APR_SUCCESS) {
                    lt_server_logger_print_apr(PRIO_PLUGIN,
                            "apr_socket_send" , FAILMESSAGE2);
                    goto close_socket;
                }

                if (data->serializerequests) {
                    status = process_response(data, lsock);
                    if (status != APR_SUCCESS) {
                        lt_server_logger_print_apr(PRIO_PLUGIN,
                                "process_response" , FAILMESSAGE2);
                        goto close_socket;
                    }
                } else {
                    framessent++;
                }
                if (!lt_server_plugin_should_run(plugin))
                    break;
            }

            for (; framessent; framessent--) {
                status = process_response(data, lsock);
                if (status != APR_SUCCESS) {
                    lt_server_logger_print_apr(PRIO_PLUGIN,
                            "process_response" , FAILMESSAGE2);
                    if (status == APR_TIMEUP) {
                        /* Hynek: not too clear, what to do here. 
                        */ 
                        break;
                    } else {
                        goto close_socket;
                    }
                }
                if (!lt_server_plugin_should_run(plugin))
                    break;
            }
            lt_var_set_real_values_sync(data->varset, -1);
            if (!lt_server_plugin_should_run(plugin))
                break;

            /* wait the poll interval */
            apr_sleep(data->refresh);
            if (!lt_server_plugin_should_run(plugin))
                break;
        }

close_socket:
        apr_socket_close(lsock);
destroy_pool:
        apr_pool_destroy(lsock_pool);
        if (!lt_server_plugin_should_run(plugin))
            break;
        lt_server_logger_print(PRIO_PLUGIN,
            "Closing current connection ...\n");
        apr_sleep(1E6);
    }


    return LT_SERVER_SUCCESS;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: modbusmaster.c 1550 2006-03-27 07:50:50Z hynek $
 */
