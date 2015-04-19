// $Id: ui.c,v 1.1 2004/01/15 13:11:56 mman Exp $
//
//   FILE: ui.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 15 January 2004
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

#include <stdio.h>

#include <qtranslator.h>
#include <qtextcodec.h>

#include "lt/project/project.h"
#include "lt/ui/ui.h"

#include "config.h"

int lt_ui_initialize(void) {

    // initialize Project library
    if( lt_project_initialize() == -1 ) {
        fprintf( stderr, "Unable to initialize Lintouch Project Library\n" );
        return -1;
    }

    // initialize our own structures

    return 0;
}

void lt_ui_terminate(void) {

    // terminate our own structures

    // terminate Project library
    lt_project_terminate();
}

void loadLocalizations( QApplication & a, 
        const QString & dir, const QString & prefix )
{
    //load localized messages
    //prefer directories in this order
    QStringList paths;
    bool loaded = false;

    //on unix we try predefined locations
#ifdef Q_OS_UNIX
    paths.append( QString( PREFIX_NATIVE ) + "/share/lintouch/locale" );
#endif
#ifdef Q_OS_WIN32
    paths.append( qApp->applicationDirPath() + "\\..\\share\\lintouch\\locale" );
#endif
    paths.append( dir );
    paths.append( "locale" );
    paths.append( "." );

    QTranslator * tr = new QTranslator( &a );

    QStringList::const_iterator it;
    for( it = paths.begin(); it != paths.end(); ++it )
    {
        //qWarning( "Trying to load from: " + (*it) );
        if( tr->load( QString( prefix ) + QTextCodec::locale(),
            *it ) ) {
            loaded = true;
            break;
        }
    }
    if( !loaded ) {
        //qWarning(
        //        QString( "Unable to load localizations for locale %1" )
        //        .arg( QTextCodec::locale() ) );
    } else {
        //qWarning(
        //        QString( "Loaded localizations from %1" )
        //        .arg( *it ) );
        a.installTranslator( tr );
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ui.c,v 1.1 2004/01/15 13:11:56 mman Exp $
