// $Id: ServerLoggingBuilder.cpp 1556 2006-04-04 12:38:17Z modesto $
//
//   FILE: ServerLoggingBuilder.cpp --
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: Mon, 03 Apr 2006
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

#include <lt/configuration/DefaultConfigurationBuilder.hh>

#include <lt/templates/Property.hh>  // because of PropertyDict

#include "ServerLogging.hh"
#include "ServerLoggingBuilder.hh"
using namespace lt;

ServerLogging *ServerLoggingBuilder::buildFromFile(QIODevice *dev,
        LoggerPtr logger /* = Logger::nullPtr() */)
{
    QString err;
    int line, col;

    DefaultConfigurationBuilder* dcb = new DefaultConfigurationBuilder();
    Q_CHECK_PTR( dcb );

    ConfigurationPtr c = dcb->buildFromFile( dev, err, line, col);

    delete dcb;

    //sanity check
    if (c.isNull())
    {
        logger->warn("Invalid logging configuration");
        return NULL;
    }

    ConfigurationPtr node;

    //load type
    QString type = c->getAttribute("type");

    //load properties
    PropertyDict properties;
    node = c->getChild("properties");
    if (!node.isNull())
    {
        ConfigurationList propertyNodes = node->getChildren("property");
        for (ConfigurationList::const_iterator it =
                const_cast<ConfigurationList&>(propertyNodes).begin();
                it != const_cast<ConfigurationList&>(propertyNodes).end();
                ++it)
        {
            QString name = (*it)->getAttribute("name");
            QString value = (*it)->getAttribute("value");
            //loading everything as string
            properties.insert(name, new Property(name, "string", value));
        }
    }

    return new ServerLogging(type, properties);
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerLoggingBuilder.cpp 1556 2006-04-04 12:38:17Z modesto $
