// $Id: InfoBuilder.cpp,v 1.5 2004/08/12 22:31:13 jbar Exp $
//
//   FILE: InfoBuilder.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 12 November 2003
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

#include <qdatetime.h>

#include "lt/project/InfoBuilder.hh"
using namespace lt;

Info InfoBuilder::buildFromConfiguration(ConfigurationPtr c,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    Info result;
    ConfigurationPtr node;

    //sanity check
    if (c.isNull())
    {
        logger->warn("Invalid info configuration");
        return Info();
    }

    //author
    node = c->getChild("author");
    if (node.isNull())
    {
        logger->warn("Missing the author in the info");
        return Info();
    }
    result.setAuthor(node->getValue());

    //version
    node = c->getChild("version");
    if (node.isNull())
    {
        logger->warn("Missing the version in the info");
        return Info();
    }
    result.setVersion(node->getValue());

    //date
    node = c->getChild("date");
    if (node.isNull())
    {
        logger->warn("Missing the date in the info");
        return Info();
    }
    QDate date = QDate::fromString(node->getValue(), Qt::ISODate);
    if (!date.isValid())
    {
        logger->warn("Invalid date in the info");
        return Info();
    }
    result.setDate(date);

    //short description
    node = c->getChild("shortdesc");
    if (node.isNull())
    {
        logger->warn("Missing the short description in the info");
        return Info();
    }
    result.setShortDescription(node->getValue());

    //long description
    node = c->getChild("longdesc");
    if (node.isNull())
        logger->warn("Missing the long description in the info");
    result.setLongDescription(node->getValue());

    //read an icon?

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: InfoBuilder.cpp,v 1.5 2004/08/12 22:31:13 jbar Exp $
