/* $Id: snmp.c 577 2005-04-22 12:33:25Z mman $
 *
 *   FILE: snmp.c --
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* for inet_ntoa to convert 32bit int to string ip address */
#include <arpa/inet.h>


#include <apr_time.h>
#include <apr_strings.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "lt/server/plugin.h"
#include "lt/server/logger.h"

#include "config.h"

/**
 * SNMP Lintouch Server Plugin.
 *
 * This is a very simple, I'd say demonstration plugin that you might use to
 * periodically poll one or more SNMP OIDs at a given remote machine.
 *
 * The plugin can be configured with two global properties:
 *
 * - hostname: defines the snmp server (a switch, router, webserver, etc.)
 * - community: SNMPv1 community, defaults to "public"
 * - refresh: defines how often to refresh all polled values
 *
 * @author Martin Man <mman@swac.cz>
 */

/* automatically define mandatory plugin symbols */
LT_SERVER_PLUGIN_EXPORT;

/* SNMP PLUGIN METHODS */

lt_server_plugin_info_t *
PLUGIN_EXPORT LT_SERVER_PLUGIN_CREATE( apr_pool_t * pool );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin );
lt_server_status_t
PLUGIN_EXPORT LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin );

/* SNMP PLUGIN SPECIFIC DATA */

typedef struct {
    /* oid from configuration */
    const char * oid;
    /* oid parsed by net-snmp code */
    oid anOID[MAX_OID_LEN];
    size_t anOID_len;
} my_vardata;

typedef struct {
    /* where to connect */
    const char * hostname;
    /* what community to use */
    const char * community;
    /* how long to sleep before next refresh */
    apr_interval_time_t refresh;
    /* varset used to communicate with the clients */
    lt_var_set_t * varset;
    /* my_vardata associated with each variable */
    apr_hash_t * vardata;
    /* mapping between OIDs and lintouch variables */
    apr_hash_t * oid2var;
} my_plugin_data;

static float _property_as_float( apr_hash_t * hash,
        const char * name, float dflt );
static int _property_as_int( apr_hash_t * hash,
        const char * name, int dflt );
static const char * _property_as_string( apr_hash_t * hash,
        const char * name, const char * dflt );

/**
 * Sleep for given time while monitoring the "should exit" flag. Optionaly
 * report how many seconds of sleep are left to given counter.
 */
static int _my_interruptible_sleep( lt_server_plugin_t * plugin,
        apr_interval_time_t sleep, lt_var_t * counter );

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
    apr_hash_set( props, "hostname", APR_HASH_KEY_STRING, (void*)"localhost" );
    apr_hash_set( props, "community", APR_HASH_KEY_STRING, (void*)"public" );
    apr_hash_set( props, "refresh", APR_HASH_KEY_STRING, (void*)"10" );

    /* bit var properties */
    /* an example is operational status of interface 0 up/down */
    apr_hash_set( bitprops, "OID", APR_HASH_KEY_STRING,
            (void*)"ifOperStatus.1" );
    /* num var properties */
    /* an example is number of received octets at interface 0 */
    apr_hash_set( numprops, "OID", APR_HASH_KEY_STRING,
            (void*)"ifInOctets.1" );
    /* string var properties */
    /* an example is the system description */
    apr_hash_set( strprops, "OID", APR_HASH_KEY_STRING,
            (void*)"system.sysDescr.0" );

    /* variable properties according to var type */
    apr_hash_set( varprops, "bit", APR_HASH_KEY_STRING, (void*)bitprops );
    apr_hash_set( varprops, "number", APR_HASH_KEY_STRING, (void*)numprops );
    apr_hash_set( varprops, "string", APR_HASH_KEY_STRING, (void*)strprops );

    /* create plugin meta-info and return it back */
    return lt_server_plugin_info_create(
            "Martin Man <mman@swac.cz>",            /* author */
            VERSION,                                /* version */
            TODAY,                                  /* date */
            "SNMP",                                 /* shortdesc */
            "SNMP Lintouch Server Plugin",          /* longdesc */
            props,                                  /* default props */
            varprops,                               /* default varprops */
            data,                                   /* plugin data */
            pool );
}

lt_server_status_t
LT_SERVER_PLUGIN_INIT( lt_server_plugin_t * plugin )
{
    /* get plugin specific data */
    my_plugin_data * data = (my_plugin_data*)
        lt_server_plugin_data_get( plugin );
    /* get plugin config */
    const lt_server_plugin_config_t * config =
        lt_server_plugin_config_get( plugin );
    /* get default plugin config ( could be optimized ) */
    const lt_server_plugin_config_t * dconfig =
        lt_server_plugin_default_config_get( plugin );

    /* parse config and initialize local data structure */
    apr_hash_t * props = config->properties;
    if( props == NULL ) props = dconfig->properties;
    data->refresh = (apr_interval_time_t)
        ( _property_as_float( props, "refresh", 10 ) * 1000 * 1000);

    /* extract hostname & community */
    data->hostname =
        _property_as_string( props, "hostname", "localhost" );
    data->community =
        _property_as_string( props, "community", "public" );

    /* remember varset to speed up access to it */
    data->varset = lt_server_plugin_varset_get( plugin );

    /* remember data for polled variables */
    data->vardata = apr_hash_make(
            lt_server_plugin_pool_get( plugin ) );
    data->oid2var = apr_hash_make(
            lt_server_plugin_pool_get( plugin ) );

    /* net-snmp specific code */
    init_snmp("lsp-snmp");
    /* EOF net-snmp specific code */

    /* now for each numerical variable that does not start with '.',
     * set up defaults + props */
    {
        apr_pool_t * pool = NULL;
        apr_pool_t * lpool = NULL;
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;

        pool = lt_server_plugin_pool_get( plugin );
        apr_pool_create( &lpool, pool );
        h = lt_var_set_variables_get( data->varset );
        hi = apr_hash_first( lpool, h );

        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
            const char * name = NULL;
            lt_var_t * var = NULL;
            apr_hash_t * varprops = NULL;
            my_vardata * vardata = NULL;

            apr_hash_this( hi, (const void**)&name, NULL, (void**)&var );

            /* ignore variables that start with '.' */
            if( name[ 0 ] == '.' ) continue;

            /* try to get variable properties */
            varprops = apr_hash_get( config->varproperties,
                    name, APR_HASH_KEY_STRING );
            if( varprops == NULL ) {
                varprops = apr_hash_get( dconfig->varproperties,
                        name, APR_HASH_KEY_STRING );
            }

            /* allocate varable limits structure and fill it with defaults */
            vardata = (my_vardata*) apr_pcalloc( pool,
                    sizeof( my_vardata ) );
            vardata->oid = _property_as_string( varprops, "OID", "" );

            /* net-snmp specific code */
            vardata->anOID_len = MAX_OID_LEN;
            if( ! snmp_parse_oid( vardata->oid,
                        vardata->anOID, &vardata->anOID_len ) ) {
                lt_server_logger_print( PRIO_DEFAULT,
                        "SNMP: Error, unknown OID: '%s'\n", vardata->oid );
                continue;
            }

            /* EOF net-snmp specific code */

            /* remember a variable this oid represents */
            apr_hash_set( data->oid2var,
                    (const void*)vardata->anOID,
                    vardata->anOID_len * sizeof( oid ), (const void*)var );

            /* remember we will be polling this variable */
            apr_hash_set( data->vardata,
                   apr_pstrdup( pool, name ), APR_HASH_KEY_STRING, vardata );

        }

        apr_pool_destroy( lpool );
    }

    return LT_SERVER_SUCCESS;
}

lt_server_status_t
LT_SERVER_PLUGIN_RUN( lt_server_plugin_t * plugin )
{
    /* get plugin specific data */
    my_plugin_data * data = (my_plugin_data*)
        lt_server_plugin_data_get( plugin );
    apr_pool_t * lpool = NULL;

    struct snmp_session session, *ss;

    /* create local pool used to allocate temporary values */
    apr_pool_create( &lpool, lt_server_plugin_pool_get( plugin ) );

    lt_server_logger_print( PRIO_PLUGIN, "SNMP: started...\n" );

    /* net-snmp specific code */
    snmp_sess_init( &session );
    session.peername = (char*)data->hostname;
    session.version = SNMP_VERSION_1;
    session.community = (unsigned char*)data->community;
    session.community_len = strlen( data->community );
    SOCK_STARTUP;

    ss = snmp_open( &session );
    if( ss == NULL ) {
        lt_server_logger_print( PRIO_DEFAULT,
                "SNMP: Error, unable to create snmp session\n" );
        snmp_perror( "snmp_open" );
    }
    /* EOF net-snmp specific code */

    while( ss != NULL && lt_server_plugin_should_run( plugin ) ) {

        /* net-snmp specific code */
        struct snmp_pdu * pdu = NULL, * response = NULL;
        int status = 0;

        /* clear the pool used to hold temporarily created values */
        apr_pool_clear( lpool );

        /* first lock requested values and verify if exist values to send
           if exist snmp set it, otherwise get it*/
        lt_var_set_requested_values_lock( data->varset );

        if( lt_var_set_wait_for_new_requested_values( data->varset, 
                data->refresh ) == LT_VAR_SUCCESS )
        {

            lt_server_logger_print( PRIO_PLUGIN + 1,
                    "SNMP: going to send requested values to the server...\n" );

            /* create PDU for our SET request */
            pdu = snmp_pdu_create( SNMP_MSG_SET );
            /* EOF net-snmp specific code */
               
            /* for each variable that passed an OID check */
            {
                char type = 'b';
                char value[SNMP_MAX_CMDLINE_OIDS];
                apr_hash_t * dirty_req =
                        lt_var_set_dirty_requested_values_get( data->varset );
                apr_hash_index_t * hi_req = apr_hash_first( lpool, dirty_req );
                        
                for(; hi_req != NULL;
                        hi_req = apr_hash_next( hi_req ) ) {
                    const char * varname_req = NULL;
                    lt_var_t * var = NULL;
                    my_vardata * vardata = NULL;
                                
                    apr_hash_this( hi_req,(const void**)&varname_req, NULL, 
                            (void**)&var );

                    vardata = apr_hash_get( data->vardata,
                            (const void**)varname_req, APR_HASH_KEY_STRING );

                    lt_server_logger_print( PRIO_PLUGIN + 1,
                            "SNMP: preparing SET PDU for variable %s\n",
                            varname_req );
                    lt_server_logger_print( PRIO_PLUGIN + 1,
                            "SNMP: variable %s mapped to OID %s\n",
                            varname_req,
                            vardata->oid );
                                
                    switch( lt_var_type_get( var ) ) {
                        case LT_IO_TYPE_BIT:
                            type = 'b';
                            snprintf (value, SNMP_MAX_CMDLINE_OIDS, "%d",
                                    lt_var_requested_value_bit_get(var));
                            break;
                        case LT_IO_TYPE_NUMBER:
                            type = 'i';    
                            snprintf (value, SNMP_MAX_CMDLINE_OIDS, "%d",
                                    lt_var_requested_value_number_get(var));
                            break;
                        case LT_IO_TYPE_STRING:
                            type = 's';    
                            snprintf (value, SNMP_MAX_CMDLINE_OIDS, "%s",
                                    lt_var_requested_value_string_get(var));
                            break;
                        case LT_IO_TYPE_NULL:
                        case LT_IO_TYPE_SEQUENCE:
                            break;
                    }	
                                
                    /* net-snmp specific code */
                    snmp_add_var(pdu, vardata->anOID, 
                            vardata->anOID_len, type, value);
                    /* EOF net-snmp specific code */
                }
            }
                
        } else {

            lt_server_logger_print( PRIO_PLUGIN + 1,
                    "SNMP: going to refresh all polled variables...\n" );

            /* create PDU for our GET request */
            pdu = snmp_pdu_create( SNMP_MSG_GET );
            /* EOF net-snmp specific code */

            /* clear the pool used to hold temporarily created values */
            apr_pool_clear( lpool );

            /* for each variable that passed an OID check */
            {
                apr_hash_t * rel = data->vardata;
                apr_hash_index_t * hi = apr_hash_first( lpool, rel );

                for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
                    const char * varname = NULL;
                    my_vardata * vardata = NULL;

                    apr_hash_this( hi, (const void**)&varname, NULL, 
                            (void**)&vardata );

                    /* net-snmp specific code */
                    snmp_add_null_var(pdu, vardata->anOID, 
                            vardata->anOID_len);
                    /* EOF net-snmp specific code */
                }
            }
        }

        /* clear dirty flags on requested values in our varset */
        lt_var_set_requested_values_sync( data->varset, -1 );

        /* and unlock our requested values so that new values can arrive
         * from the clients */
        lt_var_set_requested_values_unlock( data->varset );

        /* net-snmp specific code */
        status = snmp_synch_response(ss, pdu, &response);

        if( status == STAT_SUCCESS &&
                response->errstat == SNMP_ERR_NOERROR ) {
            lt_server_logger_print( PRIO_PLUGIN + 1,
                    "SNMP: received data...\n" );

            {
                struct variable_list * vars = NULL;

                for( vars = response->variables; vars != NULL;
                        vars = vars->next_variable ) {
                    lt_var_t * var = NULL;
                    lt_server_logger_print( PRIO_PLUGIN + 1,
                            "SNMP: received value of type %d\n",
                            vars->type );

                    /* get a lintouch variable belonging to received OID */
                    var = apr_hash_get( data->oid2var,
                            (const void*)vars->name,
                            vars->name_length * sizeof( oid ) );

                    /* string response, string var */
                    if( lt_var_type_get( var ) == LT_IO_TYPE_STRING &&
                            vars->type == ASN_OCTET_STR ) {
                        lt_var_real_value_string_set( var,
                                    vars->val.string, vars->val_len );
                    } else
                    /* IpAddress response, string var */
                    if( lt_var_type_get( var ) == LT_IO_TYPE_STRING &&
                            vars->type == 0x40 ) {
                        /* convert IPV4 addr to string */
                        {
                            struct in_addr in;
                            unsigned char * ip;
                            in.s_addr = *vars->val.integer;
                            ip = (unsigned char*)inet_ntoa( in );

                            lt_var_real_value_string_set2( var, ip );
                        }
                    } else
                    /* INTEGER response, num var */
                    if( lt_var_type_get( var ) == LT_IO_TYPE_NUMBER &&
                            vars->type == ASN_INTEGER ) {
                        lt_var_real_value_number_set( var,
                                    *vars->val.integer );
                    } else
                    /* Counter32 response, num var */
                    if( lt_var_type_get( var ) == LT_IO_TYPE_NUMBER &&
                            vars->type == 0x41 ) {
                        lt_var_real_value_number_set( var,
                                    *vars->val.integer );
                    } else
                    /* Gauge32 response, num var */
                    if( lt_var_type_get( var ) == LT_IO_TYPE_NUMBER &&
                            vars->type == 0x42 ) {
                        lt_var_real_value_number_set( var,
                                    *vars->val.integer );
                    } else
                    /* UInteger32 response, num var */
                    if( lt_var_type_get( var ) == LT_IO_TYPE_NUMBER &&
                            vars->type == 0x47 ) {
                        lt_var_real_value_number_set( var,
                                    *vars->val.integer );
                    } else
                    /* Timeticks response, num var */
                    if( lt_var_type_get( var ) == LT_IO_TYPE_NUMBER &&
                            vars->type == 0x43 ) {
                        lt_var_real_value_number_set( var,
                                    *vars->val.integer );
                    } else {
                        lt_server_logger_print( PRIO_DEFAULT,
                                "SNMP: unable to handle ASN data type 0x%x\n",
                                vars->type );
                    }
                }

            }

        } else {
            lt_server_logger_print( PRIO_DEFAULT,
                    "SNMP: Error while communicating with the SNMP server\n" );
            if (status == STAT_SUCCESS) {
                lt_server_logger_print( PRIO_DEFAULT,
                        "SNMP: Error in packet, Reason: %s\n",
                        snmp_errstring(response->errstat));
            } else {
                snmp_sess_perror("snmp_synch_response", ss);
            }
        }

        /* sync real values back to clients */
        lt_var_set_real_values_sync( data->varset, -1 );

        /* net-snmp specific code */
        if( response != NULL ) {
            snmp_free_pdu( response );
            response = NULL;
        }
        /* EOF net-snmp specific code */

        lt_server_logger_print( PRIO_PLUGIN,
                "SNMP: waiting until next refresh is needed...\n" );
    }

    /* net-snmp specific code */
    snmp_close( ss );
    SOCK_CLEANUP;
    /* net-snmp specific code */

    lt_server_logger_print( PRIO_PLUGIN, "SNMP: exiting...\n" );

    return LT_SERVER_SUCCESS;
}

float _property_as_float( apr_hash_t * hash, const char * name, float dflt )
{
    float result;
    const char * val = apr_hash_get( hash, name, APR_HASH_KEY_STRING );
    if( val == NULL ) return dflt;

    if( sscanf( val, "%f", &result ) != 1 ) return dflt;
    return result;
}

int _property_as_int( apr_hash_t * hash, const char * name, int dflt )
{
    int result;
    const char * val = apr_hash_get( hash, name, APR_HASH_KEY_STRING );
    if( val == NULL ) return dflt;

    if( sscanf( val, "%d", &result ) != 1 ) return dflt;
    return result;
}

const char * _property_as_string( apr_hash_t * hash,
        const char * name, const char * dflt )
{
    const char * val = apr_hash_get( hash, name, APR_HASH_KEY_STRING );
    if( val == NULL ) return dflt;
    return val;
}

static int _my_interruptible_sleep( lt_server_plugin_t * plugin,
        apr_interval_time_t sleep, lt_var_t * counter )
{
    int i = 0;
    int total = 0;
    apr_interval_time_t partial = 0;
    int ret = 0;
    apr_pool_t * pool;

    /* sleep required for less than one second */
    if( sleep <= 1 * 1000 * 1000 ) {
        apr_sleep( sleep );
        return ! lt_server_plugin_should_run( plugin );
    }

    /* sleep required for more than one second */
    apr_pool_create( &pool, lt_server_plugin_pool_get( plugin ) );

    partial = 1 * 1000 * 1000;
    total = sleep / partial;

    for( i = 0; i < total; i ++ ) {
        /* sleep and check exit flag */
        apr_sleep( partial );
        if( ! lt_server_plugin_should_run( plugin ) ) {
            ret = 1;
            break;
        }

        /* decrease counter (if defined) */
        if( counter != NULL ) {
            lt_var_real_value_number_set( counter, total - i - 1 );
            lt_var_set_real_values_sync(
                    lt_server_plugin_varset_get( plugin ), -1 );
        }
    }

    apr_pool_destroy( pool );
    return ret;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: snmp.c 577 2005-04-22 12:33:25Z mman $
 */
