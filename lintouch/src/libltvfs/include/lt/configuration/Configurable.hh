// $Id: Configurable.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
//
//   FILE: Configurable.hh -- 
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

#ifndef _CONFIGURABLE_HH
#define _CONFIGURABLE_HH

#include "lt/configuration/Configuration.hh"

namespace lt {

        /**
         * This interface should be implemented by classes that need to be
         * configured with custom parameters before initialization.
         * <br />
         *
         * The contract surrounding a <code>Configurable</code> is that the
         * instantiating entity must call the <code>configure</code>
         * method before it is valid.  The <code>configure</code> method
         * must be called after the constructor, and before any other method.
         * <br/>
         *
         * Note that this interface is incompatible with Parameterizable.
         *
         * @author <a href="mailto:fede@apache.org">Federico Barbieri</a>
         * @author <a href="mailto:pier@apache.org">Pierpaolo Fumagalli</a>
         * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
         * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
         * @author <a href="mailto:peter@apache.org">Peter Donald</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         * @version 1.0
         */
        class Configurable 
        {

            public:
            /**
             * Pass the <code>Configuration</code> to the
             * <code>Configurable</code> class. This method must always be
             * called after the constructor and before any other method.
             *
             * @param configuration the class configurations.
             */
            virtual void configure( const ConfigurationPtr configuration ) = 0;

            /**
             * Destructor
             */
            virtual ~Configurable () {};
        };

} // namespace lt

#endif /* _CONFIGURABLE_HH */

// vim: set et ts=4 sw=4:
// $Id: Configurable.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
