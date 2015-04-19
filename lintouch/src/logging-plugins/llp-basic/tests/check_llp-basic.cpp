// $Id: check_llp-basic.cpp 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: check_llp-basic.cpp --
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 14 March 2006
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

#include <apr_strings.h>

#include <lt/cp/cp.h>
#include <lt/server/logging.h>

apr_pool_t * pool = NULL;

char * srcdir = NULL;
char * builddir = NULL;

class TestPlugin : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE ( TestPlugin );

    CPPUNIT_TEST ( testPluginLoad );
    CPPUNIT_TEST ( testFilePluginLoad );

    CPPUNIT_TEST_SUITE_END ();

    protected:

    public:

        void setUp () {
        }

        void tearDown () {
        }

        void testPluginLoad() {
            lt_logging_plugin_t * plugin = NULL;
            const char * pluginfile = NULL;

            /* CONSOLE */
#if _WIN32
            pluginfile = apr_pstrcat( pool,
                    builddir, "/.libs/Console-0.dll", NULL );
#else
            pluginfile = apr_pstrcat( pool,
                    builddir, "/.libs/Console.so", NULL );
#endif

            plugin = lt_logging_plugin_load( pluginfile, NULL, pool );
            CPPUNIT_ASSERT( plugin != NULL );

            CPPUNIT_ASSERT( apr_strnatcmp(
                        lt_logging_plugin_shortdesc_get(plugin),
                        "Console") == 0);
            CPPUNIT_ASSERT( apr_strnatcmp(
                        lt_logging_plugin_author_get(plugin),
                        "Patrik Modesto <modesto@swac.cz>") == 0);

            apr_pool_clear( pool );
        }

        void testFilePluginLoad() {
            lt_logging_plugin_t * plugin = NULL;
            const char * pluginfile = NULL;

            /* CONSOLE */
#if _WIN32
            pluginfile = apr_pstrcat( pool,
                    builddir, "/.libs/File-0.dll", NULL );
#else
            pluginfile = apr_pstrcat( pool,
                    builddir, "/.libs/File.so", NULL );
#endif

            plugin = lt_logging_plugin_load( pluginfile, NULL, pool );
            CPPUNIT_ASSERT( plugin != NULL );

            CPPUNIT_ASSERT( apr_strnatcmp(
                        lt_logging_plugin_shortdesc_get(plugin),
                        "File") == 0);
            CPPUNIT_ASSERT( apr_strnatcmp(
                        lt_logging_plugin_author_get(plugin),
                        "Patrik Modesto <modesto@swac.cz>") == 0);

            apr_hash_t * props =
                lt_logging_plugin_default_properties_get(plugin);
            CPPUNIT_ASSERT(props != NULL);

            char * pv = (char*)apr_hash_get(props,
                    "file-name", APR_HASH_KEY_STRING);
            CPPUNIT_ASSERT(pv != NULL);

            CPPUNIT_ASSERT( apr_strnatcmp("/tmp/server.log", pv) == 0 );

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
// $Id: check_llp-basic.cpp 1774 2006-05-31 13:17:16Z hynek $
