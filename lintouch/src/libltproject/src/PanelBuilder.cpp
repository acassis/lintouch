// $Id: PanelBuilder.cpp,v 1.14 2004/09/14 16:23:26 mman Exp $
//
//   FILE: PanelBuilder.cpp --
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

#include "lt/project/PanelBuilder.hh"
#include "lt/project/TemplateBuilder.hh"
using namespace lt;

Panel PanelBuilder::buildFromConfiguration(ConfigurationPtr c,
    const LocalizatorPtr &loc,
    const TemplateManager &tm,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    //sanity check
    if (c.isNull())
    {
        logger->warn("Invalid panel configuration");
        return Panel();
    }

    Panel result;
    ConfigurationPtr node;

    //read the panel properties
    //none for now

    //templates
    node = c->getChild("templates");
    if (!node.isNull())
    {
        ConfigurationList templateNodes = node->getChildren("template");
        if (templateNodes.count() == 0)
            logger->warn("No templates defined in the panel");
        for (ConfigurationList::const_iterator it =
                const_cast<ConfigurationList&>(templateNodes).begin();
                it != const_cast<ConfigurationList&>(templateNodes).end();
                ++it)
        {
            QString name = (*it)->getAttribute("name");
            logger->debug(QString("Creating the template %1").arg(name));

            Template * t =
                TemplateBuilder::buildFromConfiguration(*it, loc,
                    tm, logger->getChildLogger(name));
            if (t == NULL)
            {
                logger->warn(QString("Unable to instantiate template %1: %2")
                        .arg(name).arg((*it)->getLocation()));
                continue;
            }

            result.addTemplate(name, t);
        }
    }

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelBuilder.cpp,v 1.14 2004/09/14 16:23:26 mman Exp $
