// $Id: VFSLocalizator.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: VFSLocalizator.cpp --
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 14 October 2003
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

#include "lt/templates/VFSLocalizator.hh"

#include <lt/vfs/VFS.hh>
using namespace lt;

#include <lt/configuration/Configuration.hh>
#include <lt/configuration/DefaultConfigurationBuilder.hh>

#include <qtranslator.h>
#include <qfile.h>

// [-- VFSLocalizatorPrivate --]
struct VFSLocalizator::VFSLocalizatorPrivate
{
    VFSLocalizatorPrivate() : m_t( NULL ) {}
    void updateQTranslator();
    void fillTranslatorFrom( QIODevice* d, const QString & rootLocation,
        Logger & logger );

    VFSPtr m_resRoot;
    VFSPtr m_resLanguage;
    VFSPtr m_resFull;

    QTranslator* m_t;
};

void VFSLocalizator::VFSLocalizatorPrivate::updateQTranslator()
{
    if( m_t != NULL ) {
        delete m_t;
        m_t = NULL;
    }
    m_t = new QTranslator( NULL );
    Q_CHECK_PTR( m_t );

    if( ! m_resRoot.isNull() ) {
        fillTranslatorFrom(
                m_resRoot->openFile( "messages.xml", IO_ReadOnly ),
                m_resRoot->rootLocation(), m_resRoot->getLogger() );
    }

    if( ! m_resLanguage.isNull() ) {
        fillTranslatorFrom(
                m_resLanguage->openFile( "messages.xml", IO_ReadOnly ),
                m_resLanguage->rootLocation(), m_resRoot->getLogger() );
    }

    if( ! m_resFull.isNull() ) {
        fillTranslatorFrom( m_resFull->openFile( "messages.xml", IO_ReadOnly ),
                m_resFull->rootLocation(), m_resRoot->getLogger() );
    }
}

void VFSLocalizator::VFSLocalizatorPrivate::fillTranslatorFrom( QIODevice* d,
    const QString & rootLocation, Logger & logger )
{
    Q_CHECK_PTR( m_t );

    if( !d ) {
        return;
    }

    if( !d->isOpen() ) {
        delete d;
        return;
    }

    QString err;
    int line, col;

    DefaultConfigurationBuilder db;
    ConfigurationPtr c = db.buildFromFile( d, err, line, col, true );

    if( !c.isNull() ) {

        ConfigurationList cl = c->getChildren( "message" );
        if( cl.size() != 0 ) {

            for( ConfigurationList::const_iterator it =
                    const_cast<ConfigurationList&>(cl).begin();
                    it != const_cast<ConfigurationList&>(cl).end(); ++it )
            {
                m_t->insert( QTranslatorMessage(
                           (*it)->getAttribute( "context" ),
                           (*it)->getAttribute( "key" ),
                           (*it)->getAttribute( "comment" ),
                           (*it)->getValue() ) );
            }
        }
        else
            logger.warn(QString("No messages in the localization file"
            "(%1messages.xml)")
            .arg(rootLocation));
    }
    else
        logger.warn(QString("The localization file(%1)"
                    " is invalid, possible error at line %2 col %3")
                        .arg(rootLocation).arg(line).arg(col));
    delete d;
}

// [-- VFSLocalizator --]
VFSLocalizator::VFSLocalizator() : Localizator(),
    d( VFSLocalizatorPrivatePtr( new VFSLocalizatorPrivate ) )
{
    Q_CHECK_PTR( d );
}

VFSLocalizator::VFSLocalizator( const QString & locale ) :
    Localizator( locale ),
    d( VFSLocalizatorPrivatePtr( new VFSLocalizatorPrivate ) )
{
    Q_CHECK_PTR( d );
}

VFSLocalizator::VFSLocalizator( const VFSPtr & sourceLocation,
        const QString & locale ) : Localizator( locale ),
    d( VFSLocalizatorPrivatePtr( new VFSLocalizatorPrivate ) )
{
    Q_CHECK_PTR( d );
    setSourceLocation( sourceLocation );
}

void VFSLocalizator::setSourceLocation( const VFSPtr & loc )
{
    Q_CHECK_PTR( d );

    d->m_resRoot.reset();
    d->m_resFull.reset();
    d->m_resLanguage.reset();

    if (!loc->isValid())
    {
        loc->getLogger().debug("No resources found");
        return;
    }
    d->m_resRoot = VFSPtr( loc->subVFS(".",
            loc->getLogger().getChildLogger("resRoot") ) );

    // FIXME: what if we have locale specific messages only
    // Does it make sence to stop here?
    if( !d->m_resRoot->isValid() ) {
        return;
    }

    if( !locale().isEmpty() ) {
        // don't initialize any VFS if we have no locale
        if( locale() != localeLanguage() ) {
            // don't create full locale VFS if we have only language locale
            d->m_resFull = VFSPtr( d->m_resRoot->subVFS( locale(),
                    loc->getLogger().getChildLogger("resFull") ) );
        }
        d->m_resLanguage = VFSPtr( d->m_resRoot->subVFS( localeLanguage(),
                loc->getLogger().getChildLogger("resLanguage") ) );
    }

    d->updateQTranslator();
}

VFSLocalizator::~VFSLocalizator()
{
    if( d && d->m_t ) {
        delete d->m_t;
    }
}

QByteArray VFSLocalizator::resourceAsByteArray( const QString & fn ) const
{
    QIODevice* dev = resourceAsIODevice( fn );

    if( !dev->isOpen() ) {
        delete dev;
        return QByteArray();
    }
    QByteArray result = dev->readAll();
    delete dev;
    return result;
}

QIODevice *VFSLocalizator::resourceAsIODevice( const QString & fn ) const
{
    QIODevice* dev = NULL;
    if( fn.isNull() || fn.isEmpty() ) {
        return new QFile();     // never return NULL
    }

    if( ! d->m_resFull.isNull() ) {
        dev = d->m_resFull->openFile( fn, IO_ReadOnly );
        if( dev ) {
            if( dev->isOpen() ) return dev;
            delete dev;
        }
    }

    if( ! d->m_resLanguage.isNull() ) {
        dev = d->m_resLanguage->openFile( fn, IO_ReadOnly );
        if( dev ) {
            if( dev->isOpen() ) return dev;
            delete dev;
        }
    }

    if( ! d->m_resRoot.isNull() ) {
        dev = d->m_resRoot->openFile( fn, IO_ReadOnly );
        if( dev ) {
            if( dev->isOpen() ) return dev;
            delete dev;
        }
    }

    return new QFile();     // never return NULL
}

QString VFSLocalizator::localizedMessage( const QString & id,
        const QString & context, const QString & comment ) const
{
    if (d->m_t == NULL)
        return id;

    QString s = d->m_t->findMessage( context, id, comment ).translation();

    if( s.isNull() || s.isEmpty() ) {
        s = id;
    }
    return s;
}

const QTranslator* VFSLocalizator::localizedMessages() const
{
    return d->m_t;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFSLocalizator.cpp 1168 2005-12-12 14:48:03Z modesto $
