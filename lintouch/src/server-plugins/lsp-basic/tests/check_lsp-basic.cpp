// $Id: check_lsp-basic.cpp 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: check_lsp-basic.cpp --
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

#include <stdlib.h>
// custom tests
#include <cppunit/extensions/HelperMacros.h>

#include <apr_strings.h>

#include <lt/cp/cp.h>
#include "lt/server/plugin.h"

apr_pool_t * pool = NULL;

char * srcdir = NULL;
char * builddir = NULL;

class TestPlugin : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE ( TestPlugin );

    CPPUNIT_TEST ( testPluginLoad );

    CPPUNIT_TEST_SUITE_END ();

    protected:

    public:

        void setUp () {
        }

        void tearDown () {
        }

        void testPluginLoad() {
            lt_server_plugin_t * plugin = NULL;
            const char * pluginfile = NULL;

            /* GENERATOR */
#if _WIN32
            pluginfile = apr_pstrcat( pool,
                    builddir, "/.libs/Generator-0.dll", NULL );
#else
            pluginfile = apr_pstrcat( pool,
                    builddir, "/.libs/Generator.so", NULL );
#endif

            plugin = lt_server_plugin_load( pluginfile, NULL, NULL, pool );
            CPPUNIT_ASSERT( plugin != NULL );

            /* LOOPBACK */
#if _WIN32
            pluginfile = apr_pstrcat( pool,
                    builddir, "/.libs/Loopback-0.dll", NULL );
#else
            pluginfile = apr_pstrcat( pool,
                    builddir, "/.libs/Loopback.so", NULL );
#endif

            plugin = lt_server_plugin_load( pluginfile, NULL, NULL, pool );
            CPPUNIT_ASSERT( plugin != NULL );

            apr_pool_clear( pool );
        }

};

CPPUNIT_TEST_SUITE_REGISTRATION ( TestPlugin );
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
// $Id: check_lsp-basic.cpp 1774 2006-05-31 13:17:16Z hynek $
