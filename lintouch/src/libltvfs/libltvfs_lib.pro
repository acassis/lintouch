# $Id: libltvfs_lib.pro 1271 2006-01-11 12:08:35Z modesto $
#
#   FILE: libltvfs_lib.pro -- 
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
TARGET = ltvfs
VERSION = 0.0.0

unix {
	target.path = $(libdir)
	target1.path = $(libdir)
}
win32 {
	target.path = $(bindir)
	target1.path = $(libdir)
	target.files = build/ltvfs???.dll
	target1.extra = $(COPY_FILE) build/libltvfs.a ${libdir}
}

headers.path = $(includedir)/lt/vfs
headers.files = include/lt/vfs/*.hh

headersA.path = $(includedir)/lt/logger
headersA.files = include/lt/logger/*.hh

headersB.path = $(includedir)/lt/configuration
headersB.files = include/lt/configuration/*.hh

headersC.path = $(includedir)/lt/parameters
headersC.files = include/lt/parameters/*.hh

headersD.path = $(includedir)/lt
headersD.files = include/lt/*.hh

INSTALLS += target target1 headers headersA headersB headersC headersD

SOURCES = \
		src/configuration/AbstractConfigurable.cpp \
		src/configuration/AbstractConfiguration.cpp \
		src/configuration/DefaultConfigurationBuilder.cpp \
		src/configuration/DefaultConfiguration.cpp \
		src/configuration/DefaultConfigurationSerializer.cpp \
		src/configuration/SAXConfigurationHandler.cpp \
		src/logger/ConsoleLogger.cpp \
		src/logger/FileLogger.cpp \
		src/logger/Logger.cpp \
		src/parameters/AbstractParameterizable.cpp \
		src/parameters/Parameters.cpp \
		src/TempFiles.cpp \
		src/VFS.cpp \
		src/VFSLocal.cpp \
		src/VFSProtected.cpp \
		src/VFSZIP.cpp \

HEADERS = \
		include/lt/configuration/AbstractConfigurable.hh \
		include/lt/configuration/AbstractConfiguration.hh \
		include/lt/configuration/Configurable.hh \
		include/lt/configuration/Configuration.hh \
		include/lt/configuration/DefaultConfigurationBuilder.hh \
		include/lt/configuration/DefaultConfiguration.hh \
		include/lt/configuration/DefaultConfigurationSerializer.hh \
		include/lt/configuration/Reconfigurable.hh \
		include/lt/configuration/SAXConfigurationHandler.hh \
		include/lt/logger/ConsoleLogger.hh \
		include/lt/logger/FileLogger.hh \
		include/lt/logger/LogEnabled.hh \
		include/lt/logger/Logger.hh \
		include/lt/parameters/AbstractParameterizable.hh \
		include/lt/parameters/Parameterizable.hh \
		include/lt/parameters/Parameters.hh \
		include/lt/parameters/Reparameterizable.hh \
		include/lt/RefCounter.hh \
		include/lt/SharedPtr.hh \
		include/lt/vfs/TempFiles.hh \
		include/lt/vfs/VFSdefs.hh \
		include/lt/vfs/VFS.hh \
		include/lt/vfs/VFSLocal.hh \
		include/lt/vfs/VFSZIP.hh \

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: libltvfs_lib.pro 1271 2006-01-11 12:08:35Z modesto $
