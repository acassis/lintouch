// $Id: DefaultConfigurationBuilder.cpp,v 1.1 2003/11/11 15:32:42 mman Exp $
//
//   FILE: DefaultConfigurationBuilder.cpp -- 
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

#include "lt/configuration/DefaultConfigurationBuilder.hh"
#include "lt/configuration/SAXConfigurationHandler.hh"

#include <qxml.h>

using namespace lt;

DefaultConfigurationBuilder::DefaultConfigurationBuilder( 
        bool /* enableNamespaces = false */ ) {

}

ConfigurationPtr DefaultConfigurationBuilder::buildFromFile( 
        const QString & filename,
        QString &error, int &line, int &column,
        bool makeRO /* = true */ ) {
    QFile f( filename );
    return buildFromFile( &f, error, line, column, makeRO );
}

ConfigurationPtr DefaultConfigurationBuilder::buildFromFile( QIODevice *dev,
        QString &error, int &line, int &column,
        bool makeRO /* = true */ ) {

    QXmlInputSource xmlsource( dev );

    QXmlSimpleReader xmlreader;
    SAXConfigurationHandler h;

    xmlreader.setContentHandler( &h );
    xmlreader.setErrorHandler( &h );

    if( xmlreader.parse( &xmlsource, FALSE ) ) {
        ConfigurationPtr c =  h.getBuiltConfiguration();
        if ( makeRO && c ) c->makeReadOnly ();
        return c;
    } else {
        QXmlParseException e = h.getParseException();
        error = e.message ();
        line = e.lineNumber();
        column = e.columnNumber();
    }
    return ConfigurationPtr();
}
            
// vim: set et ts=4 sw=4:
// $Id: DefaultConfigurationBuilder.cpp,v 1.1 2003/11/11 15:32:42 mman Exp $
