// $Id: ResourceManager.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: ResourceManager.hh --
// AUTHOR: Jiri Barton <jbar@lintouch.org>
//   DATE: 25 August 2004
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

#ifndef _RESOURCEMANAGER_HH
#define _RESOURCEMANAGER_HH

#include <lt/SharedPtr.hh>

#include <lt/vfs/VFS.hh>

namespace lt {

    /**
     * Provide access to the project resources.
     *
     * Pointer-based class.
     */
    class ResourceManager
    {
        public:

            /**
             * Create ResourceManager on the VFS.
             */
            ResourceManager(const VFSPtr &vfs);

            /**
             * Destructor.
             */
            virtual ~ResourceManager();

            /**
             * Return a resource as QByteArray.
             */
            QByteArray resourceAsByteArray(const QString &key) const;

            /**
             * Return a resource as QIODevice. The result is always
             * a valid object; call isOpen on the result to check if the
             * method has succeeded.
             */
            QIODevice *resourceAsIODevice(const QString &key) const;

        private:

            /**
             * Prevent copying.
             */
            ResourceManager(const ResourceManager &);

            /**
             * Prevent copying.
             */
            ResourceManager& operator=(const ResourceManager &);

            struct ResourceManagerPrivate;
            ResourceManagerPrivate *d;
    };

    typedef SharedPtr<ResourceManager> ResourceManagerPtr;
} // namespace lt

#endif /* _RESOURCEMANAGER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ResourceManager.hh 1168 2005-12-12 14:48:03Z modesto $
