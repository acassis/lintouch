// $Id: AbstractConfiguration.cpp,v 1.2 2004/08/12 10:19:13 hynek Exp $
//
//   FILE: AbstractConfiguration.cpp -- 
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

#include "lt/configuration/AbstractConfiguration.hh"
#include "lt/configuration/DefaultConfiguration.hh"

#include <qregexp.h>

using namespace lt;

int AbstractConfiguration::getValueAsInt( int defaultValue /* = 0 */ ) const {
    QString v = getValue();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return (int) parseLong( v );
}

unsigned int AbstractConfiguration::getValueAsUInt( 
        unsigned int defaultValue /* = 0 */ ) const {
    QString v = getValue();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return (unsigned int) parseULong( v );
}

long AbstractConfiguration::getValueAsLong( 
        long defaultValue /* = 0 */ ) const {
    QString v = getValue();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return (long) parseLong( v );
}

unsigned long AbstractConfiguration::getValueAsULong( 
        unsigned long defaultValue /* = 0 */ ) const {
    QString v = getValue();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return (unsigned long) parseULong( v );
}

float AbstractConfiguration::getValueAsFloat( 
        float defaultValue /* = 0.0 */ ) const {
    QString v = getValue();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return v.toFloat();
}

bool AbstractConfiguration::getValueAsBool( 
        bool defaultValue /* = false */ ) const {
    QString v = getValue().lower();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return parseBool( v );
}

int AbstractConfiguration::getAttributeAsInt( const QString & name, 
        int defaultValue /* = 0*/ ) const {
    QString v = getAttribute( name ).lower();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return (int) parseLong( v );
}

long AbstractConfiguration::getAttributeAsLong( const QString & name, 
        long defaultValue /* = 0 */ ) const {
    QString v = getAttribute( name ).lower();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return (long) parseLong( v );
}

float AbstractConfiguration::getAttributeAsFloat( const QString & name, 
        float defaultValue /* = 0.0 */ ) const {
    QString v = getAttribute( name ).lower();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return v.toFloat();
}

bool AbstractConfiguration::getAttributeAsBool( const QString & name, 
        bool defaultValue /* = false */ ) const {
    QString v = getAttribute( name ).lower();
    if( v.isEmpty() ) {
        return defaultValue;
    }
    return parseBool( v );
}

const ConfigurationPtr AbstractConfiguration::getChild( const QString & child, 
        bool createNew /* = true */ ) const {
    ConfigurationList children = getChildren( child );
    ConfigurationPtr ret; // NULL configuration
    if( children.count () > 0 ) {
        ret = children[ 0 ];
    }

    if( createNew ) {
        ret = ConfigurationPtr(new DefaultConfiguration( child, "-" ));
    }

    return ret;
}

long AbstractConfiguration::parseLong ( const QString & value ) const {
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

unsigned long AbstractConfiguration::parseULong ( 
        const QString & value ) const {
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

bool AbstractConfiguration::parseBool ( const QString & value ) const {
    if( value == "yes" || value == "1" || value == "true" ) {
        return true;
    }
    return false;
}

// vim: set et ts=4 sw=4:
// $Id: AbstractConfiguration.cpp,v 1.2 2004/08/12 10:19:13 hynek Exp $
