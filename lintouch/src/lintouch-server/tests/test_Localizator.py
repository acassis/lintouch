#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#   FILE: test_Localizator.py --
# AUTHOR: Jiri Barton <jbar@swac.cz>
#   DATE: 06 July 2004
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

import unittest

#find the module to be tested
import sys
sys.path.insert(0, 'src')

from lt.server.Localizator import *

class TestLocalizator(unittest.TestCase):
    def setUp(self):
        self.filename = 'tests/data/testproject.ltp'

    def tearDown(self):
        pass

    def testCreation(self):
        self.assertRaises(Exception, Localizator, 'nonexistent')
        self.assert_(Localizator, self.filename)

        loc = Localizator(self.filename, 'en_US')
        #public attributes
        self.assertEqual(loc.locale, 'en_US')

    def testResource(self):
        loc = Localizator(self.filename, 'en_US')
        self.assertRaises(Exception, loc.resource, 'nonexistent')
        self.assertEqual(loc.resource('regional'), 'Zzz')
        self.assertEqual(loc.resource('national'), 'Hi')
        self.assertEqual(loc.resource('international'), 'Huh')

suite = unittest.makeSuite(TestLocalizator, 'test')

if __name__ == '__main__':
    unittest.main()

# vim: set et ts=4 sw=4 tw=76:
# $Id: testlocalizator.py,v 1.2 2004/07/11 09:05:25 jbar Exp $
