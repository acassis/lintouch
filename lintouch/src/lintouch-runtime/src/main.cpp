// $Id: main.cpp 1277 2006-01-11 12:49:34Z modesto $
//
//   FILE: main.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 10 October 2003
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

#include <qdir.h>
#include <qfile.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include "lt/cp/cp.h"
#include "lt/ui/ui.h"

#include "config.h"
#include "Application.hh"
#include "MainWindow.hh"

#include <lt/logger/FileLogger.hh>

void printBanner( const char * progname );
void printUsage( const char * progname );

int main( int argc, char ** argv )
{
    //create qt application
    Application a( argc,( char **) argv );
    loadLocalizations( a, "src/lintouch-runtime/locale", "runtime_" );

    a.connect( &a, SIGNAL( lastWindowClosed() ), SLOT( quit() ) );

    printBanner( argv[0] );

    if( lt_ui_initialize() == -1 ) {
        qWarning( "Unable to initialize Lintouch Panel Library" );
        return -1;
    }
    atexit( lt_ui_terminate );

    if( lt_cp_initialize() == -1 ) {
        qWarning( "Unable to initialize "
                  "Lintouch Communication Protocol Library" );
        return -1;
    }
    atexit( lt_cp_terminate );

    //parse commandline args and remember them so they can be later passed
    // 0 - false, 1 - true, 2 - unspecified = project dependent
    // host/port/project related
    bool startServer = 1;
    QString host_or_project;
    int hostPortSelection = 2;
    int projectSelection = 2;
    // view/panel selection related
    int viewSelection = 2;
    int panelSelection = 2;
    bool servicePanelShortcut = true;
    // windows size related
    bool fullscreen = true;
    bool showFullScreenShortcut = true;
    // view related
    QString view;
    // port related
    int port = 5555;
    bool portGiven = false;
    // debug
    QString debug_fn;   // empty means no-debug

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

        if( ( QString( "-v" ) == argv[ i ] ) ||
                ( QString( "--view" ) == argv[ i ] ) ) {
            if ( ++i >= argc ) {
                qWarning ( "ERROR: No view specified" );
                printUsage( argv[ 0 ] );
                exit( 1 );
            }
            view = argv[ i ];
            continue;
        }

        if( ( QString( "-f" ) == argv[ i ] ) ||
                ( QString( "--fullscreen" ) == argv[ i ] ) ) {
            fullscreen = true;
            continue;
        }

        if( ( QString( "-n" ) == argv[ i ] ) ||
                ( QString( "--normal" ) == argv[ i ] ) ) {
            fullscreen = false;
            continue;
        }

        if( QString( "--debug" ) == argv[ i ] ) {
            if ( ++i >= argc ) {
                qWarning ( "ERROR: No filename specified" );
                printUsage( argv[ 0 ] );
                exit( 1 );
            }
            debug_fn = argv[ i ];
            continue;
        }

        if( ( QString( "--enable-host-port-selection" ) == argv[ i ] ) ) {
            hostPortSelection = 1;
            continue;
        }
        if( ( QString( "--disable-host-port-selection" ) == argv[ i ] ) ) {
            hostPortSelection = 0;
            continue;
        }
        if( ( QString( "--enable-project-selection" ) == argv[ i ] ) ) {
            projectSelection = 1;
            continue;
        }
        if( ( QString( "--disable-project-selection" ) == argv[ i ] ) ) {
            projectSelection = 0;
            continue;
        }
        if( ( QString( "--enable-view-switching" ) == argv[ i ] ) ) {
            viewSelection = 1;
            continue;
        }
        if( ( QString( "--disable-view-switching" ) == argv[ i ] ) ) {
            viewSelection = 0;
            continue;
        }
        if( ( QString( "--enable-panel-switching" ) == argv[ i ] ) ) {
            panelSelection = 1;
            continue;
        }
        if( ( QString( "--disable-panel-switching" ) == argv[ i ] ) ) {
            panelSelection = 0;
            continue;
        }
        if( ( QString( "--enable-service-panel-shortcut" ) == argv[ i ] ) ) {
            servicePanelShortcut = true;
            continue;
        }
        if( ( QString( "--disable-service-panel-shortcut" ) == argv[ i ] ) ) {
            servicePanelShortcut = false;
            continue;
        }
        if( ( QString( "--enable-fullscreen-shortcut" ) == argv[ i ] ) ) {
            showFullScreenShortcut = true;
            continue;
        }
        if( ( QString( "--disable-fullscreen-shortcut" ) == argv[ i ] ) ) {
            showFullScreenShortcut = false;
            continue;
        }

        if( QString( argv[ i ] ).startsWith( "-" ) ) {
            qWarning( "ERROR: Invalid argument specified: %s",
                    argv[ i ] );
            printUsage( argv[ 0 ] );
            exit( 1 );
        }

        host_or_project = argv[ i ];
    }

    // validate commandline args

    // problematic combinations
    if( hostPortSelection == 1 && projectSelection == 1 ) {
        qWarning( "ERROR: --enable-host-port-selection and "
                "--enable-project-selection"
                " can not be specified at the same time" );
        printUsage( argv[ 0 ] );
        exit( 1 );
    }
    if( hostPortSelection == 0 && projectSelection == 0 ) {
        qWarning( "ERROR: --disable-host-port-selection and "
                "--disable-project-selection"
                " can not be specified at the same time" );
        printUsage( argv[ 0 ] );
        exit( 1 );
    }

    // no special --enable given, try to determine mode according to given
    // host_or_project
    if( hostPortSelection == 2 && projectSelection == 2 ) {
        if( host_or_project.isEmpty() || QFile::exists( host_or_project ) ) {
            projectSelection = 1;
        } else {
            hostPortSelection = 1;
        }
    }

    // handle non-specified arguments and their defaults
    if( hostPortSelection == 0 ) {
        switch( projectSelection ) {
            case 1:
            case 2:
                startServer = true;
                projectSelection = 1;
                break;
        }
    } else if( hostPortSelection == 1 ) {
        switch( projectSelection ) {
            case 0:
            case 2:
                startServer = false;
                projectSelection = 0;
                break;
        }
    } else if( hostPortSelection == 2 ) {
        switch( projectSelection ) {
            case 0:
                startServer = false;
                hostPortSelection = 1;
                break;
            case 1:
            case 2:
                startServer = true;
                projectSelection = 1;
                hostPortSelection = 0;
                break;
        }
    }

    // when no port has been specified and we are supposed to start local
    // server, we'll use an unused port
    // generated for us by ServerController
    if( startServer && ! portGiven ) port = 0;

    // print summary
    if( !host_or_project.isEmpty() ) {
        if( startServer ) {
            host_or_project = QFileInfo( host_or_project ).absFilePath();
            qWarning( QString( "Starting server %1:%2 with project %3" )
                    .arg( "localhost" ).arg( port ).arg( host_or_project ) );
        } else {
            qWarning( QString( "Using server %1:%2" )
                    .arg( host_or_project ).arg( port ) );
        }
    }
    if( !view.isEmpty() ) {
        qWarning( QString( "Using view %1" ).arg( view ) );
    }

    LoggerPtr l;
    if( debug_fn.isEmpty() ) {
        l = LoggerPtr( new lt::Logger );
    } else {
        l = LoggerPtr( new lt::FileLogger(debug_fn, "",
                    lt::FileLogger::LEVEL_DEBUG) );
    }

    // create main window
    MainWindow mw( startServer, fullscreen, l, NULL );

    // pass in commmand line defaults
    if( startServer ) {
        mw.setProjectNPort( host_or_project, port, host_or_project.isEmpty() );
    } else {
        mw.setHostNPort( host_or_project, port, host_or_project.isEmpty() );
    }
    mw.setViewSelectorEnabled( viewSelection );
    mw.setPanelSelectorEnabled( panelSelection );
    mw.setView( view );
    mw.setServicePanelShortcutEnabled( servicePanelShortcut );
    mw.setShowFullScreenEnabled( showFullScreenShortcut );

    // and go ahead with startup
    a.setMainWidget( &mw );

    if( fullscreen ) {
        mw.showFullScreen();
    } else {
        mw.show();
    }

    return a.exec();
}

void printBanner( const char * /* progname */ ) {
    qWarning( QString( PACKAGE_STRING ", %1 %2" )
            .arg("Build date:").arg( __DATE__ ));
}

void printUsage( const char * progname ) {
    qWarning( "Usage is:\n" );
    qWarning(
            "%s <options>\n"
            "    To start the Lintouch Runtime and offer manual connect.\n",
            progname );
    qWarning(
            "%s [-h|--help]\n"
            "    Print these instructions.\n",
            progname );
    qWarning(
            "%s <options> host [-p|--port port] [-v|--view view]\n"
            "    To start the Lintouch Runtime and automatically connect\n"
            "    to the given ``host`` and ``port`` (defaults to 5555).\n"
            "    When the ``view`` is given it will be "
            "automatically selected.\n",
            progname );
    qWarning(
            "%s <options> project.ltp [-p|--port port] [-v|--view view]\n"
            "    To start the Lintouch Runtime and automatically connect\n"
            "    to the server started locally for given ``project.ltp``.\n"
            "    The server port can be specified via ``port`` "
            "(defaults to random).\n"
            "    When the ``view`` is given it will be "
            "automatically selected.\n",
            progname );
    qWarning(
            "The following <options> can be used:\n"
            "\n"
            "    -f|--fullscreen (default)\n"
            "    -n|--normal\n"
            "\n"
            "    --enable-host-port-selection (default)\n"
            "    --disable-host-port-selection\n"
            "\n"
            "    --enable-project-selection\n"
            "    --disable-project-selection (default)\n"
            "\n"
            "    --enable-view-switching "
            "(default for project with multiple views)\n"
            "    --disable-view-switching\n"
            "\n"
            "    --enable-panel-switching "
            "(default for project with multiple panels)\n"
            "    --disable-panel-switching\n"
            "\n"
            "    --enable-service-panel-shortcut (default)"
            "    --disable-service-panel-shortcut\n"
            "\n"
            "    --enable-fullscreen-shortcut (default)"
            "    --disable-fullscreen-shortcut\n"
            "\n"
            "    --debug <filename>"
            );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: main.cpp 1277 2006-01-11 12:49:34Z modesto $
