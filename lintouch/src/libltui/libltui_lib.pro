# $Id: libltui_lib.pro,v 1.4 2004/09/16 12:51:49 mman Exp $
#
#   FILE: libltui_lib.pro -- 
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
TARGET = ltui
VERSION = 0.0.0

unix {
	target.path = $(libdir)
	target1.path = $(libdir)
}
win32 {
	target.path = $(bindir)
	target1.path = $(libdir)
	target.files = build/ltui???.dll
	target1.extra = $(COPY_FILE) build/libltui.a ${libdir}
}

headers.path = $(includedir)/lt/ui
headers.files = include/lt/ui/*.hh include/lt/ui/*.h

INSTALLS += target target1 headers

INCLUDEPATH += src

SOURCES = \
	src/Console.cpp \
	src/PanelViewContainer.cpp \
	src/PanelView.cpp \
	src/RuntimeController.cpp \
	src/ServerController.cpp \
	src/TemplatePalette.cpp \
	src/ui.cpp \
	src/Undo.cpp \

HEADERS = \
	include/lt/ui/Console.hh \
	include/lt/ui/PanelViewContainer.hh \
	include/lt/ui/PanelView.hh \
	include/lt/ui/RuntimeController.hh \
	include/lt/ui/ServerController.hh \
	include/lt/ui/TemplatePalette.hh \
	include/lt/ui/ui.h \
	include/lt/ui/Undo.hh \

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: libltui_lib.pro,v 1.4 2004/09/16 12:51:49 mman Exp $
