# $Id: liblttemplates_lib.pro 1168 2005-12-12 14:48:03Z modesto $
#
#   FILE: liblttemplates_lib.pro -- 
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
TARGET = lttemplates
VERSION = 0.0.0

unix {
	target.path = $(libdir)
	target1.path = $(libdir)
}
win32 {
	target.path = $(bindir)
	target1.path = $(libdir)
	target.files = build/lttemplates???.dll
	target1.extra = $(COPY_FILE) build/liblttemplates.a $(libdir)
}

headers.path = $(includedir)/lt/templates
headers.files = include/lt/templates/*.hh include/lt/templates/*.h
headers1.path = $(includedir)/lt
headers1.files = include/lt/*.hh

INSTALLS += target target1 headers headers1

SOURCES = \
	src/Connection.cpp \
	src/Info.cpp \
	src/IOPin.cpp \
	src/Localizator.cpp \
	src/PluginHelper.cpp \
	src/PluginTemplateLibraryLoader.cpp \
	src/Property.cpp \
	src/Template.cpp \
	src/TemplateLibrary.cpp \
	src/TemplateManager.cpp \
	src/templates.c \
	src/Variable.cpp \
	src/VFSLocalizatorBuilder.cpp \
	src/VFSLocalizator.cpp \

HEADERS = \
	include/lt/LTDict.hh \
	include/lt/LTMap.hh \
	include/lt/templates/Connection.hh \
	include/lt/templates/Info.hh \
	include/lt/templates/IOPin.hh \
	include/lt/templates/Localizable.hh \
	include/lt/templates/Localizator.hh \
	include/lt/templates/PluginHelper.hh \
	include/lt/templates/PluginTemplateLibraryLoader.hh \
	include/lt/templates/Property.hh \
	include/lt/templates/Template.hh \
	include/lt/templates/TemplateLibrary.hh \
	include/lt/templates/TemplateManager.hh \
	include/lt/templates/templates.h \
	include/lt/templates/Variable.hh \
	include/lt/templates/VFSLocalizatorBuilder.hh \
	include/lt/templates/VFSLocalizator.hh \
	include/lt/templates/ViewAdaptor.hh \
	config.h \

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: liblttemplates_lib.pro 1168 2005-12-12 14:48:03Z modesto $
