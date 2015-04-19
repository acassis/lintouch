// $Id: qtioapi.cpp,v 1.2 2004/09/20 09:33:54 hynek Exp $
//
//   FILE: qtioapi.cpp -- 
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

#include <qiodevice.h>
#include "zlib.h"
#include "ioapi.h"
#include "qtioapi.h"

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

voidpf ZCALLBACK qt_open_file_func OF((
   voidpf opaque,
   const char* filename,
   int mode));

uLong ZCALLBACK qt_read_file_func OF((
   voidpf opaque,
   voidpf stream,
   void* buf,
   uLong size));

uLong ZCALLBACK qt_write_file_func OF((
   voidpf opaque,
   voidpf stream,
   const void* buf,
   uLong size));

long ZCALLBACK qt_tell_file_func OF((
   voidpf opaque,
   voidpf stream));

long ZCALLBACK qt_seek_file_func OF((
   voidpf opaque,
   voidpf stream,
   uLong offset,
   int origin));

int ZCALLBACK qt_close_file_func OF((
   voidpf opaque,
   voidpf stream));

int ZCALLBACK qt_error_file_func OF((
   voidpf opaque,
   voidpf stream));

voidpf ZCALLBACK 
qt_open_file_func(voidpf opaque, const char* /* filename*/, int /*mode*/) {
    QIODevice * d = (QIODevice*) opaque;
    // givven QIODevice* must be open
    if (!d || !d->isOpen())
        return NULL;
    return d;
}

uLong ZCALLBACK
qt_read_file_func(voidpf opaque, voidpf /*stream*/, void* buf, uLong size) {
    QIODevice * d = (QIODevice*) opaque;
    return d->readBlock((char *)buf, size);
}

uLong ZCALLBACK 
qt_write_file_func(voidpf opaque, voidpf /*stream*/, const void* buf, uLong size) {
    QIODevice * d = (QIODevice*) opaque;
    return d->writeBlock((const char *)buf, size);
}

long ZCALLBACK 
qt_tell_file_func(voidpf opaque, voidpf /*stream*/) {
    QIODevice * d = (QIODevice*) opaque;
    return d->at();
}

long ZCALLBACK 
qt_seek_file_func(voidpf opaque, voidpf /*stream*/, uLong offset, int origin) {
    QIODevice * d = (QIODevice*) opaque;
    QIODevice::Offset o;
    switch (origin) {
        case ZLIB_FILEFUNC_SEEK_CUR: 
            o = d->at() + offset;
            break;
        case ZLIB_FILEFUNC_SEEK_END :
            o = d->size() + offset;
            break;
        case ZLIB_FILEFUNC_SEEK_SET :
            o = offset;
            break;
        default:
        return -1;
    }
    return (d->at(o))?0:-1;
}

int ZCALLBACK 
qt_close_file_func(voidpf opaque, voidpf /*stream*/) {
    QIODevice * d = (QIODevice*) opaque;
    d->close();
    return 0;
}

int ZCALLBACK 
qt_error_file_func(voidpf opaque, voidpf /*stream*/) {
    QIODevice * d = (QIODevice*) opaque;
    return (d->status() == IO_Ok)?0:1;
}

void 
fill_qt_filefunc(zlib_filefunc_def* pzlib_filefunc_def) {
    pzlib_filefunc_def->zopen_file = qt_open_file_func;
    pzlib_filefunc_def->zread_file = qt_read_file_func;
    pzlib_filefunc_def->zwrite_file = qt_write_file_func;
    pzlib_filefunc_def->ztell_file = qt_tell_file_func;
    pzlib_filefunc_def->zseek_file = qt_seek_file_func;
    pzlib_filefunc_def->zclose_file = qt_close_file_func;
    pzlib_filefunc_def->zerror_file = qt_error_file_func;
    pzlib_filefunc_def->opaque=NULL;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: qtioapi.cpp,v 1.2 2004/09/20 09:33:54 hynek Exp $
