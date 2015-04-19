// $Id: ProjectAsyncBuilderPrivate.hh,v 1.6 2004/08/12 22:31:13 jbar Exp $
//
//   FILE: ProjectAsyncBuilderPrivate.hh --
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

#ifndef _PROJECTASYNCBUILDERPRIVATE_HH
#define _PROJECTASYNCBUILDERPRIVATE_HH

#include <qurloperator.h>
#include <qfile.h>

#include <lt/project/ProjectAsyncBuilder.hh>

namespace lt {

    class ProjectBuilder;

    /**
        * Private data of ProjectAsyncBuilder. It resides in a separate
        * file because it is a separate class (read not nested) -
        * nested classes cannot have signals or slots.
        */

    class ProjectAsyncBuilderPrivate:
        public QObject,
        public LogEnabled
    {
        Q_OBJECT

        public slots:
            void handleProgress(int bytesDone, int bytesTotal,
                    QNetworkOperation *op);
            void handleFinished(QNetworkOperation *op);
            void handleData(const QByteArray &data,
                QNetworkOperation *op);

        signals:
            void downloadProgress(int downloaded, int total);
            void finished(int);
            void downloaded();

        public:
            ProjectAsyncBuilderPrivate(
                LoggerPtr logger = Logger::nullPtr(),
                QString locale = QString::null);

            virtual ~ProjectAsyncBuilderPrivate();
            bool downloading;
            bool building;

            /**
                * The building process has been cancelled.
                */
            bool cancelled;
            QUrlOperator *uo;
            QString tempName;
            QFile *dev;
            ProjectBuilder *pb;
            Project project;

        private:
            static bool protocolsInitialized;
    };
}

#endif /* _PROJECTASYNCBUILDERPRIVATE_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectAsyncBuilderPrivate.hh,v 1.6 2004/08/12 22:31:13 jbar Exp $
