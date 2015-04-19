// $Id: TemplateBuilder.cpp,v 1.22 2004/12/01 12:10:04 mman Exp $
//
//   FILE: TemplateBuilder.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 13 November 2003
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

#include <lt/templates/IOPin.hh>

#include "DefaultTemplate.hh"
#include "lt/project/TemplateBuilder.hh"
using namespace lt;

Template * TemplateBuilder::buildFromConfiguration(
    ConfigurationPtr c,
    const LocalizatorPtr &loc,
    const TemplateManager &tm,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    ConfigurationPtr node;

    //sanity check
    if (c.isNull())
    {
        logger->warn("Invalid template configuration");
        //FIXME: should we return NULL here ???
        return NULL;
    }

    Template * t = NULL;
    DefaultTemplate *dt = NULL;

    //type (aka Template)
    QString type = c->getAttribute("type");
    QString tt, l;
    int dot = -1;
    if( ( dot = type.find( "." ) ) != -1 ) {
        l = type.left( dot );
        tt = type.right( type.length() - dot - 1 );
        t = tm.instantiate(tt, logger, l);
    } else {
        // support for unqualified templates is dropped
        logger->warn(QString("Unqualified template %1!").arg(type));
    }
    if (t == NULL)
    {
        logger->warn(QString(
            "The template of type %1 not found: %2"
            "(using the default instead)")
            .arg(c->getAttribute("type")).arg(c->getLocation()));
        t = dt = new DefaultTemplate(tt, logger);
        t->setLibrary( l );
    }

    // remember from which library we were supposed to come
    t->setLibrary( l );

    //first let the template create it's nested templates
    t->compose(tm);
    //and then localize it
    t->localize(loc);

    QKeySequence shortcut = c->getAttribute("shortcut");
    t->setShortcut( shortcut );

    l = c->getAttribute("locked");
    t->setEnabled(l != "true");

    //geometry
    node = c->getChild("geometry");
    if (node.isNull())
    {
        logger->warn(QString(
            "Missing the geometry in the template configuration: %1")
            .arg(c->getLocation()));
        delete t;
        return NULL;
    }
    int width = node->getAttributeAsInt("width", -1);
    if (width == -1)
    {
        logger->warn(QString(
            "The width of the template is missing or invalid")
            .arg(node->getLocation()));
        delete t;
        return NULL;
    }
    int height = node->getAttributeAsInt("height", -1);
    if (width == -1)
    {
        logger->warn(QString(
            "The height of the template is missing or invalid")
            .arg(node->getLocation()));
        delete t;
        return NULL;
    }
    int left = node->getAttributeAsInt("left", -1);
    if (width == -1)
    {
        logger->warn(QString(
            "The left of the template is missing or invalid")
            .arg(node->getLocation()));
        delete t;
        return NULL;
    }
    int top = node->getAttributeAsInt("top", -1);
    if (width == -1)
    {
        logger->warn(QString(
            "The top of the template is missing or invalid")
            .arg(node->getLocation()));
        delete t;
        return NULL;
    }
    int layer = node->getAttributeAsInt("layer", 0);
    QRect r;
    r.setTop(top);
    r.setLeft(left);
    r.setWidth(width);
    r.setHeight(height);
    t->setVirtualRect(r);
    t->setZ(layer);

    //properties
    ConfigurationList propertyNodes = c->getChildren("property");
    ConfigurationList::const_iterator it;
    for (it = const_cast<ConfigurationList&>(propertyNodes).begin();
            it != const_cast<ConfigurationList&>(propertyNodes).end(); ++it)
    {
        QString name = (*it)->getAttribute("name");
        QString value;
        if( (*it)->getAttributeNames().contains( "value" ) ) {
            value = (*it)->getAttribute("value");
        } else {
            value = (*it)->getValue();
        }

        if (dt != NULL) {
            Property * p = new Property(name, "string");
            p->setEnabled(false);
            dt->addProperty(name, p);
        }

        if (!t->properties().contains(name))
        {
            logger->warn(QString("There was a request to set the property"
                " %1 of the template but the template contains no such"
                " property").arg(name));
            continue;
        }
        t->properties()[name]->decodeValue(value);
    }

    return t;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateBuilder.cpp,v 1.22 2004/12/01 12:10:04 mman Exp $
