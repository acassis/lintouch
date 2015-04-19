// $Id: ConnectionBuilder.cpp,v 1.12 2004/10/19 08:34:08 jbar Exp $
//
//   FILE: ConnectionBuilder.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 19 November 2003
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

#include <lt/templates/Connection.hh>

#include "lt/project/ConnectionBuilder.hh"
#include "lt/project/VariableBuilder.hh"
using namespace lt;

Connection *ConnectionBuilder::buildFromFile(QIODevice *dev,
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
        logger->warn("Invalid connection configuration");
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
                it != const_cast<ConfigurationList&>(propertyNodes).end(); ++it) {

            QString name = (*it)->getAttribute("name");
            QString value = (*it)->getAttribute("value");
            //loading everything as string
            properties.insert(name, new Property(name, "string", value));
        }
    }

    Connection *result = new Connection(type, properties);

    //load variables
    VariableDict variables;
    node = c->getChild("variables");
    if (!node.isNull())
    {
        ConfigurationList variableNodes = node->getChildren("variable");
        for (ConfigurationList::const_iterator it =
                const_cast<ConfigurationList&>(variableNodes).begin();
                it != const_cast<ConfigurationList&>(variableNodes).end();
                ++it) {

            QString name = (*it)->getAttribute("name");
            logger->debug(QString(
                        "Creating the variable %1").arg(name));

            Variable *variable = VariableBuilder::buildFromConfiguration(
                *it, logger);

            if (variable == NULL)
                continue;

            result->addVariable(name, variable);
        }
    }

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ConnectionBuilder.cpp,v 1.12 2004/10/19 08:34:08 jbar Exp $
