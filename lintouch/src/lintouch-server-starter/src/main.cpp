// $Id: main.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: main.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 1 February 2005
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

#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include "lt/ui/ui.h"

#include "MainWindow.hh"

#include "config.h"

void printBanner( const char * progname );
void printUsage( const char * progname );

int main( int argc, char ** argv )
{
    //create qt application
    QApplication a( argc, argv );
    loadLocalizations( a, "src/lintouch-server-starter/locale",
            "server-starter_" );

    a.connect( &a, SIGNAL( lastWindowClosed() ), SLOT( quit() ) );

    printBanner( argv[0] );

    if( lt_ui_initialize() == -1 ) {
        qWarning( "Unable to initialize Lintouch Panel Library" );
        return -1;
    }
    atexit( lt_ui_terminate );

    // parse commandline
    QString project;
    int port = 0;
    bool portGiven = false;
    for( int i = 1; i < argc; i ++ ) {
        if( ( QString( "-h" ) == argv[ i ] ) ||
                ( QString( "--help" ) == argv[ i ] ) ) {
            printUsage( argv[0] );
            exit( 1 );
        }

        if( ( QString( "-p" ) == argv[ i ] ) ||
                ( QString( "--port" ) == argv[ i ] ) ) {
            if ( ++i >= argc ) {
                qWarning ( "ERROR: No port specified" );
                printUsage( argv[ 0 ] );
                exit( 1 );
            }
            port = atoi( argv[ i ] );
            portGiven = true;
            continue;
        }

        project = argv[ i ];
    }

    //create main window
    MainWindow mw(NULL);

    if( portGiven ) {
        mw.setPort( port );
    }
    mw.setProject( project );

    a.setMainWidget( &mw );

    mw.show();

    return a.exec();
}

void printBanner( const char * /* progname */ ) {
    qWarning( QString( PACKAGE_STRING ", %1 %2" )
            .arg("Build date:").arg( __DATE__ ));
}

void printUsage( const char * progname ) {
    qWarning( "Usage is:\n" );
    qWarning(
            "%s [-h|--help]\n"
            "    Print these instructions.\n",
            progname );
    qWarning(
            "%s <options> project.ltp [-p|--port port]\n"
            "    To start the Lintouch Server with given ``project.ltp``.\n"
            "    The server port can be specified via ``port`` "
            "(defaults to 5555).\n",
            progname );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: main.cpp 1168 2005-12-12 14:48:03Z modesto $
