// $Id: qzip.cpp,v 1.5 2004/08/11 20:17:55 jbar Exp $
//
//   FILE: qzip.cpp --
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

#include "lt/zip/qzip.hh"
#include "zip.h"
#include "qtioapi.h"

using namespace lt;

struct QZip::QZipPrivate {
    QZipPrivate() {
        _zipFile = NULL;
        fill_qt_filefunc(&_zlibFuncDef);
    }
    zipFile _zipFile;
    zlib_filefunc_def _zlibFuncDef;
    QString _comment;
};

QZip::QZip() {
    _d = new QZipPrivate();
    Q_CHECK_PTR(_d);
}

QZip::~QZip() {
    if (isOpen())
        close();
    delete _d;
}

bool QZip::open(QIODevice *d, bool append /*= false */) {
    if (!d) {
        qWarning("QZip::open Called with NULL.");
        return false;
    }
    if (!d->isReadWrite()) {
        qWarning("QZip::open File not open for read and write.");
        return false;
    }
    if (!d->isDirectAccess()) {
        qWarning("QZip::open File doesn't support seeking.");
        return false;
    }

    _d->_zlibFuncDef.opaque = d;
    _d->_zipFile = ::zipOpen2("",
            (append)?APPEND_STATUS_ADDINZIP:0,
            NULL,
            &_d->_zlibFuncDef);
    return (_d->_zipFile != NULL);
}

bool QZip::isOpen() const {
    return (_d->_zipFile != NULL);
}

void QZip::close() {
    if (!isOpen()) {
        qWarning("QZip::close File not open.");
        return;
    }
    ::zipClose(_d->_zipFile,
            _d->_comment.latin1());
    _d->_zipFile = NULL;
    _d->_comment = "";
}

bool QZip::setGlobalComment(const QString &comment) {
    if (!isOpen()) {
        qWarning("QZip::setGlobalComment File not open.");
        return false;
    }
    _d->_comment = comment;
    return true;
}

bool QZip::storeFile(const QString &name, QIODevice &file,
        const QDateTime &t /*= QDateTime::currentDateTime() */ )  {
    if (!isOpen()) {
        qWarning("QZip::storeFile File not open.");
        return false;
    }
    if (!file.isReadable()) {
        qWarning("QZip::storeFile File not open for reading.");
        return false;
    }

    zip_fileinfo zipfi;
    zipfi.dosDate = zipfi.internal_fa = zipfi.external_fa = 0;
    zipfi.tmz_date.tm_sec = t.time().second();
    zipfi.tmz_date.tm_min = t.time().minute();
    zipfi.tmz_date.tm_hour = t.time().hour();
    zipfi.tmz_date.tm_mday = t.date().day();
    zipfi.tmz_date.tm_mon = t.date().month()-1;
    zipfi.tmz_date.tm_year = t.date().year();

    if (::zipOpenNewFileInZip(_d->_zipFile,
                name.latin1(),
                &zipfi,
                NULL, // void* extrafield_local,
                0, // uInt size_extrafield_local,
                NULL, // const void* extrafield_global,
                0, // uInt size_extrafield_global,
                NULL, // const char* comment,
                Z_DEFLATED,
                Z_DEFAULT_COMPRESSION) != Z_OK) {
        qWarning("QZip::storeFile zipOpenNewFileInZip failed.");
        return false;
    }

    char *buf = new char[2048];
    Q_CHECK_PTR(buf);
    int len;
    while((len = file.readBlock(buf, 2048)) > 0) {
        ::zipWriteInFileInZip(_d->_zipFile, buf, len);
    }
    delete[] buf;
    ::zipCloseFileInZip(_d->_zipFile);
    file.close();

    if (len < 0) {
        qWarning("QZip::storeFile zipWriteInFileInZip failed.");
        return false;
    }
    return true;
}

bool QZip::storeFileRaw(const QString &name, QIODevice &file,
        const QDateTime &t, int method,
        unsigned long crc, unsigned long uncompressed_size)  {
    if (!isOpen()) {
        qWarning("QZip::storeFileRaw File not open.");
        return false;
    }
    if (!file.isReadable()) {
        qWarning("QZip::storeFileRaw File not open for reading.");
        return false;
    }

    zip_fileinfo zipfi;
    zipfi.dosDate = zipfi.internal_fa = zipfi.external_fa = 0;
    zipfi.tmz_date.tm_sec = t.time().second();
    zipfi.tmz_date.tm_min = t.time().minute();
    zipfi.tmz_date.tm_hour = t.time().hour();
    zipfi.tmz_date.tm_mday = t.date().day();
    zipfi.tmz_date.tm_mon = t.date().month()-1;
    zipfi.tmz_date.tm_year = t.date().year();

    if (::zipOpenNewFileInZip2(_d->_zipFile,
                name.latin1(),
                &zipfi,
                NULL, // void* extrafield_local,
                0, // uInt size_extrafield_local,
                NULL, // const void* extrafield_global,
                0, // uInt size_extrafield_global,
                NULL, // const char* comment,
                method,
                Z_DEFAULT_COMPRESSION,
                1) != Z_OK)
        return false;

    char *buf = new char[2048];
    Q_CHECK_PTR(buf);
    int len;
    while((len = file.readBlock(buf, 2048)) > 0) {
        ::zipWriteInFileInZip(_d->_zipFile, buf, len);
    }
    delete[] buf;
    ::zipCloseFileInZipRaw(_d->_zipFile,
            uncompressed_size, crc);
    file.close();

    if (len < 0) {
        qWarning("QZip::storeFileRaw zipWriteInFileInZip failed.");
        return false;
    }
    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: qzip.cpp,v 1.5 2004/08/11 20:17:55 jbar Exp $
