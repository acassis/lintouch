// $Id: AbstractConfigurable.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
//
//   FILE: AbstractConfigurable.hh -- 
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

#ifndef _ABSTRACTCONFIGURABLE_HH
#define _ABSTRACTCONFIGURABLE_HH

#include "lt/configuration/Configurable.hh"

namespace lt {

        /**
         * Utility interface that helps in common configuration management.
         *
         * It keeps the given configuration in an instance variable and 
         * provides it
         * to subclasses via getConfiguration () method. 
         *
         * The given configuration is not deleted upon class deletion.
         *
         * @author <a href="mailto:peter@apache.org">Peter Donald</a>
         * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class AbstractConfigurable : public Configurable
        {
            public:

            /**
             * Constructor
             */
            AbstractConfigurable ();

            /**
             * Provide component with a configuration.
             *
             * @param configuration the configuration
             */
            virtual void configure( const ConfigurationPtr configuration );

            /**
             * return logger to interested parties.
             */
            const ConfigurationPtr  getConfiguration() const { return m_c; };

            /**
             * Destructor
             */
            virtual ~AbstractConfigurable ();

            private:

            ConfigurationPtr m_c;

        };

} // namespace lt

#endif /* _ABSTRACTCONFIGURABLE_HH */

// vim: set et ts=4 sw=4:
// $Id: AbstractConfigurable.hh,v 1.1 2003/11/11 15:32:33 mman Exp $
