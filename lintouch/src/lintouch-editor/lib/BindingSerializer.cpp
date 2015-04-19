// $Id: BindingSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: BindingSerializer.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 20 October 2004
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

#include <lt/configuration/DefaultConfiguration.hh>
#include <lt/configuration/DefaultConfigurationSerializer.hh>

#include <lt/project/Panel.hh>

#include "BindingSerializer.hh"
#include "Binding.hh"

using namespace lt;


bool BindingSerializer::saveToFile(QIODevice *dev,
    const QString &connection, const QString &view,
    EditorProject &prj,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    ConfigurationPtr c(new DefaultConfiguration("project-binding"));
    if (!BindingSerializer::saveToConfiguration(c, connection, view,
        prj, logger))
    {
        return false;
    }
    if (!DefaultConfigurationSerializer().serializeToFile(dev, c))
    {
        logger->error("Cannot save the binding to a file");
        return false;
    }
    return true;
}

bool BindingSerializer::saveToConfiguration(ConfigurationPtr c,
    const QString &connection, const QString &view,
    EditorProject &prj,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    //sanity checks
    if (c.isNull())
    {
        logger->error("Invalid binding configuration");
        return false;
    }

    ConfigurationPtr node;

    const PanelMap &pm = prj.views()[view].panels();
    unsigned it;
    for ( it = 0; it < pm.count(); it++ )
    {
        ConfigurationPtr panelsNode = c->getChild("panel", true);
        Q_ASSERT(panelsNode);
        panelsNode->setAttribute("id", pm.keyAt(it));

        const BindingsList &bs = prj.bindings().bindingsByPanel(view,
            pm.keyAt(it));
        for (BindingsList::ConstIterator bi = bs.constBegin();
            bi != bs.constEnd(); ++bi)
        {
            if ((*bi)->connection() != connection)
                continue;
            node = panelsNode->getChild("variable", true);
            node->setAttribute("name", (*bi)->variable());
            node->setAttribute("template", (*bi)->templ());
            node->setAttribute("iopin", (*bi)->iopin());
        }
    }

    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: BindingSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
