// $Id: VFSLocal.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: VFSLocal.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 06 November 2003
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

#include <lt/vfs/VFSdefs.hh>
#include <lt/vfs/VFSLocal.hh>
#include "VFSProtected.hh"

using namespace lt;

VFSLocal::VFSLocal(const QUrl &rootLocation, bool readOnly /* = true */,
    LoggerPtr logger /* = Logger::nullPtr() */)
    : VFS(rootLocation, readOnly, logger)
{
    if (rootLocation.protocol() != "file")
    {
        getLogger().info(QString("The URL %1 is not supported by VFSLocal")
                .arg(rootLocation));
        return;
    }

    QString path = rootLocation.path();
    p->dirPath = QDir(path).absPath();
    if (!QFileInfo(path).isDir())
    {
        getLogger().info(QString(
                        "The path %1 does not exist or is not accessible")
                .arg(path));
        return;
    }

    getLogger().debug(
        QString("Created a VFS: path %1")
        .arg(p->dirPath));

    if (!p->dirPath.endsWith("/"))
        p->dirPath.append('/');

    p->valid = true;
}

VFS *VFSLocal::subVFS(const QString &subdir,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    if (logger == Logger::nullPtr())
        logger = getLoggerPtr();
    VFSLocal *result = new VFSLocal(rootLocation(), p->readOnly, logger);
    p->subVFS(result->p, subdir);
    if (!QFileInfo(result->p->dirPath).isDir())
    {
        result->getLogger().info(QString(
                        "The path %1 does not exist or is not accessible")
                .arg(result->p->dirPath));
        return result;
    }
    result->getLogger().debug(QString("Opening the subVFS %1 ...")
        .arg(result->p->dirPath));
    result->p->valid = true;
    return result;
}

QIODevice *VFSLocal::openFile(const QString &_name, int mode) const
{
    QString name = p->openFile(_name, mode);
    if (name.isNull())
        return new QFile;

    QFile *result = new QFile(name);
    result->open(mode);
    return result;
}

bool VFSLocal::mkdir(const QString &_subdir) const
{
    QString subdir = p->mkdir(_subdir);
    if (subdir.isNull())
        return false;
    return QDir().mkdir(subdir);
}

bool VFSLocal::rmdir(const QString &_subdir) const
{
    QString subdir = p->rmdir(_subdir);
    if (subdir.isNull())
        return false;
    return QDir().rmdir(subdir);
}

bool VFSLocal::unlink(const QString &_name) const
{
    QString name = p->unlink(_name);
    if (name.isNull())
        return false;
    return QDir().remove(name);
}

QStringList VFSLocal::ls(const QString &_subdir /* = "." */,
    int filter /* = VFSFilter::All */) const
{
    QString subdir = p->ls(_subdir);
    if (subdir.isNull())
        return QStringList();
    QDir::FilterSpec filterMapping[3];
    filterMapping[VFSFilter::All] = QDir::All;
    filterMapping[VFSFilter::Dirs] = QDir::Dirs;
    filterMapping[VFSFilter::Files] = QDir::Files;

    QStringList result = QDir(subdir).entryList(filterMapping[filter]);
    result.remove(".");
    result.remove("..");
    return result;
}

bool VFSLocal::exists(const QString &_path,
    int filter /* = VFSFilter::All */) const
{
    QString path = p->ls(_path);
    if (path.isNull())
        return false;

    QFileInfo fi(path);
    return filter != VFSFilter::Files && fi.isDir() ||
        filter != VFSFilter::Dirs && fi.isFile();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFSLocal.cpp 1168 2005-12-12 14:48:03Z modesto $
