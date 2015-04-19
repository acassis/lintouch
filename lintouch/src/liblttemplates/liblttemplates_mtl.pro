# $Id: liblttemplates_mtl.pro 1168 2005-12-12 14:48:03Z modesto $
#
#   FILE: liblttemplates_mtl.pro -- 
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
CONFIG += plugin dll
TARGET = mytemplatelibrary
QMAKE_LINK_OBJECT_SCRIPT = object_ld_script_mtl
QMAKE_LINK_OBJMOC_SCRIPT = objmoc_ld_script_mtl

unix {
	target.path = $(libdir)/lintouch/template-libraries
}
win32 {
	target.path = $(prefix)/template-libraries
	target.files = build/mytemplatelibrary.dll
}

locales.path = $(datadir)/lintouch/locale
locales.files = mytemplatelibrary/locale/*.qm

INSTALLS += target locales

SOURCES = \
	mytemplatelibrary/Buttona.cpp \
	mytemplatelibrary/Imagea.cpp \
	mytemplatelibrary/Lampa.cpp \
	mytemplatelibrary/MyTemplateLibrary.cpp \
	mytemplatelibrary/SimpleTrafficLights.cpp \

HEADERS = \
	mytemplatelibrary/Buttona.hh \
	mytemplatelibrary/Imagea.hh \
	mytemplatelibrary/Lampa.hh \
	mytemplatelibrary/MyTemplateLibrary.hh \
	mytemplatelibrary/SimpleTrafficLights.hh \

TRANSLATIONS = \
	mytemplatelibrary/locale/mytemplatelibrary_cs.ts \
	mytemplatelibrary/locale/mytemplatelibrary_de.ts \

QMAKE_LFLAGS += $${TC_LFLAGS}
LIBS += -L$${DESTDIR} -llttemplates

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: liblttemplates_mtl.pro 1168 2005-12-12 14:48:03Z modesto $
