// $Id: ConnectWizard.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: ConnectWizard.hh -- 
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

#ifndef _CONNECTWIZARD_HH
#define _CONNECTWIZARD_HH

#include <qurl.h>

#include <lt/project/Project.hh>

#include "ConnectWizardBase.h"

using namespace lt;

class QListBoxItem;

class CommunicationProtocolAdaptor;

/**
 * ConnectWizard.
 *
 * ConnectWizard implements a dialog which guides user through connection
 * process. It allows the user to specify server host and port, download and
 * build a project and selects it's view.
 *
 * ConnectWizard can work in three modes.
 *
 * First (default) mode simply opens the wizard and allows the user to
 * specify, server and port, then connecting to the server, downloading a
 * project and selecting a view. This mode can be invoked by calling
 * execFromConnect( false ).
 *
 * Second (auto) mode tries to connect automatically to the preselected
 * host/port and automatically select preselected view. if any of those
 * operations fails, wizard goes back to default mode. This mode can be
 * started by preselectHost, preselectPort, preselectView and
 * execFromConnect( true ).
 *
 * Third (view_selecion) mode allows user to only select another project
 * view. it can be invoked by calling execFromSelect and assumes the wizard
 * undergone throug default or auto mode at least once.
 *
 * @author Martin Man <mman@swac.cz>
 * @author Patrik Modesto <modesto@swac.cz>
 */
class ConnectWizard : public ConnectWizardBase
{
    Q_OBJECT

    public:

        /**
         * Constructor.
         */
        ConnectWizard( CommunicationProtocolAdaptor * cpa,
                swaloun::framework::LoggerPtr l, QWidget* parent,
                const char* name = 0, bool modal = true, WFlags fl = 0 );

        /**
         * Destructor.
         */
        virtual ~ConnectWizard();

        /**
         * Prefill server host input box.
         */
        void preselectHost( const QString & host );

        /**
         * Prefill server port input box.
         */
        void preselectPort( int port );

        /**
         * Prefill view selection box.
         */
        void preselectView( const QString & viewId );

    public slots:

        /**
         * Will popup the wizard and start either default (go_ahead is
         * false) or automatic (go_ahead is true) connection mode. if wait
         * is true, will wait before attempting first automatic connect.
         *
         * @return QDiialog::Accepted on success.
         */
        int execFromConnect( bool go_ahead, bool wait );

        /**
         * Will popup the wizard in select_view mode.
         *
         * @return QDiialog::Accepted on success.
         */
        int execFromSelect();

    public:

        /**
         * Returns the real server host after the wizard is accepted.
         */
        QString host() const;

        /**
         * Returns the real server port after the wizard is accepted.
         */
        unsigned int port() const;

        /**
         * Returns the real view id after the wizard is accepted.
         */
        QString selectedViewId() const;

        /**
         * Returns the project downloaded from the server.
         */
        lt::Project project() const;

        /**
         * Forgets the downloaded project so that no references to it
         * exists.
         */
        void forgetProject();

    public:

        /**
         * prepares wizard pages before they're shown.
         */
        virtual void showPage( QWidget * page );

    protected:

        virtual void back();

        virtual void next();

        virtual void accept();

        virtual void reject();

    protected slots:

        /**
         * Invoked when up button is pressed on select view page.
         */
        void slotMoveUp();

        /**
         * Invoked when down button is pressed on select view page.
         */
        void slotMoveDown();

        /**
         * Invoked when a selection bar moves within the view list.
         */
        void slotViewIdHighlighted( QListBoxItem* i );

        /**
         * Invoked when a selection is made by double click or enter within
         * the view list.
         */
        void slotViewIdSelected( QListBoxItem* i );

        /**
         * Invoked when user changes text in server host input field.
         */
        void slotHostInputChanged( const QString& text );

        /**
         * Invoked when user changes text in server port input field.
         */
        void slotPortInputChanged( const QString& text );

    protected slots:

        /**
         * Invoked when TCP/IP connection to the server is established.
         */
        void slotConnected();

        /**
         * Invoked when TCP/IP connection to the server is closed normally.
         */
        void slotDisconnected();

        /**
         * Invoked when ew receive a project URL through TCP/IP connection.
         */
        void slotReceivedProjectUrl( const QString & url );

        /**
         * Invoked when TCP/IP connection to the server is closed by error.
         */
        void slotError( int err );

        /**
         * Invoked when ProjectAsyncBuilder finishes (either ok or with
         * error) project download & build.
         */
        void slotFinished( int status );

        /**
         * Invoked when ProjectAsyncBuilder downloads a project.
         */
        void slotDownloaded();

        /**
         * Invoked when ProjectAsyncBuilder gets some more project data.
         */
        void slotDownloadProgress( int downloaded, int total );

        /**
         * Invoked when ProjectAsyncBuilder builds a part of the project.
         */
        void slotBuildProgress(int panel, int panelsTotal);

    protected slots:

        /**
         * Will start connection process.
         */
        void startConnect();

        /**
         * Will start the project async builder
         */
        void startProjectDownload( const QUrl& path );

        /**
         * Will update wizard after the project has been downloaded & built
         */
        void projectReady( const Project & p );

        /**
         * Add more space around bottom button line to fit in the size-grip.
         */
        void layOutButtonRow ( QHBoxLayout* layout );

    private:

        struct ConnectWizardPrivate;
        ConnectWizardPrivate* d;
};

#endif /* _CONNECTWIZARD_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ConnectWizard.hh 1168 2005-12-12 14:48:03Z modesto $
