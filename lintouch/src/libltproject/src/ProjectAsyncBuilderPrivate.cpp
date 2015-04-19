// $Id: ProjectAsyncBuilderPrivate.cpp,v 1.14 2004/08/24 11:24:14 jbar Exp $
//
//   FILE: ProjectAsyncBuilderPrivate.cpp --
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
#include <qnetwork.h>

#include <lt/vfs/TempFiles.hh>

#include "lt/project/ProjectBuilder.hh"

#include "ProjectAsyncBuilderPrivate.hh"

using namespace lt;


bool ProjectAsyncBuilderPrivate::protocolsInitialized = false;

ProjectAsyncBuilderPrivate::ProjectAsyncBuilderPrivate(
    LoggerPtr logger /* = Logger::nullPtr() */,
    QString locale /* = QString::null */)
    : downloading(false), building(false), cancelled(false), uo(NULL)
{
    enableLogging(logger);
    if (!protocolsInitialized)
    {
        qInitNetworkProtocols();
        protocolsInitialized = true;
    }

    //generate a unique temp filename
    tempName = getTempFileName();
    if (tempName.isEmpty())
    {
        qWarning(
            "Failed to create a temporary file using the system.");
        //generate a unique temp filename in the current directory
        for (int i = 0;
            QDir().exists(tempName =
                QString("ProjectAsyncBuilder.cache.%1.zip").arg(i));
            i++);
    }
    else
        tempName += ".zip";

    dev = new QFile(tempName);

    pb = new ProjectBuilder(logger->getChildLogger("ProjectBuilder"), locale);
}

ProjectAsyncBuilderPrivate::~ProjectAsyncBuilderPrivate()
{
    delete pb;
    delete uo;
    uo = NULL;
    delete dev;
    QDir().remove(tempName);
}

void ProjectAsyncBuilderPrivate::handleProgress(
    int bytesDone, int bytesTotal, QNetworkOperation *)
{
    emit downloadProgress(bytesDone, bytesTotal);
}

void ProjectAsyncBuilderPrivate::handleFinished(
    QNetworkOperation *op)
{
    downloading = false;
    dev->close();

    if (op->state() == QNetworkProtocol::StFailed)
    {
        const int errorMessageCount = 15;
        char *errorMessages[errorMessageCount] = {"NoError", "ErrValid",
            "ErrUnknownProtocol", "ErrUnsupported", "ErrParse",
            "ErrLoginIncorrect", "ErrHostNotFound", "ErrListChildren",
            "ErrMkDir", "ErrRemove", "ErrRename", "ErrGet", "ErrPut",
            "ErrFileNotExisting", "ErrPermissionDenied"};

        int err = op->errorCode();
        getLogger().info(QString("Download failed. Error code: %1 (%2)")
            .arg(err)
            .arg(err > 14 || err < 0 ?
                "ErrorUnknonw" :
                errorMessages[err]));
        emit finished(ProjectAsyncBuilder::Error);
    }
    else if (op->state() == QNetworkProtocol::StDone)
    {
        emit downloaded();
        getLogger().debug("Project downloaded.");
        building = true;
        project = pb->buildRuntimeProject(tempName);
        building = false;
        if (cancelled)
        {
            getLogger().debug("Building cancelled.");
            emit finished(ProjectAsyncBuilder::Cancelled);
        }
        else
        {
            getLogger().debug("Project built.");
            emit finished(ProjectAsyncBuilder::Finished);
        }
    }
    else
    {
        //otherwise, the operation has been stopped
        getLogger().debug("Download cancelled.");
        emit finished(ProjectAsyncBuilder::Cancelled);
    }
}

void ProjectAsyncBuilderPrivate::handleData(
    const QByteArray &data, QNetworkOperation *)
{
    dev->writeBlock(data);
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectAsyncBuilderPrivate.cpp,v 1.14 2004/08/24 11:24:14 jbar Exp $
