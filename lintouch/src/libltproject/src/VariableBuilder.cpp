// $Id: VariableBuilder.cpp,v 1.10 2004/08/13 14:47:58 jbar Exp $
//
//   FILE: VariableBuilder.cpp --
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

#include <lt/templates/Variable.hh>

#include "lt/project/VariableBuilder.hh"
using namespace lt;

Variable *VariableBuilder::buildFromConfiguration(ConfigurationPtr c,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    ConfigurationPtr node;

    //sanity check
    if (c.isNull())
    {
        logger->warn("Invalid variable configuration");
        return NULL;
    }

    //load type
    QString typeString = c->getAttribute("type");

    //load properties
    PropertyDict properties;
    ConfigurationList propertyNodes = c->getChildren("property");
    for (ConfigurationList::const_iterator it =
            const_cast<ConfigurationList&>(propertyNodes).begin();
            it != const_cast<ConfigurationList&>(propertyNodes).end(); ++it)
    {
        QString name = (*it)->getAttribute("name");
        QString value = (*it)->getAttribute("value");
        properties.insert(name, new Property(name, "string", value));
    }

    return new Variable(typeString, properties);
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: VariableBuilder.cpp,v 1.10 2004/08/13 14:47:58 jbar Exp $
