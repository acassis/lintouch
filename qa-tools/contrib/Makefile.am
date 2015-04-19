# $Id: Makefile.am 593 2005-04-27 09:46:33Z mman $
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

bin_SCRIPTS = \
			  mwbuilder \
			  pbuilder.debian \
			  pbuilder.ubuntu \
			  rpmbuilder

pkgdata_DATA = \
			   pbuilderrc.debian-sarge \
			   pbuilderrc.ubuntu-breezy

hookdir = $(pkgdatadir)/pbuilderhooks
hook_SCRIPTS = \
			D00setuprepo.sh

EXTRA_DIST = ${bin_SCRIPTS} \
			 ${pkgdata_DATA} \
			 ${hook_SCRIPTS}

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 593 2005-04-27 09:46:33Z mman $
