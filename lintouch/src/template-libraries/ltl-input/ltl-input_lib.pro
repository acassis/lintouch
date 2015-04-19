# $Id: ltl-input_lib.pro 1205 2006-01-05 14:31:38Z mman $
#
#   FILE: ltl-input.pro -- 
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
TARGET = ltl-input

target.path = $(libdir)/lintouch/template-libraries
win32 {
	target.files = build/ltl-input.dll
}

locales.path = $(datadir)/lintouch/locale
locales.files = locale/*.qm

INSTALLS += target locales

SOURCES = \
	src/Button.cpp \
	src/LineInput.cpp \
	src/LTLInput.cpp \
	src/Slider.cpp \

HEADERS = \
	src/Button.hh \
	src/LineInput.hh \
	src/LTLInput.hh \
	src/Slider.hh \

TRANSLATIONS = \
	locale/ltl-input_cs.ts \
	locale/ltl-input_de.ts \

QMAKE_LFLAGS += $${LTL_LFLAGS}

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: ltl-input_lib.pro 1205 2006-01-05 14:31:38Z mman $
