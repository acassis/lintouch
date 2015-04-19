// $Id: VFSLocal.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: VFSLocal.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 06 November 2003
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

#ifndef _VFSLOCAL_HH
#define _VFSLOCAL_HH

#include "VFS.hh"

namespace lt {

    /**
        * Directory manipulation on the local directory filesystem.
        *
        * Pointer-based class.
        */

    class VFSLocal:
        public VFS
    {

        public:

            /**
                * Ctor.
                */
            VFSLocal(const QUrl &rootLocation, bool readOnly = true,
                LoggerPtr logger = Logger::nullPtr());

            /**
                * Create a VFS from an existing VFS. The new VFS will
                * be in the chroot and have the same properties as the
                * existing VFS (it will be VFSLocal, readonly, etc.)
                *
                * You may change the logger, or keep it the same as for
                * the existing VFS (by leaving the default).
                */
            virtual VFS *subVFS(const QString &subdir,
                LoggerPtr logger = Logger::nullPtr());

            /**
                * Open VFS file safely, e.g., return the default QFile
                * instance if it can not be opened. The path has already
                * been cleaned.
                */
            virtual QIODevice *openFile(const QString &name,
                int mode) const;

            /**
                * Create dir. Return the success of the operation. The path
                * has already been cleaned.
                */
            virtual bool mkdir(const QString &subdir) const;

            /**
                * Remove dir. Return the success of the operation. The path
                * has already been cleaned.
                */
            virtual bool rmdir(const QString &subdir) const;

            /**
                * Remove file. Return the success of the operation. The
                * path has already been cleaned.
                */
            virtual bool unlink(const QString &name) const;

            /**
                * List the content of the directory. The path has already
                * been cleaned. Return the empty list on error.
                */
            virtual QStringList ls(const QString &subdir = ".",
                int filter = VFSFilter::All) const;

            /**
                * True if the file/directory exists within the VFS.
                * You can constrain the lookup to files and/or directories.
                */
            virtual bool exists(const QString &path,
                int filter = VFSFilter::All) const;

        private:
            /**
                * Copy protection.
                */
            VFSLocal &operator =(const VFSLocal &);

            /**
                * Copy protection.
                */
            VFSLocal(const VFSLocal &);
    };
}

#endif /* _VFSLOCAL_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFSLocal.hh 1168 2005-12-12 14:48:03Z modesto $
