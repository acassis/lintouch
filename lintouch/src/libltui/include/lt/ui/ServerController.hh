// $Id: ServerController.hh,v 1.7 2004/08/15 18:17:59 mman Exp $
//
//   FILE: ServerController.hh -- 
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

#ifndef _SERVERCONTROLLER_HH
#define _SERVERCONTROLLER_HH

#include <qobject.h>
#include <qstring.h>

class Console;

namespace lt {

    /**
     * ServerController.
     *
     * This class allows you to start Lintouch Server with given Project
     * while listening at given interface:port.
     *
     * If you are interested in server output, use lt::Console class to
     * create GUI output window.
     *
     * @author Martin Man <mman@swac.cz>
     */
    class ServerController : public QObject
    {
        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            ServerController( QObject* parent = 0, const char* name = 0 );

            /**
             * Destructor.
             */
            virtual ~ServerController();

        public:

            /**
             * Set project to be used. The specified project will be used
             * from the next startup.
             *
             * @param project The project to be used by the server.
             */
            void setProject( const QString & project );

            /**
             * Return the currently specified project.
             */
            QString project() const;

            /**
             * Set server socket port to be used. The specified port will be
             * used from the next startup. When 0 is specified, an unused
             * port will be choosen and you can later query it via ``port``
             * method.
             *
             * @param port TCP/IP port.
             */
            void setPort( Q_UINT16 port );

            /**
             * Return the server socket port as specified via ``setPort`` or
             * as assigned by the OS.
             */
            Q_UINT16 port() const;

            /**
             * Set local mode of the server.
             *
             * In local mode, the server binds to a loopback interface only
             * (127.0.0.1) and serves the project via file:/// URL thus
             * avoiding HTTP download.
             *
             * local mode is on by default.
             *
             * @param local Whether the server should be local or not.
             */
            void setLocal( bool local );

            /**
             * Return the local flag of the server.
             *
             * @see setLocal
             */
            bool local() const;

        public:

            /**
             * Set console used to ouput text.
             */
            void setConsole( Console * console );

        public slots:

            /**
             * Start the server. Will emit either serverReady() or
             * serverExited() signal.
             */
            virtual void start();

            /**
             * Stop the server. Will emit serverExited() signal.
             */
            virtual void stop();

            /**
             * Is the server running?
             */
            virtual bool isRunning();

        signals:

            /**
             * Emitted when the server is ready listening at specified port.
             */
            void serverReady( Q_UINT16 port );

            /**
             * Emitted when the server exits while giving out exit code.
             */
            void serverExited( int retcode );

        protected slots:

            /**
             * to be connected to QProcess::readyReadStdout signal.
             */
            void readyReadStdoutSlot();
            /**
             * to be connected to QProcess::readyReadStderr signal.
             */
            void readyReadStderrSlot();
            /**
             * to be connected to QProcess::processExited signal.
             */
            void processExitedSlot();

            /**
              * Called when terminating by closing stdin has failed.
              */
            void sendTerminate();

        private:

            struct ServerControllerPrivate;
            ServerControllerPrivate* d;
    };
}

#endif /* _SERVERCONTROLLER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerController.hh,v 1.7 2004/08/15 18:17:59 mman Exp $
