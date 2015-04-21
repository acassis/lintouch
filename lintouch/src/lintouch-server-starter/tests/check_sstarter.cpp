// $Id: check_sstarter.cpp,v 1.36 2004/10/29 19:43:52 modesto Exp $
//
//   FILE: check_sstarter.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 12 July 2004
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

#include <qapplication.h>

#include <lt/ui/ui.h>

QString srcdir;
QString builddir;

class TestRuntime : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestRuntime );

    //CPPUNIT_TEST( testXXX );

    CPPUNIT_TEST_SUITE_END();

    public:

    void setUp()
    {
    }

    void tearDown()
    {
    }

    void testXXX()
    {
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION ( TestRuntime );
// end of custom tests

// standard test suite
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main ( int argc, char ** argv )
{
    // figure out where our sources are and where we can write
    srcdir = getenv( "srcdir" );
    if( srcdir.isEmpty() ) srcdir = ".";
    builddir = getenv( "top_builddir" );
    if( builddir.isEmpty() ) builddir = ".";

    QApplication a ( argc, argv, false );

    if( lt_ui_initialize() == -1 ) {
        qWarning( "Unable to initialize Lintouch UI Library" );
        return -1;
    }
    atexit( lt_ui_terminate );

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
// $Id: check_sstarter.cpp,v 1.36 2004/10/29 19:43:52 modesto Exp $
