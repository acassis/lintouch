// $Id: RuntimeController.cpp,v 1.16 2004/08/15 18:17:59 mman Exp $
//
//   FILE: RuntimeController.cpp -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 4 February 2005
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

#include <qapplication.h>
#include <qdir.h>
#include <qprocess.h>
#include <qtimer.h>

#include "lt/ui/RuntimeController.hh"
using namespace lt;

#include "config.h"

#define PRE "<font color=\"red\">"
#define POST "</font>"

struct RuntimeController::RuntimeControllerPrivate
{
    RuntimeControllerPrivate() :
        process( new QProcess() ), shtimer( new QTimer() ) {}

    ~RuntimeControllerPrivate() { delete process; delete shtimer; }

    QProcess * process;
    QTimer * shtimer;

    QString project;

    QString lintouchRuntime;
};

RuntimeController::RuntimeController( QObject* parent /* = 0 */,
        const char* name /* = 0 */ ) :
        QObject( parent, name ), d( new RuntimeControllerPrivate )
{
    Q_CHECK_PTR( d );

#ifdef Q_OS_WIN32
    d->lintouchRuntime = QFileInfo( QDir( qApp->applicationDirPath() ),
            "lintouch-runtime.exe" ).absFilePath();
#else
    d->lintouchRuntime = QFileInfo( QDir( PREFIX_NATIVE ),
            "bin/lintouch-runtime" ).absFilePath();
#endif

#ifdef QT_DEBUG
    qWarning( "RUNTIME IS AT: " + d->lintouchRuntime );
#endif

    //connect interesting signals of project to us
    connect( d->process, SIGNAL( processExited() ),
            this, SLOT( processExitedSlot() ) );
    connect( d->process, SIGNAL( readyReadStdout() ),
            this, SLOT( readyReadStdoutSlot() ) );
    connect( d->process, SIGNAL( readyReadStderr() ),
            this, SLOT( readyReadStderrSlot() ) );
}

RuntimeController::~RuntimeController()
{
    Q_CHECK_PTR( d );

    delete d;
}

void RuntimeController::setProject( const QString & project )
{
    Q_CHECK_PTR( d );

    d->project = project;
}

QString RuntimeController::project() const
{
    Q_CHECK_PTR( d );

    return d->project;
}

bool RuntimeController::isReady() const
{
    Q_CHECK_PTR( d );

    if(d->project.isEmpty())
        return false;

    QFileInfo fi(d->project);
    if(!fi.exists() || !fi.isReadable())
        return false;

    fi = QFileInfo(d->lintouchRuntime);
    if(!fi.exists() || !fi.isExecutable())
        return false;

    return true;
}

void RuntimeController::start()
{
    Q_CHECK_PTR( d );

    if( d->process->isRunning() ) return;

    //prepare process args
    d->process->clearArguments();

    // lintouch-runtime
    d->process->addArgument( d->lintouchRuntime );
    // normal size window
    d->process->addArgument( "-n" );
    // project
    d->process->addArgument( d->project );

    // and finally start the server
    d->process->start();
}

void RuntimeController::stop()
{
    Q_CHECK_PTR( d );
    if( ! d->process->isRunning() ) return;

    connect( d->shtimer, SIGNAL( timeout() ),
            this, SLOT( kill() ) );
    d->shtimer->start( 10000, true );
    d->process->tryTerminate();
}

bool RuntimeController::isRunning()
{
    Q_CHECK_PTR( d );

    return d->process->isRunning();
}

void RuntimeController::readyReadStdoutSlot()
{
    Q_CHECK_PTR( d );

    while( d->process->canReadLineStdout() ) {
        QString l = d->process->readLineStdout();
    }
}

void RuntimeController::readyReadStderrSlot()
{
    Q_CHECK_PTR( d );

    while( d->process->canReadLineStdout() ) {
        QString l = d->process->readLineStdout();
    }
}

void RuntimeController::processExitedSlot()
{
    Q_CHECK_PTR( d );

    // stop the timer if this processExitedSlot is a result of correct exit,
    // i.e., after d->process->tryTerminate()
    d->shtimer->stop();

    emit( runtimeExited( this, d->process->exitStatus() ) );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: RuntimeController.cpp,v 1.16 2004/08/15 18:17:59 mman Exp $
