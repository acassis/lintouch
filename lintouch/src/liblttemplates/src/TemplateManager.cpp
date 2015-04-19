// $Id: TemplateManager.cpp 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: TemplateManager.cpp --
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

#include "lt/templates/TemplateManager.hh"
using namespace lt;

#include "config.h"

#include <qapplication.h>
#include <qdir.h>

struct TemplateManager::TemplateManagerPrivate
{

    TemplateManagerPrivate()
    {
        tlibraries.setAutoDelete( true );
    }

    ~TemplateManagerPrivate()
    {
        tlibraries.clear();
    }

    // template libraries contained within this manager
    TemplateLibraryDict tlibraries;

};

TemplateManager::TemplateManager() :
    d( new TemplateManagerPrivate() )
{
}

TemplateManager::TemplateManager( const TemplateManager & p ) :
    d( p.d )
{
}

TemplateManager & TemplateManager::operator=( const TemplateManager & p )
{
    d = p.d;
    return *this;
}

TemplateManager::~TemplateManager()
{
}

QStringList TemplateManager::TemplateManager::templateLibraryDirs()
{
    static QStringList defaultDirs;
    static bool initialized = false;

    if( !initialized ) {

        QDir appDir = qApp->applicationDirPath();
        QDir curDir = QDir::current();
        QDir prefixDir = QDir( PREFIX_NATIVE );
        QString s;

#ifdef QT_DEBUG
        qWarning( "APP DIR is: " + appDir.absPath() );
        qWarning( "CUR DIR is: " + curDir.absPath() );
        qWarning( "PREFIX DIR is: " + prefixDir.absPath() );
#endif

        // mytemplatelibrary being loaded uninstalled during the testcases
        // from the source tree
        s = QDir::cleanDirPath( curDir.filePath(
                    "../liblttemplates/build" ) );
#ifdef QT_DEBUG
        qWarning( "CLEANED PATH: " + s );
#endif
        defaultDirs.append( s );
        // ltl-display being loaded uninstalled during the testcases
        // from the source tree
        s = QDir::cleanDirPath( curDir.filePath(
                    "../template-libraries/ltl-display/build" ) );
#ifdef QT_DEBUG
        qWarning( "CLEANED PATH: " + s );
#endif
        defaultDirs.append( s );
        // ltl-input being loaded uninstalled during the testcases
        // from the source tree
        s = QDir::cleanDirPath( curDir.filePath(
                        "../template-libraries/ltl-input/build" ) );
#ifdef QT_DEBUG
        qWarning( "CLEANED PATH: " + s );
#endif
        defaultDirs.append( s );

        // any library being loaded during runtime from system installation
        // directory
        s = QDir::cleanDirPath( prefixDir.filePath(
                    "lib/lintouch/template-libraries" ) );
#ifdef QT_DEBUG
        qWarning( "CLEANED PATH: " + s );
#endif
        defaultDirs.append( s );

        // any library being loaded during runtime from system installation
        // directory
        s = QDir::cleanDirPath( appDir.filePath(
                    "../lib/lintouch/template-libraries" ) );
#ifdef QT_DEBUG
        qWarning( "CLEANED PATH: " + s );
#endif
        defaultDirs.append( s );

        initialized = true;
    }

    return defaultDirs;
}

bool TemplateManager::registerTemplateLibrary( const QString & hint,
        const TemplateLibrary * tmpl )
{
#ifdef QT_DEBUG
    qWarning( "TM::registerTemplateLibrary( hint = '" + hint + "' )" );
#endif

    if( hint.isEmpty() || d->tlibraries.contains( hint ) ) {
        return false;
    }

    if( tmpl == NULL ) {
        return false;
    }

    d->tlibraries.insert( hint, tmpl );
    return true;
}

bool TemplateManager::unregisterTemplateLibrary( const QString & hint )
{
#ifdef QT_DEBUG
    qWarning( "TM::unregisterTemplateLibrary( hint = '" + hint + "' )" );
#endif

    if( d->tlibraries.contains( hint ) ) {
        d->tlibraries.remove( hint );
        return true;
    }

    return false;
}

void TemplateManager::unregisterAllTemplateLibraries()
{
    d->tlibraries.clear();
}

const TemplateLibraryDict & TemplateManager::templateLibraries() const
{
    return d->tlibraries;
}

Template * TemplateManager::instantiate( const QString & type,
        LoggerPtr logger /* = Logger::nulPtr() */,
        const QString & hint ) const
{
#ifdef QT_DEBUG
    qWarning( "TM::instantiate( type = '" + type +
            "', hint = '" + hint + "' )" );
#endif

    // the hint was given, try to search within the appropriate library
    if( ! hint.isNull() )  {
        // do we have the library and does it contain the given type ???
        if( d->tlibraries.contains( hint ) ) {
            TemplateLibrary * tl = d->tlibraries [ hint ];
            // skip invalid (not loaded) and unavailable template libraries
            if( tl && tl->isValid() && tl->isAvailable()
                    && tl->templates().contains( type ) ) {
                Template *result = tl->instantiate( type );
                if (result != NULL)
                    result->setLibrary(hint);
                return result;
            }
        }
        return NULL;
    }

    // no hint was given, search all libraries
    for(unsigned i = 0; i < d->tlibraries.count(); ++i) {

        if( d->tlibraries[i]->templates().contains( type ) ) {
            // skip invalid (not loaded) and unavailable template
            // libraries
            if( ! d->tlibraries[i]->isValid() ||
                    ! d->tlibraries[i]->isAvailable() ) {
                continue;
            }
            // try to instanitate given type
            Template *result = d->tlibraries[i]->instantiate( type, logger );
            if (result != NULL)
            {
                logger->info(QString("The unqualified template %1 has been "
                    " found in the %2 library").arg(type)
                    .arg(d->tlibraries.keyAt(i)));
                result->setLibrary(d->tlibraries.keyAt(i));
            }
            return result;
        }
    }

    return NULL;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateManager.cpp 1774 2006-05-31 13:17:16Z hynek $
