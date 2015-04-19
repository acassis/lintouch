// $Id: main.cpp 1287 2006-01-16 09:46:11Z modesto $
//
//   FILE: main.cpp -- 
// AUTHOR: Martin Man <mman@swac.cz>
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

#include <qapplication.h>
#include <qtextcodec.h>

#include "config.h"
#include "MainWindow.hh"

#include <lt/ui/ui.h>
#include <lt/logger/FileLogger.hh>

/**
 * Will try to find file with given name in the dir by stripping locale
 * specific parts if required. example:
 *
 * name = runtime_cs_CZ.UTF-8
 * dir = /usr/share/lintouch/help
 * suffix = .html
 *
 * the following files will be tried and first one found will be returned.
 *
 * /usr/share/lintouch/help/editor_cs_CZ.UTF-8.html
 * /usr/share/lintouch/help/editor_cs_CZ.html
 * /usr/share/lintouch/help/editor_cs.html
 * /usr/share/lintouch/help/editor.html
 *
 */
QString findHelpFile( const QString & name, const QString & suffix,
        const QString & dir );


void printBanner( const char * progname );
void printUsage( const char * progname );

/**
 * main
 */
int main( int argc, char *argv[] )
{
    printBanner(argv[0]);

    QApplication a( argc, argv );
    loadLocalizations( a, "src/lintouch-editor/locale", "editor_" );

    if( lt_ui_initialize() == -1 ) {
        qWarning( "Unable to initialize Lintouch UI Library" );
        return -1;
    }
    atexit( lt_ui_terminate );

    QString debug_fn;
    QStringList projects;

    for( int i = 1; i < argc; i ++ ) {
        if( ( QString( "-h" ) == argv[ i ] ) ||
                ( QString( "--help" ) == argv[ i ] ) ) {
            printUsage( argv[0] );
            exit( 1 );
        }

        if( QString( "--debug" ) == argv[ i ] ) {
            if ( ++i >= argc ) {
                qWarning ( "ERROR: No filename specified for --debug" );
            } else {
                debug_fn = argv[ i ];
            }
            continue;
        }

        projects << argv[i];
    }

    LoggerPtr l;
    if( debug_fn.isEmpty() ) {
        l = LoggerPtr( new lt::Logger );
    } else {
        l = LoggerPtr( new lt::FileLogger(debug_fn, "",
                    lt::FileLogger::LEVEL_DEBUG) );
    }

    QStringList paths;
    QStringList::Iterator it;
    bool loaded = false;

    //load localized help
    //prefer directories in this order
    //on unix we try predefined locations
#ifdef Q_OS_UNIX
    paths.append( QString( PREFIX ) + "/share/lintouch/help/editor" );
#else
    paths.append( qApp->applicationDirPath() +
            "\\..\\share\\lintouch\\help\\editor" );
#endif
    paths.append( "src/lintouch-editor/help" );
    paths.append( "help" );
    paths.append( "." );

    QString helpfile;
    for( it = paths.begin(); it != paths.end(); ++it )
    {
        helpfile = findHelpFile( QString( "editor_" ) + QTextCodec::locale(),
                ".html", *it );
        if( ! helpfile.isEmpty() ) {
            loaded = true;
            break;
        }
    }

    //create main window
    MainWindow* mw = new MainWindow( helpfile, projects, l );
    Q_CHECK_PTR( mw );

    a.setMainWidget( mw );
    mw->show();
    return a.exec();
}

QString findHelpFile( const QString & name, const QString & suffix,
        const QString & dir ) {

    QString delims = "_.";
    QString path = dir + "/" + name;

    path = QDir::cleanDirPath( path );

    for( ;; ) {
        //qWarning( "trying " + path + suffix );
        if( QFile::exists( path + suffix ) ) {
            return path + suffix;
        }
        int index = -1;
        int rightmost = 0;
        for( unsigned i = 0; i < delims.length(); i ++ ) {
            index = path.findRev( delims.at( i ) );
            if( index != -1 ) {
                if( index > rightmost ) rightmost = index;
            }
        }
        if( rightmost == 0 ) {
            return QString::null;
        } else {
            path.truncate( rightmost );
        }
    }
}

void printBanner( const char * /* progname */ ) {
    qWarning( QString( PACKAGE_STRING ", %1 %2" )
            .arg("Build date:").arg( __DATE__ ));
}

void printUsage( const char * progname ) {
    qWarning( "Usage is:" );
    qWarning(
            "%s <options>\n"
            "    To start the Lintouch Editor.\n",
            progname );
    qWarning(
            "%s [-h|--help]\n"
            "    Print these instructions.\n",
            progname );
    qWarning(
            "%s <options> project.ltp\n"
            "    To start the Lintouch Editor and automatically open\n"
            "    the given ``project.ltp``.\n",
            progname );
    qWarning(
            "The following <options> can be used:\n"
            "    --debug <filename>"
            );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: main.cpp 1287 2006-01-16 09:46:11Z modesto $
