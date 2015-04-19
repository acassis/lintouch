// $Id: MainWindow.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: MainWindow.hh -- 
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

#ifndef _MAINWINDOW_HH
#define _MAINWINDOW_HH

#include "MainWindowBase.h"

#include <qsize.h>
#include <qstring.h>
#include <qurl.h>

/**
 * MainWindow for Lintouch Server Starter.
 *
 * @author Martin Man <mman@swac.cz>
 */
class MainWindow : public MainWindowBase
{
    Q_OBJECT

    public:

        /**
         * Create MainWindow and pass in predefined hostname, port and
         * viewId.
         */
        MainWindow( QWidget* parent, const char* name = 0,
                WFlags fl = 0 );

        virtual ~MainWindow();

    public:

        void setProject( const QString & project );
        void setPort( int port );

    protected slots:

        void fileOpen();
        void fileExit();

        void serverStart();
        void serverStop();

        void helpAbout();

        void projectOrPortChanged( const QString & prj );

    protected slots:

        void serverReadySlot( Q_UINT16 port );
        void serverExitedSlot( int retcode );

    protected:

        virtual void closeEvent( QCloseEvent * e );

    private:

        struct MainWindowPrivate;
        MainWindowPrivate* d;

};

#endif /* _MAINWINDOW_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: MainWindow.hh 1168 2005-12-12 14:48:03Z modesto $
