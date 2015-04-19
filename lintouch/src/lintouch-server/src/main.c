/* $Id: main.c 1774 2006-05-31 13:17:16Z hynek $
 *
 *   FILE: main.c --
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

#include <apr_file_io.h>
#include <apr_strings.h>
#include <apr_time.h>

#include "lt/cp/http.h"
#include "lt/cp/cp.h"
#include "lt/server/demo_plugins.h"
#include "lt/server/plugin.h"
#include "lt/server/logging.h"
#include "lt/server/project.h"
#include "lt/server/server.h"
#include "lt/server/logger.h"

#include "config.h"

static void printBanner( const char * progname );
static void printUsage( const char * progname );
static int parseCommandline( int argc, const char * const * argv );

/** Global memory pool */
static apr_pool_t * g_pool = NULL;
/** Are we in local mode */
static int g_local = 0;
/** The project specified at command line */
static const char * g_project = NULL;
/** The port specified at command line */
static int g_port = 5555;
/** The verbosity level */
static int g_level = 0;

int main( int argc, const char * const * argv, const char * const * env )
{
    /* http server and its port */
    lt_http_server_t * http = NULL;
    int http_port = 0;

    /* project url */
    const char * project_url = NULL;
    /* parsed project */
    lt_project_t * project = NULL;

    /* main server socket + server */
    apr_socket_t * socket = NULL;
    lt_server_t * server = NULL;

    /* current working directory */
    char * defaultpath = NULL;

    /* initialize APR */
    if( APR_SUCCESS !=
                apr_app_initialize( &argc, &argv, &env ) ) {
        lt_server_logger_print( 0,
                "Unable to Initalize APR Library\n" );
        return -1;
    }
    atexit( apr_terminate );

    /* initialize LTCP */
    if( lt_cp_initialize() != LT_CP_SUCCESS ) {
        lt_server_logger_print( 0,
                "Unable to Initalize Lintouch CP Library\n" );
        return -1;
    }
    atexit( lt_cp_terminate );

    /* create global memory pool */
    apr_pool_create( &g_pool, NULL );

    /* try to parse commandline arguments */
    if( ! parseCommandline( argc, argv ) ) {
        return 1;
    }

    /* set logger priority */
    lt_server_logger_priority_set( g_level );

    /* print banner */
    printBanner( argv[ 0 ] );

    /* start http server unless in local mode */
    if( ! g_local ) {
        if( lt_http_server_create( &http, g_project, 0, g_pool ) !=
                LT_HTTP_SUCCESS ) {
            lt_server_logger_print( 0,
                    "Unable to create Lintouch HTTP Server for %s\n",
                    g_project );
            return 2;
        }
        if( lt_http_server_start( http ) != LT_HTTP_SUCCESS ) {
            lt_server_logger_print( 0,
                    "Unable to start Lintouch HTTP Server for %s\n",
                    g_project );
            return 2;
        }
        http_port = lt_http_server_port_get( http );
        lt_server_logger_print( 0,
                "Lintouch HTTP Server Ready at port %d\n",
                http_port );
    }

    /* create socket */
    if( APR_SUCCESS != apr_socket_create( &socket,
                    APR_INET, SOCK_STREAM, 
#if (APR_MAJOR_VERSION > 0)
                    APR_PROTO_TCP,
#endif
                    g_pool ) ) {
        lt_server_logger_print_socket( 0, "apr_socket_create",
                "Unable to create master socket\n" );
        return 3;
    }
    if( APR_SUCCESS != apr_socket_opt_set( socket,
                    APR_SO_REUSEADDR, 1) ) {
        lt_server_logger_print_socket( 0, "apr_socket_opt_set",
                "Unable to set REUSEADDR to socket\n" );
    }

    /* bind socket to specified port */
    {
        /* prepare socket address */
        /* bind to 127.0.0.1 when in g_local mode */
        apr_sockaddr_t * sa = NULL;
        const char * localhost = "127.0.0.1";
        if( APR_SUCCESS != apr_sockaddr_info_get( &sa,
                        ( g_local ? localhost : NULL ),
                        AF_INET, g_port, 0, g_pool ) ) {
            lt_server_logger_print_socket( 0, "apr_sockaddr_info_get",
                    "Unable to create socket address for port %d\n", g_port );
            return 3;
        }

        /* bind socket to given port or let the OS to give us port */
        if( APR_SUCCESS != apr_socket_bind(
                        socket, sa ) ) {
            lt_server_logger_print_socket( 0, "apr_socket_bind",
                    "Unable to bind master socket\n" );
            return 3;
        }

        /* make the socket non-blocking */
        if( APR_SUCCESS != apr_socket_opt_set( socket,
                        APR_SO_NONBLOCK, 1 ) ) {
            lt_server_logger_print_socket( 0, "apr_socket_opt_set",
                    "Unable to make master socket non-blocking\n" );
            return 3;
        }

        /* make the master socket listening */
        if( APR_SUCCESS != apr_socket_listen( socket, 5 ) ) {
            lt_server_logger_print_socket( 0, "apr_socket_listen",
                    "Unable to make master socket listening\n" );
            return 3;
        }

        /* extract the assigned TCP/IP port in case user specified 0 at
         * commandline */
        {
            apr_sockaddr_t * sa = NULL;
            if( APR_SUCCESS != apr_socket_addr_get( &sa,
                            APR_LOCAL, socket ) ) {
                lt_server_logger_print_socket( 0, "apr_socket_addr_get",
                        "Unable to determine local address "
                        "of the master socket\n" );
                return 4;
            }
            g_port = sa->port;
        }
    }

    /* absolutize g_project to project_url */
    if( APR_SUCCESS != apr_filepath_get( &defaultpath,
                    0, g_pool ) ) {
        lt_server_logger_print_apr( 0, "apr_filepath_get",
                "Unable to determine current working directory\n" );
        return 4;
    }

    /* prepare project URL */
    if( g_local ) {
        if( APR_SUCCESS != apr_filepath_merge(
                        (char**)&project_url, defaultpath, g_project,
                        0, g_pool ) ) {
            lt_server_logger_print_apr( 0, "apr_filepath_merge",
                    "Unable to absolutize project filename %s (cwd is %s)\n",
                    g_project, defaultpath );
            return 4;
        }
    } else {
        /* FIXME: extract project filename from g_project path and use it
         * instead of hardcoded project.ltp */
        project_url = apr_pstrcat( g_pool,
                "http://%s:", apr_itoa( g_pool, http_port ),
                "/project.ltp", NULL );
    }

    lt_server_logger_print( 0, "Project URL is %s\n", project_url );

    /* create server */
    if( lt_server_create( &server, socket, project_url, g_pool ) !=
            LT_SERVER_SUCCESS ) {
        lt_server_logger_print( 0,
                "Unable to create Lintouch Server for %s\n",
                g_project );
        return 4;
    }

    /* parse project */
    if( lt_project_parse( &project, g_project,
                LT_PROJECT_INFO | LT_PROJECT_CONNECTIONS | LT_PROJECT_LOGGING,
                g_pool ) != LT_PROJECT_SUCCESS ) {
        lt_server_logger_print( 0,
                "Unable to open and parse lintouch project %s\n",
                g_project );
        return 5;
    }

    lt_server_logger_print( 0, "Loaded Project %s\n", g_project );
    lt_server_logger_print( 0, "    Description: %s\n",
            lt_project_shortdesc_get( project ) );
    lt_server_logger_print( 0, "    Author: %s\n",
            lt_project_author_get( project ) );
    lt_server_logger_print( 0, "    Version: %s\n",
            lt_project_version_get( project ) );
    lt_server_logger_print( 0, "    Date: %s\n",
            lt_project_date_get( project ) );

    /* load and init all logging plugins here */
    {
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;
        apr_pool_t * lpool = NULL;

        apr_pool_create( &lpool, g_pool );

        /* for each logging plugin */
        h = lt_project_logging_get( project );
        hi = apr_hash_first( lpool, h );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {

            const char * name = NULL;
            lt_project_logging_t * log = NULL;

            const char * pluginfile = NULL;
            lt_logging_plugin_t * plugin = NULL;

            apr_hash_this( hi, (const void**)&name, NULL, (void**)&log );

            lt_server_logger_print( 0,
                    "Bootstrapping logging %s of type %s\n",
                    name, lt_project_logging_type_get( log ) );

#if _WIN32
            pluginfile = apr_pstrcat( lpool, defaultpath,
                    "/../lib/lintouch/logging-plugins/",
                    lt_project_logging_type_get( log ),
                    "-0.dll", NULL );
#else
            pluginfile = apr_pstrcat( lpool, PREFIX_NATIVE,
                    "/lib/lintouch/logging-plugins/",
                    lt_project_logging_type_get( log ),
                    ".so", NULL );
#endif

            plugin = lt_logging_plugin_load( pluginfile, log, g_pool );
            if( plugin == NULL ) {
                lt_server_logger_print( 0,
                        "Unable to load plugin for logging %s (type %s)\n",
                        name,
                        lt_project_logging_type_get( log ) );
                return 4;
            }

            /* if successfull, register it to the server */
            if( LT_SERVER_SUCCESS != lt_server_logging_plugin_register(
                        server, name, plugin ) ) {
                lt_server_logger_print( 0,
                        "Unable to register plugin for connection"
                        " %s (type %s)\n",
                        name,
                        lt_project_logging_type_get( log ) );
                return 4;
            }

        }

        apr_pool_destroy( lpool );
    }

    /* for each connection, get type, try to load plugin and init it
     * register plugin instance with the server if successfull */
    {
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;
        apr_pool_t * lpool = NULL;

        apr_pool_create( &lpool, g_pool );

        /* for each connection */
        h = lt_project_connections_get( project );
        hi = apr_hash_first( lpool, h );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {

            const char * name = NULL;
            lt_project_connection_t * conn = NULL;

            const char * pluginfile = NULL;
            lt_server_plugin_t * plugin = NULL;

            apr_hash_this( hi, (const void**)&name, NULL, (void**)&conn );

            lt_server_logger_print( 0,
                    "Bootstrapping connection %s of type %s\n",
                    name, lt_project_connection_type_get( conn ) );

            /* create the plugin path by concating PREFIX_NATIVE +
             * lib/template/libraries/ + connection_type + platform
             * dependent extension */
#if _WIN32
            pluginfile = apr_pstrcat( lpool, defaultpath,
                    "/../lib/lintouch/server-plugins/",
                    lt_project_connection_type_get( conn ),
                    "-0.dll", NULL );
#else
            pluginfile = apr_pstrcat( lpool, PREFIX_NATIVE,
                    "/lib/lintouch/server-plugins/",
                    lt_project_connection_type_get( conn ),
                    ".so", NULL );
#endif

            /* try to load plugin, while passing in project to access
             * resources and connection to access config + varset */
            plugin = lt_server_plugin_load( pluginfile, g_project,
                    conn, g_pool );
            if( plugin == NULL ) {
                lt_server_logger_print( 0,
                        "Unable to load plugin for connection %s (type %s)\n",
                        name,
                        lt_project_connection_type_get( conn ) );
                return 4;
            }

            /* then initialize the plugin */
            if( LT_SERVER_SUCCESS != lt_server_plugin_init(
                        plugin ) ) {
                lt_server_logger_print( 0,
                        "Unable to initialize plugin for connection"
                        " %s (type %s)\n",
                        name,
                        lt_project_connection_type_get( conn ) );
                return 4;
            }

            /* if successfull, register it to the server */
            if( LT_SERVER_SUCCESS != lt_server_plugin_register(
                        server, name, plugin ) ) {
                lt_server_logger_print( 0,
                        "Unable to register plugin for connection"
                        " %s (type %s)\n",
                        name,
                        lt_project_connection_type_get( conn ) );
                return 4;
            }

        }

        apr_pool_destroy( lpool );
    }

    /* create hardcoded
     * "generator" and "loopback" plugins for testing purposes */
#if 0
    {
        lt_server_plugin_t * generator = NULL;
        lt_server_plugin_t * loopback = NULL;

        /* 10 ms generator */
        generator = lt_server_demo_plugin_generator_create(
                10 * 1000, g_pool );

        /* 10 ms latency loopback */
        loopback = lt_server_demo_plugin_loopback_create(
                10 * 1000, g_pool );

        lt_server_plugin_register( server, "generator", generator );
        lt_server_plugin_register( server, "loopback", loopback );
    }
#endif

    /* for each logging plugin call its init() function with list of all
     * connections parsed.
     */
    {
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;
        apr_pool_t * lpool = NULL;

        apr_pool_create( &lpool, g_pool );

        /* for each logging plugin */
        h = lt_server_logging_plugins_get( server );
        hi = apr_hash_first( lpool, h );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
            /* get a plugin */
            lt_logging_plugin_t * plugin = NULL;
            const char * name = NULL;
            apr_hash_this( hi, (const void**)&name, NULL, (void**)&plugin );

            lt_server_logger_print( 0, "Initializing logging %s\n", name);

            /* call the init with hash of all connections */
            if( LT_SERVER_SUCCESS != lt_logging_plugin_init( plugin,
                        lt_project_connections_get( project ) ) )
            {
                lt_server_logger_print( 0,
                        "Unable to initialize plugin for logging"
                        " %s\n", name);
                return 4;
            }
        }

        apr_pool_destroy( lpool );
    }

    /* start the server */
    if( lt_server_start( server ) != LT_SERVER_SUCCESS ) {
        lt_server_logger_print( 0,
                "Unable to start Lintouch Server for %s\n",
                g_project );
        return 4;
    }

    /* main loop waiting till the stdin is closed */
    {
        apr_file_t * stdinf = NULL;
        apr_file_open_stdin( &stdinf, g_pool );
        char dummy;

        lt_server_logger_print( 0,
                "Lintouch Server ready at port %d\n", g_port );
#if _WIN32
        lt_server_logger_print( 0, "Press Ctrl-Z and Enter to exit\n" );
#else
        lt_server_logger_print( 0, "Press Ctrl-D to exit\n" );
#endif

        /* until we can read from stdin, we simply read and discard it,
         * failure in reading means stdin is closed */
        while( APR_SUCCESS == apr_file_getc( &dummy, stdinf ) );

        lt_server_logger_print( 0, "Initiating shutdown sequence...\n" );
    }

    /* stop the server */
    if( lt_server_stop( server ) != LT_SERVER_SUCCESS ) {
        lt_server_logger_print( 0,
                "Unable to stop Lintouch Server for %s\n",
                g_project );
        return 4;
    }

    /* stop logging plugins */
    {
        apr_hash_t * h = NULL;
        apr_hash_index_t * hi = NULL;
        apr_pool_t * lpool = NULL;

        apr_pool_create( &lpool, g_pool );

        /* for each logging plugin */
        h = lt_server_logging_plugins_get( server );
        hi = apr_hash_first( lpool, h );
        for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
            /* get a plugin */
            const char* name = NULL;
            lt_logging_plugin_t * plugin = NULL;
            apr_hash_this( hi, (const void**)&name, NULL, (void**)&plugin );

            lt_server_logger_print( 0, "Stopping logging %s\n", name);

            /* call the exit function */
            lt_logging_plugin_exit( plugin );
        }

        apr_pool_destroy( lpool );
    }

    /* shutdown and close the master socket */
    apr_socket_shutdown( socket, APR_SHUTDOWN_READ );
    apr_socket_shutdown( socket, APR_SHUTDOWN_WRITE );
    apr_socket_close( socket );

    /* stop http server unless in local mode */
    if( ! g_local ) {
        if( lt_http_server_stop( http ) != LT_HTTP_SUCCESS ) {
            lt_server_logger_print( 0,
                    "Unable to stop Lintouch HTTP Server\n" );
            return 2;
        }
        if( lt_http_server_destroy( http ) != LT_HTTP_SUCCESS ) {
            lt_server_logger_print( 0,
                    "Unable to destroy Lintouch HTTP Server\n" );
            return 2;
        }
    }
    return 0;
}

void printBanner( const char * progname )
{
    lt_server_logger_print( 0, "%s, Build date: %s\n",
            PACKAGE_STRING, __DATE__ );
}

void printUsage( const char * progname )
{

    printBanner( progname );

    printf( "Usage is:\n\n" );
    printf(
            "%s [-h|--help]\n"
            "    Print these instructions.\n\n",
            progname );
    printf(
            "%s project.ltp [-p|--port port] <options>\n"
            "    To start the Lintouch Server for given ``project.ltp``.\n"
            "    The server port can be specified via ``port`` "
            "(defaults to 5555).\n\n",
            progname );
    printf(
            "%s project.ltp [-l|--local] [-p|--port port] <options>\n"
            "    To start the Lintouch Server for given ``project.ltp``.\n"
            "    Server will listen on loopback interface only, and will\n"
            "    serve the project locally instead of using HTTP.\n"
            "    The server port can be specified via ``port`` "
            "(defaults to 5555).\n\n",
            progname );
    printf(
            "The following <options> can be used:\n"
            "\n"
            "    -q (be quiet)\n"
            "    -v (increase verbosity level by 1, level is 0 by default\n"
            "        you can repeat this option as many times as you wish)\n"
            "    -v num (set verbosity level to ``num``)\n"
            "\n"
            "The following verbosity levels are defined:\n"
            "\n"
            "   0   -   print default messages and all errors\n"
            "   1   -   print project related messages\n"
            "   2   -   print socket related messages\n"
            "   3   -   print threading related messages\n"
            "   4   -   print messages generated by plugins\n"
            "\n"
            );
}

static int parseCommandline( int argc, const char * const * argv )
{
    int i;

    for( i = 1; i < argc; i ++ ) {
        if( ( strcmp( "-h", argv[ i ] ) == 0 ) ||
                ( strcmp( "--help" , argv[ i ] ) == 0 ) ) {
            printUsage( argv[ 0 ] );
            return 0;
        }

        if( ( strcmp( "-l", argv[ i ] ) == 0 ) ||
                ( strcmp( "--local" , argv[ i ] ) == 0 ) ) {
            g_local = 1;
            continue;
        }

        if( ( strcmp( "-q", argv[ i ] ) == 0 ) ) {
            g_level = -1;
            continue;
        }

        if( ( strcmp( "-v", argv[ i ] ) == 0 ) && g_level != -1 ) {
            if ( i + 1 < argc && argv[ i + 1 ][ 0 ] != '-' ) {
                g_level = atoi( argv[ i + 1 ] );
                i ++;
            } else {
                g_level += 1;
            }
            continue;
        }

        if( ( strcmp( "-p", argv[ i ] ) == 0 ) ||
                ( strcmp( "--port" , argv[ i ] ) == 0 ) ) {
            if ( ++i >= argc ) {
                fprintf( stderr,  "ERROR: No port specified\n" );
                printUsage( argv[ 0 ] );
                return 0;
            }
            g_port = atoi( argv[ i ] );
            continue;
        }

        if( argv[ i ][ 0 ] == '-'  ) {
            fprintf( stderr, "ERROR: Invalid argument specified: %s\n",
                    argv[ i ] );
            printUsage( argv[ 0 ] );
            return 0;
        }

        g_project = argv[ i ];
    }

    /* when no project has been specified, we can't continue */
    if( g_project == NULL ) {
        printUsage( argv[ 0 ] );
        return 0;
    }

    return 1;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: main.c 1774 2006-05-31 13:17:16Z hynek $
 */
