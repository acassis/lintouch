// $Id: ViewSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ViewSerializer.cpp --
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

#include <ViewSerializer.hh>
#include <PanelSerializer.hh>
#include <lt/logger/Logger.hh>
#include <lt/configuration/DefaultConfiguration.hh>
#include <lt/configuration/DefaultConfigurationSerializer.hh>

using namespace lt;

bool ViewSerializer::saveToFile(QIODevice *dev,
    const View &view,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    ConfigurationPtr c(new DefaultConfiguration("project-view"));
    if (!ViewSerializer::saveToConfiguration(c, view, logger))
    {
        return false;
    }
    if (!DefaultConfigurationSerializer().serializeToFile(dev, c))
    {
        logger->error("Cannot save the view to a file");
        return false;
    }
    return true;
}

bool ViewSerializer::saveToConfiguration(ConfigurationPtr c,
    const View &view,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    //sanity checks
    if (c.isNull())
    {
        logger->error("Invalid view configuration");
        return false;
    }

    ConfigurationPtr node;

    //virtual dimensions
    node = c->getChild("geometry", true);
    Q_ASSERT(node);
    node->setAttribute("width", QString::number(view.virtualWidth()));
    node->setAttribute("height", QString::number(view.virtualHeight()));

    //panels
    ConfigurationPtr panelsNode = c->getChild("panels", true);
    Q_ASSERT(panelsNode);
    const PanelMap & pm = view.panels();
    unsigned it;
    for ( it = 0; it < pm.count(); it++ )
    {
        ConfigurationPtr node = panelsNode->getChild("panel", true);
        Q_ASSERT(node);
        node->setAttribute("id", pm.keyAt( it ));
        if (!PanelSerializer::saveToConfiguration(node, pm[ it ],
            logger->getChildLogger(QString("Panel %1")
            .arg(pm.keyAt( it )))))
        {
            logger->error(QString("Cannot serialize the panel%1")
                .arg(pm.keyAt( it )));
            return false;
        }
    }

    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ViewSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
