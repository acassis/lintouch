# $Id: libltvfs_test.pro 865 2005-06-23 11:07:30Z mman $
#
#   FILE: libltvfs_testswl.pro -- 
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
TARGET = check_libltvfsswl
CONFIG += console static

SOURCES = tests/suite.cpp \
		  tests/TestDefaultConfiguration.cpp \
		  tests/TestDefaultConfigurationBuilder.cpp \
		  tests/TestDefaultConfigurationSerializer.cpp \
		  tests/TestLogger.cpp \
		  tests/TestParameters.cpp \

LIBS += -L$${DESTDIR} -lltvfs $${TC_LFLAGS}

INCLUDEPATH += $${CPPUNIT_INCLUDEPATH}
LIBS += $${CPPUNIT_LIBS}

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: libltvfs_test.pro 865 2005-06-23 11:07:30Z mman $
