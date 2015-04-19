// $Id: Parameters.cpp,v 1.4 2004/08/12 10:19:13 hynek Exp $
//
//   FILE: Parameters.cpp -- 
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
#include <qregexp.h>

#include "lt/parameters/Parameters.hh"

using namespace lt;

struct Parameters::ParametersPrivate {
    ParametersPrivate ( bool ro ) : readOnly ( ro ) {};
    bool readOnly;
    QMap <QString, QString> params;
};

Parameters::Parameters () {
    d = new ParametersPrivate ( false );
    Q_CHECK_PTR ( d );
}

Parameters::~Parameters () {
    delete d;
}

Parameters::Parameters (
        const Parameters & other ) {
    d = new ParametersPrivate ( false );
    Q_CHECK_PTR ( d );
    *d = *other.d;
}

Parameters & Parameters::operator = (
        const Parameters & other ) {
    *d = *other.d;
    return *this;
}

const QString Parameters::setParameter( const QString & name, 
        const QString & value ) {
    QString empty;

    if( ! isWriteable() ) return empty;

    if( name.isEmpty() ) return empty;

    QString p = d->params[ name ];

    if( value.isEmpty() ) {
        d->params.remove( name );
        return p;
    }

    d->params[ name ] = value;
    return p;
}

void Parameters::removeParameter( const QString & name ) {
    if( ! isWriteable() ) return;
    d->params.remove ( name );
}

const QStringList Parameters::getParameterNames() const {
    QStringList sl;
    QMapConstIterator <QString, QString> i =
        const_cast<QMap<QString,QString> &>(d->params).begin();

    for( ; i != const_cast<QMap<QString,QString> &>(d->params).end(); ++i ) {
        sl.append( i.key() );
    }
    return sl;
}

bool Parameters::isParameter( const QString & name ) const {
    return d->params.contains( name );
}

const QString Parameters::getParameter( const QString & name, 
        const QString & defaultValue /*= ""*/) const {
    if( d->params.contains( name )) {
        return d->params[ name ];
    }
    return defaultValue;
}

int Parameters::getParameterAsInt( const QString & name, 
        int defaultValue /*= 0*/ ) const {
    if( d->params.contains( name )) {
        return (int) parseLong( d->params[ name ] );
    }
    return defaultValue;
}

unsigned int Parameters::getParameterAsUInt( const QString & name, 
        unsigned int defaultValue /*= 0*/) const {
    if( d->params.contains( name )) {
        return (unsigned int) parseULong( d->params[ name ] );
    }
    return defaultValue;
}

long Parameters::getParameterAsLong( const QString & name, 
        long defaultValue /*= 0*/) const {
    if( d->params.contains( name )) {
        return parseLong( d->params[ name ] );
    }
    return defaultValue;
}

unsigned long Parameters::getParameterAsULong( const QString & name, 
        unsigned long defaultValue /*= 0*/) const {
    if( d->params.contains( name )) {
        return parseULong( d->params[ name ] );
    }
    return defaultValue;
}

float Parameters::getParameterAsFloat( const QString & name, 
        float defaultValue /*= 0.0*/) const {
    if( d->params.contains( name )) {
        return d->params[ name ].toFloat();
    }
    return defaultValue;
}

bool Parameters::getParameterAsBool( const QString & name, 
        bool defaultValue /*= false*/ ) const {
    if( d->params.contains( name )) {
        QString p = d->params[ name ].lower();
        if( p == "yes" || p == "1" || p == "true" ) {
            return true;
        }
        return false;
    }
    return defaultValue;
}

void Parameters::merge( const ParametersPtr other ) {
    if( other.isNull() ) return;
    
    const QStringList names = other->getParameterNames();

    QStringList::const_iterator i = const_cast<QStringList&>(names).begin();

    for( ; i != const_cast<QStringList&>(names).end(); ++i ) {
        d->params[ *i ] = other->getParameter( *i );
    }

    return;
}

ParametersPtr Parameters::fromConfiguration( 
        const ConfigurationPtr configuration,
        const QString & elementName /* = "parameter" */ ) {

    if( configuration.isNull() ) return ParametersPtr();

    const ConfigurationList parameters = 
        configuration->getChildren( elementName );

    QString p, v;
    ParametersPtr ret(new Parameters());
    ConfigurationList::const_iterator it; 
    for(it = const_cast<ConfigurationList&>(parameters).begin();
            it != const_cast<ConfigurationList&>(parameters).end (); it ++ ) {
        const ConfigurationPtr &x = *it;
        Q_ASSERT( x );
        p = x->getAttribute( "name" );
        v = x->getAttribute( "value" );

        if( !p.isEmpty () && !v.isEmpty ()) {
            ret->setParameter (p, v);
        }
    }
    return ret;
}


void Parameters::makeReadOnly() {
    d->readOnly = true;
}

bool Parameters::isWriteable() const {
    return d->readOnly == false;
}

long Parameters::parseLong ( const QString & value ) const {
    // hex number
    if( value.find( QRegExp( "^0x[0-9a-fA-F]+" )) != -1 ) {
        return value.mid( 2 ).toLong( NULL, 16 );
    } else if( value.find( QRegExp( "^0o[0-7]+" )) != -1 ) {
        return value.mid( 2 ).toLong( NULL, 8 );
    } else if( value.find( QRegExp( "^0b[0-1]+" )) != -1 ) {
        return value.mid( 2 ).toLong( NULL, 2 );
    } else {
        return value.toLong();
    }
    return 0;
}

unsigned long Parameters::parseULong ( const QString & value ) const {
    if( value.find( QRegExp( "^0x[0-9a-fA-F]+" )) != -1 ) {
        return value.mid( 2 ).toULong( NULL, 16 );
    } else if( value.find( QRegExp( "^0o[0-7]+" )) != -1 ) {
        return value.mid( 2 ).toULong( NULL, 8 );
    } else if( value.find( QRegExp( "^0b[0-1]+" )) != -1 ) {
        return value.mid( 2 ).toULong( NULL, 2 );
    } else {
        return value.toULong();
    }
    return 0;
}

// vim: set et ts=4 sw=4:
// $Id: Parameters.cpp,v 1.4 2004/08/12 10:19:13 hynek Exp $
