// $Id: ServerLoggingSerializer.cpp 1562 2006-04-06 17:07:14Z modesto $
//
//   FILE: ServerLoggingSerializer.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 13 October 2004
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

#include <ServerLoggingSerializer.hh>
#include <VariableSerializer.hh>

#include <lt/logger/Logger.hh>
#include <lt/configuration/DefaultConfiguration.hh>
#include <lt/configuration/DefaultConfigurationSerializer.hh>

using namespace lt;

bool ServerLoggingSerializer::saveToFile(QIODevice *dev,
    const ServerLogging &sl,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    ConfigurationPtr c(new DefaultConfiguration("project-logging"));
    if (!ServerLoggingSerializer::saveToConfiguration(c, sl, logger))
    {
        return false;
    }
    if (!DefaultConfigurationSerializer().serializeToFile(dev, c))
    {
        logger->error("Cannot save the Server Logging to a file");
        return false;
    }
    return true;
}

bool ServerLoggingSerializer::saveToConfiguration(ConfigurationPtr c,
    const ServerLogging &sl,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    //sanity checks
    if (c.isNull())
    {
        logger->error("Invalid Server Logging configuration");
        return false;
    }

    c->setAttribute("type", sl.type());
    ConfigurationPtr node;

    //properties
    ConfigurationPtr propertiesNode = c->getChild("properties", true);
    Q_ASSERT(propertiesNode);
    const PropertyDict properties = sl.properties();
    unsigned int i;
    for (i = 0; i < properties.count(); i++ )
    {
        ConfigurationPtr node = propertiesNode->getChild("property", true);
        Q_ASSERT(node);
        node->setAttribute("name", properties.keyAt(i));
        node->setAttribute("value", properties[i]->encodeValue());
    }

    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerLoggingSerializer.cpp 1562 2006-04-06 17:07:14Z modesto $
