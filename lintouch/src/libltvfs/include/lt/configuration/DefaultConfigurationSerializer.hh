// $Id: DefaultConfigurationSerializer.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
//
//   FILE: DefaultConfigurationSerializer.hh -- 
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

#ifndef _DEFAULTCONFIGURATIONSERIALIZER_HH
#define _DEFAULTCONFIGURATIONSERIALIZER_HH

#include "lt/configuration/Configuration.hh"

namespace lt {

        /** 
         * A DefaultConfigurationSerializer serializes 
         * <code>Configuration</code>
         * to XML by recursively outputting respective XML declaration for
         * given Configuration.
         *
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class DefaultConfigurationSerializer
        {

            public:

            /**
             * Construct a serializer with optionaly enabled namespace support.
             */
            DefaultConfigurationSerializer( bool enableNamespaces = false, 
                    unsigned indent = 4 );

            /**
             * Destructor
             */
            virtual ~DefaultConfigurationSerializer ();

            /**
             * Copy Constructor.
             */
            DefaultConfigurationSerializer (
                    const DefaultConfigurationSerializer & other );

            /**
             * Assignment operator.
             */
            DefaultConfigurationSerializer & operator = (
                    const DefaultConfigurationSerializer & other );
    
            /**
             * Serialize a configuration to a file using a filename.
             * @param filename name of the file
             * @param configuration configuration object
             * @return <code>true</code> when succesfull, false otherwise.
             */
            bool serializeToFile( const QString & filename, 
                    const ConfigurationPtr configuration );

            /**
             * Serialize a configuration to a file using 
             * a QIODevice object.
             * @param dev a QIODevice object
             * @param configuration configuration object
             * @return <code>true</code> when succesfull, false otherwise.
             */
            bool serializeToFile( QIODevice *dev, 
                    const ConfigurationPtr configuration );

            /**
             * Serialize a configuration to a stream using 
             * a QTextStream object.
             * @param stream a QTextStream object
             * @param configuration configuration object
             * @return <code>true</code> when succesfull, false otherwise.
             */
            bool serializeToStream( QTextStream *stream, 
                    const ConfigurationPtr configuration );

            protected:

            /**
             * serialize xml header to the file.
             */
            bool serializePreambule ( QTextStream *st );

            /**
             * Serialize one node to a file and recursively invoke itself for
             * each child configuration node.
             */
            bool serializeNode ( QTextStream *st, const ConfigurationPtr node,
                    unsigned indent );

            /**
             * encode given string replacing <>&"' with respective &XXX;
             * entities.
             */
            QString escape ( const QString & str );

            private:

            struct DefaultConfigurationSerializerPrivate;
            DefaultConfigurationSerializerPrivate *d;
        };

} // namespace lt

#endif /* _DEFAULTCONFIGURATIONSERIALIZER_HH */

// vim: set et ts=4 sw=4:
// $Id: DefaultConfigurationSerializer.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
