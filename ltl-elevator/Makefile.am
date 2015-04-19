# $Id: Makefile.am 952 2005-07-14 12:41:07Z modesto $
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

# export varaibles needed by qmake
QMAKE=@QTDIR@/bin/qmake
export QMAKE
QTDIR=@QTDIR@
export QTDIR

export prefix
export includedir
export libdir
export datadir
export LTLELEVATOR_TOP_SRCDIR
export top_srcdir
export top_builddir

# wrappers for qmake targets
all-local:Makefile_lib Makefile_test @MWOPTTARGETS@
	$(MAKE) -f Makefile_lib all
	$(MAKE) -f Makefile_test all
	$(QMAKE) -o Makefile_lib ltl-elevator_lib.pro

clean-local:Makefile_lib Makefile_test
	$(MAKE) -f Makefile_lib clean
	$(MAKE) -f Makefile_test clean
	rm -rf Makefile_*
	rm -rf build/ copydeps.stamp

install-exec-local:Makefile_lib
	$(MAKE) check
	$(MAKE) -f Makefile_lib install INSTALL_ROOT=$(DESTDIR)

uninstall-local:Makefile_lib
	$(MAKE) -f Makefile_lib uninstall INSTALL_ROOT=$(DESTDIR)

# makefile regeneration
Makefile%:				ltl-elevator%.pro $(wildcard *.pri)
	@QTDIR@/bin/lrelease $(LTLELEVATOR_TOP_SRCDIR)/ltl-elevator_lib.pro
	$(QMAKE) -o $@ $<

dist-hook:
	-find $(distdir) -name .svn -type d -exec rm -rf {} \;

copydeps.stamp:
	touch copydeps.stamp

TESTS = build/check_ltl-elevator

SUBDIRS = . doc

EXTRA_DIST = TODO src tests locale \
	ltl-elevator_lib.pro \
	ltl-elevator_test.pro

CLEANFILES = copydeps.stamp

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 952 2005-07-14 12:41:07Z modesto $
