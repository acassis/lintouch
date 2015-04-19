# $Id:$
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

all: ltl-bootstrap.rb ltl-bootstrap.tar.bz2

ltl-bootstrap.tar.bz2:
	(cd src; tar -cjf ../ltl-bootstrap.tar.bz2 --exclude=.svn *)

ltl-bootstrap.rb: ltl-bootstrap.tar.bz2
	cat ltl-bootstrap.rb.part1 > ltl-bootstrap.rb
	ruby -e 'require "base64"; puts Base64.encode64($$<.read)' ltl-bootstrap.tar.bz2 >> ltl-bootstrap.rb
	cat ltl-bootstrap.rb.part2 >> ltl-bootstrap.rb
	chmod +x ltl-bootstrap.rb

clean:
	-rm ltl-bootstrap.tar.bz2 ltl-bootstrap.rb

# vim: set noet ts=4 sw=4 tw=76:
# $Id:$
