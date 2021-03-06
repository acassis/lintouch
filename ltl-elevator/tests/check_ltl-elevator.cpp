// $Id: check_ltl-elevator.cpp 950 2005-07-14 11:26:30Z modesto $
//
//   FILE: check_ltl-elevator.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 15 January 2003
//
// Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
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

#include <qapplication.h>

#include "lt/templates/templates.h"
#include "lt/templates/PluginTemplateLibraryLoader.hh"

#include "lt/logger/ConsoleLogger.hh"
using namespace lt;

QString srcdir;
QString builddir;

class TestTL : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE( TestTL );

    CPPUNIT_TEST( test );

    CPPUNIT_TEST_SUITE_END();

    public:

        void setUp() {
            // fill in
        }

        void tearDown() {
            // fill in
        }

        void test() {
            LoggerPtr l( new ConsoleLogger() );
            TemplateLibrary * lib =
                PluginTemplateLibraryLoader::loadFrom(
                        "build/ltl-elevator", QString::null, l );

            CPPUNIT_ASSERT( lib != NULL );
            CPPUNIT_ASSERT( lib->isValid() );
        }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestTL );
// end of custom tests

// standard test suite
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main( int argc, char ** argv )
{
    // figure out where our sources are and where we can write
    srcdir = getenv( "srcdir" );
    if( srcdir.isEmpty() ) srcdir = ".";
    builddir = getenv( "top_builddir" );
    if( builddir.isEmpty() ) builddir = ".";

    if( lt_templates_initialize() == -1 ) {
        fprintf( stderr, "Unable to initialize Lintouch Templates Library\n" );
        return -1;
    }
    atexit( lt_templates_terminate );

    QApplication a( argc, argv, false );

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = 
        CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );

    bool wasThereError = runner.run( "", false );

    // shell expects 0 if successfull
    return !wasThereError;
}
// end of standard test suite

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_ltl-elevator.cpp 950 2005-07-14 11:26:30Z modesto $
