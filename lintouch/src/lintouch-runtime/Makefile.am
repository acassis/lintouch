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

# export varaibles needed by qmake
QMAKE=@QTDIR@/bin/qmake
export QMAKE
QTDIR=@QTDIR@
export QTDIR

export prefix
export includedir
export bindir
export libdir
export datadir
export LTRUNTIME_TOP_SRCDIR
export top_srcdir
export top_builddir

# wrappers for qmake targets
all-local:				Makefile_exe Makefile_test @MWOPTTARGETS@
	$(MAKE) -f Makefile_exe all
	$(MAKE) -f Makefile_test all
	$(QMAKE) -o Makefile_exe lintouch-runtime_exe.pro

clean-local:			Makefile_exe Makefile_test
	$(MAKE) -f Makefile_exe clean
	$(MAKE) -f Makefile_test clean
	rm -rf Makefile_*
	rm -rf build/ copydeps.stamp

install-exec-local:		Makefile_exe
	$(MAKE) check
	$(MAKE) -f Makefile_exe install INSTALL_ROOT=$(DESTDIR)
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	$(INSTALL_DATA) src/images/lintouch.png \
	$(DESTDIR)$(datadir)/pixmaps/gnome-mime-application-x-lintouch-project.png

uninstall-local:		Makefile_exe
	$(MAKE) -f Makefile_exe uninstall INSTALL_ROOT=$(DESTDIR)

# makefile regeneration
Makefile%:				lintouch-runtime%.pro $(wildcard *.pri)
	@QTDIR@/bin/lrelease $(LTRUNTIME_TOP_SRCDIR)/lintouch-runtime_exe.pro
	$(QMAKE) -o $@ $<

dist-hook:
	-find $(distdir) -name .svn -type d -exec rm -rf {} \;

copydeps.stamp:
	cp ../libltui/build/ltui???.dll build/
	cp ../libltproject/build/ltproject???.dll build/
	cp ../liblttemplates/build/lttemplates???.dll build/
	cp ../libltzip/build/ltzip???.dll build/
	cp ../libltvfs/build/ltvfs???.dll build/
	cp ../libltvariables/.libs/libltvariables-*.dll build/
	cp ../libltcp/.libs/libltcp-*.dll build/
	cp ../libltfsm/.libs/libltfsm-*.dll build/
	touch copydeps.stamp

TESTS = build/check_lintouch-runtime
SUBDIRS = . doc

pixmapsdir = $(datadir)/pixmaps
pixmaps_DATA = src/images/lintouch.png src/images/lintouch.xpm

launcherdir = $(datadir)/applications
launcher_DATA = lintouch-runtime.desktop

mimedir = $(datadir)/mime/packages
mime_DATA = lintouch.xml

EXTRA_DIST = TODO lintouch-runtime_exe.pro lintouch-runtime_test.pro \
			 src tests locale \
			 lintouch.xml lintouch-runtime.desktop

CLEANFILES = copydeps.stamp

# vim: set noet ts=4 sw=4 tw=76:
# $Id: Makefile.am 1216 2006-01-09 11:46:01Z modesto $
