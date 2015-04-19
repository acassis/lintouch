// $Id: ServerLogging.cpp 1556 2006-04-04 12:38:17Z modesto $
//
//   FILE: ServerLogging.cpp --
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

#include "ServerLogging.hh"
using namespace lt;

struct ServerLogging::ServerLoggingPrivate
{
    // type of this connection
    QString type;

    // properties of this connection
    PropertyDict properties;

    // should the ServerLogging be visible within editor?
    bool isAvailable;
};

ServerLogging::ServerLogging( const QString & type,
        const PropertyDict & properties ) :
    d ( new ServerLoggingPrivate )
{
    Q_CHECK_PTR(d);
    d->type = type;
    d->properties = properties;
    d->isAvailable = true;
}

ServerLogging::~ServerLogging()
{
    d->properties.setAutoDelete( true );
    d->properties.clear();

    delete d;
}

QString ServerLogging::type() const
{
    Q_ASSERT(d);
    return d->type;
}

const PropertyDict& ServerLogging::properties() const
{
    Q_ASSERT(d);
    return d->properties;
}

PropertyDict& ServerLogging::properties()
{
    Q_ASSERT(d);
    return d->properties;
}

bool ServerLogging::isAvailable() const
{
    return d->isAvailable;
}

void ServerLogging::setAvailable( bool available )
{
    d->isAvailable = available;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerLogging.cpp 1556 2006-04-04 12:38:17Z modesto $
