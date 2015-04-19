# $Id: libltzip_lib.pro,v 1.4 2004/09/16 12:51:49 mman Exp $
#
#   FILE: libltzip_lib.pro -- 
# AUTHOR: Hynek Petrak <hynek@swac.cz>
#   DATE: 9 November 2003
#
# Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
# Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
#
# THIS SOFTWARE IS DISTRIBUTED AS IS AND WITHOUT ANY WARRANTY.
#
# This application is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This application is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this application; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

include(./config_unix.pri)

TEMPLATE = lib
CONFIG += dll
TARGET = ltzip
VERSION = 0.0.0

unix {
	target.path = $(libdir)
	target1.path = $(libdir)
}
win32 {
	target.path = $(bindir)
	target1.path = $(libdir)
	target.files = build/ltzip???.dll
	target1.extra = $(COPY_FILE) build/libltzip.a ${libdir}
}

headers.path = $(includedir)/lt/zip
headers.files = include/lt/zip/*.hh

INSTALLS += target target1 headers

SOURCES = \
		src/ioapi.c \
		src/unzip.c \
		src/zip.c \
		src/qtioapi.cpp \
		src/qunzip.cpp \
		src/qzip.cpp \

HEADERS = \
		src/ioapi.h \
		src/unzip.h \
		src/zip.h \
		src/qtioapi.h \
		include/lt/zip/qunzip.hh \
		include/lt/zip/qzip.hh \

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: libltzip_lib.pro,v 1.4 2004/09/16 12:51:49 mman Exp $
