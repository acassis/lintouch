// $Id: Console.hh,v 1.7 2004/08/15 18:17:59 mman Exp $
//
//   FILE: Console.hh -- 
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

#ifndef _CONSOLE_HH
#define _CONSOLE_HH

#include <qtextedit.h>

class QProcess;

namespace lt {

    /**
     * Console for non-GUI applications.
     *
     * This class allows you to show stdout/stderr output of any console
     * application.
     *
     * The application whose output will be displayed is represented by
     * QProcess instance.
     *
     * @author Martin Man <mman@swac.cz>
     */
    class Console : public QTextEdit
    {
        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            Console( QWidget* parent, const char* name = 0 );

            /**
             * Destructor.
             */
            virtual ~Console();

        public slots:

            /**
             * Clear the console window.
             */
            void clear();

            /**
             * Append a line of text to the window as stdout, i.e., in
             * default color.
             */
            void appendStdout( const QString & m );

            /**
             * Append a line of text to the window as stderr, i.e., using
             * different color from stdout.
             */
            void appendStderr( const QString & m );

        private:

            struct ConsolePrivate;
            ConsolePrivate* d;
    };
}

#endif /* _CONSOLE_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Console.hh,v 1.7 2004/08/15 18:17:59 mman Exp $
