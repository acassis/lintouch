// $Id: qunzip.cpp,v 1.5 2004/08/11 20:17:55 jbar Exp $
//
//   FILE: qunzip.cpp --
// AUTHOR: Hynek Petrak <hynek@swac.cz>
//   DATE: 18 November 2003
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

#include "lt/zip/qunzip.hh"
#include "unzip.h"
#include "qtioapi.h"

using namespace lt;

struct QUnZip::QUnZipPrivate {
    QUnZipPrivate() {
        _unzFile = NULL;
        fill_qt_filefunc(&_zlibFuncDef);
    }
    unzFile _unzFile;
    zlib_filefunc_def _zlibFuncDef;
};

QUnZip::QUnZip() {
    _d = new QUnZipPrivate();
    Q_CHECK_PTR(_d);
}

QUnZip::~QUnZip() {
    if (isOpen())
        close();
    delete _d;
}

bool QUnZip::open(QIODevice *d) {
    if (!d) {
        qWarning("QUnZip::open Called with NULL.");
        return false;
    }
    if (!d->isReadable()) {
        qWarning("QUnZip::open File not readable.");
        return false;
    }
    if (!d->isDirectAccess()) {
        qWarning("QUnZip::open File doesn't support seeking.");
        return false;
    }

    _d->_zlibFuncDef.opaque = d;
    _d->_unzFile = ::unzOpen2("", &_d->_zlibFuncDef);
    return (_d->_unzFile != NULL);
}

bool QUnZip::isOpen() const {
    return (_d->_unzFile != NULL);
}

void QUnZip::close() {
    if (!isOpen()) {
        qWarning("QUnZip::close File not open.");
        return;
    }

    ::unzClose(_d->_unzFile);
    _d->_unzFile = NULL;
}

int QUnZip::getFileCount() {
    if (!isOpen()) {
        qWarning("QUnZip::getFileCount File not open.");
        return 0;
    }

    unz_global_info info;
    return (::unzGetGlobalInfo(_d->_unzFile, &info) == UNZ_OK)?
        info.number_entry:0;
}

bool QUnZip::getGlobalComment(QString & comment) {
    if (!isOpen()) {
        qWarning("QUnZip::getGlobalComment File not open.");
        return false;
    }

    unz_global_info info;
    if (::unzGetGlobalInfo(_d->_unzFile, &info) != UNZ_OK) {
        qWarning("QUnZip::getGlobalComment unzGetGlobalInfo failed.");
        return false;
    }

    uLong bufsize = info.size_comment;
    char * buf = new char[bufsize];
    Q_CHECK_PTR(buf);
    if (::unzGetGlobalComment(_d->_unzFile, buf, bufsize) < 0) {
        delete[] buf;
        qWarning("QUnZip::getGlobalComment unzGetGlobalComment failed.");
        return false;
    }
    comment.setLatin1(buf, bufsize);
    delete[] buf;
    return true;
}


bool QUnZip::gotoFirstFile() {
    if (!isOpen()) {
        qWarning("QUnZip::gotoFirstFile File not open.");
        return false;
    }
    return (UNZ_OK == ::unzGoToFirstFile(_d->_unzFile));
}

bool QUnZip::gotoNextFile() {
    if (!isOpen()) {
        qWarning("QUnZip::gotoNextFile File not open.");
        return false;
    }
    return (UNZ_OK == ::unzGoToNextFile(_d->_unzFile));
}

bool QUnZip::locateFile(const QString &name,
        bool casesensitive /*= false*/) {
    if (!isOpen()) {
        qWarning("QUnZip::locateFile File not open.");
        return false;
    }
    return (UNZ_OK == ::unzLocateFile(_d->_unzFile,
                name.latin1(), (casesensitive)?1:2));
}

bool QUnZip::getCurrentFile(QIODevice &d) {
    if (!isOpen()) {
        qWarning("QUnZip::locateFile File not open.");
        return false;
    }
    if (!d.isWritable()) {
        qWarning("QUnZip::getCurrentFile File not open for writing.");
        return false;
    }
    if (unzOpenCurrentFile(_d->_unzFile) != UNZ_OK) {
        qWarning("QUnZip::getCurrentFile unzOpenCurrentFile failed.");
        return false;
    }
    char * buf = new char[1024];
    Q_CHECK_PTR(buf);
    int len;
    while ((len = ::unzReadCurrentFile(_d->_unzFile,
                buf, 1024)) > 0) {
        d.writeBlock(buf, len);
    }
    delete []buf;
    ::unzCloseCurrentFile(_d->_unzFile);
    if (d.isBuffered())
        d.flush();
    if (len == 0) {
        return true;
    }
    qWarning("QUnZip::getCurrentFile unzReadCurrentFile failed.");
    return false;
}

bool QUnZip::getCurrentFileRaw(QIODevice &d, int &method,
        unsigned long &crc, unsigned long &uncompressed_size) {
    if (!isOpen()) {
        qWarning("QUnZip::getCurrentFileRaw File not open.");
        return false;
    }
    if (!d.isWritable()) {
        qWarning("QUnZip::getCurrentFileRaw File not open for writing.");
        return false;
    }
    if (unzOpenCurrentFile2(_d->_unzFile, &method, NULL, 1) != UNZ_OK) {
        qWarning("QUnZip::getCurrentFileRaw unzOpenCurrentFile2 failed.");
        return false;
    }
    unz_file_info fi;
    if (::unzGetCurrentFileInfo(_d->_unzFile, &fi,
                NULL, 0 , NULL, 0, NULL, 0) != UNZ_OK) {
        qWarning("QUnZip::getCurrentFileRaw unzGetCurrentFileInfo failed.");
        return false;
    }
    crc = fi.crc;
    uncompressed_size = fi.uncompressed_size;

    char * buf = new char[1024];
    Q_CHECK_PTR(buf);
    int len;
    while ((len = ::unzReadCurrentFile(_d->_unzFile,
                buf, 1024)) > 0) {
        d.writeBlock(buf, len);
    }
    delete[] buf;
    ::unzCloseCurrentFile(_d->_unzFile);
    if (d.isBuffered())
        d.flush();
    if (len == 0) {
        return true;
    }
    qWarning("QUnZip::getCurrentFileRaw unzReadCurrentFile failed.");
    return false;
}

bool QUnZip::getCurrentFileInfo(QString &name,
        QDateTime *t /* = NULL */) {
    if (!isOpen()) {
        qWarning("QUnZip::getCurrentFileInfo File not open.");
        return false;
    }

    name = "";
    unz_file_info fi;
    if (::unzGetCurrentFileInfo(_d->_unzFile, &fi,
                NULL, 0 , NULL, 0, NULL, 0) != UNZ_OK) {
        qWarning("QUnZip::getCurrentFileInfo 1st unzGetCurrentFileInfo failed.");
        return false;
    }

    char *nt;
    if (fi.size_filename > 0) {
        nt = new char[fi.size_filename];
        Q_CHECK_PTR(nt);
    } else
        nt = NULL;

    if (::unzGetCurrentFileInfo(_d->_unzFile, &fi,
                nt, fi.size_filename,
                NULL, 0,
                NULL, 0)
            != UNZ_OK) {
        if (nt)
            delete []nt;
        qWarning("QUnZip::getCurrentFileInfo 2nd unzGetCurrentFileInfo failed.");
        return false;
    }
    if (nt) {
        name.setLatin1(nt, fi.size_filename);
        delete[] nt;
    }

    if (t != NULL) {
        QDate d; QTime tt;

        tt.setHMS(fi.tmu_date.tm_hour,
                fi.tmu_date.tm_min, fi.tmu_date.tm_sec);
        d.setYMD(fi.tmu_date.tm_year,
                fi.tmu_date.tm_mon+1, fi.tmu_date.tm_mday);
        *t = QDateTime(d, tt);
    }

    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: qunzip.cpp,v 1.5 2004/08/11 20:17:55 jbar Exp $
