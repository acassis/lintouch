// $Id: VariableSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: VariableSerializer.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 18 October 2004
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

#include "VariableSerializer.hh"

using namespace lt;

bool VariableSerializer::saveToConfiguration(ConfigurationPtr c,
    const Variable &variable,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    //sanity checks
    if (c.isNull())
    {
        logger->error("Invalid variable configuration");
        return false;
    }

    //type
    IOPin::TypeList types;
    types.append(variable.type());
    c->setAttribute("type", IOPin::encodeTypes(types));

    ConfigurationPtr node;

    //properties
    const PropertyDict properties = variable.properties();
    if (properties.count())
    {
        for (unsigned int i = 0; i < properties.count(); i++)
        {
            node = c->getChild("property", true);
            Q_ASSERT(node);
            node->setAttribute("name", properties.keyAt(i));
            node->setAttribute("value", properties[i]->encodeValue());
        }
    }

    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: VariableSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
