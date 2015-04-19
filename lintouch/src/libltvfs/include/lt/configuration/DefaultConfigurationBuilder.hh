// $Id: DefaultConfigurationBuilder.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
//
//   FILE: DefaultConfigurationBuilder.hh -- 
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

#ifndef _DEFAULTCONFIGURATIONBUILDER_HH
#define _DEFAULTCONFIGURATIONBUILDER_HH

#include "lt/configuration/Configuration.hh"

namespace lt {

        /** 
         * A DefaultConfigurationBuilder builds <code>Configuration</code>
         * from XML via SAX parser included with QT XML Module.
         *
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class DefaultConfigurationBuilder
        {

            public:

            /**
             * Construct a builder with optionaly enabled namespace support.
             */
            DefaultConfigurationBuilder( bool enableNamespaces = false );

            /**
             * Build a configuration object from a file using a filename.
             * @param filename name of the file
             * @param error the error message (in case of error, e.g. NULL
             * returned)
             * @param line the line number (in case of error, e.g. NULL
             * returned)
             * @param column the column number (in case of error, e.g. NULL
             * returned)
             * @param makeRO make the returned configuration read only.
             * @return a <code>Configuration</code> object or NULL in case of
             * error
             */
            ConfigurationPtr buildFromFile( const QString & filename, 
                    QString &error, int &line, int &column, 
                    bool makeRO = true );

            /**
             * Build a configuration object from a file using 
             * a QIODevice object.
             * @param dev a QIODevice object
             * @param error the error message (in case of error, e.g. NULL
             * returned)
             * @param line the line number (in case of error, e.g. NULL
             * returned)
             * @param column the column number (in case of error, e.g. NULL
             * returned)
             * @param makeRO make the returned configuration read only.
             * @return a <code>Configuration</code> object or NULL in case of
             * error
             */
            ConfigurationPtr buildFromFile( QIODevice *dev,
                    QString &error, int &line, int &column,
                    bool makeRO = true );

        };

} // namespace lt

#endif /* _DEFAULTCONFIGURATIONBUILDER_HH */

// vim: set et ts=4 sw=4:
// $Id: DefaultConfigurationBuilder.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
