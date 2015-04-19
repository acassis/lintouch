// $Id: TemplateSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateSerializer.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 09 September 2004
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

#include <TemplateSerializer.hh>

using namespace lt;

bool TemplateSerializer::saveToConfiguration(
    ConfigurationPtr c,
    const Template *t,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    ConfigurationPtr node;

    //sanity checks
    if (c.isNull())
    {
        logger->error("Invalid template configuration");
        return false;
    }
    Q_ASSERT(t);

    //type
    if (t->library().isEmpty())
        c->setAttribute("type", t->type());
    else
        c->setAttribute("type", QString("%1.%2")
            .arg(t->library()).arg(t->type()));

    //shortcut
    c->setAttribute("shortcut", t->shortcut());

    // lock-status
    c->setAttribute("locked", t->isEnabled()?"false":"true");

    //geometry
    node = c->getChild("geometry", true);
    Q_ASSERT(node);
    QRect r = t->virtualRect();
    node->setAttribute("width", QString::number(r.width()));
    node->setAttribute("height", QString::number(r.height()));
    node->setAttribute("left", QString::number(r.left()));
    node->setAttribute("top", QString::number(r.top()));
    node->setAttribute("layer", QString::number((int)t->z()));

    //properties
    const PropertyDict & props = t->properties();
    for( unsigned it = 0; it < props.count(); it++ )
    {
        if (props[ it ]->isDefault())
            continue;
        node = c->getChild("property", true);
        Q_ASSERT(node);
        node->setAttribute("name", props.keyAt( it ));
        node->setValue(props[ it ]->encodeValue());
    }

    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
