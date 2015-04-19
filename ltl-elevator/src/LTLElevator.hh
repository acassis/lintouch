// $Id:$
//
// Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
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


#ifndef _LTLELEVATOR_HH
#define _LTLELEVATOR_HH

#include "lt/templates/TemplateLibrary.hh"
using namespace lt;

namespace lt {

    /**
     * LTLElevator.
     *
     * @author Patrik Modesto <modesto@swac.cz>
     */
    class LTLElevator : public TemplateLibrary
    {
        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            LTLElevator();

            /**
             * Destructor.
             */
            virtual ~LTLElevator();

        public:

            virtual Template * instantiate( const QString & type,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        private:

            struct LTLElevator_private;
            LTLElevator_private* d;
    };

} // namespace lt

#endif /* _LTLBOOSTRAP_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id:$
