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

all: lsp-bootstrap.rb lsp-bootstrap.tar.bz2

lsp-bootstrap.tar.bz2:
	(cd src; tar -cjf ../lsp-bootstrap.tar.bz2 --exclude=.svn *)

lsp-bootstrap.rb: lsp-bootstrap.tar.bz2
	cat lsp-bootstrap.rb.part1 > lsp-bootstrap.rb
	ruby -e 'require "base64"; puts Base64.encode64($$<.read)' lsp-bootstrap.tar.bz2 >> lsp-bootstrap.rb
	cat lsp-bootstrap.rb.part2 >> lsp-bootstrap.rb
	chmod +x lsp-bootstrap.rb

clean:
	-rm lsp-bootstrap.tar.bz2 lsp-bootstrap.rb

# vim: set noet ts=4 sw=4 tw=76:
# $Id:$
