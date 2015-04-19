// $Id: AbstractParameterizable.hh,v 1.1 2003/11/11 15:32:38 mman Exp $
//
//   FILE: AbstractParametrizable.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 13 February 2003
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

#ifndef _ABSTRACTPARAMETERIZABLE_HH
#define _ABSTRACTPARAMETERIZABLE_HH

#include "lt/parameters/Parameterizable.hh"

namespace lt {

        /**
         * Utility interface that helps in common parameters management.
         *
         * It keeps the given parameters in an instance variable and 
         * provides it
         * to subclasses via getParameters () method. 
         *
         * The given parameters are not deleted upon class deletion.
         *
         * @author <a href="mailto:peter@apache.org">Peter Donald</a>
         * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class AbstractParameterizable : public Parameterizable
        {
            public:

            /**
             * Constructor
             */
            AbstractParameterizable ();

            /**
             * Provide component with the parameters.
             *
             * @param parameters the parameters
             */
            virtual void parameterize( const ParametersPtr paremeters );

            /**
             * return logger to interested parties.
             */
            const ParametersPtr getParameters() const { return m_p; };

            /**
             * Destructor
             */
            virtual ~AbstractParameterizable ();

            private:

            ParametersPtr m_p;

        };

} // namespace lt

#endif /* _ABSTRACTPARAMETERIZABLE_HH */

// vim: set et ts=4 sw=4:
// $Id: AbstractParameterizable.hh,v 1.1 2003/11/11 15:32:38 mman Exp $
