# $Id: Makefile.am 348 2005-02-23 15:10:15Z mman $
#
#   FILE: Makefile.am -- 
# AUTHOR: Martin Man <mman@swac.cz>
#   DATE: 07 December 2004
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

ACLOCAL_AMFLAGS = -I config

COMMON_CFLAGS = -g -Wall -fno-strict-aliasing \
				-I@LTSERVER_TOP_SRCDIR@/include \
				-I@LTSERVER_TOP_SRCDIR@/src \
				@APR_CFLAGS@ @APR_INCLUDES@ \
				@APU_INCLUDES@ \
				@LTSERVER_CFLAGS@ -lz

bin_PROGRAMS = lintouch-server
lintouch_server_SOURCES = src/main.c
lintouch_server_CFLAGS = $(COMMON_CFLAGS)
lintouch_server_LDADD = -L. -lltserver @LTSERVER_LIBS@
lintouch_server_DEPENDENCIES = libltserver.la

lib_LTLIBRARIES = libltserver.la
libltserver_la_SOURCES = src/server.c src/client.c src/plugin.c src/logger.c \
						 src/demo_plugins.c src/ziparchive.c src/project.c \
						 src/ioapi.c src/unzip.c src/zip.c \
						 src/logging.c src/plugin_common.c
libltserver_la_CFLAGS = $(COMMON_CFLAGS)
# interface number 	= 0 (increment when methods are deleted, changed, added)
# revision			= 0 (increment with new upstream version)
# age				= 0 (increment when methods are added)
libltserver_la_LDFLAGS = @LTSERVER_LIBS@ @APR_LIBS@ @APU_LIBS@ \
						 -version-info 0:0:0

noinst_PROGRAMS = check_lintouch-server
check_lintouch_server_SOURCES = tests/check_lintouch-server.cpp
check_lintouch_server_CXXFLAGS = $(COMMON_CFLAGS)
check_lintouch_server_LDADD = -L. -lltserver @LTSERVER_LIBS@ @CPPUNIT_LIBS@
check_lintouch_server_LDFLAGS = -g -static
check_lintouch_server_DEPENDENCIES = libltserver.la

TESTS = check_lintouch-server
SUBDIRS = . include doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = lintouch-server.pc

EXTRA_DIST = TODO src/plugin-private.h src/logging-private.h \
			 src/ziparchive.h projects tests/data src/ioapi.h \
			 src/crypt.h src/unzip.h src/zip.h

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 348 2005-02-23 15:10:15Z mman $
