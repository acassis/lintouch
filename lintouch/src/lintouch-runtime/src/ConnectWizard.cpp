// $Id: ConnectWizard.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: ConnectWizard.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 08 January 2004
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
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtextedit.h>
#include <qtimer.h>

#include <lt/templates/Info.hh>
#include <lt/project/ProjectAsyncBuilder.hh>
#include <lt/project/View.hh>

#include "CommunicationProtocolAdaptor.hh"
#include "ConnectWizard.hh"

using namespace lt;

struct ConnectWizard::ConnectWizardPrivate
{
    ConnectWizardPrivate() : cpa( NULL ), pb( NULL ),
    autoload( false ), host_empty( true ),
    port_empty( true ), viewItem( NULL ), reconnectTimer( NULL ) {}

    CommunicationProtocolAdaptor * cpa;

    Project project;

    ProjectAsyncBuilder* pb;
    LoggerPtr logger;

    // defaults as specified on commandline
    QString viewId;

    bool autoload;
    bool should_wait;

    bool host_empty;
    bool port_empty;

    QMap< QListBoxItem*, QString > viewMap;

    QListBoxItem * viewItem;

    QTimer * reconnectTimer;
    unsigned tries;
};

ConnectWizard::ConnectWizard( CommunicationProtocolAdaptor * cpa,
        LoggerPtr logger, QWidget* parent,
        const char* name /*= 0*/, bool modal /*= true*/, WFlags fl /*= 0*/ ) :
    ConnectWizardBase( parent, name, modal, fl ),
    d( new ConnectWizardPrivate )
{
    Q_CHECK_PTR( d );

    // remember logger
    d->logger = logger;

    connect( m_buildLog, SIGNAL( textChanged() ),
            m_buildLog, SLOT( scrollToBottom() ) );

    // communication protocol adaptor stuff
    d->cpa = cpa;
    Q_CHECK_PTR( cpa );

    connect( d->cpa, SIGNAL( connected() ),
            this, SLOT( slotConnected() ) );
    connect( d->cpa, SIGNAL( disconnected() ),
            this, SLOT( slotDisconnected() ) );
    connect( d->cpa, SIGNAL( receivedProjectUrl( const QString & ) ),
            this, SLOT( slotReceivedProjectUrl( const QString & ) ) );
    connect( d->cpa, SIGNAL( error( int ) ),
            this, SLOT( slotError( int ) ) );

    // project builder stuff
    d->pb = new ProjectAsyncBuilder( d->logger );
    Q_CHECK_PTR( d->pb );

    connect( d->pb, SIGNAL( downloaded() ),
            this, SLOT( slotDownloaded() ) );
    connect( d->pb, SIGNAL( finished( int ) ),
            this, SLOT( slotFinished( int ) ) );
    connect( d->pb, SIGNAL( downloadProgress( int, int ) ),
            this, SLOT( slotDownloadProgress( int , int ) ) );
    connect( d->pb, SIGNAL( buildProgress( int, int ) ),
            this, SLOT( slotBuildProgress( int , int ) ) );

    m_moveUp->setText( tr( "Up" ) );
    m_moveUp->setPixmap( QPixmap::fromMimeSource( "stock_up.png" ) );
    m_moveDown->setText( tr( "Down" ) );
    m_moveDown->setPixmap( QPixmap::fromMimeSource( "stock_down.png" ) );

    connect( m_moveUp, SIGNAL( clicked() ), this, SLOT( slotMoveUp() ) );
    connect( m_moveDown, SIGNAL( clicked() ), this, SLOT( slotMoveDown() ) );

    d->reconnectTimer = new QTimer( this );
    connect( d->reconnectTimer, SIGNAL( timeout() ),
            this, SLOT( startConnect() ) );

    // we don't have help
    helpButton()->hide();

    // allow localization of wizard buttons
    nextButton()->setText( tr( "&Next >" ) );
    backButton()->setText( tr( "< &Back" ) );
    cancelButton()->setText( tr( "&Cancel" ) );
    finishButton()->setText( tr( "&Select" ) );

    // we have back/next disabled on progress page by default
    setBackEnabled( ProgressPage, false );
    setNextEnabled( ProgressPage, false );
}

ConnectWizard::~ConnectWizard()
{
    Q_CHECK_PTR( d );

    if( d->pb ) {
        delete d->pb;
    }

    delete d;
}

void ConnectWizard::preselectHost( const QString & host ) {
    m_serverHost->setText( host );
    //slotHostInputChanged( host );
}

void ConnectWizard::preselectPort( int port ) {
    QString n = QString::number( port );
    m_serverPort->setText( n );
    //slotPortInputChanged( n );
}

void ConnectWizard::preselectView( const QString & viewId ) {
    d->viewId = viewId;
}

int ConnectWizard::execFromConnect( bool go_ahead, bool wait )
{
    // allow going back from view selection
    setBackEnabled( SelectViewPage, true );

    showPage( ConnectPage );

    d->tries = 1;

    // start automatically when we have a host and port and user wants it
    d->autoload = go_ahead &&
        ! m_serverHost->text().isEmpty() &&
        m_serverPort->text().toUInt() != 0;

    if( d->autoload ) {
        d->should_wait = wait;
        next();
    } else {
        d->should_wait = 0;
    }

    return exec();
}

int ConnectWizard::execFromSelect()
{
    // don't allow going back from view selection
    setBackEnabled( SelectViewPage, false );

    showPage( SelectViewPage );

    return exec();
}

QString ConnectWizard::host() const
{
    return m_serverHost->text();
}

unsigned int ConnectWizard::port() const
{
    return m_serverPort->text().toUInt();
}

QString ConnectWizard::selectedViewId() const
{
    return d->viewId;
}

Project ConnectWizard::project() const
{
    return d->project;
}

void ConnectWizard::forgetProject()
{
    d->project = Project();
    d->pb->forgetProject();
}

void ConnectWizard::showPage( QWidget * page ) {
    // when on connect page, focus host input 
    if( page == ConnectPage ) {
        m_serverHost->setFocus();
    }

    // when on select view page, focus view selector
    if( page == SelectViewPage ) {
        m_viewList->setFocus();
    }

    ConnectWizardBase::showPage( page );
}

void ConnectWizard::back()
{
    //qWarning( ">>> ConnectWizard::back" );

    // when selecting view, disconnect and go back to progress, next back
    // will take us to the first page.
    if( currentPage() == SelectViewPage ) {
        d->cpa->disconnect();
        showPage( ConnectPage );
    } else {

        // when on progress page, ensure the timer stops and we go back to
        // manual mode
        if( currentPage() == ProgressPage ) {
            d->autoload = false;
            d->should_wait = false;
            d->reconnectTimer->stop();
            d->logger->info( "Automatic reconnect disabled" );
        }

        // otherwise go back as usually
        ConnectWizardBase::back();

    }

    //qWarning( "<<< ConnectWizard::back" );
}

void ConnectWizard::next()
{
    //qWarning( ">>> ConnectWizard::next" );

    // when on connect page try to connect
    if( currentPage() == ConnectPage ) {

        if( d->should_wait ) {
            //when we should wait we only start the timer
            d->tries ++;
            m_buildProgress->reset();
            m_buildLog->clear();
            m_buildLog->append(
                    tr( "Waiting 10 seconds before "
                        "attempting to reconnect" ) );
            d->logger->info(
                    "Waiting 10 seconds before "
                    "attempting to reconnect" );
            d->reconnectTimer->start( 10 * 1000, true );

            // go display progress
            ConnectWizardBase::next();

        } else {
            // begin connection
            startConnect();
        }

    } else {
        // otherwise normal next
        ConnectWizardBase::next();
    }

    //qWarning( "<<< ConnectWizard::next" );
}

void ConnectWizard::accept()
{
    //qWarning( ">>> ConnectWizard::accept" );

    d->viewId = d->viewMap [ d->viewItem = m_viewList->selectedItem() ];

    ConnectWizardBase::accept();

    //qWarning( "<<< ConnectWizard::accept" );
}

void ConnectWizard::reject()
{
    //qWarning( ">>> ConnectWizard::reject" );
    //qWarning( "CPA state is %d", d->cpa->state() );

    // when on progress page and not idle, cancel only stops progress

    // when on progress page, stop the builder, disconnect and possibly
    // close wizard
    if( currentPage() == ProgressPage ) {

        // ensure timer is not running
        d->reconnectTimer->stop();
        d->logger->info( "Automatic reconnect disabled" );

        // check whether the connection is active.
        bool should_stay =
            ( d->cpa->state() > CommunicationProtocolAdaptor::Idle );

        setBackEnabled( ProgressPage, true );

        // and abort when build was active.
        if( should_stay ) {
            // disconnect from the server
            d->cpa->disconnect();

            // stop the builder (just in case it's been already running)
            d->pb->cancel();

            m_buildLog->append( tr( "Aborting as requested..." ) );
            d->logger->info( "Abort requested by user..." );
            return;
        }
    }

    // when on select view page, close wizard and select view in list box to
    // the one which runtime is displaying
    if( currentPage() == SelectViewPage ) {
        m_viewList->setSelected( d->viewItem, true );
    }

    // and do normal reject which will close the wizard
    ConnectWizardBase::reject();

    //qWarning( "<<< ConnectWizard::reject" );
}

void ConnectWizard::slotMoveUp()
{
    // when nothing is selected, select the first
    QListBoxItem* i = m_viewList->selectedItem();
    if( !i ) {
        m_viewList->setSelected( 0, true );
        return;
    }
    // when previous exists, select it, otherwise do nothing
    if( i->prev() ) {
        m_viewList->setSelected( i->prev(), true );
    } else {
        m_viewList->setSelected( i, true );
    }
}

void ConnectWizard::slotMoveDown()
{
    // when nothing is selected, select the first
    QListBoxItem* i = m_viewList->selectedItem();
    if( !i ) {
        m_viewList->setSelected( 0, true );
        return;
    }
    // when next exists, select it, otherwise do nothing
    if( i->next() ) {
        m_viewList->setSelected( i->next(), true );
    } else {
        m_viewList->setSelected( i, true );
    }
}

void ConnectWizard::slotViewIdHighlighted( QListBoxItem* i )
{
    setFinishEnabled( SelectViewPage, i != NULL );
}

void ConnectWizard::slotViewIdSelected( QListBoxItem* i )
{
    if( i ) {
        d->viewId = d->viewMap[ i ];
        d->viewItem = i;
        accept();
    }
}

void ConnectWizard::slotHostInputChanged( const QString& text )
{
    d->host_empty = text.isEmpty();

    setNextEnabled( ConnectPage, !d->host_empty && !d->port_empty );
}

void ConnectWizard::slotPortInputChanged( const QString& text )
{
    d->port_empty = text.isEmpty();

    setNextEnabled( ConnectPage, !d->host_empty && !d->port_empty );
}

//////////

void ConnectWizard::slotConnected() {
    //qWarning( ">>> ConnectWizard::slotConnected" );

    m_buildLog->append( tr( "Connected to %1:%2" )
        .arg( m_serverHost->text() ).arg( m_serverPort->text() ) );
    d->logger->info( QString( "Connected to %1:%2" )
        .arg( m_serverHost->text() ).arg( m_serverPort->text() ) );

    //qWarning( "<<< ConnectWizard::slotConnected" );
}

void ConnectWizard::slotDisconnected() {
    //qWarning( ">>> ConnectWizard::slotDisconnected" );

    m_buildLog->append( tr( "Disconnected from %1:%2" )
        .arg( m_serverHost->text() ).arg( m_serverPort->text() ) );
    d->logger->info( QString( "Disconnected from %1:%2" )
        .arg( m_serverHost->text() ).arg( m_serverPort->text() ) );

    // if we accidentally have a project downloaded and building, do not let
    // him progress any further
    d->pb->cancel();

    setBackEnabled( ProgressPage, true );

    //qWarning( "<<< ConnectWizard::slotDisconnected" );
}

void ConnectWizard::slotReceivedProjectUrl( const QString & url ) {
    //qWarning( ">>> ConnectWizard::slotReceivedProjectUrl( " +
    //        url + " )" );

    m_buildLog->append( tr( "Project location is %1" ).arg( url ) );
    d->logger->info( QString( "Project location is %1" ).arg( url ) );

    startProjectDownload( QUrl( url ) );

    //qWarning( "<<< ConnectWizard::slotReceivedProjectUrl" );
}

void ConnectWizard::slotError( int err ) {
    //qWarning( ">>> ConnectWizard::slotError(%d)", err );

    // ensure the progress page is shown on error
    showPage( ProgressPage );

    QString msg;
    switch( ( CommunicationProtocolAdaptor::Error ) err ) {
        case CommunicationProtocolAdaptor::ErrHostNotFound:
            msg = tr( "ERROR: Host %1 not found" )
                .arg( m_serverHost->text() );
            d->logger->error( QString( "Host %1 not found" )
                    .arg( m_serverHost->text() ) );
            break;
        case CommunicationProtocolAdaptor::ErrConnectionRefused:
            msg = tr( "ERROR: Connection refused by %1:%2" )
                .arg( m_serverHost->text() )
                .arg( m_serverPort->text() );
            d->logger->error( QString( "Connection refused by %1:%2" )
                    .arg( m_serverHost->text() )
                    .arg( m_serverPort->text() ) );
            break;
        case CommunicationProtocolAdaptor::ErrSocketRead:
            msg = tr( "ERROR: Connection reset by peer" );
            d->logger->error( QString( "Connection reset by peer" ) );
            break;
        case CommunicationProtocolAdaptor::ErrCommunicationProtocolError:
            msg = tr( "ERROR: Connection reset by peer" );
            d->logger->error( QString( "Connection reset by peer" ) );
            break;
    }

    m_buildLog->append( QString( "<font color=red>%1</font>" ).arg(msg) );

    if( d->autoload ) {
        m_buildLog->append(
                tr( "Reconnecting in 10 seconds (attempt %1)" )
                .arg( d->tries ) );
        d->logger->info( QString ( "Reconnecting in 10 seconds (attempt %1)" )
                .arg( d->tries ) );
        // singleshot after 10 seconds
        d->reconnectTimer->start( 10 * 1000, true );
        d->tries ++;
    }

    // stop the builder anyway
    d->pb->cancel();

    // disconnect from the server anyway
    d->cpa->disconnect();

    setBackEnabled( ProgressPage, true );

    //qWarning( "<<< ConnectWizard::slotError(%d)", err );
}

void ConnectWizard::slotFinished( int status )
{
    //qWarning( ">>> ConnectWizard::slotFinished( status=%d )", status );

    if ( status == ProjectAsyncBuilder::Error ) {

        m_buildLog->append( "<font color=red>" +
                tr( "ERROR: Unable to download project, "
                    "check Logger for more details" ) +
                "</font>" );
        d->logger->error( "Unable to download project, "
                "check messages above for more details" );

        d->cpa->disconnect();

    } else if( status == ProjectAsyncBuilder::Cancelled ) {

        // user cancel handled by reject.

    } else {

        // set total steps only when sucessfull, otherwise leave it where
        // it finished...
        m_buildProgress->setProgress( m_buildProgress->totalSteps() );

        // well, the builder has prepared a project for us
        Project tp = d->pb->getProject();

        // if it does have at least one view
        if( tp.isValid() && tp.views().count() > 0 ) {

            m_buildLog->append( tr( "Completed" ) );
            d->logger->info( "Completed" );

            // we update the selection dialog
            projectReady( tp );

            // and if he has only one view, we just select it
            if( tp.views().count() == 1 ) {
                d->viewId = tp.views().keys().first();
            }

            // sanity check
            if( ! d->viewId.isEmpty() && tp.views().contains( d->viewId ) )
            {
                accept();
            }

            // register project with CommunicationProtocolAdaptor
            d->cpa->registerProject( tp );

            // and go to select view page
            showPage( SelectViewPage );

        } else {

            m_buildLog->append( "<font color=red>" +
                    tr( "ERROR: Project contains errors, "
                        "check Logger for more details" ) +
                    "</font>" );
            d->logger->error( "Project contains errors, "
                    "check messages above for more details" );

            d->cpa->disconnect();

        }
    }

    //qWarning( "<<< ConnectWizard::slotFinished( status=%d )", status );
}

void ConnectWizard::slotDownloaded()
{
    //qWarning( ">>> ConnectWizard::slotDownloaded" );

    m_buildLog->append( tr( "Building project..." ) );
    d->logger->debug( "Building project..." );

    //qWarning( "<<< ConnectWizard::slotDownloaded" );
}

void ConnectWizard::slotDownloadProgress( int downloaded, int total )
{
    //qWarning( ">>> ConnectWizard::slotDownloadProgress(%d, %d)",
    //        downloaded, total );

    // download progress to the half of the progress bar
    if( total != -1 ) {
        m_buildProgress->setProgress( downloaded, total * 2 );
    } else {
        m_buildProgress->setProgress( downloaded, 0 );
    }

    // here we don't need to process events since we wait for data on a
    // socket

    //qWarning( "<<< ConnectWizard::slotDownloadProgress(%d, %d)",
    //        downloaded, total );
}

void ConnectWizard::slotBuildProgress(int panel, int panelsTotal)
{
    //qWarning( ">>> ConnectWizard::slotBuildProgress(%d, %d)",
    //        panel, panelsTotal );

    float progr = 50 * panel/(float)panelsTotal;

    // build progress from half to the end
    m_buildProgress->setProgress( 50 + (int)progr, 100 );

    // we need to process pending events otherwise cancel button won't work
    qApp->processEvents();

    //qWarning( "<<< ConnectWizard::slotBuildProgress(%d, %d)",
    //        panel, panelsTotal );
}

void ConnectWizard::startConnect()
{
    m_buildLog->clear();
    m_buildLog->append( tr( "Connecting to %1:%2" )
            .arg( m_serverHost->text() ).arg( m_serverPort->text() ) );
    d->logger->info( QString( "Connecting to %1:%2" )
            .arg( m_serverHost->text() ).arg( m_serverPort->text() ) );

    // reset to initial position
    m_buildProgress->reset();

    // disable back before starting signal slot hell
    setBackEnabled( ProgressPage, false );

    // try to connect and wait for connected/receivedProjectUrl or error
    d->cpa->connectToHost( m_serverHost->text(),
            m_serverPort->text().toUInt() );

    // and display progress
    showPage( ProgressPage );
}

void ConnectWizard::startProjectDownload( const QUrl& path )
{
    //qWarning( ">>> ConnectWizard::startProjectDownload( url= " +
    //        path.toString() + " )" );

    m_buildLog->append( tr( "Downloading project..." ) );
    d->logger->info( "Downloading project..." );

    d->pb->buildFromUrl( path );

    //qWarning( "<<< ConnectWizard::startProjectDownload" );
}

void ConnectWizard::projectReady( const Project & p )
{
    bool any_selected = 0;

    d->project = p;

    setFinishEnabled( SelectViewPage, false );

    m_viewList->clear();
    d->viewMap.clear();

    QValueVector<QString> vv = p.views().keys();
    for( unsigned i = 0; i < vv.count(); i++ ) {

        QListBoxText* item = new QListBoxText( m_viewList,
                p.localizator()->localizedMessage( vv[ i ] ) );
        d->viewMap.insert( item, vv[ i ] );

        // preselect view in listbox
        if( d->viewId == vv[ i ] ) {
            m_viewList->setSelected( item, true );
            any_selected = true;
        }
    }

    if( ! any_selected && p.views().count() > 0 ) {
        m_viewList->setSelected( 0, true );
    }
}

void ConnectWizard::layOutButtonRow ( QHBoxLayout* layout )
{
    ConnectWizardBase::layOutButtonRow( layout );

    layout->setMargin( 5 );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ConnectWizard.cpp 1168 2005-12-12 14:48:03Z modesto $
