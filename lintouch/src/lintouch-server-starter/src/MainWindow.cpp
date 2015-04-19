// $Id: MainWindow.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: MainWindow.cpp -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 1 February 2005
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
#include <qfiledialog.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>

#include <lt/ui/Console.hh>
#include <lt/ui/ServerController.hh>

#include "AboutDialog.hh"
#include "MainWindow.hh"
using namespace lt;

#include "config.h"

struct MainWindow::MainWindowPrivate
{
    MainWindowPrivate() :
        sc( new ServerController() ), about( 0 ), canClose( true ) {}
    ~MainWindowPrivate() { delete sc; }

    ServerController * sc;
    AboutDialog * about;
    bool canClose;
};

MainWindow::MainWindow( QWidget* parent,
        const char* name /*= 0*/, WFlags fl /*= 0*/ ) :
    MainWindowBase( parent, name, fl ),
    d( new MainWindowPrivate )
{
    Q_CHECK_PTR( d );

    // hide statusbar
    ( statusBar() )->close();

    // set initial values
    m_port->setText( "5555" );
    m_project->setText( "" );

    d->sc->setConsole( m_console );
    d->sc->setLocal( 0 );

    // create about dialog
    d->about = new AboutDialog( this );

    connect( d->sc, SIGNAL( serverReady( Q_UINT16 ) ),
            this, SLOT( serverReadySlot( Q_UINT16 ) ) );
    connect( d->sc, SIGNAL( serverExited( int ) ),
            this, SLOT( serverExitedSlot( int ) ) );
}

MainWindow::~MainWindow()
{
    if( d ) {
        delete d;
    }
}

void MainWindow::setProject( const QString & project )
{
    m_project->setText( project );
}

void MainWindow::setPort( int port )
{
    m_port->setText( QString::number( port ) );
}

void MainWindow::fileOpen()
{
    Q_CHECK_PTR( d );

    QString fn = QFileDialog::getOpenFileName(
            QString::null, /* start dir */
            tr( "Lintouch Projects (*.ltp)" ),
            this, NULL,
            tr( "Choose Lintouch Project" ) );

    m_project->setText( fn );
}

void MainWindow::fileExit()
{
    Q_CHECK_PTR( d );
    close();
}

void MainWindow::serverStart()
{
    Q_CHECK_PTR( d );
    if( ! d->sc->isRunning() ) {
        m_console->clear();
        d->sc->start();

        if( d->sc->isRunning() ) {
            // disable start, enable stop
            serverStartAction->setEnabled( false );
            serverStopAction->setEnabled( true );

            // disable project editing functions
            m_port->setEnabled( false );
            m_project->setEnabled( false );
            m_projectChooser->setEnabled( false );
            fileOpenAction->setEnabled( false );

            //indicate that we can not exit immediatelly
            d->canClose = false;
        }
    }
}

void MainWindow::serverStop()
{
    Q_CHECK_PTR( d );
    if( d->sc->isRunning() ) {
        // disable second stop for long-ending tasks
        serverStopAction->setEnabled( false );
        d->sc->stop();
    }
}

void MainWindow::helpAbout()
{
    Q_CHECK_PTR( d );
    d->about->show();
}

void MainWindow::projectOrPortChanged( const QString & )
{
    // prepare server controller
    d->sc->setProject( m_project->text() );
    d->sc->setPort( m_port->text().toUInt() );

    if( ! m_project->text().isEmpty() && ! m_port->text().isEmpty() ) {
        // enable start
        serverStartAction->setEnabled( true );
    } else {
        // disable start & stop
        serverStartAction->setEnabled( false );
        serverStopAction->setEnabled( false );
    }
}

void MainWindow::serverReadySlot( Q_UINT16 /* port */ )
{
}

void MainWindow::serverExitedSlot( int /* retcode */ )
{
    // enable start, disable stop
    serverStartAction->setEnabled( true );
    serverStopAction->setEnabled( false );

    // enable project editing functions
    m_port->setEnabled( true );
    m_project->setEnabled( true );
    m_projectChooser->setEnabled( true );
    fileOpenAction->setEnabled( true );

    //indicate that we can exit
    d->canClose = true;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
    // first stop the server
    serverStop();

    // wait for clean shutdown
    while( ! d->canClose ) {
        qApp->processEvents();
    }

    // and accept the close event
    e->accept();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: MainWindow.cpp 1168 2005-12-12 14:48:03Z modesto $
