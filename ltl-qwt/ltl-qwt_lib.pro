# $Id: ltl-qwt_lib.pro 357 2005-02-24 12:51:23Z mman $
#
#   FILE: ltl-qwt.pro -- 
# AUTHOR: Patrik Modesto <modesto@swac.cz>
#   DATE: 04 November 2003
#
# Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
# Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
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
TARGET = ltl-qwt

target.path = $(libdir)/lintouch/template-libraries
win32 {
	target.files = build/ltl-qwt.dll
}

locales.path = $(datadir)/lintouch/locale
locales.files = locale/*.qm

INSTALLS += target locales

SOURCES = \
	src/LTLQwt.cpp \
	src/QWTSlider.cpp \
	src/QWTWheel.cpp \
	src/QWTThermo.cpp \
	src/QWTScale.cpp \
	src/QWTActiveText.cpp \
	src/QWTDial.cpp \
	src/QWTButton.cpp \
	src/QWTAnalogClock.cpp \
	src/QWTKnob.cpp \
	src/QWTPlot.cpp \

HEADERS = \
	src/LTLQwt.hh \
	src/QWTSlider.hh \
	src/QWTWheel.hh \
	src/QWTThermo.hh \
	src/QWTScale.hh \
	src/QWTActiveText.hh \
	src/QWTDial.hh \
	src/QWTButton.hh \
	src/QWTAnalogClock.hh \
	src/QWTKnob.hh \
	src/QWTPlot.hh \

TRANSLATIONS = \
	locale/ltl-qwt_cs.ts \
	locale/ltl-qwt_de.ts \

QMAKE_LFLAGS += $${LTL_LFLAGS}

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: ltl-qwt_lib.pro 357 2005-02-24 12:51:23Z mman $
