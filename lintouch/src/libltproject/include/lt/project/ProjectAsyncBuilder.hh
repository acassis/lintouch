// $Id: ProjectAsyncBuilder.hh,v 1.10 2004/08/12 22:31:13 jbar Exp $
//
//   FILE: ProjectAsyncBuilder.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 27 November 2003
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

#ifndef _PROJECTASYNCBUILDER_HH
#define _PROJECTASYNCBUILDER_HH

#include <qurl.h>

#include <lt/logger/Logger.hh>

#include <lt/templates/Localizator.hh>

#include "lt/project/Project.hh"

namespace lt {

    /**
        * Factory for the Project class. Unlike ProjectBuilder class, this
        * class keeps status of the current operation in progress.
        * The project can be created from:
        * <ul>
        *     <li>remote ZIP (i.e. ZIP file on a HTTP or FTP server)
        *     <li>local ZIP
        *     <li>local directory>
        * </ul>
        * Pointer-based class.
        */

    class ProjectAsyncBuilderPrivate;

    class ProjectAsyncBuilder:
        public QObject,
        public LogEnabled
    {
        Q_OBJECT

        public:
            /**
                * Constructor.
                */
            ProjectAsyncBuilder(LoggerPtr logger = Logger::nullPtr(),
                QString locale = QString::null);

            /**
                * Destructor.
                */
            virtual ~ProjectAsyncBuilder();

            /**
                * Begin the download. Only one download is supported; any
                * subsequent calls are ignored as long as the download is
                * in progress.
                */
            void buildFromUrl(const QUrl& location);

            /**
                * Return the recently finished project. Return an invalid
                * project if no project has been finished.
                */
            Project getProject() const;

            /**
                * Forget the recently finished project. Used to free all
                * references to the build project so that the project can
                * be cleaned up right away.
                */
            void forgetProject();

            /**
                * Status of the finished built process.
                * <ul>
                *   <li>Finished - the project has been found. Nothing
                * more, nothing less. The project still may not be valid.
                *   <li>Cancelled - the download of the project has been
                * cancelled.
                *    <li>Error - the project could not be found.
                * </ul>
                */
            enum EndStatus {Finished, Cancelled, Error};

        public slots:
            /**
                * Cancel the downloading or the building.
                */
            void cancel();

        signals:
            /**
                * Report the download progress. Total may be -1, if not
                * known.
                */
            void downloadProgress(int downloaded, int total);

            /**
                * Report the progress of the build process.
                */
            void buildProgress(int panel, int panelsTotal);

            /**
                * Signal the building end. Give the reason why.
                */
            void finished(int status);

            /**
                * Fire when the file has been downloaded. Applies to local
                * files too.
                */
            void downloaded();

        private:

            /**
                * Hide the copy constructor.
                */
            ProjectAsyncBuilder(const ProjectAsyncBuilder &other);

            /**
                * Hide the copy assignment operator.
                */
            ProjectAsyncBuilder &operator =(
                const ProjectAsyncBuilder &other);

            ProjectAsyncBuilderPrivate *d;
    };
}

#endif /* _PROJECTASYNCBUILDER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectAsyncBuilder.hh,v 1.10 2004/08/12 22:31:13 jbar Exp $
