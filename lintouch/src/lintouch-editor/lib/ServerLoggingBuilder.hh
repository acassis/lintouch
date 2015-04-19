// $Id: ServerLoggingBuilder.hh 1556 2006-04-04 12:38:17Z modesto $
//
//   FILE: ServerLoggingBuilder.hh --
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

#ifndef _LOGGINGBUILDER_HH
#define _LOGGINGBUILDER_HH

#include <qiodevice.h>

#include <lt/logger/Logger.hh>

namespace lt {

    /**
     * Factory for the ServerLogging class.
     */

    class ServerLogging;

    class ServerLoggingBuilder
    {
        public:

            /**
                * Build a logging instance from the file.
                */
            static ServerLogging *buildFromFile(QIODevice *dev,
                    LoggerPtr logger = Logger::nullPtr());
    };
}

#endif /* _LOGGINGBUILDER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerLoggingBuilder.hh 1556 2006-04-04 12:38:17Z modesto $
