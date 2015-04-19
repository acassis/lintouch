# $Id: ltl-display_lib.pro 1205 2006-01-05 14:31:38Z mman $
#
#   FILE: ltl-display.pro -- 
# AUTHOR: Patrik Modesto <modesto@swac.cz>
#   DATE: 04 November 2003
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

include(config_unix.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = ltl-display

target.path = $(libdir)/lintouch/template-libraries
win32 {
	target.files = build/ltl-display.dll
}

locales.path = $(datadir)/lintouch/locale
locales.files = locale/*.qm

INSTALLS += target locales

SOURCES = \
	src/Chart.cpp \
	src/Image.cpp \
	src/Lamp.cpp \
	src/LTLDisplay.cpp \
	src/Meter.cpp \
	src/Number.cpp \
	src/SVGImage.cpp \
	src/Tester.cpp \
	src/Text.cpp \
	src/TextMap.cpp \

HEADERS = \
	src/Chart.hh \
	src/Image.hh \
	src/Lamp.hh \
	src/LTLDisplay.hh \
	src/Meter.hh \
	src/Number.hh \
	src/SVGImage.hh \
	src/Tester.hh \
	src/Text.hh \
	src/TextMap.hh \

TRANSLATIONS = \
	locale/ltl-display_cs.ts \
	locale/ltl-display_de.ts \

QMAKE_LFLAGS += $${LTL_LFLAGS}

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: ltl-display_lib.pro 1205 2006-01-05 14:31:38Z mman $
