# $Id: Localizator.py 347 2005-02-23 14:56:10Z modesto $
#
#   FILE: Localizator.py --
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

from zipfile import ZipFile
from zipfile import BadZipfile

class Localizator:
    def __init__(self, filename, uselocale=''):
        """Create a Localizator instance.

        - filename is the name of the ZIP file
        - uselocale is the locale name in the ISO format; the user specific
            is used if the argument is omitted"""

        self.locale = uselocale

        try:
            self.archive = ZipFile(filename, 'r')
        except (IOError, BadZipfile), e:
            raise Exception('%s: %s' % (filename, e))
        except Exception, e:
            raise Exception('Error while opening the file %s: %s' % (
                filename, e))

        self.subdirs = []
        if self.locale != 'C':
            self.subdirs.append(self.locale + '/')
            try:
                self.subdirs.append(self.locale.split('_', 1)[0] + '/')
            except:
                pass
        self.subdirs.append('')
        self.subdirs = ['resources/' + s for s in self.subdirs]

    def resource(self, name):
        """Return the localized resource from inside the archive as a string."""

        for subdir in self.subdirs:
            try:
                return self.archive.read(subdir + name)
            except:
                pass
        raise Exception('Resource %s not found' % name)

# vim: set et ts=4 sw=4 tw=76:
# $Id: Localizator.py 347 2005-02-23 14:56:10Z modesto $
