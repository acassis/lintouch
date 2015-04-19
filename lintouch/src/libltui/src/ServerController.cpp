// $Id: ServerController.cpp,v 1.16 2004/08/15 18:17:59 mman Exp $
//
//   FILE: ServerController.cpp -- 
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
#include <qserversocket.h>
#include <qtimer.h>

#include "lt/ui/Console.hh"

#include "lt/ui/ServerController.hh"
using namespace lt;

#include "config.h"

#define PRE "<font color=\"red\">"
#define POST "</font>"

/**
 * Empty QServerSocket implementation.
 *
 * Used to find an unused TCP/IP Port for local Lintouch Server.
 */
class MyServerSocket : public QServerSocket
{
    public:

        MyServerSocket( Q_UINT16 port ) : QServerSocket( port ) {};
        virtual ~MyServerSocket() {};
        virtual void newConnection( int ) {};
};

struct ServerController::ServerControllerPrivate
{
    ServerControllerPrivate() :
        process( new QProcess() ), shtimer( new QTimer() ),
        local( 1 ), port( 0 ), real_port( 0 ), console( NULL ) {}

    ~ServerControllerPrivate() { delete process; delete shtimer; }

    QProcess * process;
    QTimer * shtimer;

    bool local;
    Q_UINT16 port;
    Q_UINT16 real_port;

    QString project;

    QString lintouchServer;

    Console * console;

    QDir origDir;
};

ServerController::ServerController( QObject* parent /* = 0 */,
        const char* name /* = 0 */ ) :
        QObject( parent, name ), d( new ServerControllerPrivate )
{
    Q_CHECK_PTR( d );

#ifdef Q_OS_WIN32
    d->lintouchServer = qApp->applicationDirPath() +
        "/lintouch-server";
#else
    d->lintouchServer = QString( PREFIX_NATIVE ) +
        "/bin/lintouch-server";
#endif

    //connect interesting signals of project to us
    connect( d->process, SIGNAL( processExited() ),
            this, SLOT( processExitedSlot() ) );
    connect( d->process, SIGNAL( readyReadStdout() ),
            this, SLOT( readyReadStdoutSlot() ) );
    connect( d->process, SIGNAL( readyReadStderr() ),
            this, SLOT( readyReadStderrSlot() ) );

#ifdef Q_OS_WIN32
    // on windows, cd to the appDir so that we can find required .dlls
    d->origDir = QDir::current();
    if( ! QDir::setCurrent( qApp->applicationDirPath() ) ) {
        qWarning( QString( "ERROR: unable to cd to %1" )
                .arg( qApp->applicationDirPath() ) );
    }
#endif
}

ServerController::~ServerController()
{
    Q_CHECK_PTR( d );

#ifdef Q_OS_WIN32
    // on windows, get back to origdir
    if( ! QDir::setCurrent( d->origDir.absPath() ) ) {
        qWarning( QString( "ERROR: unable to cd to %1" )
                .arg( d->origDir.absPath() ) );
    }
#endif

    delete d;
}

void ServerController::setProject( const QString & project )
{
    Q_CHECK_PTR( d );

    d->project = project;
}

QString ServerController::project() const
{
    Q_CHECK_PTR( d );

    return d->project;
}

void ServerController::setPort( Q_UINT16 port )
{
    Q_CHECK_PTR( d );

    d->port = port;
}

Q_UINT16 ServerController::port() const
{
    Q_CHECK_PTR( d );

    return ( ( d->port != 0 ) ? ( d->port ) : ( d->real_port ) );
}

void ServerController::setLocal(
        bool local )
{
    Q_CHECK_PTR( d );

    d->local = local;
}

bool ServerController::local() const
{
    Q_CHECK_PTR( d );

    return d->local;
}

void ServerController::setConsole( Console * console )
{
    Q_CHECK_PTR( d );

    d->console = console;
}

void ServerController::start()
{
    Q_CHECK_PTR( d );

    if( d->process->isRunning() ) return;

    if( d->port == 0 ) {
        // this code is rather fuzzy, it tries to create server socket
        // on an unused port and remember the port.
        MyServerSocket * sock = new MyServerSocket( 0 );
        d->real_port = sock->port();
        delete sock;
    } else {
        d->real_port = d->port;
    }

    //prepare process args
    d->process->clearArguments();

    // lintouch-server
    d->process->addArgument( d->lintouchServer );
    // port
    d->process->addArgument( "-p" );
    d->process->addArgument( QString( "%1" ).arg( d->real_port ) );
    // local mode
    if( d->local ) {
        d->process->addArgument( "-l" );
    }
    // project
    d->process->addArgument( d->project );

    //qWarning( d->process->arguments().join( "|" ) );
    if( d->console ) {
        d->console->appendStderr(
                QString( PRE ">>> Executing command: %1" POST )
                .arg( d->process->arguments().join( " " ) ) );
    }

    // and finally start the server
    d->process->start();
}

void ServerController::stop()
{
    Q_CHECK_PTR( d );
    if( ! d->process->isRunning() ) return;

    if( d->console ) {
        d->console->appendStderr(
                QString( PRE ">>> Waiting 10 seconds for the server "
                    "to shut down" POST ) );
    }

    connect( d->shtimer, SIGNAL( timeout() ),
            this, SLOT( sendTerminate() ) );
    d->shtimer->start( 10000, true );
    d->process->closeStdin();
}

void ServerController::sendTerminate()
{
    Q_CHECK_PTR( d );
    d->shtimer->stop();
    if( ! d->process->isRunning() ) return;

    if( d->console ) {
        d->console->appendStderr(
                QString( PRE ">>> Waiting other 10 seconds for the server "
                    "to shut down" POST ) );
    }

    //try to terminate the process, wait 10 seconds and kill it
    connect( d->shtimer, SIGNAL( timeout() ),
            d->process, SLOT( kill() ) );
    d->shtimer->start( 10000, true );
    d->process->tryTerminate();
}

bool ServerController::isRunning()
{
    Q_CHECK_PTR( d );

    return d->process->isRunning();
}

void ServerController::readyReadStdoutSlot()
{
    Q_CHECK_PTR( d );

    // try to find the ``magic`` string in the server's output
    while( d->process->canReadLineStdout() ) {
        QString l = d->process->readLineStdout();
        if( d->console ) d->console->appendStdout( l );
        if( l.find( "Press " ) == 0 ) {
            if( d->console ) {
                d->console->appendStderr(
                        QString( PRE ">>> Server ready at port %1" POST )
                        .arg( d->real_port ) );
            }
            emit( serverReady( d->real_port ) );
        }
    }
}

void ServerController::readyReadStderrSlot()
{
    Q_CHECK_PTR( d );

    while( d->process->canReadLineStderr() ) {
        QString l = d->process->readLineStderr();
        if( d->console ) d->console->appendStderr( l );
    }
}

void ServerController::processExitedSlot()
{
    Q_CHECK_PTR( d );

    // stop the timer if this processExitedSlot is a result of correct exit,
    // i.e., after d->process->tryTerminate()
    d->shtimer->stop();

    if( d->console ) {
        d->console->appendStderr(
                QString( PRE ">>> Server has exited with status code %1" POST )
                .arg( d->process->exitStatus() ) );
    }

    emit( serverExited( d->process->exitStatus() ) );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerController.cpp,v 1.16 2004/08/15 18:17:59 mman Exp $
