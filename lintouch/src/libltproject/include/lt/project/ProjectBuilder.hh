// $Id: ProjectBuilder.hh,v 1.7 2004/08/12 22:31:13 jbar Exp $
//
//   FILE: ProjectBuilder.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 11 November 2003
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

#ifndef _PROJECTBUILDER_HH
#define _PROJECTBUILDER_HH

#include <qurl.h>

#include <lt/logger/Logger.hh>

#include "lt/project/Project.hh"

namespace lt {

    /**
        * Factory for the Project class.
        *
        * Pointer-based class.
        */
    class ProjectBuilder:
        public QObject,
        public LogEnabled
    {
        Q_OBJECT

        public:

            /**
                * Initialize the class.
                */
            ProjectBuilder(LoggerPtr logger = Logger::nullPtr(),
                QString locale = QString::null );

            /**
                * Destructor.
                */
            virtual ~ProjectBuilder();

            /**
                * Build a Project from the VFS location. Loads only the
                * template libraries specified in the project.
                */
            Project buildRuntimeProject(const QUrl& location);

            /**
                * Cancel the build process.
                */
            void cancel();

        signals:

            /**
                * Report the progress of the build process.
                */
            void buildProgress(int panel, int panelsTotal);

        private:

            /**
                * Hide the copy constructor.
                */
            ProjectBuilder(const ProjectBuilder &other);

            /**
                * Hide the copy assignment operator.
                */
            ProjectBuilder &operator =(const ProjectBuilder &other);

        protected:

            struct ProjectBuilderProtected;
            ProjectBuilderProtected *d;

    };
}

#endif /* _PROJECTBUILDER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectBuilder.hh,v 1.7 2004/08/12 22:31:13 jbar Exp $
