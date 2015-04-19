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
				-I@LIBLTVARIABLES_TOP_SRCDIR@/include \
				@APR_CFLAGS@ @APR_INCLUDES@

lib_LTLIBRARIES = libltvariables.la
libltvariables_la_SOURCES = src/io.c src/variables.c
libltvariables_la_CFLAGS = $(COMMON_CFLAGS)
# interface number 	= 1 (increment when methods are deleted, changed, added)
# revision			= 0 (increment with new upstream version)
# age				= 1 (increment when methods are added)
libltvariables_la_LDFLAGS = @APR_LIBS@ -version-info 1:0:1

noinst_PROGRAMS = check_libltvariables
check_libltvariables_SOURCES = tests/check_libltvariables.cpp
check_libltvariables_CXXFLAGS = $(COMMON_CFLAGS)
check_libltvariables_LDADD = -L. -lltvariables @CPPUNIT_LIBS@
check_libltvariables_LDFLAGS = -g -static
check_libltvariables_DEPENDENCIES = libltvariables.la

TESTS = check_libltvariables
SUBDIRS = . include doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libltvariables.pc

EXTRA_DIST = TODO

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 1216 2006-01-09 11:46:01Z modesto $
