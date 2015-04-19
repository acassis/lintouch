# $Id: Makefile.am 1573 2006-04-13 12:57:19Z hynek $
#
#   FILE: Makefile.am -- 
# AUTHOR: Patrik Modesto <modesto@swac.cz>
#   DATE: 14 March 2006
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
				-I@LLPBASIC_TOP_SRCDIR@/include \
				-I@LLPBASIC_TOP_SRCDIR@/src \
				@APR_CFLAGS@ @APR_INCLUDES@ \
				@APU_INCLUDES@ \
				@LLPBASIC_CFLAGS@

llpdir = $(libdir)/lintouch/logging-plugins
llp_LTLIBRARIES = Console.la File.la

Console_la_SOURCES = src/file_console.c
Console_la_CFLAGS = $(COMMON_CFLAGS) -DCONSOLE_LOGGING
Console_la_LDFLAGS = @LLPBASIC_LIBS@ @APR_LIBS@ @APU_LIBS@ \
						 -module

File_la_SOURCES = src/file_console.c
File_la_CFLAGS = $(COMMON_CFLAGS)
File_la_LDFLAGS = @LLPBASIC_LIBS@ @APR_LIBS@ @APU_LIBS@ \
						 -module

noinst_PROGRAMS = check_llp-basic
check_llp_basic_SOURCES = tests/check_llp-basic.cpp
check_llp_basic_CXXFLAGS = $(COMMON_CFLAGS)
check_llp_basic_LDADD = @LLPBASIC_LIBS@ @CPPUNIT_LIBS@
check_llp_basic_LDFLAGS = -g -static
check_llp_basic_DEPENDENCIES = Console.la File.la

TESTS = check_llp-basic
SUBDIRS = .

EXTRA_DIST = TODO

all-local:	@MWOPTTARGETS@

copydeps.stamp:
	cp ../../libltvariables/.libs/libltvariables-*.dll .
	cp ../../libltcp/.libs/libltcp-*.dll .
	cp ../../libltfsm/.libs/libltfsm-*.dll .
	cp ../../lintouch-server/.libs/libltserver-*.dll .
	touch copydeps.stamp

CLEANFILES = copydeps.stamp

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 1573 2006-04-13 12:57:19Z hynek $
