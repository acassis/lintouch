// $Id: PluginHelper.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: PluginHelper.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 20 October 2003
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

#include <qstringlist.h>
#include <qmap.h>

#include "lt/templates/PluginHelper.hh"

bool verify_lt_plugin( const char * classnamestr,
        const char * versioninfo ) {

    // convert given char * into map by spliting at "," and then at "="
    QStringList vars = QStringList::split( ",", versioninfo );
    QMap <QString, QString> map;
    for( QStringList::const_iterator it =
            const_cast<QStringList&>(vars).begin();
            it != const_cast<QStringList&>(vars).end(); ++it )
    {
        QStringList l = QStringList::split( "=", *it );
        map.insert( l [ 0 ], l [ 1 ] );
    }

    // check exported interface
    if( map [ "interface" ] != classnamestr ) {
        qWarning( QString( "Error when loading plugin"
                    ", plugin implements interface '%1'"
                    ", you wanted '%2'" )
                .arg( map [ "interface" ] )
                .arg( classnamestr ) );
        return false;
    }

    // check qt version
    if( map [ "qt_version" ] != QT_VERSION_STR ) {
        qWarning( QString( "Error when loading plugin"
                    ", plugin was compiled with qt version '%1'"
                    ", your application is compiled with qt version "
                    "'"QT_VERSION_STR"'" )
                .arg( map [ "qt_version" ] ) );
        return false;
    }

    // check lttemplates major only
    if( map [ "lttemplates_major" ] != LT_TEMPLATES_MAJOR_STR ) {
        qWarning( QString( "Error when loading plugin"
                    ", plugin was compiled with lttemplates version "
                    "'%1.%2.%3'"
                    ", your application is compiled with lttemplates version "
                    "'"
                    LT_TEMPLATES_MAJOR_STR"."
                    LT_TEMPLATES_MINOR_STR"."
                    LT_TEMPLATES_PATCH_STR"'"
                    )
                .arg( map [ "lttemplates_major" ] )
                .arg( map [ "lttemplates_minor" ] )
                .arg( map [ "lttemplates_patchlevel" ] ) );

        return false;
    }
    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: PluginHelper.cpp 1168 2005-12-12 14:48:03Z modesto $
