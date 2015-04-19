// $Id: VFSProtected.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: VFSProtected.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 06 August 2004
//
// Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
//
// THIS SOFTWARE IS DISTRIBUTED AS IS AND WITHOUT ANY WARRANTY.
//
// This application is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License, or (at
// your option) any later version.
//
// This application is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this application; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef _VFSPROTECTED_HH
#define _VFSPROTECTED_HH

#include <lt/vfs/VFSdefs.hh>
#include <lt/vfs/VFS.hh>
#include <lt/logger/LogEnabled.hh>

namespace lt {

    struct VFS::VFSProtected : public LogEnabled
    {
        /**
         * Path inside the ZIP file or path within the filesystem.
         * Always starting and ending with slash.
         */
        QString dirPath;

        /**
         * VFS is read-only.
         */
        bool readOnly;

        /**
         * VFS being valid.
         */
        bool valid;

        /**
         * Default values.
         */
        VFSProtected(LoggerPtr logger = Logger::nullPtr());

        /**
         * Copy constructor.
         */
        VFSProtected(const VFSProtected &other);

        /**
         * Initialize the other VFS with this VFS. Check before if
         * that is allowed.
         */
        void subVFS(VFSProtectedPtr other, const QString &subdir);

        /**
         * Append dir2 to the current dirPath and return the result.
         */
        QString chroot(QString filename) const;

        /**
         * Check if you can open the specified file and return the new
         * path or null string on error.
         */
        QString openFile(const QString &name,
                int mode /* = IO_ReadOnly */) const;

        /**
         * Check if you can create the specified directory and return
         * the new path or null string on error.
         */
        QString mkdir(const QString &dir) const;

        /**
         * Check if you can remove the specified directory and return
         * the new path or null string on error.
         */
        QString rmdir(const QString &dir) const;

        /**
         * Check if you can delete the specified file or directory and
         * return the new path or null string on error.
         */
        QString unlink(const QString &name) const;

        /**
         * Check if you can list the content of the specified directory
         * and return the new path or null string on error.
         */
        QString ls(const QString &dir = "." ,
            int filter = VFSFilter::All) const;
    };
}

#endif /* _VFSPROTECTED_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFSProtected.hh 1168 2005-12-12 14:48:03Z modesto $
