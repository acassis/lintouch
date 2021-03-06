// $Id: ResourceManager.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: ResourceManager.cpp --
// AUTHOR: Jiri Barton <jbar@lintouch.org>
//   DATE: 25 August 2004
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

#include "lt/project/ResourceManager.hh"
using namespace lt;

struct ResourceManager::ResourceManagerPrivate
{
    /**
     * The root location of the resources.
     */
    VFSPtr v;
};

ResourceManager::ResourceManager(const VFSPtr &vfs)
    : d(new ResourceManagerPrivate)
{
    d->v = vfs;
}

ResourceManager::~ResourceManager()
{
    delete d;
}

QByteArray ResourceManager::resourceAsByteArray(const QString &key) const
{
    QIODevice *dev = resourceAsIODevice(key);

    if( !dev->isOpen() )
    {
        delete dev;
        return QByteArray();
    }

    QByteArray result = dev->readAll();
    delete dev;
    return result;
}

QIODevice *ResourceManager::resourceAsIODevice(const QString &key) const
{
    return d->v->openFile(key);
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ResourceManager.cpp 1168 2005-12-12 14:48:03Z modesto $
