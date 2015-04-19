// $Id: EditorProject.hh 1558 2006-04-06 12:21:31Z modesto $
//
//   FILE: EditorProject.hh --
// AUTHOR: Jiri Barton <jbar@swac.cz>
//   DATE: 23 July 2004
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

#ifndef _EDITORPROJECT_HH
#define _EDITORPROJECT_HH

#include <qmap.h>
#include <qstring.h>

#include <lt/SharedPtr.hh>

#include <lt/vfs/VFS.hh>
#include <lt/project/Project.hh>

#include "Bindings.hh"
#include "ServerLogging.hh"


namespace lt {
    class UndoStack;
    class EditorProject;

    typedef QMap <QString, EditorProject> EditorProjectMap;

    /**
     * EditorProject.
     */
    class EditorProject
        : public Project, public LogEnabled
    {

        public:

            /**
             * Constructor.
             */
            EditorProject (LoggerPtr logger = Logger::nullPtr());

            EditorProject ( const EditorProject & p );

            EditorProject & operator = ( const EditorProject & p );

            /**
             * Destructor.
             */
            virtual ~EditorProject ();

        public:

            /**
             * The project has been modified.
             */
            bool isDirty() const;

            /**
             * Set the dirty flag.
             */
            void setDirty(bool flag);

            /**
             * The project is new and has not yet been saved.
             */
            bool isNew() const;

            /**
             * The project has been saved.
             */
            void clearNew();

            /**
             * Set UndoStack.
             */
            void setUndoStack( UndoStack* undoStack );

            /**
             * The undo stack.
             */
            UndoStack *undoStack() const;

        public:

            /**
             * Set the name of the temporary file - the copy of the
             * original file.
             */
            void setTempFilename(const QString &filename);

            /**
             * Return the name of the temporary file - the copy of the
             * original file. It may be null if the project has not yet
             * been saved.
             */
            QString tempFilename() const;

            /**
             * Set the local VFS for the unpacked resources.
             */
            void setUnpackedResources(const VFSPtr &vfs);

            /**
             * Return the local VFS for the unpacked resources, previously
             * set by the setUnpackedResources method.
             */
            VFSPtr unpackedResources() const;

            /**
             * Set the local VFS for the deleted resources.
             */
            void setDeletedResources(const VFSPtr &vfs);

            /**
             * Return the local VFS for the deleted resources, previously
             * set by the setDeletedResources method.
             */
            VFSPtr deletedResources() const;

            /**
             * Copy the resource to the resources storage. Return the key
             * (filename) under which the file is to be retrieved, or the
             * null string on error.
             */
            QString addResource(const QString &filename);

            /**
             * Copy the resource to the resources storage with the given
             * key. Return the key (filename) under which the file is to be
             * retrieved, or the null string on error.
             */
            QString addResource(const QString &filename, const QString& key);

            /**
             * Remove the resource from the resources storage. Return the
             * key under which the resource can be undeleted using the
             * EditorProject::undeleteResource (i.e. undo), or the null
             * string on error.
             */
            QString deleteResource(const QString &key);

            /**
             * Move the resource back to the resources storage after it
             * has been deleted with the method deleteResource. That method
             * returns an undoname - the one that one needs to use in order
             * to restore the resource to its original state.
             *
             * Return the success of the operation.
             */
            bool undeleteResource(const QString &key,
                const QString &undoname);

            /**
             * Rename a resource within the resources storage. Return
             * the success of the operation.
             */
            bool renameResource(const QString &oldname,
                const QString &newname);

            /**
             * Returns the full path to the given resource. If the full path
             * is not known, return null string.
             */
            QString fullPathToResouce(const QString& key);

            /**
             * Return the project bindings.
             */
            const Bindings &bindings() const;

            /**
             * Return the project bindings.
             */
            Bindings &bindings();

            /**
             * Return the unavailable connections within this project.
             */
            const ConnectionDict& disabledConnections() const;

            /**
             * Return the unavailable connections within this project.
             */
            ConnectionDict& disabledConnections();

            /**
             * Checks if connection name is or is not in conflict with both
             * connection lists.
             *
             * \param cn New name of the connection
             * \return True is the new connection name is free, false
             * otherwise
             */
            bool checkConnectionName(const QString& cn) const;

        public:

            /**
             * Add logging to this project. returns false if the key
             * is already used.
             */
            bool addServerLogging ( const QString & key,
                    const ServerLogging * l );

            /**
             * Remove ServerLogging from the project. returns false if the
             * key is not registered.
             */
            bool removeServerLogging ( const QString & key );

            /**
             * Return all logging plugins associated with this project.
             */
            const ServerLoggingDict& serverLoggings() const;

            /**
             * Return all logging plugins associated with this project.
             */
            ServerLoggingDict& serverLoggings();

        private:

            typedef struct EditorProjectPrivate;
            typedef SharedPtr<EditorProjectPrivate>
                EditorProjectPrivatePtr;

            EditorProjectPrivatePtr d;

    };
} // namespace lt

#endif /* _EDITORPROJECT_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditorProject.hh 1558 2006-04-06 12:21:31Z modesto $
