// $Id: SAXConfigurationHandler.hh,v 1.2 2003/11/15 18:14:40 hynek Exp $
//
//   FILE: SAXConfigurationHandler.hh -- 
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

#ifndef _SAXCONFIGURATIONHANDLER_HH
#define _SAXCONFIGURATIONHANDLER_HH

#include <qxml.h>

#include "lt/configuration/SAXConfigurationHandler.hh"
#include "lt/configuration/DefaultConfiguration.hh"

namespace lt {

        /**
         * SAX handler that builds configuration from a XML file. This
         * handler can be set as a QXmlContentHandler to some form of
         * XQmlReader and after successfully calling QXmlReader::parse the
         * built configuration can be obtained via getBuiltConfiguration
         * method. 
         *
         * Note that built configuration is not freed automatically
         * but rather left simply flying in the space and is up to the user
         * to free it after it won't be needed, since handler can't know
         * when the user is finished with it.
         *
         * @author Martin Man <mman@swac.cz>
         */
        class SAXConfigurationHandler : public QXmlDefaultHandler
        {
            public:

                /**
                 * Constructor.
                 */
                SAXConfigurationHandler(); 

                /**
                 * Destructor.
                 */
                virtual ~SAXConfigurationHandler();

                /**
                 * Copy Constructor.
                 */
                SAXConfigurationHandler (
                        const SAXConfigurationHandler & other );

                /**
                 * Assignment operator.
                 */
                SAXConfigurationHandler & operator = (
                        const SAXConfigurationHandler & other );
    
                /**
                 * Return the built configuration.
                 */
                ConfigurationPtr getBuiltConfiguration() const;

                /**
                 * Return the latest parsing exception.
                 */
                QXmlParseException getParseException() const;

                /**
                 * handler method
                 */
                virtual void setDocumentLocator ( QXmlLocator * locator );

                /**
                 * handler method
                 */
                virtual bool startDocument();

                /**
                 * handler method
                 */
                virtual bool endDocument();
                
                /**
                 * handler method
                 */
                virtual bool startPrefixMapping ( const QString & prefix, 
                        const QString & uri );

                /**
                 * handler method
                 */
                virtual bool endPrefixMapping ( const QString & prefix );

                /**
                 * handler method
                 */
                virtual bool startElement ( const QString & namespaceURI, 
                        const QString & localName, const QString & qName, 
                        const QXmlAttributes & atts );

                /**
                 * handler method
                 */
                virtual bool endElement ( const QString & namespaceURI, 
                        const QString & localName, const QString & qName );

                /**
                 * handler method
                 */
                virtual bool characters ( const QString & ch );

                /**
                 * handler method
                 */
                virtual bool ignorableWhitespace ( const QString & ch );

                /**
                 * handler method
                 */
                virtual bool warning( const QXmlParseException & exception );

                /**
                 * handler method
                 */
                virtual bool error( const QXmlParseException & exception );
                
                /**
                 * handler method
                 */
                virtual bool fatalError( const QXmlParseException & exception );

            protected:

                /**
                 * return nicely formated string of current location.
                 */
                QString getLocationString() const;

            private:

                struct SAXConfigurationHandlerPrivate;
                SAXConfigurationHandlerPrivate *d;

        };

} // namespace lt

#endif /* _SAXCONFIGURATIONHANDLER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: SAXConfigurationHandler.hh,v 1.2 2003/11/15 18:14:40 hynek Exp $
