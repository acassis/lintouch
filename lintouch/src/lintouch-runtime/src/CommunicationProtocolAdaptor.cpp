// $Id: CommunicationProtocolAdaptor.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: CommunicationProtocolAdaptor.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 09 January 2004
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

#include <qsocket.h>
#include <qtimer.h>

#include <lt/project/ProjectAsyncBuilder.hh>
using namespace lt;

#include "CommunicationProtocolAdaptor.hh"

#define RECEIVE_BUFFER_SIZE 65536

struct CommunicationProtocolAdaptor::CommunicationProtocolAdaptorPrivate
{

    CommunicationProtocolAdaptorPrivate() :
        socket( NULL ),
        pb( NULL ),
        session( 0 ),
        receive_buffer( 0 ),
        receive_buffer_len( 0 ),
        send_buffer( 0 ),
        send_buffer_len( 0 ),
        eventTimer( NULL )
    {
    }

    virtual ~CommunicationProtocolAdaptorPrivate()
    {
        delete pb;

        if( session != NULL ) {
            apr_pool_destroy( lt_cp_session_pool_get( session ) );
        }
        delete []receive_buffer;
    };

    // logger
    LoggerPtr logger;

    // communication socket
    QSocket * socket;

    // builder used to download & extract project
    ProjectAsyncBuilder * pb;

    // host
    QString host;

    // port
    Q_UINT16 port;

    // project url
    QUrl projectUrl;

    // downloaded project
    Project project;

    // currently selected view
    QString view;

    // state of the communication
    CommunicationProtocolAdaptor::State state;

    // underlying cp session
    lt_cp_session_t * session;

    // fixed size receive buffer
    char * receive_buffer;

    // how many bytes we have in receive buffer?
    apr_size_t receive_buffer_len;

    // variable size send buffer (not allocated, merely pointing to the cp
    // structures
    char * send_buffer;

    // size of the send buffer
    apr_size_t send_buffer_len;

    // is shutdown in progress ?
    bool closing;

    // processPendingEvents timer
    QTimer * eventTimer;
};

CommunicationProtocolAdaptor::CommunicationProtocolAdaptor(
        const LoggerPtr & logger,
        QObject * parent, const char* name /* = 0 */ ) :
    QObject( parent, name ),
    d( new CommunicationProtocolAdaptorPrivate() )
{
    Q_ASSERT( d );

    // remember logger
    d->logger = logger;

    // create socket and conect its signals
    d->socket = new QSocket( this );
    Q_CHECK_PTR( socket );

    connect( d->socket, SIGNAL( hostFound() ),
            this, SLOT( socketHostFound() ) );
    connect( d->socket, SIGNAL( connected() ),
            this, SLOT( socketConnected() ) );
    connect( d->socket, SIGNAL( connectionClosed() ),
            this, SLOT( socketConnectionClosed() ) );
    connect( d->socket, SIGNAL( delayedCloseFinished() ),
            this, SLOT( socketConnectionClosed() ) );
    connect( d->socket, SIGNAL( readyRead() ),
            this, SLOT( socketReadyRead() ) );
    connect( d->socket, SIGNAL( error( int ) ),
            this, SLOT( socketError( int ) ) );

    // create project builder and connect its signals
    d->pb = new ProjectAsyncBuilder( d->logger );
    Q_CHECK_PTR( d->pb );

    connect( d->pb, SIGNAL( downloaded() ),
            this, SLOT( projectDownloaded() ) );
    connect( d->pb, SIGNAL( finished( int ) ),
            this, SLOT( projectBuilt( int ) ) );
    connect( d->pb, SIGNAL( downloadProgress( int, int ) ),
            this, SLOT( projectDownloadProgress( int , int ) ) );
    connect( d->pb, SIGNAL( buildProgress( int, int ) ),
            this, SLOT( projectBuildProgress( int , int ) ) );

    // create receive buffer of a fixed size
    d->receive_buffer = new char[ RECEIVE_BUFFER_SIZE ];
    Q_CHECK_PTR( d->receive_buffer );

    // inital state of the session
    d->state = CommunicationProtocolAdaptor::Idle;

    // processPendingEvents timer
    d->eventTimer = new QTimer( this );
    Q_CHECK_PTR( d->eventTimer );
    connect( d->eventTimer, SIGNAL( timeout() ),
            this, SLOT( processPendingEvents() ) );
}

CommunicationProtocolAdaptor::~CommunicationProtocolAdaptor()
{
    delete d;
}

void CommunicationProtocolAdaptor::connectToHost(
        const QString & host, Q_UINT16 port )
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    //qWarning( ">>> CommunicationProtocolAdaptor::connectToHost()" );

    d->closing = 0;

    d->state = CommunicationProtocolAdaptor::Connecting;

    d->socket->connectToHost( host, port );

    //qWarning( "<<< CommunicationProtocolAdaptor::connectToHost()" );
}

void CommunicationProtocolAdaptor::disconnect()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    //qWarning( ">>> CommunicationProtocolAdaptor::disconnect()" );

    // remember that we wanted to close
    d->closing = 1;

    // perform clean shutdown when we are already connected
    if( d->session != NULL ) {

        // first unselect view if some is selected
        if( lt_cp_session_is_view_selected( d->session ) ) {
            lt_cp_session_send_msg_unselect_view( d->session );
        }

        // send disconnect only when ready
        if( lt_cp_session_is_alive( d->session ) ) {
            lt_cp_session_send_msg_disconnect( d->session );
        }

        processPendingEvents();
    }

    // non-hard close
    cleanupSocketNSession( false );

    //qWarning( "<<< CommunicationProtocolAdaptor::disconnect()" );
}

const QUrl & CommunicationProtocolAdaptor::projectUrl() const
{
    Q_ASSERT( d );
    return d->projectUrl;
}

const Project & CommunicationProtocolAdaptor::project() const
{
    Q_ASSERT( d );
    return d->project;
}

QString CommunicationProtocolAdaptor::view() const
{
    Q_ASSERT( d );
    return d->view;
}

CommunicationProtocolAdaptor::State
CommunicationProtocolAdaptor::state() const
{
    Q_ASSERT( d );
    return d->state;
}

void CommunicationProtocolAdaptor::selectView( const QString & name )
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );
    //qWarning( ">>> CommunicationProtocolAdaptor::selectView(" + name + ")" );

    if( lt_cp_session_is_view_selected( d->session ) ) {
        //qWarning("View already selected, sending unselect first");
        lt_cp_session_send_msg_unselect_view( d->session );
    } else {
        //qWarning("No View selected, sending select directly");
    }

    //FIXME: what about view names with international characters?
    lt_cp_session_send_msg_select_view( d->session, name.latin1() );

    processPendingEvents();

    //qWarning( "<<< CommunicationProtocolAdaptor::selectView()" );
}

void CommunicationProtocolAdaptor::unselectView()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );
    //qWarning( ">>> CommunicationProtocolAdaptor::unselectView()" );

    lt_cp_session_send_msg_unselect_view( d->session );

    processPendingEvents();

    //qWarning( "<<< CommunicationProtocolAdaptor::unselectView()" );
}

void CommunicationProtocolAdaptor::processPendingEvents()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    //qWarning( ">>> CommunicationProtocolAdaptor::processPendingEvents()" );

    // if we don't have a session, we don't need to process any events
    if( d->session == NULL ) return;

    // fetch requested values from project connections to our cp core.
    d->project.syncRequestedValues();

    // read all available data from the socket
    if( ! readFromNetwork() ) {
        emit( error( CommunicationProtocolAdaptor::ErrSocket ) );
        cleanupSocketNSession( true );
        return;
    }

    // process read data and generate data to be sent
    if( ! processBuffers() ) {
        emit( error( CommunicationProtocolAdaptor::ErrProtocol ) );
        cleanupSocketNSession( true );
        return;
    }

    // write all available data to the socket
    if( ! writeToNetwork() ) {
        emit( error( CommunicationProtocolAdaptor::ErrSocket ) );
        cleanupSocketNSession( true );
        return;
    }

    // write real values from project connections to connected templates.
    d->project.syncRealValues();

    // now examine in which state we are and where we are going

    if( d->state < CommunicationProtocolAdaptor::Connected ) {

        // not yet connected,
        // but already having a session created in socketConnected
        if( d->session != NULL && lt_cp_session_is_alive( d->session ) ) {

            // go to connected state
            d->state = CommunicationProtocolAdaptor::Connected;

            // remember url
            d->projectUrl = lt_cp_session_project_url_get( d->session );
            emit( projectUrlReceived() );

            // schedule project download
            QTimer::singleShot( 0, this, SLOT( startProjectDownload() ) );
        }

    } else if( d->state < CommunicationProtocolAdaptor::ViewSelected ) {

        // view not yet selected, but we are trying to select a view
        if( lt_cp_session_is_view_selection_inprogress( d->session ) ) {

            // go the selecting view state
            d->state = CommunicationProtocolAdaptor::SelectingView;

        }

        if( lt_cp_session_is_view_selected( d->session ) ) {

            // go to view selected state
            d->state = CommunicationProtocolAdaptor::ViewSelected;
        }
    }

    //qWarning( "emitting eventsProcessed" );
    emit( eventsProcessed() );

    //qWarning( "<<< CommunicationProtocolAdaptor::processPendingEvents()" );
}

void CommunicationProtocolAdaptor::socketHostFound()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    //qWarning( ">>> CommunicationProtocolAdaptor::socketHostFound()" );
    //qWarning( "<<< CommunicationProtocolAdaptor::socketHostFound()" );
}

void CommunicationProtocolAdaptor::socketConnected()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    //qWarning( ">>> CommunicationProtocolAdaptor::socketConnected()" );

    // create session & send msg connect
    lt_cp_session_create( &d->session, LT_CP_CLIENT, NULL );
    Q_CHECK_PTR( d->session );
    lt_cp_session_send_msg_connect( d->session );

    // let the others know we are somewhat connected
    emit( connected() );

    // schedule process when in socket generated slot
    //qWarning( "CPA::socketConnected: starting 0-timer for processPEvents" );
    d->eventTimer->start( 0, true );

    //qWarning( "<<< CommunicationProtocolAdaptor::socketConnected()" );
}

void CommunicationProtocolAdaptor::socketConnectionClosed()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    //qWarning( ">>> CommunicationProtocolAdaptor::socketConnectionClosed" );

    d->state = CommunicationProtocolAdaptor::Idle;

    // if the disconnect was not initiated from our side, we treat it as a
    // protocol error
    if( ! d->closing ) {
        emit( error( CommunicationProtocolAdaptor::
                    ErrProtocol ) );
    } else {
        emit( disconnected() );
    }

    //qWarning( "<<< CommunicationProtocolAdaptor::socketConnectionClosed" );
}

void CommunicationProtocolAdaptor::socketReadyRead()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    //qWarning( ">>> CommunicationProtocolAdaptor::socketReadyRead" );

    // schedule process when in socket generated slot
    //qWarning( "CPA::socketReadyRead: starting 0-timer for processPEvents" );
    d->eventTimer->start( 0, true );

    //qWarning( "<<< CommunicationProtocolAdaptor::socketReadyRead" );
}

void CommunicationProtocolAdaptor::socketError( int err )
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    //qWarning( ">>> CommunicationProtocolAdaptor::socketError( %d )", err );

    d->state = CommunicationProtocolAdaptor::Idle;

    int newerr;
    switch( err ) {
        case QSocket::ErrHostNotFound:
            newerr = CommunicationProtocolAdaptor::ErrHostNotFound;
            break;
        case QSocket::ErrConnectionRefused:
            newerr = CommunicationProtocolAdaptor::ErrConnectionRefused;
            break;
        case QSocket::ErrSocketRead:
            newerr = CommunicationProtocolAdaptor::ErrSocket;
            break;
        default:
            newerr = CommunicationProtocolAdaptor::ErrProtocol;
    }

    emit( error( newerr ) );

    // forget everything
    cleanupSocketNSession( true );

    //qWarning( "<<< CommunicationProtocolAdaptor::socketError()" );
}

void CommunicationProtocolAdaptor::projectDownloadProgress(
        int /* downloaded */, int /* total */ )
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );
    //qWarning( ">>> CommunicationProtocolAdaptor::projectDownloadProgress("
    //        " done=%d, total=%d", downloaded, total );
}

void CommunicationProtocolAdaptor::projectDownloaded()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );
    //qWarning( ">>> CommunicationProtocolAdaptor::projectDownloaded" );
    //qWarning( "<<< CommunicationProtocolAdaptor::projectDownloaded" );
}

void CommunicationProtocolAdaptor::projectBuildProgress(
        int /* panel */, int /* panelsTotal */ )
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );
    //qWarning( ">>> CommunicationProtocolAdaptor::projectBuildProgress("
    //        " done=%d, total=%d", panel, panelsTotal );
}

void CommunicationProtocolAdaptor::projectBuilt( int status )
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );
    //qWarning( ">>> CommunicationProtocolAdaptor::projectBuilt" );

    if( status == ProjectAsyncBuilder::Finished ) {
        // go to project downloaded state
        d->state = CommunicationProtocolAdaptor::ProjectDownloaded;

        // remember project
        d->project = d->pb->getProject();

        // register project connections to our varset
        ConnectionDict cd = d->project.connections();
        for( unsigned i = 0; i < cd.count(); ++i ) {
            QCString key = cd.keyAt(i).utf8();
            QCString key1 = key.replace( ".", "_" );

            lt_cp_session_variables_register( d->session,
                    key1.data(), cd[i]->var_set_t() );

        }

        emit( projectReady() );

    } else {
        // disconnect
        cleanupSocketNSession( true );
    }

    //qWarning( "<<< CommunicationProtocolAdaptor::projectBuilt" );
}

void CommunicationProtocolAdaptor::startProjectDownload()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );
    Q_ASSERT( d->pb );
    //qWarning( ">>> CommunicationProtocolAdaptor::startProjectDownload" );

    // go to downloading project state
    d->state = CommunicationProtocolAdaptor::DownloadingProject;

    // build a project from given URL
    d->pb->buildFromUrl( d->projectUrl );

    //qWarning( "<<< CommunicationProtocolAdaptor::startProjectDownload" );
}

bool CommunicationProtocolAdaptor::readFromNetwork()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );

    // see how many bytes we can receive from the socket
    Q_LONG available = d->socket->bytesAvailable();
    Q_LONG read_total = 0, read = 0;

    //qWarning( "CPA: can read %lu bytes", available );

    if( available ) {

        //FIXME: we need some buffer size incrementing
        if( available > RECEIVE_BUFFER_SIZE ) {
            qWarning( "CPA: more data in a buffer than we can handle" );
        }

        do {
            // read a batch
            read = d->socket->readBlock(
                    d->receive_buffer + read_total,
                    RECEIVE_BUFFER_SIZE - read_total );

            // when read error occured
            if( read == -1 ) {
                return false;
            }

            // accummulate
            read_total += read;

        } while( read_total < available );
    }

    d->receive_buffer_len = available;

    return true;
}

bool CommunicationProtocolAdaptor::processBuffers()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );

    //qWarning( "CPA: going to process %d received bytes", read );

    if( lt_cp_session_process( d->session,
                d->receive_buffer, d->receive_buffer_len,
                (void**)&d->send_buffer, &d->send_buffer_len ) != LT_CP_SUCCESS ) {

        //qWarning( "CPA: ERROR LIKE A PIG: process returned error" );

        return false;
    }

    return true;
}

bool CommunicationProtocolAdaptor::writeToNetwork()
{
    Q_ASSERT( d );
    Q_ASSERT( d->socket );
    Q_ASSERT( d->session );

    if( d->send_buffer != NULL && d->send_buffer_len > 0 ) {

        //qWarning( "CPA: should send %u bytes", d->send_buffer_len );

        // send data from session_process to the socket
        Q_ULONG written_total = 0;
        Q_LONG written = 0;

        do {
            // write batch
            written = d->socket->writeBlock(
                    d->send_buffer + written_total,
                    d->send_buffer_len - written_total );

            // when write error occured
            if( written == -1 ) {
                return false;
            }

            // accummulate
            written_total += written;

        } while( written_total < d->send_buffer_len );
    }

    return true;
}

void CommunicationProtocolAdaptor::cleanupSocketNSession(
        bool hard /* = false */ )
{

    //qWarning( QString(">>> CommunicationProtocolAdaptor::"
    //            "cleanupSocketNSession( hard=%1 )").arg(hard) );

    Q_ASSERT( d );
    Q_ASSERT( d->socket );

    // forget any scheduled processPendingEvents
    d->eventTimer->stop();

    if( hard ) {
        //qWarning("clearing pending data on socket");
        d->socket->clearPendingData();
    }

    //qWarning("closing socket in socket->state %d...", d->socket->state());
    d->socket->close();

    // disconnect all varsets from all connections from the cp session
    ConnectionDict cd = d->project.connections();

    for(unsigned i = 0; i < cd.count(); ++i ) {
        lt_var_set_source_set( cd[i]->var_set_t(), NULL );
    }

    // and destroy cp session
    if( d->session ) {
        lt_cp_session_destroy( d->session );
        apr_pool_destroy( lt_cp_session_pool_get( d->session ) );
        d->session = NULL;
    }

    // reset internal variables
    d->state = CommunicationProtocolAdaptor::Idle;
    d->project = Project();
    d->projectUrl = "";
    d->host = QString::null;
    d->port = 0;
    d->view = QString::null;

    //qWarning( "<<< CommunicationProtocolAdaptor::cleanupSocketNSession" );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: CommunicationProtocolAdaptor.cpp 1168 2005-12-12 14:48:03Z modesto $
