#!/usr/bin/env python
# $Id: $
#
#   FILE: test_mobusmaster.py --
# AUTHOR: Jiri Barton <jbar@swac.cz>
#   DATE: 14 June 2005
#
# Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
# All Rights Reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import sys
import time
from modlib import *

def server():
    c = ModbusServerContext(
        coils_address=1, coils_count=3,
        discrete_inputs_address=0, discrete_inputs_count=16,
        holding_registers_address=17, holding_registers_count=2,
        input_registers_address=0, input_registers_count=8)
    s = ModbusTCPServer(context=c, port=10502)
    try:
        s.serve_forever()
    except KeyboardInterrupt:
        print "Ctrl+C pressed - exiting..."
    s.server_close()

def client():
    cn = TCPMasterConnection('127.0.0.1', 10502)
    try:
        c = 0
        while True:
            #print the address space of the server
            for req in [ReadCoilsRequest(address=1, count=3),
                ReadDiscreteInputsRequest(address=0, count=16),
                ReadHoldingRegistersRequest(address=17, count=2),
                ReadInputRegistersRequest(address=0, count=8)]:

                name = req.__class__.__name__[4:-7].lower()
                tr = cn.createTransaction(req)
                res = tr.execute()

                print ' %s:' % name,
                if isinstance(res, ExceptionResponse):
                    print 'exception, %d' % res.exception_code,
                else:
                    if 'registers' in name:
                        print res.registers,
                    else:
                        print [[0, 1][i] for i in res.bits],
            print
            time.sleep(1)

            #flash with a coil
            req = WriteSingleCoilRequest(address=3, value=bool(c%2))
            tr = cn.createTransaction(req)
            res = tr.execute()

            #display the number in a holding register
            req = WriteSingleRegisterRequest(address=17, value=c)
            tr = cn.createTransaction(req)
            res = tr.execute()

            c += 1

    except KeyboardInterrupt:
        print "Ctrl+C pressed - exiting..."
    cn.close()

if __name__ == '__main__':
    if len(sys.argv) < 2 or sys.argv[1] in ['--server', '-s']:
        print 'Starting the ModbusTCP server...'
        server()
    elif sys.argv[1] in ['--client', '-c']:
        print 'Starting the ModbusTCP client...'
        client()
    else:
        print 'usage: %s [--server|-s|--client|-c]' % sys.argv[0]

# vim: set et ts=4 sw=4 tw=76 si:
# $Id: $
