// $Id: DefaultConfiguration.cpp,v 1.3 2004/05/18 11:03:29 mman Exp $
//
//   FILE: DefaultConfiguration.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 19 May 2002
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

#include <qmap.h>

#include "lt/configuration/DefaultConfiguration.hh"

using namespace lt;

#define LOCAL_DEBUG 0

struct DefaultConfiguration::DefaultConfigurationPrivate {
    DefaultConfigurationPrivate () {
    };
    DefaultConfigurationPrivate (
            const QString & n, const QString & l,
            const QString & nss, const QString & pr,
            bool ro ) : name ( n ), location ( l ), 
                ns ( nss ), prefix ( pr ), readOnly ( ro ) {
    };
    QString name;
    QString location;
    QString ns;
    QString prefix;

    QString value;
    ConfigurationList children;
    QMap <QString, QString> attributes;
    bool readOnly;
};

DefaultConfiguration::DefaultConfiguration( const QString & name, 
        const QString & location /* = "" */, const QString & ns /* = "" */,
        const QString & prefix /* = "" */ ) {
    d = new DefaultConfigurationPrivate ( name, location, ns, prefix, false );
}

DefaultConfiguration::~DefaultConfiguration() {
    delete d;
}

DefaultConfiguration::DefaultConfiguration (
        const DefaultConfiguration & other ) : AbstractConfiguration ( other ) {
    d = new DefaultConfigurationPrivate ();
    Q_CHECK_PTR ( d );
    *d = *other.d;
}

DefaultConfiguration & DefaultConfiguration::operator = (
        const DefaultConfiguration & other ) {
    *d = *other.d;
    return *this;
}
QString DefaultConfiguration::getName() const {
    return d->name;
}

QString DefaultConfiguration::getLocation() const {
    return d->location;
}

QString DefaultConfiguration::getNamespace() const {
    return d->ns;
}

QString DefaultConfiguration::getPrefix() const {
    return d->prefix;
}

ConfigurationList DefaultConfiguration::getChildren( 
        const QString & name /* = "" */ ) const {
    ConfigurationList pv;

    ConfigurationList::const_iterator it; 
    for(it = const_cast<ConfigurationList&>(d->children).begin();
            it != const_cast<ConfigurationList&>(d->children).end (); it ++ ) {
        const ConfigurationPtr &x = *it;
        Q_ASSERT( x );
        if( name.isEmpty () || x->getName() == name ) {
            pv.append( x );
        }
    }
    return pv;
}

const ConfigurationPtr DefaultConfiguration::getChild( const QString & child, 
        bool createNew /* = false */ ) const {

    if( createNew ) {
        ConfigurationPtr c(new DefaultConfiguration( child, "-" ));
        Q_ASSERT ( c );
        d->children.append ( c );
        return c;
    }

    ConfigurationList::const_iterator it; 
    for(it = const_cast<ConfigurationList&>(d->children).begin();
            it != const_cast<ConfigurationList&>(d->children).end (); it ++ ) {
        const ConfigurationPtr &x = *it;
        Q_ASSERT( x );
        if( x->getName() == child ) {
            return x;
        }
    }

    return ConfigurationPtr();
}

const QStringList DefaultConfiguration::getAttributeNames() const {
    QStringList sl;
    QMapConstIterator <QString, QString> i =
        const_cast<QMap<QString,QString> &>(d->attributes).begin();
    for( ; i !=
            const_cast<QMap<QString,QString> &>(d->attributes).end(); ++i ) {
        sl.append( i.key() );
    }
    return sl;
}

QString DefaultConfiguration::getValue( 
        const QString & defaultValue /* = "" */ ) const {
    if( !d->value.isNull() ) {
        return d->value;
    }
    return defaultValue;
}

QString DefaultConfiguration::getAttribute( const QString & name, 
        const QString & defaultValue /* = "" */ ) const {
    if( d->attributes.contains( name ) ) {
        return d->attributes[ name ];
    }
    return defaultValue;
}

bool DefaultConfiguration::addChild( ConfigurationPtr configuration ) {
    if( configuration.isNull() ) return false;
    if( !isWriteable() ) return false;
#if LOCAL_DEBUG
    qDebug( "Adding child: %s", configuration->getName().latin1() );
#endif
    d->children.append(configuration);
    return true;
}

bool DefaultConfiguration::removeChild( const ConfigurationPtr configuration ) {
    if( configuration.isNull() ) return false;
    if( !isWriteable() ) return false;
#if LOCAL_DEBUG
    qDebug( "Removing child: %s", configuration->getName().latin1() );
#endif
    d->children.remove( configuration );
    return true;
}

bool DefaultConfiguration::setAttribute( const QString & name, 
        const QString & value ) {
    if( !isWriteable() ) return false;
#if LOCAL_DEBUG
    qDebug( "Setting attribute: %s -> %s", name.latin1(), value.latin1() );
#endif
    d->attributes[ name ] = value;
    return true;
}

bool DefaultConfiguration::setValue( const QString & value ) {
    if( !isWriteable() ) return false;
#if LOCAL_DEBUG
    qDebug( "Setting value: %s", value.latin1() );
#endif
    d->value = value;
    return true;
}

void DefaultConfiguration::makeReadOnly() {
    d->readOnly = true;
    ConfigurationList::const_iterator it; 
    for(it = const_cast<ConfigurationList&>(d->children).begin();
            it != const_cast<ConfigurationList&>(d->children).end (); it ++ ) {
        const ConfigurationPtr &x = *it;
        Q_ASSERT ( x );
        x -> makeReadOnly ();
    }
}

bool DefaultConfiguration::isWriteable() const {
    return d->readOnly != true;
}

bool DefaultConfiguration::setLocation ( const QString & location ) {
    if ( ! isWriteable () ) return false;
    d->location = location;
    return true;
}

bool DefaultConfiguration::setPrefix ( const QString & prefix ) {
    if ( ! isWriteable () ) return false;
    d->prefix = prefix;
    return true;
}

bool DefaultConfiguration::setNamespace ( const QString & ns ) {
    if ( ! isWriteable () ) return false;
    d->ns = ns;
    return true;
}

bool DefaultConfiguration::setName ( const QString & name ) {
    if ( ! isWriteable () ) return false;
    d->name = name;
    return true;
}

// vim: set et ts=4 sw=4:
// $Id: DefaultConfiguration.cpp,v 1.3 2004/05/18 11:03:29 mman Exp $
