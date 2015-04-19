// $Id: PanelBuilder.hh,v 1.7 2004/09/14 16:23:26 mman Exp $
//
//   FILE: PanelBuilder.hh --
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

#ifndef _PANELBUILDER_HH
#define _PANELBUILDER_HH

#include <lt/logger/Logger.hh>
#include <lt/configuration/Configuration.hh>

#include <lt/templates/Localizator.hh>
#include <lt/templates/TemplateManager.hh>

#include "lt/project/Panel.hh"

namespace lt {

    /**
        * Factory for the Panel class.
        */

    class PanelBuilder
    {
        public:

            /**
             * Build a panel instance from the configuration.
             *
             * @param c Configuration describing the connection.
             * @param loc Localizator
             * @param tm TemplateManager used to instantiate templates.
             * @param logger Logger
             * @return Newly built panel.
             */
            static Panel buildFromConfiguration(ConfigurationPtr c,
                const LocalizatorPtr &loc,
                const TemplateManager &tm,
                LoggerPtr logger = Logger::nullPtr());
    };
}

#endif /* _PANELBUILDER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelBuilder.hh,v 1.7 2004/09/14 16:23:26 mman Exp $
