// $Id: RuntimeController.hh,v 1.7 2004/08/15 18:17:59 mman Exp $
//
//   FILE: RuntimeController.hh -- 
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

#ifndef _RUNTIMECONTROLLER_HH
#define _RUNTIMECONTROLLER_HH

#include <qobject.h>
#include <qstring.h>

namespace lt {

    /**
     * RuntimeController.
     *
     * This class allows you to start Lintouch Runtime with given Project.
     *
     * @author Martin Man <mman@swac.cz>
     */
    class RuntimeController : public QObject
    {
        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            RuntimeController( QObject* parent = 0, const char* name = 0 );

            /**
             * Destructor.
             */
            virtual ~RuntimeController();

        public:

            /**
             * Set project to be used. The specified project will be used
             * from the next startup.
             *
             * @param project The project to be used by the runtime.
             */
            void setProject( const QString & project );

            /**
             * Return the currently specified project.
             */
            QString project() const;

            /**
             * Checks all required parameters needed to run lintouch-runtime
             * successfuly. Returns true if everything looks ok.
             */
            bool isReady() const;

        public slots:

            /**
             * Start the runtime. Will emit either runtimeReady() or
             * runtimeExited() signal.
             */
            virtual void start();

            /**
             * Stop the runtime. Will emit runtimeExited() signal.
             */
            virtual void stop();

            /**
             * Is the runtime running?
             */
            virtual bool isRunning();

        signals:

            /**
             * Emitted when the runtime is ready 
             */
            void runtimeReady();

            /**
             * Emitted when the runtime exits while giving out exit code.
             */
            void runtimeExited( RuntimeController * sender, int retcode );

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

        private:

            struct RuntimeControllerPrivate;
            RuntimeControllerPrivate* d;
    };
}

#endif /* _RUNTIMECONTROLLER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: RuntimeController.hh,v 1.7 2004/08/15 18:17:59 mman Exp $
