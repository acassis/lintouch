// $Id: InfoSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: InfoSerializer.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 08 September 2004
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

#include <InfoSerializer.hh>
#include <qdatetime.h>

using namespace lt;

bool InfoSerializer::saveToConfiguration(ConfigurationPtr c,
    const Info &info, LoggerPtr logger /* = Logger::nullPtr() */)
{
    ConfigurationPtr node;

    //sanity checks
    if (c.isNull())
    {
        logger->error("Invalid info configuration");
        return false;
    }
    if (!info.date().isValid())
    {
        logger->error("The info is invalid");
        return false;
    }

    //author
    node = c->getChild("author", true);
    Q_ASSERT(node);
    node->setValue(info.author());

    //version
    node = c->getChild("version", true);
    Q_ASSERT(node);
    node->setValue(info.version());

    //date
    node = c->getChild("date", true);
    Q_ASSERT(node);
    node->setValue(info.date().toString(Qt::ISODate));

    //short description
    node = c->getChild("shortdesc", true);
    Q_ASSERT(node);
    node->setValue(info.shortDescription());

    //long description
    node = c->getChild("longdesc", true);
    Q_ASSERT(node);
    node->setValue(info.longDescription());

    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: InfoSerializer.cpp 1169 2005-12-12 15:22:15Z modesto $
