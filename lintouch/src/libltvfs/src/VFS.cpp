// $Id: VFS.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: VFS.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 08 October 2003
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


#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>

#include <lt/logger/Logger.hh>
#include <lt/vfs/VFS.hh>
#include "VFSProtected.hh"

using namespace lt;


VFS::VFS(const QUrl &, bool readOnly /* = true */,
    LoggerPtr logger /* = Logger::nullPtr() */)
    : p(new VFSProtected(logger))
{
    enableLogging(logger);
    p->readOnly = readOnly;
}

VFS::~VFS()
{
}

bool VFS::isValid() const
{
    return p->valid;
}

bool VFS::isReadOnly() const
{
    return p->readOnly;
}

QUrl VFS::rootLocation() const
{
    return p->dirPath;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFS.cpp 1168 2005-12-12 14:48:03Z modesto $
