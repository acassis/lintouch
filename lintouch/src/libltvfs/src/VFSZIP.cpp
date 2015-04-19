// $Id: VFSZIP.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: VFSZIP.cpp --
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

#include <stdio.h>
#include <qbuffer.h>
#include <qfile.h>
#include <qdir.h>
#include <lt/zip/qunzip.hh>
#include <lt/zip/qzip.hh>
#include <lt/vfs/VFSdefs.hh>
#include <lt/vfs/VFSZIP.hh>
#include "VFSProtected.hh"

using namespace lt;

struct lt::VFSZIPPrivate
    : public LogEnabled
{
    /**
     * Path to the ZIP file if the VFS within a ZIP file; always starting
     * with slash (or drive or whatever). Empty otherwise.
     */
    QString zipFileName;

    /**
     * Repack the zip file while omitting the specified file.
     */
    bool repack(QString omit) const;

    /**
     * Reformat the path from the VFS format (i.e. /path) to the file format
     * used within ZIP archivers (i.e. path)
     */
    QString ZIPFormatFile(const QString &path) const;

    /**
     * Reformat the path from the VFS format (i.e. /path) to the dir format
     * used within ZIP archivers (i.e. path/)
     */
    QString ZIPFormatDir(const QString &path) const;

    /**
     * Return listing of the ZIP file in the ZIP format. The success
     * indicates the success of the operation.
     */
    QStringList list(bool &success) const;

    /**
     * Constructor.
     */
    VFSZIPPrivate(LoggerPtr logger = Logger::nullPtr());
};

VFSZIPPrivate::VFSZIPPrivate(LoggerPtr logger /* = Logger::nullPtr() */)
{
    enableLogging(logger);
}

QString VFSZIPPrivate::ZIPFormatFile(const QString &path) const
{
    return path.startsWith("/") ? path.right(path.length()-1) : path;
}

class ZipQFile:
    public QBuffer, public LogEnabled
{
    /**
     * Files inside ZIP archives need a different approach when
     * accessed via QIODevice.
     * Pointer-based class.
     */

    public:
        /**
         * Constructor.
         */
        ZipQFile(VFSZIPPrivatePtr t, const QString &name,
            LoggerPtr = Logger::nullPtr());

        /**
         * Descructor. It closes the file.
         */
        virtual ~ZipQFile();

        /**
         * When a file inside a ZIP archive is written to, it has to be
         * compressed and added to the archive when closed. This class
         * overrides the close method for the this to work.
         */
        virtual void close();

    private:
        /**
         * Private ass op to prevent copying.
         */
        ZipQFile& operator =(const ZipQFile &);

        /**
         * Private cpy ctor to prevent copying.
         */
        ZipQFile(const ZipQFile &);

        VFSZIPPrivatePtr t;
        QString name;
};

QString VFSZIPPrivate::ZIPFormatDir(const QString &path) const
{
    return ZIPFormatFile(path) + (path.endsWith("/") ? "" : "/");
}


bool VFSZIPPrivate::repack(QString omit) const
{
    QFile zipFile(zipFileName);
    zipFile.open(IO_ReadOnly);
    if (!zipFile.isOpen())
        return false;

    QUnZip u;
    if (!u.open(&zipFile))
        return false;

    QBuffer newZip;
    newZip.open(IO_ReadWrite);

    QZip z;
    if (!z.open(&newZip))
        return false;

    int filescount = 0;
    if (u.gotoFirstFile())
    {
        do
        {
            QString filename;
            QDateTime dt;
            if (!u.getCurrentFileInfo(filename, &dt))
                return false;
            if (filename == omit)
                continue;

            QBuffer raw;
            raw.open(IO_WriteOnly);
            int method;
            unsigned long crc, uncompressed_size;
            if (!u.getCurrentFileRaw(raw, method, crc, uncompressed_size))
                return false;
            raw.close();
            raw.open(IO_ReadOnly);
            if (!z.storeFileRaw(filename, raw, dt, method, crc,
                uncompressed_size))
                return false;
            filescount++;
        }
        while (u.gotoNextFile());
    }

    z.close();
    u.close();
    zipFile.close();

    if (filescount == 0)
    {
        QDir().remove(zipFileName);
        return true;
    }
    if (!zipFile.open(IO_WriteOnly))
        return false;
    zipFile.writeBlock(newZip.buffer());
    return true;
}

QStringList VFSZIPPrivate::list(bool &success) const
{
    success = true;
    if (!QDir().exists(zipFileName))
        return QStringList();

    QFile zipFile(zipFileName);
    zipFile.open(IO_ReadOnly);
    if (!zipFile.isOpen())
    {
        getLogger().warn("Cannot open the zip file");
        success = false;
        return QStringList();
    }

    QUnZip u;
    if (!u.open(&zipFile))
    {
        getLogger().warn("Invalid format of the zip file");
        success = false;
        return QStringList();
    }


    QStringList result;
    do
    {
        QString name;
        if (!u.getCurrentFileInfo(name))
            continue;

        result.append(name);
    }
    while (u.gotoNextFile());

    return result;
}


ZipQFile::ZipQFile(VFSZIPPrivatePtr _t, const QString &_name,
    LoggerPtr logger /* = Logger::nullPtr() */)
    : t(_t), name(_name)
{
    enableLogging(logger);
}

ZipQFile::~ZipQFile()
{
    close();
}

void ZipQFile::close()
{
    if (!isOpen() || !isWritable())
        return;

    t->repack(name);
    bool existed = QDir().exists(t->zipFileName);

    QFile zipFile(t->zipFileName);
    zipFile.open(IO_ReadWrite);
    if (!zipFile.isOpen())
    {
        getLogger().warn("Cannot open the zip file");
        return;
    }

    QZip z;
    if (!z.open(&zipFile, existed))
    {
        getLogger().warn("!!Cannot open the zip file for writing - invalid format");
        return;
    }

    QBuffer::close();
    open(IO_ReadOnly);
    if (!z.storeFile(name, *this))
    {
        getLogger().warn("Error while storing the file inside the archive");
        return;
    }
}

VFSZIP::VFSZIP(const QUrl &rootLocation, bool readOnly /* = true */,
    LoggerPtr logger /* = Logger::nullPtr() */)
    : VFS(rootLocation, readOnly, logger), d(new VFSZIPPrivate)
{
    if (rootLocation.protocol() != "file")
    {
        getLogger().info(QString("The URL %1 is not supported by VFSLocal")
                .arg(rootLocation));
        return;
    }

    QString path = QDir(rootLocation.path()).absPath();
    if (path.endsWith("/"))
        path.truncate(path.length());

    //look for the zip file
    int end = -1;
    QString existing;
    for (existing = path.section('/', 0, end);
        existing.length() && !QFile(existing).exists();
        --end)
        existing = path.section('/', 0, end);

    if (existing.length() == 0)
    {
        getLogger().info(QString("Invalid path specified: %1")
            .arg(rootLocation.path()));
        return;
    }

    if (p->readOnly && !QFileInfo(existing).isFile())
    {
        getLogger().info(QString("Invalid path specified: %1 does not"
            " exist or is not a regular file")
            .arg(rootLocation.path()));
        return;
    }

    if (p->readOnly || QFileInfo(existing).isFile())
    {
        d->zipFileName = existing;
        p->dirPath = end == -1 ? QString() : path.section('/', end+2);
        if (!p->dirPath.endsWith("/"))
            p->dirPath.append('/');
        if (!p->dirPath.startsWith("/"))
            p->dirPath.prepend('/');

        p->valid = true;
        if (!exists("/", VFSFilter::Dirs))
        {
            p->valid = false;
            getLogger().info(QString("The specified path %1 does not "
                "exist within the archive").arg(p->dirPath));
            return;
        }
        p->valid = false;
    }
    else
    { //write access required and the archive does not exist
        end += 2;
        d->zipFileName = path.section('/', 0, end);
        if (end != -1)
        {
            getLogger().info(QString("Specified an invalid path."
                " The archive does not exist and so the path %1 inside"
                " the archive is not allowed.")
                .arg(path.section('/', end+1)));
            return;
        }
        p->dirPath = "/";
    }

    getLogger().debug(
        QString("Created a VFS: ZIP file %1, the inner path %2")
        .arg(d->zipFileName).arg(p->dirPath));

    p->valid = true;
}

VFS *VFSZIP::subVFS(const QString &subdir,
    LoggerPtr logger /* = Logger::nullPtr() */)
{
    if (logger == Logger::nullPtr())
        logger = getLoggerPtr();
    VFSZIP *result = new VFSZIP(rootLocation(), p->readOnly, logger);
    result->d = VFSZIPPrivatePtr(new VFSZIPPrivate(*d));
    p->subVFS(result->p, subdir);
    result->p->valid = true;
    bool b = result->p->readOnly
        || QFileInfo(result->d->zipFileName).isFile();
    if (b && !result->exists("/", VFSFilter::Dirs)
        || !b && result->p->dirPath != "/")
    {
        result->p->valid = false;
        result->getLogger().info(QString("The specified path %1 does not "
            "exist within the archive").arg(result->p->dirPath));
        return result;
    }
    result->getLogger().debug(QString("Opening the subVFS %1 ...")
        .arg(result->p->dirPath));
    return result;
}

QUrl VFSZIP::rootLocation() const
{
    return d->zipFileName + p->dirPath;
}

QIODevice *VFSZIP::openFile(const QString &_name, int mode) const
{
    QString name = p->openFile(_name, mode);
    if (name.isNull())
        return new QFile;

    name = d->ZIPFormatFile(name);
    if (mode == IO_ReadOnly)
    {
        QFile zipFile(d->zipFileName);
        zipFile.open(IO_ReadOnly);
        if (!zipFile.isOpen())
            return new QFile;

        QUnZip u;
        if (!u.open(&zipFile))
            return new QFile;

        //remove the prepending slash
        if (!u.locateFile(name))
            return new QFile;

        QBuffer *result = new QBuffer;
        result->open(IO_WriteOnly);
        if (!u.getCurrentFile(*result))
            return new QFile;
        if (result->isOpen())
            result->close();
        result->open(mode);

        return result;
    }

    if (mode != IO_WriteOnly)
    {
        getLogger().warn("Unsupported openFile mode");
        return new QFile;
    }

    //IO_WriteOnly
    bool existed = QDir().exists(d->zipFileName);
    bool success;
    QStringList listing = d->list(success);

    if (!success)
    {
        return new QFile;
    }

    if (listing.findIndex(name) == -1)
    {   //there is no such file inside the archive
        int parentLen = name.findRev('/', -2);
        if (parentLen != -1)
        {   //creating a non top-level file
            //check if the parent nameectory exists
            QString parent = name.left(parentLen+1);
            if (listing.findIndex(parent) == -1)
            {
                getLogger().warn("The parent directory does not exist");
                return new QFile;
            }
        }
    }
    else if (!existed)
    {
        QFile zipFile(d->zipFileName);
        zipFile.open(IO_ReadWrite);
        if (!zipFile.isOpen())
        {
            getLogger().warn("Cannot open the zip file");
            return new QFile;
        }
        zipFile.close();
        QDir().remove(d->zipFileName);
    }

    ZipQFile *result = new ZipQFile(d, name);
    result->open(IO_WriteOnly);
    if (!result->isOpen())
    {
        delete result;
        return new QFile;
    }
    return result;
}

bool VFSZIP::mkdir(const QString &_dir) const
{
    QString dir = p->mkdir(_dir);
    if (dir.isNull())
        return false;

    //format the dir to use it within the ZIP naming convention
    dir = d->ZIPFormatDir(dir);

    //check if creating the root
    if (dir.isEmpty())
        return false;

    bool existed = QDir().exists(d->zipFileName);
    bool success;
    QStringList listing = d->list(success);

    if (!success)
        return false;

    if (listing.findIndex(dir) != -1)
    {
        getLogger().warn("The directory already exists");
        return false;
    }

    int parentLen = dir.findRev('/', -2);
    if (parentLen != -1)
    {   //creating a non top-level directory
        //check if the parent directory exists
        QString parent = dir.left(parentLen+1);
        if (listing.findIndex(parent) == -1)
        {
            getLogger().warn("The parent directory does not exist");
            return false;
        }
    }

    QFile zipFile(d->zipFileName);
    zipFile.open(IO_ReadWrite);
    if (!zipFile.isOpen())
    {
        getLogger().warn("Cannot open the zip file");
        return false;
    }

    QZip z;
    if (!z.open(&zipFile, existed))
    {
        getLogger().warn("Cannot open the zip file for writing - "
            "invalid format");
        return false;
    }

    QBuffer emptyBuffer;
    emptyBuffer.open(IO_ReadOnly);
    return z.storeFile(dir, emptyBuffer);
}

bool VFSZIP::rmdir(const QString &_dir) const
{
    QString dir = p->rmdir(_dir);
    if (dir.isNull())
        return false;

    //format the dir to use it within the ZIP naming convention
    dir = d->ZIPFormatDir(dir);

    bool success;
    QStringList listing = d->list(success);

    if (!success)
        return false;

    if (listing.findIndex(dir) == -1)
    {
        getLogger().warn("The directory does not exist");
        return false;
    }

    for (QStringList::const_iterator it =
            const_cast<QStringList&>(listing).begin();
            it != const_cast<QStringList&>(listing).end(); ++it)
        if ((*it).startsWith(dir) && (*it) != dir)
        {
            getLogger().warn("The directory is not empty");
            return false;
        }

    return d->repack(dir);
}

bool VFSZIP::unlink(const QString &_filename) const
{
    QString filename = p->unlink(_filename);
    if (filename.isNull())
        return false;

    //format the dir to use it within the ZIP naming convention
    filename = d->ZIPFormatFile(filename);

    bool success;
    QStringList listing = d->list(success);

    if (!success)
        return false;

    if (listing.findIndex(filename) == -1)
    {
        getLogger().warn("The file does not exist");
        return false;
    }

    return d->repack(filename);
}

QStringList VFSZIP::ls(const QString &_dir /* = "." */,
    int filter /* = VFSFilter::All */) const
{
    QString subdir = p->ls(_dir);
    if (subdir.isNull())
        return QStringList();

    bool success;
    QStringList listing = d->list(success);
    if (!success || listing.isEmpty())
        return QStringList();

    QString dir = d->ZIPFormatDir(subdir), file = d->ZIPFormatFile(subdir);
    QStringList result;
    for (unsigned int i = 0; i < listing.count(); i++)
    {
        QString name = listing[i];
        if (listing[i].startsWith(dir))
        {
            if (name.endsWith("/"))
            {   //directory
                if (filter == VFSFilter::Files || name == dir)
                    continue;
                name.remove(name.length()-1, 1);
            }
            else
            {   //file
                if (filter == VFSFilter::Dirs)
                    continue;
            }
            name.remove(0, dir.length());
            if (name.find('/') != -1)
                continue;
            result.append(name);
        }
        else if (name == file && filter != VFSFilter::Dirs)
            result.append(name);
    }
    return result;
}

bool VFSZIP::exists(const QString &_path,
    int filter /* = VFSFilter::All */) const
{
    QString path = p->ls(_path);
    if (path.isNull())
        return false;

    //the root path is not listed
    if (filter != VFSFilter::Files && path == "/")
        return true;

    bool success;
    QStringList listing = d->list(success);
    if (!success)
        return false;

    bool result = false;
    if (filter != VFSFilter::Files)
    {
        QString dir = d->ZIPFormatDir(path);
        if (dir.isEmpty() || listing.findIndex(dir) != -1)
            result = true;
    }
    if (!result && filter != VFSFilter::Dirs)
    {
        QString file = d->ZIPFormatFile(path);
        if (listing.findIndex(file) != -1)
            result = true;
    }

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFSZIP.cpp 1168 2005-12-12 14:48:03Z modesto $
