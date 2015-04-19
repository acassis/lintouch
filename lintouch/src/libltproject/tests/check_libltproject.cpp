// $Id: check_libltproject.cpp,v 1.6 2004/09/14 13:08:19 mman Exp $
//
//   FILE: check_libltproject.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 06 November 2003
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

#include "check_testproject.hh"

QString srcdir;
QString builddir;

QApplication *g_app = NULL;

CPPUNIT_TEST_SUITE_REGISTRATION ( TestProject );
// end of custom tests

// standard test suite
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "lt/project/project.h"

int main ( int argc, char ** argv )
{
    // figure out where our sources are and where we can write
    srcdir = getenv( "srcdir" );
    if( srcdir.isEmpty() ) srcdir = ".";
    builddir = getenv( "top_builddir" );
    if( builddir.isEmpty() ) builddir = ".";

    if ( lt_project_initialize() == -1 ) {
        fprintf ( stderr, "Unable to initialize Lintouch Project Library\n" );
        return -1;
    }
    atexit ( lt_project_terminate );

    QApplication a ( argc, argv, false );
    g_app = &a;

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
// $Id: check_libltproject.cpp,v 1.6 2004/09/14 13:08:19 mman Exp $

