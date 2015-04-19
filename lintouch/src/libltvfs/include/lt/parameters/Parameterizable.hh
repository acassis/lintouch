// $Id: Parameterizable.hh,v 1.1 2003/11/11 15:32:38 mman Exp $
//
//   FILE: Parameterizable.hh -- 
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

#ifndef _PARAMETERIZABLE_HH
#define _PARAMETERIZABLE_HH

#include "lt/parameters/Parameters.hh"

namespace lt {

        /**
         * Components should implement this interface if they wish to
         * be provided with parameters during startup.
         * <p>
         * The Parameterizable interface is a light-weight alternative to the
         * {@link org.apache.avalon.framework.configuration.Configurable}
         * interface.  As such, the <code>Parameterizable</code> interface and
         * the <code>Configurable</code> interface are <strong>not</strong>
         * compatible.
         * </p><p>
         * This interface will be called after
         * <code>Composable.compose()</code> and before
         * <code>Initializable.initialize()</code>.
         * </p>
         *
         * @author <a href="mailto:peter@apache.org">Peter Donald</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class Parameterizable
        {
            public:
            /**
             * Provide component with parameters.
             *
             * @param parameters the parameters
             */
            virtual void parameterize( const ParametersPtr parameters ) = 0;
            
            /**
             * Destructor
             */
            virtual ~Parameterizable () {};
        };

} // namespace lt

#endif /* _PARAMETERIZABLE_HH */

// vim: set et ts=4 sw=4:
// $Id: Parameterizable.hh,v 1.1 2003/11/11 15:32:38 mman Exp $
