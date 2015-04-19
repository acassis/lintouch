// $Id: ViewSerializer.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ViewSerializer.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 13 November 2003
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

#ifndef _VIEWSERIALIZER_HH
#define _VIEWSERIALIZER_HH

#include <qiodevice.h>

#include <lt/logger/Logger.hh>
#include <lt/configuration/Configuration.hh>

#include <lt/project/View.hh>

namespace lt {

    /**
     * Save the View.
     */

    class ViewSerializer
    {
        public:

            /**
             * Serialize the View to the file and return the
             * success of the operation.
             */
            static bool saveToFile(QIODevice *dev,
                const View &view,
                LoggerPtr logger = Logger::nullPtr());

            /**
             * Serialize the View to the configuration and return the
             * success of the operation.
             */
            static bool saveToConfiguration(ConfigurationPtr c,
                const View &view,
                LoggerPtr logger = Logger::nullPtr());
    };
}

#endif /* _VIEWSERIALIZER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ViewSerializer.hh 1169 2005-12-12 15:22:15Z modesto $
