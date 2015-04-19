# $Id: libltzip_test.pro,v 1.2 2004/02/15 22:21:20 mman Exp $
#
#   FILE: libltzip_test.pro -- 
# AUTHOR: Hynek Petrak <hynek@swac.cz>
#   DATE: 9 October 2003
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

TEMPLATE = app
TARGET = check_libltzip
CONFIG += console static

SOURCES = tests/check_libltzip.cpp

HEADERS = include/lt/zip/qunzip.hh

LIBS += -L$${DESTDIR} -lltzip $${TC_LFLAGS}

INCLUDEPATH += $${CPPUNIT_INCLUDEPATH}
LIBS += $${CPPUNIT_LIBS}

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: libltzip_test.pro,v 1.2 2004/02/15 22:21:20 mman Exp $
