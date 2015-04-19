# $Id: Makefile.am 1216 2006-01-09 11:46:01Z modesto $
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
				-I@LIBLTCP_TOP_SRCDIR@/include \
				-I@LIBLTCP_TOP_SRCDIR@/src \
				@APR_CFLAGS@ @APR_INCLUDES@ \
				@LIBLTCP_CFLAGS@

lib_LTLIBRARIES = libltcp.la
libltcp_la_SOURCES = src/cp.c src/http.c
libltcp_la_CFLAGS = $(COMMON_CFLAGS)
# interface number 	= 0 (increment when methods are deleted, changed, added)
# revision			= 0 (increment with new upstream version)
# age				= 0 (increment when methods are added)
libltcp_la_LDFLAGS = @LIBLTCP_LIBS@ @APR_LIBS@ -version-info 0:0:0

noinst_PROGRAMS = check_libltcp
check_libltcp_SOURCES = tests/check_libltcp.cpp
check_libltcp_CXXFLAGS = $(COMMON_CFLAGS)
check_libltcp_LDADD = -L. -lltcp @CPPUNIT_LIBS@
check_libltcp_LDFLAGS = -g -static
check_libltcp_DEPENDENCIES = libltcp.la

TESTS = check_libltcp
SUBDIRS = . include doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libltcp.pc

EXTRA_DIST = TODO src/cp-private.h

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 1216 2006-01-09 11:46:01Z modesto $
