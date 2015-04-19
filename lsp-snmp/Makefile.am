# $Id: Makefile.am 574 2005-04-22 08:15:51Z mman $
#
#   FILE: Makefile.am -- 
# AUTHOR: Martin Man <mman@swac.cz>
#   DATE: 07 December 2004
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

ACLOCAL_AMFLAGS = -I config

COMMON_CFLAGS = -g -Wall -fno-strict-aliasing \
				-I@LSPSNMP_TOP_SRCDIR@/include \
				-I@LSPSNMP_TOP_SRCDIR@/src \
				@APR_CFLAGS@ @APR_INCLUDES@ \
				@APU_INCLUDES@ \
				@NETSNMP_CFLAGS@ \
				@LSPSNMP_CFLAGS@

lspdir = $(libdir)/lintouch/server-plugins
lsp_LTLIBRARIES = SNMP.la

SNMP_la_SOURCES = src/snmp.c
SNMP_la_CFLAGS = $(COMMON_CFLAGS)
SNMP_la_LDFLAGS = @LSPSNMP_LIBS@ @APR_LIBS@ @APU_LIBS@ @NETSNMP_LIBS@ \
						 -module

noinst_PROGRAMS = check_lsp-snmp
check_lsp_snmp_SOURCES = tests/check_lsp-snmp.cpp
check_lsp_snmp_CXXFLAGS = $(COMMON_CFLAGS)
check_lsp_snmp_LDADD = @LSPSNMP_LIBS@ @CPPUNIT_LIBS@
check_lsp_snmp_LDFLAGS = -g -static
check_lsp_snmp_DEPENDENCIES = SNMP.la

TESTS = check_lsp-snmp
SUBDIRS = .

EXTRA_DIST = TODO

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 574 2005-04-22 08:15:51Z mman $
