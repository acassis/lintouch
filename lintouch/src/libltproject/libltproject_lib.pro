# $Id: libltproject_lib.pro 1556 2006-04-04 12:38:17Z modesto $
#
#   FILE: libltproject_lib.pro -- 
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
TARGET = ltproject
VERSION = 1.0.0

INCLUDEPATH += src

unix {
	target.path = $(libdir)
	target1.path = $(libdir)
}
win32 {
	target.path = $(bindir)
	target1.path = $(libdir)
	target.files = build/ltproject???.dll
	target1.extra = $(COPY_FILE) build/libltproject.a ${libdir}
}

headers.path = $(includedir)/lt/project
headers.files = include/lt/project/*.hh include/lt/project/*.h

INSTALLS += target target1 headers

SOURCES = \
	src/ConnectionBuilder.cpp \
	src/DefaultTemplate.cpp \
	src/InfoBuilder.cpp \
	src/ltproject.c \
	src/PanelBuilder.cpp \
	src/Panel.cpp \
	src/ProjectAsyncBuilder.cpp \
	src/ProjectAsyncBuilderPrivate.cpp \
	src/ProjectBuilder.cpp \
	src/ProjectBuilderProtected.cpp \
	src/Project.cpp \
	src/ResourceManager.cpp \
	src/TemplateBuilder.cpp \
	src/TemplateManagerBuilder.cpp \
	src/VariableBuilder.cpp \
	src/ViewBuilder.cpp \
	src/View.cpp \

HEADERS = \
	include/lt/project/ConnectionBuilder.hh \
	include/lt/project/InfoBuilder.hh \
	include/lt/project/PanelBuilder.hh \
	include/lt/project/Panel.hh \
	include/lt/project/ProjectAsyncBuilder.hh \
	include/lt/project/ProjectBuilder.hh \
	include/lt/project/ProjectBuilderProtected.hh \
	include/lt/project/project.h \
	include/lt/project/Project.hh \
	include/lt/project/ResourceManager.hh \
	include/lt/project/TemplateBuilder.hh \
	include/lt/project/TemplateManagerBuilder.hh \
	include/lt/project/VariableBuilder.hh \
	include/lt/project/ViewBuilder.hh \
	include/lt/project/View.hh \
	src/DefaultTemplate.hh \
	src/ProjectAsyncBuilderPrivate.hh \

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: libltproject_lib.pro 1556 2006-04-04 12:38:17Z modesto $
