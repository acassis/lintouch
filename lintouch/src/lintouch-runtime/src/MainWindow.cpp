// $Id: MainWindow.cpp 1286 2006-01-16 09:03:55Z modesto $
//
//   FILE: MainWindow.cpp --
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 20 November 2003
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

#include <qaction.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qfiledialog.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qmime.h>
#include <qpushbutton.h>
#include <qstatusbar.h>
#include <qtimer.h>

#include <lt/project/Project.hh>
#include <lt/ui/Console.hh>
#include <lt/ui/PanelViewContainer.hh>
#include <lt/ui/ServerController.hh>

#include "CommunicationProtocolAdaptor.hh"
#include "MainWindow.hh"
#include "ServicePanel.hh"

using namespace lt;

struct MainWindow::MainWindowPrivate {
    MainWindowPrivate() :
        serverController( 0 ), cpa( NULL ),
        showSpAction( NULL ), showFsAction( NULL ), sp( NULL ),
        mode( 1 ), fullscreen( 0 ), autoStart( false ), retry( true ),
        serverRestartTimer( 0 ), reconnectTimer( 0 )
    {
    }

    // global logger
    LoggerPtr logger;

    // server controller
    lt::ServerController * serverController;

    // project file for local server (if startServer == true )
    QString projectFile;
    // server hostname
    QString host;
    // server port
    int port;
    // view specified from cmdline
    QString defaultView;

    // view selector policy 0=off, 1=on, 2=project dependent
    int viewSelectorPolicy;

    // panel selector policy 0=off, 1=on, 2=project dependent
    int panelSelectorPolicy;

    // project we are displaying
    Project project;
    // selected view
    QString view;
    // selected panel
    QString panel;

    // connection related structures
    CommunicationProtocolAdaptor * cpa;

    // show service panel action
    QAction * showSpAction;

    // show service panel action
    QAction * showFsAction;

    // ServicePanel
    ServicePanel* sp;

    // operation mode 0 = host/port, 1 = project (aka start server)
    int mode;

    // fullscreen shown?
    bool fullscreen;

    // do we have enough info to autostart?
    bool autoStart;

    // should we retry after delay when something goes wrong?
    bool retry;

    // server restart timer
    QTimer * serverRestartTimer;

    // reconnect timer
    QTimer * reconnectTimer;
};

MainWindow::MainWindow( int mode, bool fullscreen, LoggerPtr l,
        QWidget* parent, const char* name /*= 0*/, WFlags fl /*= 0*/ ) :
    MainWindowBase( parent, name, fl ),
    d( new MainWindowPrivate )
{
    Q_CHECK_PTR( d );

    // remember mode
    d->mode = mode;

    // remember fulscreen
    d->fullscreen = fullscreen;

    // logger
    d->logger = l;

    // create the ServicePanel
    d->sp = new ServicePanel(this);
    Q_CHECK_PTR(d->sp);
    m_panelViewContainer->setServicePanel(d->sp);
    connect( d->sp, SIGNAL( selectViewPanelRequested(
                    const QString&, const QString& ) ),
            this, SLOT( selectViewPanelSlot(
                    const QString&, const QString& ) ) );
    connect( d->sp, SIGNAL( startStopServer() ),
            this, SLOT( startStopServerSlot() ) );
    connect( d->sp, SIGNAL( connectDisconnect() ),
            this, SLOT( connectDisconnectSlot() ) );
    connect( d->sp, SIGNAL( projectSelectionRequested() ),
            this, SLOT( selectProjectSlot() ) );

    // create show sp action
    d->showSpAction = new QAction( "", QKeySequence( "F12" ), this );
    Q_CHECK_PTR(d->showSpAction);
    connect( d->showSpAction, SIGNAL( activated() ),
            this, SLOT( showHideServicePanelSlot() ) );

    // create show fulscreen action
    d->showFsAction = new QAction( "", QKeySequence( "F11" ), this );
    Q_CHECK_PTR(d->showFsAction);
    connect( d->showFsAction, SIGNAL( activated() ),
            this, SLOT( maximizeUnmaximizeSlot() ) );

    // create server controller
    d->serverController = new ServerController( this );
    connect( d->serverController, SIGNAL( serverReady( Q_UINT16 ) ),
            this, SLOT( serverReadySlot( Q_UINT16 ) ) );
    connect( d->serverController, SIGNAL( serverExited( int ) ),
            this, SLOT( serverExitedSlot( int ) ) );
    // use the console from service panel
    d->serverController->setConsole( d->sp->serverConsole );

    // get notified about panel changes
    connect( m_panelViewContainer, SIGNAL( panelDisplayed( const QString& ) ),
            this, SLOT( panelSelectedSlot( const QString& ) ) );

    // create communication protocol adaptor
    d->cpa = new CommunicationProtocolAdaptor( d->logger, this,
            "CommunicationProtocolAdaptor" );

    // when cpa processes events, update canvas immediately
    connect( d->cpa, SIGNAL( eventsProcessed() ),
            m_panelViewContainer, SLOT( updatePanelOnTop() ) );

    // when cpa does something with socket, log events
    connect( d->cpa, SIGNAL( connected() ),
            this, SLOT( connectedSlot() ) );
    connect( d->cpa, SIGNAL( projectReady() ),
            this, SLOT( projectReadySlot() ) );
    connect( d->cpa, SIGNAL( disconnected() ),
            this, SLOT( disconnectedSlot() ) );
    connect( d->cpa, SIGNAL( error( int ) ),
            this, SLOT( connectionErrorSlot( int ) ) );

    // prepare auto... timers
    d->serverRestartTimer = new QTimer( this );
    connect( d->serverRestartTimer, SIGNAL( timeout() ),
            this, SLOT( startServer() ) );
    d->reconnectTimer = new QTimer( this );
    connect( d->reconnectTimer, SIGNAL( timeout() ),
            this, SLOT( startConnect() ) );

    // play with event filter
    //qApp->installEventFilter( this );
}

MainWindow::~MainWindow()
{
    m_panelViewContainer->hide();
    delete d;
}

void MainWindow::setHostNPort( const QString & host, int port,
        bool editable /* = true */ )
{
    Q_ASSERT( d != NULL );

    //qWarning( "MainWindow::setHostNPort, editable=%d", editable );

    d->sp->setHostPortEnabled( true );
    d->sp->setProjectEnabled( false );
    d->sp->setServerConsoleEnabled( false );

    // prefill values and editable flag
    d->sp->setHostNPort(host, port, editable);

    d->host = host;
    d->projectFile = QString::null;
    d->port = port;

    // if host is non-empty, flag for autoconnect
    d->autoStart = ! host.isEmpty();
}

void MainWindow::setProjectNPort( const QString & project, int port /* = 0 */,
        bool editable /* = true */ )
{
    Q_ASSERT( d != NULL );

    //qWarning( "MainWindow::setProjectNPort, editable=%d", editable );

    d->sp->setHostPortEnabled( false );
    d->sp->setProjectEnabled( true );
    d->sp->setServerConsoleEnabled( true );

    //FIXME: prefill values and editable flag
    d->sp->setProject(project, editable);

    d->host = QString::null;
    d->projectFile = project;
    d->port = port;

    // refresh info in server controller
    d->serverController->setProject( d->projectFile );
    d->serverController->setPort( d->port );
    d->serverController->setLocal( 1 );

    // if project is non-empty, flag for autoserverstart
    d->autoStart = ! project.isEmpty();
}

void MainWindow::setView( const QString & view )
{
    d->defaultView = view;
}

void MainWindow::setServicePanelShortcutEnabled( bool enabled )
{
    d->showSpAction->setEnabled( enabled );
}

void MainWindow::setShowFullScreenEnabled( bool enabled )
{
    d->showFsAction->setEnabled( enabled );
}

void MainWindow::setViewSelectorEnabled( int state )
{
    Q_ASSERT( d != NULL );

    if( state == 2 || state == 0 ) {
        d->sp->setViewSelectorEnabled( false );
    } else {
        d->sp->setViewSelectorEnabled( true );
    }

    d->viewSelectorPolicy = state;
}

void MainWindow::setPanelSelectorEnabled( int state )
{
    Q_ASSERT( d != NULL );

    if( state == 2 || state == 0 ) {
        d->sp->setPanelSelectorEnabled( false );
    } else {
        d->sp->setPanelSelectorEnabled( true );
    }

    d->panelSelectorPolicy = state;
}

void MainWindow::show()
{
    //show invoked on some versions of QTE also for maximize/unmaximize,
    //connect only during the first startup.
    if( !isVisible() ) {
        // either start server, or connect
        if( d->autoStart ) {
            if( d->mode == 1 ) {
                d->serverRestartTimer->start( 0, true );
            } else {
                d->reconnectTimer->start( 0, true );
            }
        }
    }
    MainWindowBase::show();
}

void MainWindow::startServer()
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::startServer" );

    logToConsole( QString( "Starting local server for project %1..." )
        .arg( d->projectFile ) );
    d->serverController->start();

    //qWarning( "<<< MainWindow::startServer" );
}

void MainWindow::stopServer()
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::stopServer" );

    d->serverRestartTimer->stop();

    disconnect();

    // shutdown server & wait for clean shutdown
    d->serverController->stop();
    while( d->serverController->isRunning() ) {
        qApp->processEvents();
    }

    //qWarning( "<<< MainWindow::stopServer" );
}

void MainWindow::serverReadySlot( Q_UINT16 port )
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::serverReadySlot: port %d", port );

    // update host/port defaults
    d->host = "localhost";
    d->port = port;

    logToConsole( QString( "Local server ready at port %1" ).arg( port ) );

    // and continue with connect
    d->reconnectTimer->start( 0, true );
}

void MainWindow::serverExitedSlot( int retcode )
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::serverExitedSlot: retcode %d", retcode );

    logToConsole( QString( "Local server exited with return code %1" )
            .arg( retcode ) );

    // schedule server restart in 10 seconds
    if( d->retry ) {

        logToConsole(
                QString( "Scheduling server restart in 10 seconds..." ) );

        d->serverRestartTimer->start( 1000 * 10, true );
        m_panelViewContainer->showServicePanel();
    }

    //qWarning( "<<< MainWindow::serverExitedSlot: retcode %d", retcode );
}

void MainWindow::startConnect()
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::startConnect" );

    //qWarning( "retry is: %d", d->retry );
    logToConsole( QString( "Connecting to the server %1:%2..." )
            .arg( d->host ).arg( d->port ) );

    d->cpa->connectToHost( d->host, d->port );

    //qWarning( "<<< MainWindow::startConnect" );
}

void MainWindow::disconnect()
{
    //qWarning( ">>> MainWindow::disconnect" );

    d->reconnectTimer->stop();

    // disconnect from the server
    if( d->cpa->state() > CommunicationProtocolAdaptor::Idle ) {
        logToConsole( QString( "Disconnecting from the server..." ) );
        d->cpa->disconnect();
    }

    // wait for the connection to die
    while( d->cpa->state() != CommunicationProtocolAdaptor::Idle ) {
        qApp->processEvents();
    }

    //qWarning( "<<< MainWindow::disconnect" );
}

void MainWindow::connectedSlot()
{
    Q_ASSERT( d != NULL );
    //qWarning( ">>> MainWindow::connectedSlot" );
    d->retry = true;
    logToConsole( QString( "Connected to the server,"
                " waiting for the project download to complete..." ) );
    //qWarning( "<<< MainWindow::connectedSlot" );
}

void MainWindow::projectReadySlot()
{
    Q_ASSERT( d != NULL );
    //qWarning( ">>> MainWindow::projectReadySlot" );

    logToConsole( QString( "Project downloaded" ) );

    // store the project we downloaded
    d->project = d->cpa->project();

    // populate service panel and show default/first view
    // third arg indicates whether to open deault/fist view/panel
    // automatically
    d->sp->setProject( d->project, d->defaultView, true );

    // when we have project dependent view policy we check whether to even
    // show view selector, show only for projects with more than one view
    if( d->viewSelectorPolicy == 2 ) {
        if( d->project.views().count() > 1 ) {
            d->sp->setViewSelectorEnabled( true );
        } else {
            d->sp->setViewSelectorEnabled( false );
        }
    }
    // FIXME: panel selector show when there is more than one panel in
    // current view. I don't know whether this is the way
    if( d->panelSelectorPolicy == 2 ) {
        d->sp->setPanelSelectorEnabled( true );
    }

    //qWarning( "<<< MainWindow::projectReadySlot" );
}

void MainWindow::disconnectedSlot()
{
    Q_ASSERT( d != NULL );
    //qWarning( ">>> MainWindow::disconnectedSlot" );

    logToConsole( QString( "Disconnected from the server" ) );

    //qWarning( "<<< MainWindow::disconnectedSlot" );
}

void MainWindow::connectionErrorSlot( int error )
{
    Q_ASSERT( d != NULL );
    //qWarning( ">>> MainWindow::connectionErrorSlot( %d )", error );

    //qWarning( "retry is: %d", d->retry );

    QString msg;
    switch( error ) {
        case CommunicationProtocolAdaptor::ErrHostNotFound:
            msg = "Host not found";
            break;
        case CommunicationProtocolAdaptor::ErrConnectionRefused:
            msg = "Connection refused";
            break;
        case CommunicationProtocolAdaptor::ErrSocket:
            msg = "Socket error";
            break;
        case CommunicationProtocolAdaptor::ErrProtocol:
            msg = "Protocol error";
            break;
    }

    logToConsole( QString( "Connection to the server died: %1" )
            .arg( msg ) );

    // schedule connection restart in 10 seconds iff the server is running
    // or if we connect to the remote server
    if( d->retry ) {
        if( d->mode == 0 ||
                ( d->mode == 1 && d->serverController->isRunning() ) ) {

            logToConsole(
                    QString( "Scheduling reconnect in 10 seconds..." ) );

            d->reconnectTimer->start( 1000 * 10, true );
            m_panelViewContainer->showServicePanel();

        }
    }

    //qWarning( "<<< MainWindow::connectionErrorSlot" );
}

void MainWindow::startStopServerSlot()
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::startStopServerSlot" );

    d->retry = false;

    stopServer();

    //FIXME: use sp getter instead of var
    setProjectNPort( d->sp->projFilePath->text() );

    d->autoStart = false;
    d->retry = true;

    startServer();

    //qWarning( "<<< MainWindow::startStopServerSlot" );
}

void MainWindow::connectDisconnectSlot()
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::connectDisconnectSlot" );

    d->retry = false;

    disconnect();

    //FIXME: use sp getter instead of var
    setHostNPort( d->sp->hostName->text(), d->sp->hostPort->text().toInt() );

    d->autoStart = false;
    d->retry = true;

    startConnect();

    //qWarning( "<<< MainWindow::connectDisconnectSlot" );
}

void MainWindow::selectProjectSlot()
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::selectProjectSlot" );

    QString fn = QFileDialog::getOpenFileName(
            QString::null, /* start dir */
            tr( "Lintouch Projects (*.ltp)" ),
            this, NULL,
            tr( "Choose Lintouch Project" ) );

    setProjectNPort( fn );

    //qWarning( "<<< MainWindow::selectProjectSlot" );
}

void MainWindow::showHideServicePanelSlot()
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::showHideServicePanelSlot" );

    if( m_panelViewContainer->isServicePanelVisible() ) {
        m_panelViewContainer->hideServicePanel();
    } else {
        m_panelViewContainer->showServicePanel();
    }

    //qWarning( "<<< MainWindow::showHideServicePanelSlot" );
}

void MainWindow::maximizeUnmaximizeSlot()
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::maximizeUnmaximizeSlot" );

    if( d->fullscreen ) {
        showNormal();
        d->fullscreen = false;
    } else {
        showFullScreen();
        d->fullscreen = true;
    }

    //qWarning( "<<< MainWindow::maximizeUnmaximizeSlot" );
}

void MainWindow::selectViewPanelSlot( const QString & view,
        const QString & panel )
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::selectViewPanelSlot( "
    //        + view + ", " + panel + " )" );

    logToConsole( QString( "Opening View/Panel: %1/%2..." )
            .arg( view ).arg( panel ) );

    // first populate GUI with templates
    m_panelViewContainer->setProjectView( d->project.views()[ view ] );
    d->view = view;

    if( ! panel.isEmpty() && d->panel != panel ) {
        m_panelViewContainer->displayPanel( panel );
    }

    // then tell the communication core to select view and receive initial
    // variable values
    d->cpa->selectView( view );

    //qWarning( "<<< MainWindow::selectViewPanelSlot( "
    //        + view + ", " + panel + " )" );
}

void MainWindow::panelSelectedSlot( const QString & panel )
{
    Q_CHECK_PTR( d );
    //qWarning( ">>> MainWindow::panelSelectedSlot( " + panel + " )" );

    d->panel = panel;

    //qWarning( "<<< MainWindow::panelSelectedSlot( " + panel + " )" );
}

void MainWindow::customEvent( QCustomEvent * e )
{
    //qWarning( ">>> MainWindow::customEvent( type = %d )", e->type() );

    if( (int)e->type() == (int)Template::IOPinRealValuesChangedEvent ) {
        //at the moment templates changing their iopins' real values are of
        //no interest to us.
    }
    if( (int)e->type() == (int)Template::IOPinRequestedValuesChangedEvent ) {
        //some template changed its iopins' requested values, invoke
        //CommunicationProtocolAdaptor which will forward changed requested
        //values to network
        d->cpa->processPendingEvents();
    }

    //qWarning( "<<< MainWindow::customEvent( type = %d )", e->type() );
}

bool MainWindow::eventFilter( QObject *o, QEvent *e )
{
    // qWarning( ">>> MainWindow::eventFilter" );
    // qWarning( "sending event type %d to 0x%lx", e->type(), (long)o);
    // qWarning( "<<< MainWindow::eventFilter" );
    // return false;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
    //qWarning( ">>> MainWindow::closeEvent( this = 0x%x )", (unsigned)this );

    // do not retry to restart server/connect in a shutdown phase
    d->retry = false;

    // disconnect from the server
    disconnect();

    // stop possibly running local server
    stopServer();

    // then close the window
    e->accept();

    //qWarning( "<<< MainWindow::closeEvent" );

    MainWindowBase::closeEvent(e);
}

void MainWindow::languageChange()
{
    //qWarning( ">>> MainWindow::languageChange" );

    if( d->mode ) {
        setProjectNPort( d->projectFile, d->port, !d->autoStart );
    } else {
        setHostNPort( d->host, d->port, !d->autoStart );
    }

    //qWarning( "<<< MainWindow::languageChange" );
}

void MainWindow::logToConsole(const QString& msg)
{
    d->logger->info(msg);
    d->sp->loggerConsole->appendStdout(msg);
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: MainWindow.cpp 1286 2006-01-16 09:03:55Z modesto $
