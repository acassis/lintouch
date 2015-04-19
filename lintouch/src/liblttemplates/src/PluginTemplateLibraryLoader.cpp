// $Id: PluginTemplateLibraryLoader.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: PluginTemplateLibraryLoader.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 09 October 2003
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
#include <qdir.h>
#include <qlibrary.h>
#include <qtextcodec.h>

#include "lt/templates/PluginHelper.hh"
#include "lt/templates/PluginTemplateLibraryLoader.hh"

#include "config.h"

PluginTemplateLibraryLoader::PluginTemplateLibraryLoader() {
}

PluginTemplateLibraryLoader::~PluginTemplateLibraryLoader() {
}

TemplateLibrary * PluginTemplateLibraryLoader::loadFrom(
        const QString & filename, const QString & translationPrefix,
        LoggerPtr logger /* = Logger::nullPtr() */ ) {

    typedef TemplateLibrary *( * create_func_t )();
    typedef const char *( * version_func_t )();

    bool valid = false;

    create_func_t create = NULL;
    version_func_t version = NULL;

    QLibrary lib( filename );
    lib.setAutoUnload( false );

    create =( create_func_t ) lib.resolve( "create" );
    version =( version_func_t ) lib.resolve( "lt_version_info" );

    if( ! lib.isLoaded() )
    {
        logger->debug( QString( "Error when loading plugin"
                    ", plugin %1 can not be loaded "
                    "due to dynamic linking problems").arg(filename) );
    }
    else
    {
        if( version )
        {
            valid = verify_lt_plugin(
                    "lt::TemplateLibrary", version() );
            if( !valid )
            {
                logger->error( QString( "Error when loading plugin"
                            ", plugin %1 does not implement"
                            " lt::TemplateLibrary" )
                        .arg(filename) );
            }
        }
        else
        {
            logger->error( QString( "Error when loading plugin"
                    ", plugin %1 does not provide lt_version_info" )
                    .arg(filename) );
        }
    }

    TemplateLibrary *result = NULL;
    if( create && valid ) {
        logger->debug( QString( "Loaded plugin %1" ).arg( filename ) );

        if( ! translationPrefix.isEmpty() ) {
            //now try to load translations

            //prefer directories in this order
            QStringList paths;
            //on unix we try predefined locations
#ifdef Q_OS_UNIX
            paths.append( QString( PREFIX_NATIVE ) + "/share/lintouch/locale" );
            //I'll leave these two commented for now, we will decide whether
            //to support them in the future
            //paths.append( "/usr/local/share/lintouch/locale" );
            //paths.append( "/usr/share/lintouch/locale" );
#endif
#ifdef Q_OS_WIN32
            paths.append( qApp->applicationDirPath() +
                    "\\..\\share\\lintouch\\locale" );
#endif
            paths.append( "locale" );
            paths.append( "." );

            QStringList::const_iterator it;
            bool found = false;
            for( it = paths.begin(); it != paths.end(); ++it )
            {
                QTranslator * tr = new QTranslator( qApp );
                Q_CHECK_PTR( tr );
                if( tr->load( translationPrefix + "_" + QTextCodec::locale(),
                            (*it) ) ) {
                    logger->info( QString(
                                "Loaded translations for %1 from %2 "
                                "(%3 locale)")
                            .arg( filename ).arg( *it )
                            .arg( QTextCodec::locale() ) );
                    qApp->installTranslator( tr );
                    found = true;
                    break;
                } else {
                    delete tr;
                }
            }
            if( ! found ) {
                logger->warn( QString(
                            "Unable to find translations for %1. (prefix %2) "
                            "(%3 locale)")
                        .arg( filename ).arg( translationPrefix )
                        .arg( QTextCodec::locale() ) );
            }
        } else {
            logger->debug(
                    QString( "No translation file specified for %1" )
                    .arg( filename ) );
        }
        // finally create the library
        result = create();
    }

    if (result == NULL)
        result = new TemplateLibrary;
    else
        result->validate();

    result->setFilename(QDir().absFilePath(filename));

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: PluginTemplateLibraryLoader.cpp 1168 2005-12-12 14:48:03Z modesto $
