# $Id: Makefile.am 574 2005-04-22 08:15:51Z mman $
#
#   FILE: Makefile.am -- 
# AUTHOR: Hynek Petrak <hynek@swac.cz>
#   DATE: 07 April 2004
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
				-I@LSPAB_TOP_SRCDIR@/include \
				-I@LSPAB_TOP_SRCDIR@/src \
				@LSPAB_CFLAGS@

lspdir = $(libdir)/lintouch/server-plugins
noinst_LIBRARIES = libdf1.a libcell.a

libdf1_a_SOURCES = df1/main.c df1/common.c df1/df1.c df1/serial.c df1/read_A2.c \
	df1/calc_address.c df1/read_boolean.c df1/read_float.c df1/read_integer.c \
	df1/select_fnct.c df1/read_socket.c df1/write_AA.c df1/write_integer.c \
	df1/write_float.c df1/write_AB.c df1/write_boolean.c df1/server.c
libdf1_a_CFLAGS = $(COMMON_CFLAGS)
#libdf1_a_LDFLAGS = 

libcell_a_SOURCES = \
cell/lib/aliascheck.c \
cell/lib/attach.c \
cell/lib/cell_geterror.c \
cell/lib/cell_perror.c \
cell/lib/cpf.c \
cell/lib/cpf_data.c \
cell/lib/defs.c \
cell/lib/df1_close.c \
cell/lib/df1_create_memory.c \
cell/lib/df1_get_status.c \
cell/lib/df1_open.c \
cell/lib/df1_typed_read.c \
cell/lib/df1_word_read.c \
cell/lib/df1_word_write.c \
cell/lib/df1_read_section_size.c \
cell/lib/establish_connection.c \
cell/lib/fill_header.c \
cell/lib/get_backplane_data.c \
cell/lib/get_device_data.c \
cell/lib/get_dh_map.c \
cell/lib/get_object_config.c \
cell/lib/get_object_config_list.c \
cell/lib/get_object_details.c \
cell/lib/get_object_details_list.c \
cell/lib/get_object_size.c \
cell/lib/get_port_data.c \
cell/lib/get_program_details.c \
cell/lib/get_program_list.c \
cell/lib/get_reply_status.c \
cell/lib/get_remote_dh.c \
cell/lib/get_struct_config.c \
cell/lib/get_struct_details.c \
cell/lib/get_struct_list.c \
cell/lib/getsize.c \
cell/lib/get_element_size.c \
cell/lib/ioi.c \
cell/lib/list_interfaces.c \
cell/lib/list_ports.c \
cell/lib/list_services.c \
cell/lib/makepath.c \
cell/lib/nameconv5.c \
cell/lib/nop.c \
cell/lib/object_bit_modify_word.c \
cell/lib/object_bit_modify_byte.c \
cell/lib/read_memory_value.c \
cell/lib/read_multi_object_value.c \
cell/lib/read_object_value.c \
cell/lib/read_object_range_value.c \
cell/lib/read_struct_element.c \
cell/lib/read_tag.c \
cell/lib/readdata.c \
cell/lib/register_session.c \
cell/lib/sendConnecteddata.c \
cell/lib/sendRRdata.c \
cell/lib/senddata.c \
cell/lib/send_df1.c \
cell/lib/settimeout.c \
cell/lib/unconnected_send.c \
cell/lib/unregister_session.c \
cell/lib/who.c \
cell/lib/write_object_value.c \
cell/lib/write_object_range_value.c \
cell/lib/write_tag.c \
cell/lib/libcell.h
libcell_a_CFLAGS = $(COMMON_CFLAGS)
#libcell_a_LDFLAGS = 

TESTS = 
SUBDIRS = .

EXTRA_DIST = TODO

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 574 2005-04-22 08:15:51Z mman $
