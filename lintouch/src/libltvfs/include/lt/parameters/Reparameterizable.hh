// $Id: Reparameterizable.hh,v 1.1 2003/11/11 15:32:38 mman Exp $
//
//   FILE: Reparametrizable.hh -- 
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

#ifndef _REPARAMETERIZABLE_HH
#define _REPARAMETERIZABLE_HH

#include "lt/parameters/Parameters.hh"

namespace lt {

        /**
         * Components should implement this interface if they wish to
         * be provided with parameters during its lifetime. This interface
         * will be called after Startable.start() and before
         * Startable.stop(). It is incompatible with the
         * Reconfigurable interface.
         *
         * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class Reparameterizable : public Parameterizable
        {
            public:
            /**
             * Provide component with parameters.
             *
             * @param parameters the parameters
             * @throws ParameterException if parameters are invalid
             */
            virtual void reparameterize( ParametersPtr parameters ) = 0;

            /**
             * Destructor
             */
            virtual ~Reparameterizable () {};
        };

} // namespace lt

#endif /* _REPARAMETERIZABLE_HH */

// vim: set et ts=4 sw=4:
// $Id: Reparameterizable.hh,v 1.1 2003/11/11 15:32:38 mman Exp $
