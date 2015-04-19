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
			  qat-product-cleanup \
			  qat-product-install \
			  qat-product-release \
			  qat-product-release-deb \
			  qat-product-release-predeb \
			  qat-product-release-mingw \
			  qat-product-release-premingw \
			  qat-product-release-rpm \
			  qat-product-release-prerpm \
			  qat-product-snapshot \
			  qat-product-snapshot-deb \
			  qat-product-snapshot-mingw \
			  qat-product-snapshot-rpm \
			  qat-product-svn-branch \
			  qat-product-svn-branch-deb \
			  qat-product-svn-branch-mingw \
			  qat-product-svn-branch-rpm \
			  qat-product-svn-release \
			  qat-product-svn-release-deb \
			  qat-product-svn-release-mingw \
			  qat-product-svn-release-rpm \
			  qat-suite-snapshot \
			  qat-suite-release \
			  qat-suite-snapshot-deb \
			  qat-suite-snapshot-mingw \
			  qat-suite-snapshot-rpm \
			  qat-suite-release-deb \
			  qat-suite-release-mingw \
			  qat-suite-release-rpm

pkgdata_DATA = functions \
			   default-deb-builder.conf \
			   default-email-reporting.conf \
			   default-mingw-builder.conf \
			   default-rpm-builder.conf \
			   default-svn-layout.conf \
			   dummy-product.product \
			   dummy-product.suite

EXTRA_DIST = TODO \
			 $(pkgdata_DATA)

SUBDIRS = contrib

ACLOCAL_AMFLAGS = -I config

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 593 2005-04-27 09:46:33Z mman $
