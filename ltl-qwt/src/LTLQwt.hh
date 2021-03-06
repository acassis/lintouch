// $Id: LTLQwt.hh 347 2005-02-23 14:56:10Z modesto $
//
//   FILE: LTLQwt.hh -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 04 November 2003
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


#ifndef _LTLQWT_HH
#define _LTLQWT_HH

#include "lt/templates/TemplateLibrary.hh"
using namespace lt;

namespace lt {

    /**
     * LTLQwt.
     *
     * @author Patrik Modesto <modesto@swac.cz>
     */
    class LTLQwt : public TemplateLibrary
    {
        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            LTLQwt();

            /**
             * Destructor.
             */
            virtual ~LTLQwt();

        public:

            virtual Template * instantiate( const QString & type,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        private:

            struct LTLQwt_private;
            LTLQwt_private* d;
    };

} // namespace lt

#endif /* _LTLQWT_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: LTLQwt.hh 347 2005-02-23 14:56:10Z modesto $
