// $Id: SAXConfigurationHandler.cpp,v 1.2 2003/11/15 18:14:41 hynek Exp $
//
//   FILE: SAXConfigurationHandler.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 27 June 2002
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

#include <qvaluestack.h>

#include "lt/configuration/SAXConfigurationHandler.hh"

using namespace lt;

struct SAXConfigurationHandler::SAXConfigurationHandlerPrivate {
    SAXConfigurationHandlerPrivate () : configuration ( ), locator ( 0 ) {
    };
    ~SAXConfigurationHandlerPrivate() {
    }
    /**
     * The top-level configuration being currently built.
     */
    ConfigurationPtr configuration;

    /**
     * Locator.
     */
    QXmlLocator *locator;

    /**
     * Stack of currently processed elements.
     */
    QValueStack <ConfigurationPtr> elements;

    /**
     * Latest parse exception (in case of error)
     */
    QXmlParseException exception;
};

SAXConfigurationHandler::SAXConfigurationHandler() {
    d = new SAXConfigurationHandlerPrivate ();
    Q_CHECK_PTR ( d );
}

SAXConfigurationHandler::~SAXConfigurationHandler() {
    delete d;
}

SAXConfigurationHandler::SAXConfigurationHandler (
        const SAXConfigurationHandler & other ) : QXmlDefaultHandler ( other ) {
    d = new SAXConfigurationHandlerPrivate ();
    Q_CHECK_PTR ( d );
    *d = *other.d;
}

SAXConfigurationHandler & SAXConfigurationHandler::operator = (
        const SAXConfigurationHandler & other ) {
    *d = *other.d;
    return *this;
}

ConfigurationPtr SAXConfigurationHandler::getBuiltConfiguration() const {
    return d->configuration;
}

void SAXConfigurationHandler::setDocumentLocator ( QXmlLocator * locator ) {
    d->locator = locator;
}

bool SAXConfigurationHandler::startDocument() {
    d->configuration = ConfigurationPtr();
    return true;
}

bool SAXConfigurationHandler::endDocument() {
    return true;
}

bool SAXConfigurationHandler::startPrefixMapping ( 
        const QString & /* prefix */, 
        const QString & /* uri */ ) {
    return true;
}

bool SAXConfigurationHandler::endPrefixMapping ( 
        const QString & /* prefix */ ) {
    return true;
}

bool SAXConfigurationHandler::startElement ( const QString & namespaceURI, 
        const QString & localName, const QString & qName, 
        const QXmlAttributes & atts ) {
    int idx = qName.find ( ':' );
    QString prefix = "";
    if ( idx > 0 ) {
        prefix = qName.left ( idx );
    }
    ConfigurationPtr c(new DefaultConfiguration( localName, 
            getLocationString(), namespaceURI, prefix ));

    // if processing the toplevel item simply push it, otherwise link it
    // with the parent
    if( d->configuration.isNull() ) {
        d->configuration = c;
    } else {
        ConfigurationPtr parent = d->elements.top();
        parent->addChild( c );
    }

    // process attributes
    for( int i = 0; i < atts.length(); i ++ ) {
        c->setAttribute( atts.localName( i ), atts.value( i ) );
    }

    // push currently built configuration to the stack
    d->elements.push( c );

    return true;
}

bool SAXConfigurationHandler::endElement ( 
        const QString & /* namespaceURI */, 
        const QString & /* localName */, const QString & /* qName */ ) {

    // remove processed element from the stack
    d->elements.pop();

    return true;
}

bool SAXConfigurationHandler::characters ( const QString & ch ) {
    //qDebug( "got characters: `%s'", ch.latin1() );
    ConfigurationPtr c = d->elements.top();
    c->setValue( c->getValue() + ch );
    return true;
}

bool SAXConfigurationHandler::ignorableWhitespace ( const QString & /* ch */ ) {
    //qDebug( "got ignorableWhitespace: `%s'", ch.latin1() );
    // FIXME: if ignore whitespace then just skip, otherwise append to
    // current configuration's value
    return true;
}

bool SAXConfigurationHandler::warning( 
        const QXmlParseException & exception ) {
    //qWarning( "SAXConfigurationHandler::warning at line %d, column %d", 
    //        exception.lineNumber(), exception.columnNumber() );
    d->exception = exception;
    return true;
}

bool SAXConfigurationHandler::error( 
        const QXmlParseException & exception ) {
    //qWarning( "SAXConfigurationHandler::error at line %d, column %d", 
    //        exception.lineNumber(), exception.columnNumber() );
    d->exception = exception;
    return false;
}

bool SAXConfigurationHandler::fatalError( 
        const QXmlParseException & exception ) {
    //qWarning( "SAXConfigurationHandler::fatalError at line %d, column %d", 
    //        exception.lineNumber(), exception.columnNumber() );
    d->exception = exception;
    return false;
}

QXmlParseException SAXConfigurationHandler::getParseException() const {
    return d->exception;
}

QString SAXConfigurationHandler::getLocationString() const {
    if( d->locator != NULL ) {
        return QString( "line %1, column %2" ).arg (
                d->locator->lineNumber() ).arg( d->locator->columnNumber() );
    }
    return "unknown";
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: SAXConfigurationHandler.cpp,v 1.2 2003/11/15 18:14:41 hynek Exp $
