/* $Id: modbusslave.c 1520 2006-03-08 12:22:15Z hynek $
 *
 *   FILE: modbusslave.c --
 * AUTHOR: Jiri Barton <jbar@swac.cz>
 *   DATE: 03 May 2005
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
} my_vardata;

typedef struct {
    /* The address space of the plugin. 0 - coils, 1 - discrete inputs,
    2 - holding registers, 3 - input registers.
    For the bit data types (coils and discrete inputs), the space is packed,
    so that every bit in the address space has its address.
    For the number data types (registers), the space contains two bytes
    numbers (sa_t type). The numbers are stored in the network order and
    thus have to be converted before using (syncing to lintouch).*/
    apr_array_header_t *addressspace[4];

    /* The starting address of the corresponding addressspace. Each address
    space spans over all the possible addresses. To save space, the first
    byte of the address space is the least used address among all the
    variables.
    Similarly, each address space's largest address contains the largest
    used address of the variables.*/
    sa_t addressspacestart[4];

    /* The list of variables of the corresponding addressspace. The pointed
    array contains pointer to the lt_var_t type and is unsorted. */
    apr_array_header_t *varlists[4];

    /* the plugin's varset */
    lt_var_set_t * varset;

    /* port for Modbus to listen on */
    int port_no;
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
    apr_hash_set( props, "port", APR_HASH_KEY_STRING, (void*)"502" );

    /* bit var properties */
    apr_hash_set( bitprops, "address", APR_HASH_KEY_STRING, (void*)"" );
    /* num var properties */
    apr_hash_set( numprops, "address", APR_HASH_KEY_STRING, (void*)"" );
    /* string var properties */
    /* none */

    /* variable properties according to var type */
    apr_hash_set( varprops, "bit", APR_HASH_KEY_STRING, (void*)bitprops );
    apr_hash_set( varprops, "number", APR_HASH_KEY_STRING, (void*)numprops );
    apr_hash_set( varprops, "string", APR_HASH_KEY_STRING, (void*)strprops );

    /* create plugin meta-info and return it back */
    return lt_server_plugin_info_create(
            "Jiri Barton <jbar@swac.cz>",                       /* author */
            VERSION,                                            /* version */
            TODAY,                                              /* date */
            "ModbusTCP Slave",                                  /* shortdesc */
            "ModbusTCP Slave (Server) Lintouch Server Plugin",  /* longdesc */
            props,                                              /* default props */
            varprops,                                           /* default varprops */
            data,                                               /* plugin data */
            pool );
}

lt_server_status_t
LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin )
{
    apr_pool_t *pluginpool = lt_server_plugin_pool_get(plugin);

    /* plugin specific data */
    my_plugin_data * data = (my_plugin_data*)
        lt_server_plugin_data_get( plugin );

    /* get the plugin config */
    const lt_server_plugin_config_t * config =
        lt_server_plugin_config_get( plugin );

    /* retrieve the port number from the plugin config */
    const char * tmp = NULL;
    int tmpd = 502;
    tmp = apr_hash_get( config->properties,
            "port", APR_HASH_KEY_STRING );
    if( tmp == NULL ) {
        tmp = "502";
    }
    sscanf( tmp, "%d", &tmpd );
    data->port_no = tmpd;

    /* cache the varset */
    data->varset = lt_server_plugin_varset_get(plugin);

    /* hash all the variables into CO, DI, IR, or HR category */
    {
        apr_pool_t * lpool = NULL;
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;
        int i, datatype;

        apr_pool_create( &lpool, pluginpool );
        h = lt_var_set_variables_get(lt_server_plugin_varset_get(plugin));
        hi = apr_hash_first( lpool, h );

        /* initialize the used data type range */
        sa_t addressspaceend[4];
        for (i = 0; i < 4; i++)
        {
            data->addressspacestart[i] = (1 << (int) sizeof(sa_t)*8) - 1;
            addressspaceend[i] = 0;

            data->varlists[i] =
                apr_array_make(pluginpool, 0, sizeof(lt_var_t *)); 
        }

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
            datatype = -1;
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
            sa_t addr = (sa_t) atoi(address + 2);

            /* enlarge the address space if necessary */
            if (addr < data->addressspacestart[datatype])
            {
                data->addressspacestart[datatype] = addr;
            }
            if (addr > addressspaceend[datatype])
            {
                addressspaceend[datatype] = addr;
            }

            /* add the variable to the varlist */
            lt_var_userdata_set(var, (void *) ((long) datatype<<16 | addr));
            *(lt_var_t **) apr_array_push(data->varlists[datatype]) = var;
        }

        for (i = 0; i < 4; i++)
        {
            /* check if there are any variables of that datatype */
            if (addressspaceend[i] < data->addressspacestart[i])
            {
                data->addressspacestart[i] = addressspaceend[i];
            }

            int sizetobe = (i < 2 ?
                (addressspaceend[i] - data->addressspacestart[i])>>3 :
                (addressspaceend[i] - data->addressspacestart[i])<<1)
                + 2;
            data->addressspace[i] = apr_array_make(pluginpool, sizetobe, 1);

            /* a dirty workaround to initialize the array with the
            required number of elements */
            int j;
            for (j = 0; j < sizetobe; j++)
            {
                apr_array_push(data->addressspace[i]);
            }

            memset(data->addressspace[i]->elts,
                data->addressspace[i]->nelts, 0);
        }

        apr_pool_destroy( lpool );
    }

    return LT_SERVER_SUCCESS;
}

/* Synchronize the data from the address space of the modbus plugin to
the real variables. */
void modbus_sync_addressspace(apr_array_header_t *varlists[],
    apr_array_header_t *addressspace[])
{
  int i, j;
  /* update COILS and DISCRETE_INPUTS */
  for (i = 0; i < 2; i++)
  {
      apr_array_header_t *varlist = varlists[i];
      apr_byte_t *as = (apr_byte_t *) addressspace[i]->elts;
      for (j = 0; j < varlist->nelts; j++)
      {
          lt_var_real_value_bit_set(((lt_var_t **) varlist->elts)[j],
             as[j>>3] & (1 << (j&7)) ? 1 : 0);
      }
  }

  /* update the registers */
  for (i = 2; i < 4; i++)
  {
      apr_array_header_t *varlist = varlists[i];
      apr_byte_t *as = (apr_byte_t *) addressspace[i]->elts;
      for (j = 0; j < varlist->nelts; j++, as++, as++)
      {
          lt_var_real_value_number_set(((lt_var_t **) varlist->elts)[j],
             ntohs(*(apr_uint16_t *)as));
      }
  }
}

lt_server_status_t
LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin )
{
    my_plugin_data * data = (my_plugin_data *)
        lt_server_plugin_data_get( plugin );
    apr_socket_t *lsock;
    apr_pool_t *lsock_pool = NULL;
    apr_status_t status;

    #define FAILMESSAGE "Cannot start the MODBUS Slave plugin: "

    /* create local pool */
    apr_pool_create( &lsock_pool, lt_server_plugin_pool_get( plugin ) );

    /* start the TCP server */
    status = apr_socket_create_ex(&lsock, APR_INET, SOCK_STREAM,
        APR_PROTO_TCP, lsock_pool);
    if(status != APR_SUCCESS) {
        lt_server_logger_print_apr(PRIO_DEFAULT, "apr_socket_create_ex",
            FAILMESSAGE);
        return LT_SERVER_INVALID_ARG;
    }
    status = apr_socket_opt_set(lsock, APR_SO_REUSEADDR, 1);
    if(status != APR_SUCCESS) {
        apr_socket_close(lsock);
        lt_server_logger_print_apr(PRIO_DEFAULT, "setsockopt", FAILMESSAGE);
        return LT_SERVER_INVALID_ARG;
    }

    apr_sockaddr_t *server_addr;
    apr_sockaddr_info_get(&server_addr, NULL, APR_INET, data->port_no, 0,
        lsock_pool);

    status = apr_socket_bind(lsock, server_addr);
    if(status != APR_SUCCESS) {
        apr_socket_close(lsock);
        lt_server_logger_print_apr(PRIO_DEFAULT, "bind", FAILMESSAGE);
        return LT_SERVER_INVALID_ARG;
    }

    status = apr_socket_listen(lsock, 1);
    if(status != APR_SUCCESS) {
        apr_socket_close(lsock);
        lt_server_logger_print_apr(PRIO_DEFAULT, "listen", FAILMESSAGE);
        return LT_SERVER_INVALID_ARG;
    }

    lt_server_logger_print(PRIO_PLUGIN,
        "MODBUS server listening on TCP port %d.\n", data->port_no);

    apr_pollfd_t pollfd;
    apr_pollset_t *pollset;

    apr_pollset_create(&pollset, 100, lsock_pool, 0);

    pollfd.p = lsock_pool;
    pollfd.desc_type = APR_POLL_SOCKET;
    pollfd.reqevents = APR_POLLIN;
    pollfd.desc.s = lsock;
    pollfd.client_data = NULL;
    apr_pollset_add(pollset, &pollfd);

    apr_time_t lastsync = 0;
    const apr_time_t syncperiod = 1E5; /* 100ms */
    while( lt_server_plugin_should_run( plugin ) ) {
        apr_time_t timenow = apr_time_now();
        apr_int32_t num;
        const apr_pollfd_t * desc;
        int i;

        status = apr_pollset_poll(pollset, syncperiod, &num, &desc);

        for(i = 0; i < num; i++, desc++) {
            if (desc->desc.s == lsock) {
                apr_socket_t *new_sock;
                apr_pool_t *new_pool;
                apr_pollfd_t *new_pollfd;

                apr_pool_create(&new_pool, lsock_pool);
                apr_socket_accept(&new_sock, lsock, new_pool);

                lt_server_logger_print(PRIO_PLUGIN, "accepted new client\n");

                new_pollfd = apr_pcalloc(new_pool, sizeof(apr_pollfd_t));
                new_pollfd->p = new_pool;
                new_pollfd->desc_type = APR_POLL_SOCKET;
                new_pollfd->reqevents = APR_POLLIN;
                new_pollfd->desc.s = new_sock;

                new_pollfd->client_data = modbus_create(new_pool,
                    data->addressspace, data->addressspacestart);
                apr_pollset_add(pollset, new_pollfd);

                continue;
            }
            // check if we can read
            if (desc->rtnevents & APR_POLLIN) {

                if (timenow - syncperiod > lastsync)
                {
                    lt_var_set_requested_values_lock( data->varset );

                    /* copy the req values to the real ones as in loopback*/
                    if (apr_hash_count(
                        lt_var_set_dirty_requested_values_get(
                        data->varset)))
                    {
                        apr_hash_t * dirty_req =
                            lt_var_set_dirty_requested_values_get(
                                data->varset );
                        apr_hash_index_t * hi =
                            apr_hash_first( lsock_pool, dirty_req );

                        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
                            const char * varname = NULL;
                            lt_var_t * var = NULL;

                            apr_hash_this( hi, (const void**)&varname, NULL,
                                (void**)&var );

                            lt_var_real_value_set( var,
                                 lt_var_requested_value_get( var ));

                            /* update the address space */
                            long address = (long) lt_var_userdata_get(var);
                            char *addressspace = data->addressspace[
                                (address & 0x30000) >> 16]->elts;
                            address &= 0xFFFF;
                            if (lt_var_type_get(var) == LT_IO_TYPE_BIT)
                            {
                                addressspace[address>>3] =
                                    (~(1 << (address&7))
                                    & addressspace[address>>3])
                                    | (lt_var_real_value_bit_get(var)
                                        ? 1 << (address&7) : 0);
                            }
                            else if (lt_var_type_get(var) ==
                                LT_IO_TYPE_NUMBER)
                            {
                                ((sa_t *) addressspace)[address] =
                                    htons((sa_t)
                                    lt_var_real_value_number_get(var));
                            }
                        }
                    }

                    /* clear the dirty flags */
                    lt_var_set_requested_values_sync( data->varset, -1 );
                    lt_var_set_requested_values_unlock( data->varset );
                }

                /* and, action! */
                status = modbus_indication(desc);

                if (status != APR_SUCCESS) {
                    modbus_destroy(desc->client_data);
                    apr_socket_close(desc->desc.s);
                    lt_server_logger_print_apr(PRIO_PLUGIN, "send/recv",
                        "closing the socket");
                    apr_pollset_remove(pollset, desc);
                    apr_pool_destroy( desc->p );
                    continue;
                }
            }
        }
        /* sync the real values to the clients */
        if (timenow - syncperiod > lastsync)
        {
            modbus_sync_addressspace(data->varlists, data->addressspace);
            lt_var_set_real_values_sync( data->varset, -1 );
            lastsync = timenow;
        }
    }

    return LT_SERVER_SUCCESS;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: modbusslave.c 1520 2006-03-08 12:22:15Z hynek $
 */
