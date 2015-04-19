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
				-I@LSPBOOTSTRAP_TOP_SRCDIR@/include \
				-I@LSPBOOTSTRAP_TOP_SRCDIR@/src \
				@APR_CFLAGS@ @APR_INCLUDES@ \
				@APU_INCLUDES@ \
				@LSPBOOTSTRAP_CFLAGS@

lspdir = $(libdir)/lintouch/server-plugins
lsp_LTLIBRARIES = Loopback.la

Loopback_la_SOURCES = src/loopback.c
Loopback_la_CFLAGS = $(COMMON_CFLAGS)
Loopback_la_LDFLAGS = @LSPBOOTSTRAP_LIBS@ @APR_LIBS@ @APU_LIBS@ \
						 -module

noinst_PROGRAMS = check_lsp-bootstrap
check_lsp_bootstrap_SOURCES = tests/check_lsp-bootstrap.cpp
check_lsp_bootstrap_CXXFLAGS = $(COMMON_CFLAGS)
check_lsp_bootstrap_LDADD = @LSPBOOTSTRAP_LIBS@ @CPPUNIT_LIBS@
check_lsp_bootstrap_LDFLAGS = -g -static
check_lsp_bootstrap_DEPENDENCIES = Loopback.la

TESTS = check_lsp-bootstrap
SUBDIRS = .

EXTRA_DIST = TODO

copydeps.stamp:
	touch copydeps.stamp

CLEANFILES = copydeps.stamp

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 1216 2006-01-09 11:46:01Z modesto $
