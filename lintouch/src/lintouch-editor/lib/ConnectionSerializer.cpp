// $Id: ConnectionSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ConnectionSerializer.cpp --
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

#include <ConnectionSerializer.hh>
#include <VariableSerializer.hh>

#include <lt/logger/Logger.hh>
#include <lt/configuration/DefaultConfiguration.hh>
#include <lt/configuration/DefaultConfigurationSerializer.hh>

using namespace lt;

bool ConnectionSerializer::saveToFile(QIODevice *dev,
    const Connection &connection,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    ConfigurationPtr c(new DefaultConfiguration("project-connection"));
    if (!ConnectionSerializer::saveToConfiguration(c, connection, logger))
    {
        return false;
    }
    if (!DefaultConfigurationSerializer().serializeToFile(dev, c))
    {
        logger->error("Cannot save the connection to a file");
        return false;
    }
    return true;
}

bool ConnectionSerializer::saveToConfiguration(ConfigurationPtr c,
    const Connection &connection,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    //sanity checks
    if (c.isNull())
    {
        logger->error("Invalid connection configuration");
        return false;
    }

    c->setAttribute("type", connection.type());
    ConfigurationPtr node;

    //properties
    ConfigurationPtr propertiesNode = c->getChild("properties", true);
    Q_ASSERT(propertiesNode);
    const PropertyDict properties = connection.properties();
    unsigned int i;
    for (i = 0; i < properties.count(); i++ )
    {
        ConfigurationPtr node = propertiesNode->getChild("property", true);
        Q_ASSERT(node);
        node->setAttribute("name", properties.keyAt(i));
        node->setAttribute("value", properties[i]->encodeValue());
    }

    //variables
    ConfigurationPtr variablesNode = c->getChild("variables", true);
    Q_ASSERT(variablesNode);
    const VariableDict variables = connection.variables();
    for (i = 0; i < variables.count(); i++)
    {
        ConfigurationPtr node = variablesNode->getChild("variable", true);
        Q_ASSERT(node);
        node->setAttribute("name", variables.keyAt(i));
        if (!VariableSerializer::saveToConfiguration(node, *variables[i],
            logger->getChildLogger(QString("Variable %1")
            .arg(variables.keyAt(i)))))
        {
            logger->error(QString("Cannot serialize the variable %1")
                .arg(variables.keyAt(i)));
            return false;
        }
    }

    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ConnectionSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
