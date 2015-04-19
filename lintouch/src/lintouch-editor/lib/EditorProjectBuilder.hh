// $Id: EditorProjectBuilder.hh 1558 2006-04-06 12:21:31Z modesto $
//
//   FILE: EditorProjectBuilder.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 30 July 2004
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

#ifndef _EDITORPROJECTBUILDER_HH
#define _EDITORPROJECTBUILDER_HH

#include <qurl.h>

#include <lt/logger/Logger.hh>
#include <lt/configuration/Configuration.hh>

#include <lt/templates/TemplateManager.hh>
#include <lt/project/ProjectBuilder.hh>

#include "EditorProject.hh"
#include "PluginManager.hh"
#include "ServerLoggingManager.hh"

namespace lt {

    /**
     * Factory for the Project class.
     *
     * Pointer-based class.
     */
    class EditorProjectBuilder:
        public ProjectBuilder
    {
        Q_OBJECT

        public:

            /**
             * Initialize the class.
             */
            EditorProjectBuilder(LoggerPtr logger = Logger::nullPtr(),
                QString locale = QString::null );

            /**
             * Desctructor.
             */
            virtual ~EditorProjectBuilder();

            /**
             * Build the project from the location using the template
             * libraries in the template manager.
             * The es parametter is set by this method, it is output!
             */
            EditorProject buildEditorProject(const QUrl& location,
                const TemplateManager &manager,
                const PluginManager &pluginManager,
                const ServerLoggingManager &slManager,
                ConfigurationPtr& es);

            /**
             * Create a new project. The new project may be invalid.
             */
            EditorProject buildEditorProject(
                const TemplateManager &manager);
        private:

            /**
             * Hide the copy constructor.
             */
            EditorProjectBuilder(const EditorProjectBuilder &other);

            /**
             * Hide the copy assignment operator.
             */
            EditorProjectBuilder &operator =(
                const EditorProjectBuilder &other);

            /**
             * Private data.
             */
            struct EditorProjectBuilderPrivate;
            EditorProjectBuilderPrivate *p;
    };
}

#endif /* _EDITORPROJECTBUILDER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditorProjectBuilder.hh 1558 2006-04-06 12:21:31Z modesto $
