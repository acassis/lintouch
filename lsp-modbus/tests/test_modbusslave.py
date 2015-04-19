#!/usr/bin/env python
# $Id: $
#
#   FILE: test_mobusslave.py --
# AUTHOR: Jiri Barton <jbar@swac.cz>
#   DATE: 17 May 2005
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
import unittest
from modlib import *

# because the asserts from unittest do not work otherwise
import modlib

""" This test suite expect the Modbus Slave (=server) is initialized with
the following varset:

(variable addresses)
DI0
CO0
CO1
CO2
CO3
CO4
CO5
IR17
IR18
HR0
HR2
"""

class TestSequenceFunctions(unittest.TestCase):

    def setUp(self):
        self.cn = TCPMasterConnection('127.0.0.1', 10502)
        self.res = None

    def tearDown(self):
        self.cn.close()

    def testNotImplemented(self):
        req = ReadDiscreteInputsRequest(address=1, count=4)
        # pretend the request has an unimplemented function code
        req.function_code = 100
        tr = self.cn.createTransaction(req)
        self.res = tr.execute()
        self.assert_(isinstance(self.res, ExceptionResponse))
        self.assertEqual(self.res.exception_code, 1)

    def testAddressException(self):
        for req in [ReadDiscreteInputsRequest(address=20, count=4),
            ReadCoilsRequest(address=20, count=7),
            ReadInputRegistersRequest(address=20, count=1),
            ReadHoldingRegistersRequest(address=30, count=3),
            WriteMultipleCoilsRequest(address=20, count=7)]:

            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assertEqual(self.res.exception_code, 2)

    def testReadDiscreteInputs(self):
        # read the discrete input #0. It should always be 0.
        req = ReadDiscreteInputsRequest(address=0, count=1)
        tr = self.cn.createTransaction(req)
        self.res = tr.execute()
        self.assert_(isinstance(self.res, modlib.ReadDiscreteInputsResponse))
        self.assert_(len(self.res.bits) >= 2)
        self.assert_(not self.res.bits[0])

    def testSingleCoil(self):
        """WriteSingleCoil and ReadCoils."""

        for v in [True, False]:
            # re/set the coil #2
            req = WriteSingleCoilRequest(address=2, value=v)
            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assert_(isinstance(self.res, modlib.WriteSingleCoilResponse))

            # read back the coil set in #2
            req = ReadCoilsRequest(address=0, count=5)
            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assert_(isinstance(self.res, modlib.ReadCoilsResponse))
            self.assertEqual(self.res.bits[2], v)

    def testReadInputRegisters(self):
        # read the discrete inputs #17 and 18. They should always be 0.
        req = ReadInputRegistersRequest(address=17, count=2)
        tr = self.cn.createTransaction(req)
        self.res = tr.execute()
        self.assert_(isinstance(self.res, modlib.ReadInputRegistersResponse))
        self.assertEqual(len(self.res.registers), 2)
        for i in [0, 1]:
            self.assertEqual(self.res.registers[i], 0)

    def testSingleRegister(self):
        """WriteSingleRegister and ReadHoldingRegisters."""

        for v in [0, 3182, 17, 0]:
            # set the register #2 to the respective value
            req = WriteSingleRegisterRequest(address=2, value=v)
            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assert_(isinstance(self.res, modlib.WriteSingleRegisterResponse))

            # read back the coil set in #2
            req = ReadHoldingRegistersRequest(address=2, count=1)
            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assert_(isinstance(self.res, modlib.ReadHoldingRegistersResponse))
            self.assertEqual(self.res.registers[0], v)

    def testMultipleCoils(self):
        for v in [[True, False, True], [False, True, False], [False]*3]:
            # set the coils #1, #2, and #3
            req = WriteMultipleCoilsRequest(address=1, count=3)
            req.coils[:3] = v
            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assert_(isinstance(self.res, modlib.WriteMultipleCoilsResponse))

            # read back the coils
            req = ReadCoilsRequest(address=1, count=3)
            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assert_(isinstance(self.res, modlib.ReadCoilsResponse))
            self.assertEqual(self.res.bits[:3], v)

    def testMultipleRegisters(self):
        for v in [0, 3182, 17, 0]:
            # set the register #2 to the respective value
            req = WriteMultipleRegistersRequest(address=2, count=1)
            req.registers[0] = v
            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assert_(isinstance(self.res, modlib.WriteMultipleRegistersResponse))

            # read back the coil set in #2
            req = ReadHoldingRegistersRequest(address=2, count=1)
            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assert_(isinstance(self.res, modlib.ReadHoldingRegistersResponse))
            self.assertEqual(self.res.registers[0], v)

    def testReadWriteMultipleRegisters(self):
        for readfrom, writeto, readwhat, writewhat in [[2, 2, 0, 0],
            [2, 0, 0, 3182], [0, 2, 3182, 17], [2, 0, 17, 0], [0, 2, 0, 0]]:
            req = ReadWriteMultipleRegistersRequest(raddress=readfrom,
                rcount=1, waddress=writeto, wcount=1)
            req.wregisters[0] = writewhat
            tr = self.cn.createTransaction(req)
            self.res = tr.execute()
            self.assert_(isinstance(self.res,
                modlib.ReadWriteMultipleRegistersResponse))
            self.assertEqual(self.res.rregisters[0], readwhat)

if __name__ == '__main__':
    unittest.main()


# vim: set et ts=4 sw=4 tw=76 si:
# $Id: $
