// $Id: PanelSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: PanelSerializer.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 10 September 2004
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

#include <PanelSerializer.hh>
#include <TemplateSerializer.hh>

using namespace lt;

bool PanelSerializer::saveToConfiguration(ConfigurationPtr c,
    const Panel &panel,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    //sanity checks
    if (c.isNull())
    {
        logger->error("Invalid panel configuration");
        return false;
    }

    ConfigurationPtr node;

    //templates
    ConfigurationPtr templatesNode = c->getChild("templates", true);
    Q_ASSERT(templatesNode);

    const TemplateDict & td = panel.templates();
    unsigned it;
    for( it = 0; it < td.count(); it++ )
    {
        node = templatesNode->getChild("template", true);
        Q_ASSERT(node);
        node->setAttribute("name", td.keyAt( it ));
        if (!TemplateSerializer::saveToConfiguration(node, td[ it ],
            logger->getChildLogger(QString("Template %1")
            .arg(td.keyAt( it ) ) ) ) )
        {
            logger->error(QString("Cannot serialize the template %1")
                .arg(td.keyAt( it )));
            return false;
        }

    }

    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
