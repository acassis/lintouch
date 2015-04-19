// $Id: InfoSerializer.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: InfoSerializer.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 09 September 2004
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

#ifndef _INFOSERIALIZER_HH
#define _INFOSERIALIZER_HH

#include <lt/logger/Logger.hh>
#include <lt/configuration/Configuration.hh>

#include <lt/templates/Info.hh>

namespace lt {

    /**
     * Save the Info.
     */

    class InfoSerializer
    {
        public:

            /**
             * Serialize the info into the configuration. If an error
             * occurs, report to the logger and return false.
             */
            static bool saveToConfiguration(ConfigurationPtr c,
                const Info &info, LoggerPtr logger = Logger::nullPtr());
    };
}

#endif /* _INFOSERIALIZER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: InfoSerializer.hh 1169 2005-12-12 15:22:15Z modesto $
