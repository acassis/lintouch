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
				-I@LSPMODBUS_TOP_SRCDIR@/include \
				-I@LSPMODBUS_TOP_SRCDIR@/src \
				@APR_CFLAGS@ @APR_INCLUDES@ \
				@APU_INCLUDES@ \
				@LSPMODBUS_CFLAGS@

lspdir = $(libdir)/lintouch/server-plugins
lsp_LTLIBRARIES = ModbusTCPSlave.la ModbusTCPMaster.la

ModbusTCPSlave_la_SOURCES = src/modbusslave.c src/modbus.c
ModbusTCPSlave_la_CFLAGS = $(COMMON_CFLAGS)
ModbusTCPSlave_la_LDFLAGS = @LSPMODBUS_LIBS@ @APR_LIBS@ @APU_LIBS@ \
						 -module

ModbusTCPMaster_la_SOURCES = src/modbusmaster.c src/modbus.c
ModbusTCPMaster_la_CFLAGS = $(COMMON_CFLAGS)
ModbusTCPMaster_la_LDFLAGS = @LSPMODBUS_LIBS@ @APR_LIBS@ @APU_LIBS@ \
						 -module

noinst_PROGRAMS = check_lsp-modbus
check_lsp_modbus_SOURCES = tests/check_lsp-modbus.cpp
check_lsp_modbus_CXXFLAGS = $(COMMON_CFLAGS)
check_lsp_modbus_LDADD = @LSPMODBUS_LIBS@ @CPPUNIT_LIBS@
check_lsp_modbus_LDFLAGS = -g -static
check_lsp_modbus_DEPENDENCIES = ModbusTCPSlave.la ModbusTCPMaster.la

TESTS = check_lsp-modbus
SUBDIRS = .

EXTRA_DIST = TODO src/modbus.h tests/modlib.py tests/test_modbusslave.py \
			 tests/test_modbusmaster.py projects/factory.ltp \
			 projects/beckhoff_test.ltp projects/parking.ltp

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 574 2005-04-22 08:15:51Z mman $
