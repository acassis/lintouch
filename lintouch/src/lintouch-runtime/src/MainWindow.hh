// $Id: MainWindow.hh 1286 2006-01-16 09:03:55Z modesto $
//
//   FILE: MainWindow.hh -- 
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

#ifndef _MAINWINDOW_HH
#define _MAINWINDOW_HH

#include "MainWindowBase.h"

#include <qsize.h>
#include <qstring.h>
#include <qurl.h>

#include <lt/logger/ConsoleLogger.hh>

/**
 * MainWindow for Lintouch Runtime.
 *
 * @author Martin Man <mman@swac.cz>
 * @author Patrik Modesto <modesto@swac.cz>
 */
class MainWindow : public MainWindowBase
{
    Q_OBJECT

    public:

        /**
         * Create MainWindow.
         *
         * @param mode 0 = host/port, 1 = project (aka start local server)
         * @param fullscreen 0 = normal, 1 = fullscreen
         * @param l Logger
         * @param parent parent
         * @param name name
         * @param fl flags
         */
        MainWindow( int mode, bool fullscreen, lt::LoggerPtr l,
                QWidget* parent, const char* name = 0,
                WFlags fl = 0 );

        /**
         * Destructor.
         */
        virtual ~MainWindow();

    public:

        /**
         * Prefill host/port. in case host is nonempty it will also disable
         * the ''disconnect'' button.
         */
        void setHostNPort( const QString & host, int port,
                bool editable = true );

        /**
         * Prefill project. in case project is nonempty it will also disable
         * the ''stop server'' button. If the port is 0 a random port will
         * be used.
         */
        void setProjectNPort( const QString & project, int port = 0,
                bool editable = true );

        /**
         * Prefill view.
         */
        void setView( const QString & view );

        /**
         * Enable/Disable service panel shortcut (F12).
         */
        void setServicePanelShortcutEnabled( bool enabled );

        /**
         * Enable/Disable show fullscreen shortcut (F11).
         */
        void setShowFullScreenEnabled( bool enabled );

        /**
         * Enable/Disable view selector.
         * 0 - disable
         * 1 - enable
         * 2 - unspecified -> project dependent
         */
        void setViewSelectorEnabled( int state );

        /**
         * Enable/Disable panel selector.
         * 0 - disable
         * 1 - enable
         * 2 - unspecified -> project dependent
         */
        void setPanelSelectorEnabled( int state );

    public slots:

        /**
         * Show window and schedule startup routines. Depending on the
         * commandline args, either startServer or startConnect will be
         * scheduled to start as soon as possible.
         */
        virtual void show();

    protected slots:

        /**
         * Start local server if the project was specified at the
         * commandline. server startup will finish either by
         * serverReadySlot(port) or serverExitedSlot(code).
         */
        void startServer();

        /**
         * Stop local server and wait till it cleanly exits
         */
        void stopServer();

        /**
         * Invoked when the server is ready.
         */
        void serverReadySlot( Q_UINT16 port );

        /**
         * Invoked when the server dies.
         */
        void serverExitedSlot( int retcode );

    protected slots:

        /**
         * Connect to the server. It will be either local server or the
         * server specified at commandline.
         */
        void startConnect();

        /**
         * Disconnect from the server cleanly.
         */
        void disconnect();

        /**
         * Invoked when the connection to the server is ready.
         */
        void connectedSlot();

        /**
         * Invoked when the project has been extracted.
         */
        void projectReadySlot();

        /**
         * Invoked when the connection to the server is shutdown.
         */
        void disconnectedSlot();

        /**
         * Invoked when the connection to the server dies for some reason.
         * @see CommunicationProtocolAdaptor::Error.
         */
        void connectionErrorSlot( int error );

    protected slots:

        /**
         * GUI entry to start/stop the local server.
         */
        void startStopServerSlot();

        /**
         * GUI entry to connect/disconnect to/from the server.
         */
        void connectDisconnectSlot();

        /**
         * GUI entry to select project tool button.
         */
        void selectProjectSlot();

        /**
         * GUI entry to show/hide service panel.
         */
        void showHideServicePanelSlot();

        /**
         * GUI entry to maximize.
         */
        void maximizeUnmaximizeSlot();

        /**
         * GUI entry to select view.
         */
        void selectViewPanelSlot( const QString & view,
                const QString & panel );

        /**
         * GUI entry to be notified about panel changes.
         */
        void panelSelectedSlot( const QString & panel );

    protected slots:

        /**
         * Called by Qt on QTranslator add/del/change. Completely
         * undocumented, but causes some widgets to loose values.
         */
        virtual void languageChange();

    protected:

        /**
         * Close event handler, used to get shutdown possibly running server
         * and disconnect cleanly.
         */
        virtual void closeEvent( QCloseEvent * e );

        /**
         * Custom event handler, used to get notifications about iopins
         * changed by templates.
         */
        virtual void customEvent( QCustomEvent * e );

        /**
         * Event filter to ease debugging.
         */
        virtual bool eventFilter( QObject *o, QEvent *e );

        /**
         * Log given message to logger and to the Console at ServicePanel.
         */
        void logToConsole(const QString& msg);

    private:

        struct MainWindowPrivate;
        MainWindowPrivate* d;

};

#endif /* _MAINWINDOW_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: MainWindow.hh 1286 2006-01-16 09:03:55Z modesto $
