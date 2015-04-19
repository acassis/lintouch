// $Id: VFSProtected.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: VFSProtected.cpp --
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

#include "VFSProtected.hh"

using namespace lt;

VFS::VFSProtected::VFSProtected(LoggerPtr logger /* = Logger::nullPtr() */)
    : readOnly(true), valid(false)
{
    enableLogging(logger);
}

VFS::VFSProtected::VFSProtected(const VFSProtected &other)
    : LogEnabled(other), dirPath(other.dirPath), readOnly(other.readOnly),
    valid(other.valid)
{
    enableLogging(other.getLoggerPtr());
}

void VFS::VFSProtected::subVFS(VFSProtectedPtr other, const QString &subdir)
{
    other->valid = false;

    if (!valid)
    {
        getLogger().warn("Attempt to create a VFS from an invalid VFS");
        return;
    }

    QString toOpen = other->chroot(subdir);
    if (toOpen.isNull())
    {
        getLogger().info(QString(
            "Referring to the dir outside the root: %1")
             .arg(subdir));
        return;
    }

    other->dirPath = toOpen;
    if (!other->dirPath.endsWith("/"))
        other->dirPath += "/";
}

QString VFS::VFSProtected::chroot(QString filename) const
{
    filename = QDir::cleanDirPath(filename);
    while (filename.startsWith("/"))
        filename = filename.right(filename.length()-1);
    filename = QDir::cleanDirPath(dirPath + filename);
    return (filename + "/").startsWith(dirPath) ? filename : QString::null;
}

QString VFS::VFSProtected::openFile(const QString &name,
    int mode /* = IO_ReadOnly */) const
{
    if (!valid || mode != IO_ReadOnly && mode != IO_WriteOnly)
    {
        getLogger().warn("Opening a file in an invalid VFS");
        return QString();
    }

    if (mode != IO_ReadOnly && mode != IO_WriteOnly)
    {
        getLogger().warn("Invalid open mode");
        return QString();
    }

    if (readOnly && mode == IO_WriteOnly)
    {
        getLogger().warn(
            "Attempt to open a file for writing in the read-only VFS");
        return QString();
    }

    QString toOpen = chroot(name);
    if (toOpen.isNull())
    {
        getLogger().info(
            QString("Referring to the file outside the root: %1")
            .arg(name));
        return QString();
    }
    getLogger().debug(QString("Opening the file %1").arg(toOpen));
    return toOpen;
}

QString VFS::VFSProtected::mkdir(const QString &dir) const
{
    if (!valid || readOnly)
        return QString();
    QString cleanedDir = chroot(dir);
    if (cleanedDir.isNull())
    {
        getLogger().info(
            QString( "Referring to the dir outside the root: %1")
            .arg(dir));
        return QString();
    }
    getLogger().debug(QString("Making the dir %1").arg(cleanedDir));
    return cleanedDir;
}

QString VFS::VFSProtected::rmdir(const QString &dir) const
{
    if (!valid || readOnly)
        return QString();
    QString cleanedDir = chroot(dir);
    if (cleanedDir.isNull())
    {
        getLogger().info(
            QString( "Referring to the dir outside the root: %1")
            .arg(dir));
        return QString();
    }
    getLogger().debug(QString("Removing the dir %1").arg(cleanedDir));
    return cleanedDir;
}

QString VFS::VFSProtected::unlink(const QString &name) const
{
    if (!valid || readOnly)
        return QString();
    QString toRemove = chroot(name);
    if (toRemove.isNull())
    {
        getLogger().info(
            QString( "Referring to the file outside the root: %1")
            .arg(name));
        return QString();
    }
    getLogger().debug(QString("Removing the file %1").arg(toRemove));
    return toRemove;
}

QString VFS::VFSProtected::ls(const QString &dir /* = "." */,
    int /* filter = All */) const
{
    if (!valid)
        return QString();
    QString cleanedDir = chroot(dir);
    if (cleanedDir.isNull())
    {
        getLogger().info(
            QString( "Referring to the location outside the root: %1")
            .arg(dir));
        return QString();
    }
    return cleanedDir;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFSProtected.cpp 1168 2005-12-12 14:48:03Z modesto $
