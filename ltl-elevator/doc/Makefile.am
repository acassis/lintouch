# $Id: Makefile.am 950 2005-07-14 11:26:30Z modesto $
#
#   FILE: Makefile.am -- 
# AUTHOR: Martin Man <mman@swac.cz>
#   DATE: 30 July 2003
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

export top_srcdir

all : doc

doc : doc.stamp

doc.stamp : ltl-elevator.dox
	test -z "@DOXYGEN@" || @DOXYGEN@ ltl-elevator.dox
	touch doc.stamp

CLEANFILES = doc.stamp html/* latex/*

EXTRA_DIST = ltl-elevator.dox.in

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 950 2005-07-14 11:26:30Z modesto $
