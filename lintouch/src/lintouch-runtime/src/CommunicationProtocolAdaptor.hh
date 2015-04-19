// $Id: CommunicationProtocolAdaptor.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: CommunicationProtocolAdaptor.hh -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 25 January 2004
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

#ifndef _COMMPROADAPTOR_HH
#define _COMMPROADAPTOR_HH

#include <qobject.h>

#include <lt/logger/Logger.hh>

#include "lt/cp/cp-client.h"
#include "lt/project/Project.hh"
using namespace lt;

/**
 * CommunicationProtocolAdaptor serves as a C++ abstraction around QSocket
 * and Lintouch Communication Protocol Library (libltcp).
 *
 * This class is entirely asynchronous and uses QSocket underneth, it means
 * that it must be used only in singlethreaded code until stated otherwise
 * in QT Documentation.
 *
 * This class can be reused more times, disconnect() followed by connect()
 * will work as expected.
 *
 * @author Martin Man <mman@swac.cz>
 */
class CommunicationProtocolAdaptor : public QObject
{
    Q_OBJECT

    public:

        /**
         * Communication protocol errors.
         */
        typedef enum
        {
            /**
             * Host not found.
             */
            ErrHostNotFound,

            /**
             * Connection refused.
             */
            ErrConnectionRefused,

            /**
             * Socket read/write error.
             */
            ErrSocket,

            /**
             * higher level problem.
             */
            ErrProtocol,

        } Error;

        /**
         * State of the communication protocol session.
         */
        typedef enum
        {
            /**
             * Not connected.
             */
            Idle,

            /**
             * Connection in progress.
             */
            Connecting,

            /**
             * Connected to the server and having a project url.
             */
            Connected,

            /**
             * Downloading & extracting a project.
             */
            DownloadingProject,

            /**
             * Project ready.
             */
            ProjectDownloaded,

            /**
             * Selecting some view.
             */
            SelectingView,

            /**
             * View selected.
             */
            ViewSelected

        } State;

    public:

        /**
         * Constructor.
         */
        CommunicationProtocolAdaptor( const LoggerPtr & logger,
                QObject * parent, const char* name = 0 );

        /**
         * Destructor.
         */
        virtual ~CommunicationProtocolAdaptor();

    public:

        /**
         * Connect to given host/port.
         * type determines which connection protocol to use.
         */
        void connectToHost( const QString & host, Q_UINT16 port );

        /**
         * Disconnect.
         */
        void disconnect();

        /**
         * Return the host we are connected.
         * Valid when state() >= Connected.
         */
        QString host();

        /**
         * Return the port we are connected to.
         * Valid when state() >= Connected.
         */
        Q_UINT16 port();

        /**
         * Return the project URL.
         * Valid when state() >= Connected.
         */
        const QUrl & projectUrl() const;

        /**
         * Return the server downloaded project.
         * Valid when state() >= ProjectDownloaded.
         */
        const Project & project() const;

        /**
         * Return the selected view.
         * Valid when state() >= ViewSelected.
         */
        QString view() const;

        /**
         * Return the state of the communication protocol.
         */
        CommunicationProtocolAdaptor::State state() const;

    signals:

        /**
         * Connected to given host.
         * Emitted after the TCP/IP connection is successfully initiated.
         */
        void connected();

        /**
         * Disconnected from given host.
         * Emitted after the TCP/IP connection is successfully shutdown.
         */
        void disconnected();

        /**
         * Got project URL.
         * Emitted some time after connect (when successfull).
         */
        void projectUrlReceived();

        /**
         * Got project.
         * Emitted some time after connect (when successfull).
         */
        void projectReady();

        /**
         * Error occured.
         * Emitted when TCP/IP or Communication Protocol occurs.
         */
        void error( int err );

    public slots:

        /**
         * Tell the server we are going to visualize the given view.
         */
        void selectView( const QString & name );

        /**
         * Tell the server we are not going to visualize the given view any
         * more.
         */
        void unselectView();

        /**
         * Examine the registered project for any changes that might occured
         * since the last call to this method and send those changes to the
         * network.
         *
         * You are expected to call this function from time to time when you
         * know something might have changed within the Project (for example
         * due to mouse clicks taking place at panel).
         */
        void processPendingEvents();

    signals:

        /**
         * emitted when events are processed.
         */
        void eventsProcessed();

    protected slots:

        void socketHostFound();
        void socketConnected();
        void socketConnectionClosed();
        void socketReadyRead();
        void socketError( int );

        void projectDownloadProgress( int downloaded, int total );
        void projectDownloaded();
        void projectBuildProgress( int panel, int panelsTotal );
        void projectBuilt( int status );

    private slots:

        /**
         * Start project async builder.
         */
        void startProjectDownload();

    private:

        /**
         * Read available data from the network and place them to the
         * receive buffer. return false in case of problem.
         */
        bool readFromNetwork();

        /**
         * Process data in read buffer and generate send buffer.
         */
        bool processBuffers();

        /**
         * Write data to the network from the send buffer.
         * return false in case of problem.
         */
        bool writeToNetwork();

        /**
         * close socket (while clearing any pending date when hard cleanup
         * is required), disconnect connected project and destroy the
         * protocol session.
         */
        void cleanupSocketNSession( bool hard = false );

    private:

        typedef struct CommunicationProtocolAdaptorPrivate;
        CommunicationProtocolAdaptorPrivate * d;

};

#endif /* _COMMPROADAPTOR_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: CommunicationProtocolAdaptor.hh 1168 2005-12-12 14:48:03Z modesto $
