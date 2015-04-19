// $Id: ServerLogging.hh 1556 2006-04-04 12:38:17Z modesto $
//
//   FILE: ServerLogging.hh --
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: Mon, 03 Apr 2006
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

#ifndef _LOGGING_HH
#define _LOGGING_HH

#include <qmap.h>
#include <qstring.h>

#include <lt/templates/Property.hh> // for PropertyDict

#include <lt/LTDict.hh>

namespace lt {

    class ServerLogging;

    typedef LTDict<ServerLogging> ServerLoggingDict;

    /**
     * ServerLogging.
     *
     *
     * @author Patrik Modesto <modesto@swac.cz>
     */
    class ServerLogging
    {

        public:

            /**
             * The meaningful constructor. Creates the ServerLogging of given
             * type and properties.
             */
            ServerLogging( const QString & type,
                    const PropertyDict & properties );

            /**
             * Destructor.
             * Deletes the given properties and all variables within this
             * ServerLogging.
             */
            virtual ~ServerLogging();

        private:

            /**
             * Disable unintentional copying.
             */
            ServerLogging( const ServerLogging & p );
            ServerLogging & operator=( const ServerLogging & p );

        public:

            /**
             * Return the type of this ServerLogging.
             */
            QString type() const;

            /**
             * Return the Properties defined by this ServerLogging.
             */
            const PropertyDict& properties() const;

            /**
             * Return the Properties defined by this ServerLogging.
             */
            PropertyDict& properties();

            /**
             * Should the ServerLogging be available within editor?
             */
            bool isAvailable() const;

            /**
             * Set the availability flag.
             */
            void setAvailable( bool available );

        private:

            typedef struct ServerLoggingPrivate;
            ServerLoggingPrivate * d;

    };

} // namespace lt

#endif /* _LOGGING_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerLogging.hh 1556 2006-04-04 12:38:17Z modesto $
