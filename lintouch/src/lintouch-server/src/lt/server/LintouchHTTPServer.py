# $Id: LintouchHTTPServer.py 481 2005-03-16 13:21:00Z mman $
#
#   FILE: LintouchHTTPServer.py --
# AUTHOR: Jiri Barton <jbar@swac.cz>
#   DATE: 01 February 2004
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

"""
USAGE:
* LintouchHTTPServer(project.zip [, port])
    run the server in a separate thread
    serve /project.zip and nothing else
* LintouchHTTPServer.server_port
    the port, assigned by OS or passed in the ctor
* LintouchHTTPServer.server_close()
    shutdown the server

TODO:
- add the file header
- write test cases using PyUnit:
    - server is running
    - server is not running after LintouchHTTPServer.server_close
    - the correct location succeedes
    - an incorrect location fails
"""

import SimpleHTTPServer
import BaseHTTPServer
import sys
import os
import thread
import urllib

class LintouchHTTPRequest(SimpleHTTPServer.SimpleHTTPRequestHandler):
    """The lintouch HTTP request accompanies the lintouch HTTP server.

    Only one location is allowed: the location of the project is stored
    in the server.
    """

    def send_head(self):
        """Check if the current path is allowed."""

        if urllib.unquote(self.path) == '/' + os.path.basename(self.server.project):
            try:
                f = open(self.server.project, 'rb')
            except IOError:
                self.send_error(404, 'File not found')
                return None
            self.send_response(200)
            self.send_header('Content-type', 'application/octet-stream')
            self.send_header('Content-Length', str(os.fstat(f.fileno())[6]))
            self.end_headers()
            return f
        else:
            self.send_error(404, 'File not found')
            return None

class LintouchHTTPServer(BaseHTTPServer.HTTPServer):
    """The lintouch HTTP server.

    It runs in a separate thread. Its request handler can handle only the
    following requests:
        http://<server>[:<port>]/<filename>.zip
    """

    def __init__(self, project):
        """Start a new server in a thread.

        port - if 0 or omitted, LintouchHTTPServer gets a port from
            the OS. You can read the assigned port in the server_port
            member.
        """

        self.project = project
        BaseHTTPServer.HTTPServer.__init__(self, ('', 0),
                LintouchHTTPRequest)
        thread.start_new_thread(self.serve_forever, ())

    def handle_error(self, request, client_address):
        print 'An error occurred while sending the project (%s): %s' % (
            client_address, sys.exc_info()[1])

def test():
    lhs = LintouchHTTPServer(sys.argv[0])
    print 'Listening on port', lhs.server_port
    sys.stdin.readline()
    lhs.server_close()

if __name__ == "__main__":
    test()

# vim: set et ts=4 sw=4 tw=76:
# $Id: LintouchHTTPServer.py 481 2005-03-16 13:21:00Z mman $
