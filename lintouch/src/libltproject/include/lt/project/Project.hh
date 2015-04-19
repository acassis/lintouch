// $Id: Project.hh 1556 2006-04-04 12:38:17Z modesto $
//
//   FILE: Project.hh --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
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

#ifndef _PROJECT_HH
#define _PROJECT_HH

#include <qmap.h>
#include <qstring.h>

#include <lt/SharedPtr.hh>

#include <lt/templates/Connection.hh>
#include <lt/templates/Info.hh>
#include <lt/templates/Localizator.hh>
#include <lt/templates/TemplateManager.hh>

#include "lt/project/View.hh"
#include "lt/project/ResourceManager.hh"

namespace lt {

    class Project;

    typedef QMap <QString, Project> ProjectMap;

    /**
        * Project.
        *
        *
        * When used as a value based class the contents is implicitly
        * shared among all instances created by assigning or via copy
        * constructor:
        *
        * There is no support for copy-on-write ATM. It'll be added as it
        * will be needed (if ever).
        *
        * @author <a href="mailto:mman@swac.cz">Martin Man</a>
        */
    class Project
    {

        public:

            /**
             * Constructor.
             */
            Project ();

            Project ( const Project & p );

            Project & operator = ( const Project & p );

            /**
             * Destructor.
             */
            virtual ~Project ();

        public:

            /**
             * Is this project valid ???
             */
            bool isValid () const;

        public:

            /**
             * Set the info about this project.
             */
            void setInfo ( const Info & i );

            /**
             * Return info about this project.
             */
            const Info & info () const;

        public:

            /**
             * Set the localizator used by this project.
             */
            void setLocalizator ( const LocalizatorPtr & l );

            /**
             * Return localizator used by this project.
             */
            LocalizatorPtr localizator () const;

        public:

            /**
             * Set the template manager used by this project.
             */
            void setTemplateManager ( const TemplateManager & tm );

            /**
             * Return template manager used by this project.
             */
            const TemplateManager & templateManager () const;

        public:

            /**
             * Add connection to this project. returns false if the key
             * is already used.
             */
            bool addConnection ( const QString & key,
                    const Connection * connection );

            /**
             * Remove connection from the project. returns false if the
             * key is not registered.
             */
            bool removeConnection ( const QString & key );

            /**
             * Return all connections within this project.
             */
            const ConnectionDict & connections () const;

            /**
             * Return all connections within this project.
             */
            ConnectionDict & connections ();

        public:

            /**
             * Return the resource manager associated with this project.
             */
            ResourceManagerPtr resourceManager() const;

            /**
             * Asssociate a resource manager associated with this project.
             */
            void setResourceManager(const ResourceManagerPtr &rm);

        public:

            /**
             * Add view to this project. returns false if the key
             * is already used.
             */
            bool addView ( const QString & key,
                    const View & view );

            /**
             * Remove view from the project. returns false if the
             * view is not registered.
             */
            bool removeView ( const QString & key );

            /**
             * Return all views within this project.
             */
            const ViewMap & views () const;

            /**
             * Return all views within this project.
             */
            ViewMap & views ();

        public:

            /**
             * Synchronize all dirty real values in all our
             * connections() so they are delivered to IOPins of
             * variables which use our variables.
             */
            void syncRealValues();

            /**
             * Synchronize all dirty requested values in all our
             * connections() that has been modified by some templates
             * so that the changes are delivered to communication
             * protocol library for TCP/IP enqueuing.
             */
            void syncRequestedValues();

        private:

            typedef struct ProjectPrivate;
            typedef SharedPtr<ProjectPrivate>
                ProjectPrivatePtr;

            ProjectPrivatePtr d;

    };
} // namespace lt

#endif /* _PROJECT_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Project.hh 1556 2006-04-04 12:38:17Z modesto $
