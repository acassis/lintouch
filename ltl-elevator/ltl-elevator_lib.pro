# $Id: ltl-elevator_lib.pro 951 2005-07-14 12:13:06Z modesto $
#
#   FILE: ltl-elevator.pro --
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
TARGET = ltl-elevator

target.path = $(libdir)/lintouch/template-libraries
win32 {
	target.files = build/ltl-elevator.dll
}

locales.path = $(datadir)/lintouch/locale
locales.files = locale/*.qm

INSTALLS += target locales

SOURCES = \
  src/LTLElevator.cpp \
  src/Lift.cpp \
  src/Conveyor.cpp \
  src/ImageButton.cpp

HEADERS = \
	src/LTLElevator.hh \
	src/Conveyor.hh \
  src/ImageButton

TRANSLATIONS = \
	locale/ltl-elevator_cs.ts \
	locale/ltl-elevator_de.ts \

QMAKE_LFLAGS += $${LTL_LFLAGS}

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: ltl-elevator_lib.pro 951 2005-07-14 12:13:06Z modesto $
