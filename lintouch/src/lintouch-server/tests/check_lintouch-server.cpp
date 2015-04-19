// $Id: check_lintouch-server.cpp 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: check_lintouch-server.cpp --
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 16 June 2003
//
// Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
//
// THIS SOFTWARE IS DISTRIBUTED AS IS AND WITHOUT ANY WARRANTY.
//
// This application is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License, or (at
// your option) any later version.
//
// This application is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this application; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

// custom tests
#include <cppunit/extensions/HelperMacros.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <apr_strings.h>
#include <apr_time.h>

#include <lt/cp/cp.h>
#include "lt/server/project.h"
#include "lt/server/logger.h"
#include "src/ziparchive.h"

apr_pool_t * pool = NULL;

char * srcdir = NULL;
char * builddir = NULL;

class TestServer : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE ( TestServer );

    CPPUNIT_TEST ( testZipExtract );
    CPPUNIT_TEST ( testProjectParser );
    CPPUNIT_TEST ( testLogger );
    //CPPUNIT_TEST ( testXXX );

    CPPUNIT_TEST_SUITE_END ();

    protected:

    public:

        void setUp () {
        }

        void tearDown () {
        }

        void testZipExtract() {
            apr_file_t * extr = NULL;
            const char * zip = apr_pstrcat( pool,
                    srcdir, "/tests/data/archive.zip", NULL );

            extr = lt_server_zip_extract_file( zip, "file.txt", pool );

            CPPUNIT_ASSERT( extr != NULL );

            {
                char buffer[ 1024 ] = "";
                apr_size_t len = 1023;
                apr_file_read( extr, buffer, &len );
                //printf( "read: %d bytes: %s\n", len, buffer );
                CPPUNIT_ASSERT( apr_strnatcmp(
                            "This is file.txt", buffer ) == 0 );
            }

            apr_file_close( extr );

            extr = lt_server_zip_extract_file( zip, "nested/file.txt", pool );

            CPPUNIT_ASSERT( extr != NULL );

            {
                char buffer[ 1024 ] = "";
                apr_size_t len = 1023;
                apr_file_read( extr, buffer, &len );
                //printf( "read: %d bytes: %s\n", len, buffer );
                CPPUNIT_ASSERT( apr_strnatcmp(
                            "This is nested/file.txt", buffer ) == 0 );
            }

            apr_file_close( extr );

            apr_pool_clear( pool );
        }

        void testProjectParser() {
            lt_project_t * proj = NULL;
            apr_hash_t * h = NULL;
            apr_hash_t * l = NULL;
            const char * zip = apr_pstrcat( pool,
                    srcdir, "/projects/server-tester.ltp", NULL );

            CPPUNIT_ASSERT( LT_PROJECT_SUCCESS == 
                    lt_project_parse( &proj, zip, LT_PROJECT_ALL, pool ) );

            CPPUNIT_ASSERT( apr_strnatcmp( "Martin Man <mman@swac.cz>",
                        lt_project_author_get( proj ) ) == 0 );
            CPPUNIT_ASSERT( apr_strnatcmp( "1.0",
                        lt_project_version_get( proj ) ) == 0 );
            CPPUNIT_ASSERT( apr_strnatcmp( "2005-04-18",
                        lt_project_date_get( proj ) ) == 0 );
            CPPUNIT_ASSERT( apr_strnatcmp( "Lintouch Server Tester",
                        lt_project_shortdesc_get( proj ) ) == 0 );
            CPPUNIT_ASSERT( apr_strnatcmp( "This is just a testing project.",
                        lt_project_longdesc_get( proj ) ) == 0 );

            CPPUNIT_ASSERT(
                    ( h = lt_project_connections_get( proj ) ) != NULL );
            CPPUNIT_ASSERT( apr_hash_count( h ) == 2 );

            CPPUNIT_ASSERT(
                    ( l = lt_project_logging_get( proj ) ) != NULL );
            CPPUNIT_ASSERT( apr_hash_count( l ) == 2 );

            {
                lt_project_connection_t * conn = NULL;
                apr_hash_t * h1 = NULL;
                lt_var_set_t * vset = NULL;
                lt_var_t * var = NULL;

                /* generator connection */
                conn = (lt_project_connection_t*)
                    apr_hash_get( h, "generator", APR_HASH_KEY_STRING );
                CPPUNIT_ASSERT( conn != NULL );
                CPPUNIT_ASSERT( apr_strnatcmp( "Generator",
                        lt_project_connection_type_get( conn ) ) == 0 );
                vset = lt_project_connection_variables_get( conn );
                CPPUNIT_ASSERT( vset != NULL );

                h1 = lt_var_set_variables_get( vset );
                CPPUNIT_ASSERT( var = (lt_var_t*)
                        apr_hash_get( h1, "bit", APR_HASH_KEY_STRING ) );
                CPPUNIT_ASSERT( lt_var_type_get( var ) == LT_IO_TYPE_BIT );
                CPPUNIT_ASSERT( var = (lt_var_t*)
                        apr_hash_get( h1, "number", APR_HASH_KEY_STRING ) );
                CPPUNIT_ASSERT( lt_var_type_get( var ) == LT_IO_TYPE_NUMBER );
                CPPUNIT_ASSERT( var = (lt_var_t*)
                        apr_hash_get( h1, "string", APR_HASH_KEY_STRING ) );
                CPPUNIT_ASSERT( lt_var_type_get( var ) == LT_IO_TYPE_STRING );

                h1 = lt_project_connection_properties_get( conn );
                CPPUNIT_ASSERT( h1 != NULL );
                CPPUNIT_ASSERT( apr_hash_count( h1 ) == 1 );
                CPPUNIT_ASSERT( apr_strnatcmp(
                            "0.1", (const char*)apr_hash_get(
                                h1, "refresh", APR_HASH_KEY_STRING ) ) == 0 );

                /* loopback connection */
                conn = (lt_project_connection_t*)
                    apr_hash_get( h, "loopback", APR_HASH_KEY_STRING );
                CPPUNIT_ASSERT( conn != NULL );
                CPPUNIT_ASSERT( apr_strnatcmp( "Loopback",
                        lt_project_connection_type_get( conn ) ) == 0 );
                vset = lt_project_connection_variables_get( conn );
                CPPUNIT_ASSERT( vset != NULL );

                h1 = lt_var_set_variables_get( vset );
                CPPUNIT_ASSERT( var = (lt_var_t*)
                        apr_hash_get( h1, "bit", APR_HASH_KEY_STRING ) );
                CPPUNIT_ASSERT( lt_var_type_get( var ) == LT_IO_TYPE_BIT );
                CPPUNIT_ASSERT( var = (lt_var_t*)
                        apr_hash_get( h1, "number", APR_HASH_KEY_STRING ) );
                CPPUNIT_ASSERT( lt_var_type_get( var ) == LT_IO_TYPE_NUMBER );
                CPPUNIT_ASSERT( var = (lt_var_t*)
                        apr_hash_get( h1, "string", APR_HASH_KEY_STRING ) );
                CPPUNIT_ASSERT( lt_var_type_get( var ) == LT_IO_TYPE_STRING );

                h1 = lt_project_connection_properties_get( conn );
                CPPUNIT_ASSERT( h1 != NULL );
                CPPUNIT_ASSERT( apr_hash_count( h1 ) == 1 );
                CPPUNIT_ASSERT( apr_strnatcmp(
                            "0", (const char*)apr_hash_get(
                                h1, "latency", APR_HASH_KEY_STRING ) ) == 0 );
            }

            {
                lt_project_logging_t * log = NULL;
                apr_hash_t * h1 = NULL;

                /* console logging */
                log = (lt_project_logging_t*)
                    apr_hash_get( l, "Console", APR_HASH_KEY_STRING );
                CPPUNIT_ASSERT( log != NULL );
                CPPUNIT_ASSERT( apr_strnatcmp( "Console",
                        lt_project_logging_type_get( log ) ) == 0 );

                h1 = lt_project_logging_properties_get( log );
                CPPUNIT_ASSERT( h1 != NULL );
                CPPUNIT_ASSERT( apr_hash_count( h1 ) == 0 );

                /* File logging */
                log = (lt_project_logging_t*)
                    apr_hash_get( l, "File", APR_HASH_KEY_STRING );
                CPPUNIT_ASSERT( log != NULL );
                CPPUNIT_ASSERT( apr_strnatcmp( "File",
                        lt_project_logging_type_get( log ) ) == 0 );

                h1 = lt_project_logging_properties_get( log );
                CPPUNIT_ASSERT( h1 != NULL );
                CPPUNIT_ASSERT( apr_hash_count( h1 ) == 1 );
                CPPUNIT_ASSERT( apr_strnatcmp(
                            "/tmp/server.log", (const char*)apr_hash_get(
                                h1, "file-name", APR_HASH_KEY_STRING ) ) == 0 );
            }

            apr_pool_clear( pool );
        }

        void testLogger() {
            lt_server_logger_priority_set( 0 );
            lt_server_logger_print( 0, "You should see this message\n" );
            lt_server_logger_print( 1, "You should NOT see this message\n" );

            lt_server_logger_priority_set( 1 );
            lt_server_logger_print( 1, "You should see this message\n" );
            lt_server_logger_print( 2, "You should NOT see this message\n" );

            lt_server_logger_priority_set( 2 );
            lt_server_logger_print( 2, "You should see this message\n" );
            lt_server_logger_print( 3, "You should NOT see this message\n" );
        }

};

CPPUNIT_TEST_SUITE_REGISTRATION ( TestServer );
// end of custom tests

// standard test suite
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main ( int argc, const char * const * argv, const char * const * env )
{
    if ( APR_SUCCESS !=
                apr_app_initialize ( &argc, &argv, &env ) ) {
        fprintf ( stderr, "Unable to initialize APR Library\n" );
        return -1;
    }
    atexit ( apr_terminate );

    if ( lt_cp_initialize () != LT_CP_SUCCESS ) {
        fprintf ( stderr, "Unable to initialize LT CP Library\n" );
        return -1;
    }
    atexit ( lt_cp_terminate );

    apr_pool_create ( &pool, NULL );

    // figure out where our sources are and where we can write
    srcdir = getenv( "srcdir" );
    if( srcdir == NULL ) srcdir = ".";
    builddir = getenv( "top_builddir" );
    if( builddir == NULL ) builddir = ".";

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry =
        CppUnit::TestFactoryRegistry::getRegistry ();
    runner.addTest ( registry.makeTest () );

    bool wasThereError = runner.run ( "", false );

    // shell expects 0 if successfull
    return !wasThereError;
}
// end of standard test suite

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_lintouch-server.cpp 1774 2006-05-31 13:17:16Z hynek $
