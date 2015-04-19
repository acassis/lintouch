// $Id: Reconfigurable.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
//
//   FILE: Reconfigurable.hh -- 
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

#ifndef _RECONFIGURABLE_HH
#define _RECONFIGURABLE_HH

#include "lt/configuration/Configurable.hh"

namespace lt {

        /**
         * This interface should be implemented by classes that need to be
         * reconfigured with custom parameters during their life-cycle.
         * <br />
         *
         * Note that this interface is incompatible with Parameterizable.
         *
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         * @version 1.0
         */
        class Reconfigurable : public Configurable 
        {

            public:
            /**
             * Pass the <code>Configuration</code> to the
             * <code>Reconfigurable</code> class.
             *
             * @param configuration the class configurations.
             */
            virtual void reconfigure( ConfigurationPtr configuration ) = 0;

            /**
             * Destructor
             */
            virtual ~Reconfigurable () {};
        };

} // namespace lt

#endif /* _RECONFIGURABLE_HH */

// vim: set et ts=4 sw=4:
// $Id: Reconfigurable.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
