# $Id: LintouchCommServer.py 364 2005-02-25 17:51:47Z mman $
#
#   FILE: LintouchCommServer.py --
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

"""The Lintouch Communication Server."""

import SocketServer
import sys
import thread
import threading
import time
import traceback
import socket
import string
from lt.cp import *

debugCount = 0

class LintouchCommRequest(SocketServer.BaseRequestHandler):
    """Talk to a client."""

    def setup(self):
        SocketServer.BaseRequestHandler.setup(self)
        self.session = Session(LT_CP_SERVER)
        self.cancontinue = True
        self.session.set_project_url(
            self.server.projecturl % self.request.getsockname()[0])
        for connname, connvarset in self.server.pluginvarsets.items():
            self.session.register_variables(
                    string.replace(connname, ".", "_"), connvarset)

    def handle(self):
        """Handle the communication."""

        print 'Accepting the connection %x from %s:%d' % (
            thread.get_ident(),
            self.request.getpeername()[0],
            self.request.getpeername()[1])

        self.request.settimeout(30)
        try:
            try:
                rbuffer = self.request.recv(1024)
            except socket.timeout:
                print 'The connection %s timed out - closing' % thread.get_ident()
                try:
                    self.request.close()
                except:
                    pass
                return
            else:
                sbuffer = self.session.process(rbuffer)
                self.request.sendall(sbuffer)
        except Exception, e:
            try:
                self.request.close()
            except:
                pass
            print '%s.\nConnection %x closed' % (e, thread.get_ident())
            return

        self.request.settimeout(0.01)
        self.server.connectionpool.append(self)
        while (self.session.is_inprogress() or self.session.is_alive())\
            and self.cancontinue:
            rbuffer = ''
            try:
                rbuffer = self.request.recv(1024)
            except:
                pass
            else:
                if not rbuffer:
                    #connection reset
                    break

            try:
                sbuffer = self.session.process(rbuffer)
                self.request.sendall(sbuffer)
            except Exception, e:
                print 'Closing the connection:', e
                break


        del self.session
        self.request.close()
        print 'Connection %x closed' % thread.get_ident()
        self.server.connectionpool.remove(self)

class LintouchCommServer(SocketServer.ThreadingTCPServer):
    """Start the server in a thread."""

    def __init__(self, pluginvarsets, projecturl, port = 5555):
        """Initalize the Lintouch server.

        Set the ascendants parameters.
        """

        self.pluginvarsets = pluginvarsets
        self.projecturl = projecturl
        self.daemon_threads = True
        self.allow_reuse_address = True
        self.connectionpool = []
        SocketServer.ThreadingTCPServer.__init__(self, ('', port),
                LintouchCommRequest)
        thread.start_new_thread(self.serve_forever, ())

    def handle_error(self, request, client_address):
        """Handle a connection error.

        This method actually should not be called if all goes well.
        It is intended a fallback.
        """

        print 'A connection closed due to communication error'
        ei = sys.exc_info()
        print '%s: %s\n%s' % (ei[0], ei[1], '\n'.join (
            traceback.format_tb (ei[2])))

    def handle_request_in_shutdown(self, *args, **kwds):
        """Handle the request in a shutdown process.

        This method intentionally does nothing.
        """

        return False

    def server_close(self):
        """Ask the connection to terminate.

        Wait for 4 seconds, then kill them.
        """

        self.verify_request = self.handle_request_in_shutdown
        self.handler_request = self.handle_request_in_shutdown
        self.socket.close()

        if not self.connectionpool:
            return
        print 'Closing connections...'
        for req in self.connectionpool[:]:
            req.cancontinue = False
        for i in range(20):
            if not self.connectionpool:
                break
            sys.stdout.write('.')
            sys.stdout.flush()
            time.sleep(0.2)
        else:
            print 'Some connections still open. Closing forced.'

# vim: set et ts=4 sw=4 tw=76:
# $Id: LintouchCommServer.py 364 2005-02-25 17:51:47Z mman $
