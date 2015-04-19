// $Id: VFS.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: VFS.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 04 November 2003
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

#ifndef _VFS_HH
#define _VFS_HH

#include <qstring.h>
#include <qstringlist.h>
#include <qurl.h>
#include <qiodevice.h>

#include <lt/SharedPtr.hh>
#include <lt/logger/Logger.hh>
#include <lt/logger/LogEnabled.hh>
#include <lt/vfs/VFSdefs.hh>

namespace lt {

    /**
        * Virtual FileSystem.
        *
        * Virtual filesystem represents a directory, a ZIP file, or a
        * a directory inside a ZIP file on local filesystem.
        *
        * All paths are expressed as QUrls.
        *
        * The access to the resources is synchronous (= blocking).
        *
        * This class is abstract. To work with ZIP archives, use the class
        * VFSZIP. To work with directories, use the class VFSLocal.
        *
        * Pointer-based class.
        *
        * @author <a href="mailto:barton@swac.cz">Jiri Barton</a>
        */

    class VFS;
    typedef SharedPtr<VFS> VFSPtr;

    class VFS:
        public LogEnabled
    {
        public:

            /**
                * Ctor.
                */
            VFS(const QUrl &rootLocation, bool readOnly = true,
                LoggerPtr logger = Logger::nullPtr());

            /**
                * Valid state of the VFS. An invalid VFS can be used
                * but then all the methods do not do anything actually.
                */
            virtual bool isValid() const;

            /**
                * Destructor.
                */
            virtual ~VFS();

        public:

            /**
                * Create a VFS from an existing VFS. The new VFS will
                * be in the chroot and have the same properties as the
                * existing VFS (type, readonly, etc.)
                *
                * You may change the logger, or keep it the same as for
                * the existing VFS (by leaving the default).
                */
            virtual VFS *subVFS(const QString &subdir,
                LoggerPtr logger = Logger::nullPtr()) = 0;

            /**
                * Open VFS file safely, e.g., return the default QFile
                * instance if it can not be opened.
                * The supported modes:
                * <ul>
                * <li>IO_ReadOnly
                * <li>IO_WriteOnly
                * </ul>
                * No other modes are supported and not likely to be
                * supported in the future because of the ZIP support.
                */
            virtual QIODevice *openFile(const QString &name,
                int mode = IO_ReadOnly) const = 0;

            /**
                * Create dir. Return the success of the operation.
                */
            virtual bool mkdir(const QString &subdir) const = 0;

            /**
                * Remove dir. Return the success of the operation.
                */
            virtual bool rmdir(const QString &subdir) const = 0;

            /**
                * Remove the specified file.
                */
            virtual bool unlink(const QString &name) const = 0;

            /**
                * True if the VFS is read only as passed to the ctor.
                */
            virtual bool isReadOnly() const;

            /**
                * Return the top-level location (as created or modified) of
                * this VFS.
                */
            virtual QUrl rootLocation() const;

            /**
                * List the content of the directory.
                */
            virtual QStringList ls(const QString &subdir = ".",
                int filter = VFSFilter::All) const = 0;

            /**
                * True if the file/directory exists within the VFS.
                * You can constrain the lookup to files and/or directories.
                */
            virtual bool exists(const QString &path,
                int filter = VFSFilter::All) const = 0;

        private:

            /**
                * Prevent copying. Copy constructor.
                */
                VFS(const VFS &other);

                /**
                * Prevent copying. Copy assignment operator.
                */
                VFS &operator =(const VFS &other);

        protected:

                /**
                * The shared protected data.
                */
                struct VFSProtected;
                typedef SharedPtr<VFSProtected>
                    VFSProtectedPtr;
                VFSProtectedPtr p;

    };
} // namespace lt

#endif /* _VFS_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFS.hh 1168 2005-12-12 14:48:03Z modesto $
