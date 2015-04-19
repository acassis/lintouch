// $Id: ProjectAsyncBuilder.cpp,v 1.11 2004/08/12 22:31:13 jbar Exp $
//
//   FILE: ProjectAsyncBuilder.cpp --
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

#include <qdir.h>

#include "lt/project/ProjectAsyncBuilder.hh"
#include "lt/project/ProjectBuilder.hh"

#include "ProjectAsyncBuilderPrivate.hh"

using namespace lt;

ProjectAsyncBuilder::ProjectAsyncBuilder(
    LoggerPtr logger /* = Logger::nullPtr() */,
    QString locale /* = QString::null */)
    : d(new ProjectAsyncBuilderPrivate(logger, locale))
{
    enableLogging(logger);

    connect(
        d,
        SIGNAL(downloadProgress(int, int)),
        this,
        SIGNAL(downloadProgress(int, int)));

    connect(d->pb,
            SIGNAL(buildProgress(int, int)),
            this,
            SIGNAL(buildProgress(int, int)));

    connect(
        d,
        SIGNAL(finished(int)),
        this,
        SIGNAL(finished(int)));
    connect(
        d,
        SIGNAL(downloaded()),
        this,
        SIGNAL(downloaded()));
}

ProjectAsyncBuilder::~ProjectAsyncBuilder()
{
    delete d;
}

void ProjectAsyncBuilder::buildFromUrl(const QUrl& location)
{
    if (d->downloading)
        return;

    delete d->uo;
    d->uo = NULL;

    d->project = Project();
    if (!location.isValid())
    {
        getLogger().info(QString("Invalid URL %1").arg(location));
        emit finished(Error);
        return;
    }

    if (location.isLocalFile())
    {
        int size = QDir().exists(location.path()) ?
            QFile(location.path()).size() : 0;
        emit downloadProgress(size, size != 0 ? size : -1);

        if (!QDir().exists(location.path()))
        {
            getLogger().info(QString(
                        "The local path %1 does not exist or is inaccessible")
                    .arg(location.path()) );
            emit finished(Error);
            return;
        }

        emit downloaded();

        d->building = true;
        d->cancelled = false;
        d->project = d->pb->buildRuntimeProject(location);
        d->building = false;

        if (d->cancelled)
        {
            getLogger().debug("Building cancelled.");
            emit finished(Cancelled);
        }
        else
        {
            getLogger().debug("Project built.");
            emit finished(Finished);
        }
        return;
    }

    d->uo = new QUrlOperator(location);
    if (!d->uo->isValid())
    {
        getLogger().info(QString("Invalid URL %1").arg(location));
        delete d->uo;
        d->uo = NULL;
        emit finished(Error);
        return;
    }

    connect(
        d->uo,
        SIGNAL(dataTransferProgress(int, int, QNetworkOperation *)),
        d,
        SLOT(handleProgress(int, int, QNetworkOperation *)));

    connect(
        d->uo,
        SIGNAL(data(const QByteArray &, QNetworkOperation *)),
        d,
        SLOT(handleData(const QByteArray &, QNetworkOperation *)));

    connect(
        d->uo,
        SIGNAL(finished(QNetworkOperation *)),
        d,
        SLOT(handleFinished(QNetworkOperation *)));

    d->dev->open(IO_WriteOnly);
    d->downloading = true;
    d->cancelled = false;
    d->uo->get();
}

Project ProjectAsyncBuilder::getProject() const
{
    return d->project;
}

void ProjectAsyncBuilder::forgetProject()
{
    d->project = Project();
}

void ProjectAsyncBuilder::cancel()
{
    if (d->downloading)
    {
        d->uo->stop();
        delete d->uo;
        d->uo = NULL;
    }
    else if (d->building)
    {
        d->cancelled = true;
        d->pb->cancel();
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectAsyncBuilder.cpp,v 1.11 2004/08/12 22:31:13 jbar Exp $
