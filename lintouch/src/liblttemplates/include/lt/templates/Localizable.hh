// $Id: Localizable.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Localizable.hh -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 07 October 2003
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

#ifndef _LOCALIZABLE_HH
#define _LOCALIZABLE_HH

#include <lt/templates/Localizator.hh>

namespace lt {

    /**
     * Pure interface. Class that implements this interface, marks
     * itself as localizable. A framework then provide the class with
     * it's localizator.
     *
     * \author Patrik Modesto <modetso@swac.cz>
     */
    class Localizable
    {
        public:
            virtual ~Localizable() {}

            /**
             * Called by framework when there is a localizator
             * available.
             *
             * \param l The localizator.
             */
            virtual void localize( LocalizatorPtr l ) = 0;
    };

} // lt

#endif /* _LOCALIZABLE_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Localizable.hh 1168 2005-12-12 14:48:03Z modesto $
