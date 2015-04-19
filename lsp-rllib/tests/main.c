/* $Id: main.c 420 2005-03-13 11:16:53Z hynek $
 *
 *   FILE: main.c --
 * AUTHOR: Hynek Petrak <hynek@swac.cz>
 *   DATE: 10 March 2005
 *
 * Copyright (c) 2001-2004 S.W.A.C. GmbH, Germany.
 * Copyright (c) 2001-2004 S.W.A.C. Bohemia s.r.o, Czech Republic.
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS DISTRIBUTED AS IS AND WITHOUT ANY WARRANTY.
 *
 * Redistribution of the sources in any form is not permitted without
 * permission obtained from the copyright holder.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <apr_poll.h>
#include "modbus.h"

/*
#include <lt/cp/cp.h>
#include <lt/cp/cp-server.h>
#include <lt/variables/variables.h>
*/
//#include "plugin.h"

volatile int run = 1;
int port_no = 502;

void signal_handler(int state);
void signal_handler(int state) {
    run = 0;
}

/*
lt_var_set_t *vars1, *vars2;
lt_var_t *var1;
*/
int socket_server(void);
int socket_server(void) {


    int retval;

    // sesion related
    #define MODBUSFRAMELEN 256
    unsigned char modbusdata[MODBUSFRAMELEN];
    apr_size_t rbuflen;
    unsigned char * sbuf;
    unsigned sbuflen;

    apr_socket_t *lsock;
    apr_pool_t  *lsock_pool;

    apr_status_t status;

    apr_pool_create(&lsock_pool, NULL);


    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    status = apr_socket_create_ex(&lsock, APR_INET, SOCK_STREAM, APR_PROTO_TCP, lsock_pool);
    if(status != APR_SUCCESS) {
        printf("socket error %d\n", status);
        return status;
    }
    status = apr_socket_opt_set(lsock, APR_SO_REUSEADDR, 1);
    if(status != APR_SUCCESS) {
        apr_socket_close(lsock);
        printf("setsockopt error %d\n", status);
        return status;
    }

    apr_sockaddr_t *server_addr;
    apr_sockaddr_info_get(&server_addr, NULL, APR_INET, port_no, 0, lsock_pool);

    status = apr_socket_bind(lsock, server_addr);
    if(status != APR_SUCCESS) {
        apr_socket_close(lsock);
        printf("bind error %d\n", status);
        return status;
    }

    status = apr_socket_listen(lsock, 1);
    if(status != APR_SUCCESS) {
        apr_socket_close(lsock);
        printf("listen error %d\n", status);
        return status;
    }

    printf("modbus server listening on TCP port %d.\n", port_no);

    apr_pollfd_t pollfd;
    apr_pollset_t *pollset;

    apr_pollset_create(&pollset, 20, lsock_pool, 0);

    pollfd.p = lsock_pool;
    pollfd.desc_type = APR_POLL_SOCKET;
    pollfd.reqevents = APR_POLLIN;
    pollfd.desc.s = lsock;
    pollfd.client_data = NULL;
    apr_pollset_add(pollset, &pollfd);

    while(run) {
        apr_interval_time_t timeout;
        apr_int32_t num;
        const apr_pollfd_t * desc;
        int i, slave, function;

        timeout = 1E6; // 1 second
        status = apr_pollset_poll(pollset, timeout, &num, &desc);

        if (num == 0) {
            continue; // on timeout
        }
        for(i = 0; i < num; i++, desc++) {
            if (desc->desc.s == lsock) {
                apr_socket_t *new_sock;
                apr_pool_t *new_pool;
                apr_pollfd_t *new_pollfd;
                //lt_cp_session_t * session;

                apr_pool_create(&new_pool, lsock_pool);
                apr_socket_accept(&new_sock, lsock, new_pool);

                printf("accepted new client\n");

                new_pollfd = apr_pcalloc(new_pool, sizeof(apr_pollfd_t));
                new_pollfd->p = new_pool;
                new_pollfd->desc_type = APR_POLL_SOCKET;
                new_pollfd->reqevents = APR_POLLIN | APR_POLLOUT;
                new_pollfd->desc.s = new_sock;

                // setup session here
                //lt_cp_session_create(&session, LT_CP_SERVER, new_pool);
                //lt_cp_session_project_url_set(session, "file:/home/hynek/slider.ltp");

                //lt_cp_session_variables_register(session, "Loopback", vars1);
            //    lt_cp_session_variables_register(session, "conn2", vars1);
                //
                new_pollfd->client_data = modbus_create(new_pool);
                apr_pollset_add(pollset, new_pollfd);

                continue;
            }
            // check if we can read
            if (desc->rtnevents & APR_POLLIN) {
                rbuflen = MODBUSFRAMELEN;

                status = modbus_indication(desc, &slave, &function, modbusdata);
                if (status != APR_SUCCESS) {
                    modbus_destroy(desc->client_data);
                    apr_socket_close(desc->desc.s);
                    printf("recv error %d\n", status);
                    apr_pollset_remove(pollset, desc);
                    apr_pool_destroy( desc->p );
                    continue;
                }

                //the following block is hardcoded for the testing purposes
                if (function == 1)
                {
                    //read coils
                    int startaddr = (int) modbusdata[0] << 8 + modbusdata[1];
                    int numcoils = (int) modbusdata[2] << 8 + modbusdata[3];
                    int i;

                    //fill in the buffer with alternating bits
                    *modbusdata = (apr_byte_t) numcoils/8 + numcoils%8 ? 0 : 1;
                    memset(modbusdata + 1, 0, *modbusdata);
                    for (i = 0; i < numcoils; i++)
                    {
                         modbusdata[i/8+1] |= (apr_byte_t) ((startaddr + i) % 2) << i%8;
                    }

                    status = modbus_response(desc, slave, function, modbusdata, *modbusdata + 1);
                }
                else
                {
                    //function code not supported
                    *modbusdata = 1;
                    status = modbus_response(desc, slave, function+0x80, modbusdata, 1);
                }
                //printf("read: %d \n", rbuflen);

                //retval = lt_cp_session_process(desc->client_data, rbuf, rbuflen, NULL, 0);
                // handle session errors here
            }
            // handle the active connection
            /*
            lt_var_set_requested_values_sync( vars1, -1 );
            lt_var_real_value_set( var1,
            lt_var_requested_value_get( var1 ));
            lt_var_set_real_values_sync( vars1, -1 );*/

            //if (desc->rtnevents & APR_POLLOUT) {
                //lt_cp_session_process(desc->client_data, NULL, 0, (void **) &sbuf, &sbuflen);
            //    if (sbuflen > 0) {
            //        printf("sending: %d \n", sbuflen);
            //        apr_socket_send(desc->desc.s,  sbuf, &sbuflen);
                    if (status != APR_SUCCESS) {
                        //lt_cp_session_destroy(desc->client_data);
                        modbus_destroy(desc->client_data);
                        apr_socket_close(desc->desc.s);
                        printf("sendv error %d\n", status);
                        apr_pollset_remove(pollset, desc);
                        apr_pool_destroy( desc->p );
                        continue;
                    }
            //    }
            //}

        }
    }
    return 0;
}

int main(void) {

    apr_initialize();
    //lt_cp_initialize();

    //init_plugins();

    /*
    vars1 = lt_var_set_create(0, NULL);

    vars2 = lt_var_set_create(0, NULL);

    lt_var_set_variable_register( vars1, "var1",
            var1 = lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
    lt_var_set_variable_register( vars2, "var2",
            lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
    */
    socket_server();

    //apr_pool_destroy( lt_var_set_pool_get( vars1 ) );
    //apr_pool_destroy( lt_var_set_pool_get( vars2 ) );

    //lt_cp_terminate();

    return 0;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: main.c 420 2005-03-13 11:16:53Z hynek $
 */
