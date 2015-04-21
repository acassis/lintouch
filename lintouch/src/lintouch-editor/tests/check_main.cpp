// $Id: check_main.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: check_main.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 25 January 2005
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
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <qstring.h>
#include <qapplication.h>

#include <lt/ui/ui.h>

QString srcdir;
QString builddir;

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

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_main.cpp 1169 2005-12-12 15:22:15Z modesto $
