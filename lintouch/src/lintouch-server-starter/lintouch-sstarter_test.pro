# $Id: lintouch-sstarter_test.pro 1168 2005-12-12 14:48:03Z modesto $
#
#   FILE: lintouch-runtime_test.pro --
# AUTHOR: Martin Man <mman@swac.cz>
#   DATE: 12 July 2004
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
TARGET = check_lintouch-sstarter
CONFIG += console static

DEFINES += QT_FATAL_ASSERT

SOURCES = \
	tests/check_sstarter.cpp \

HEADERS =  \

LIBS += $${TC_LFLAGS}

LIBS += $${CPPUNIT_LIBS}
INCLUDEPATH += $${CPPUNIT_INCLUDEPATH}

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: lintouch-sstarter_test.pro 1168 2005-12-12 14:48:03Z modesto $
