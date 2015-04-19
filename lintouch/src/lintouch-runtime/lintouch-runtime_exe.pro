# $Id: lintouch-runtime_exe.pro 1168 2005-12-12 14:48:03Z modesto $
#
#   FILE: lintouch-runtime_exe.pro --
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

TEMPLATE     = app
TARGET = lintouch-runtime
INCLUDEPATH += src

unix {
	target.path = $(bindir)
}
win32 {
	target.path = $(bindir)
	target.files = build/lintouch-runtime.exe
}

locales.path = $(datadir)/lintouch/locale
locales.files = locale/*.qm

INSTALLS += target locales

SOURCES	= \
	src/Application.cpp \
	src/CommunicationProtocolAdaptor.cpp \
	src/main.cpp \
	src/MainWindow.cpp \
	src/ServicePanel.cpp \

HEADERS	= \
	src/Application.hh \
	src/CommunicationProtocolAdaptor.hh \
	src/MainWindow.hh \
	src/ServicePanel.hh \

TRANSLATIONS = \
	locale/runtime_cs.ts \
	locale/runtime_de.ts \

FORMS = \
	src/MainWindowBase.ui \
	src/ServicePanelBase.ui \

IMAGES = \
	src/images/logger.png \
	src/images/stock_about.png \
	src/images/stock_cancel.png \
	src/images/stock_connect.png \
	src/images/stock_disconnect.png \
	src/images/stock_dnd_multiple.png \
	src/images/stock_down.png \
	src/images/stock_exit.png \
	src/images/stock_help.png \
	src/images/stock_home_24.png \
	src/images/stock_index.png \
	src/images/stock_left.png \
	src/images/stock_right.png \
	src/images/stock_up.png \
  
RC_FILE = src/lintouch-runtime.rc

QMAKE_LFLAGS += $${EXE_LFLAGS} $${TC_LFLAGS}

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: lintouch-runtime_exe.pro 1168 2005-12-12 14:48:03Z modesto $
