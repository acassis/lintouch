// $Id: Logger.hh,v 1.3 2004/02/24 14:21:50 mman Exp $
//
//   FILE: Logger.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 19 May 2002
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

#ifndef _LOGGER_HH
#define _LOGGER_HH

#include <string>
#include <qstring.h>
#include <qvaluelist.h>
#include "lt/SharedPtr.hh"

namespace lt {

        class Logger;

        typedef SharedPtr <Logger> LoggerPtr;

        /**
         * This is a facade for the different logging subsystems.
         * It offers a simplified interface that follows IOC patterns
         * and a simplified priority/level/severity abstraction. 
         *
         * @author <a href="mailto:peter@apache.org">Peter Donald</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class Logger
        {
            public:

                /**
                 * shared empty logger.
                 */
                static Logger *null();

                /**
                 * shared empty logger.
                 */
                static LoggerPtr nullPtr();

            public:

                /**
                 * Log a debug message.
                 *
                 * @param message the message
                 */
                virtual void debug( const QString & /* message */ ) {};

                /**
                 * Determine if messages of priority "debug" will be logged.
                 *
                 * @return true if "debug" messages will be logged
                 */
                virtual bool isDebugEnabled() const { return false; };

                /**
                 * Log a info message.
                 *
                 * @param message the message
                 */
                virtual void info( const QString & /* message */ ) {};

                /**
                 * Determine if messages of priority "info" will be logged.
                 *
                 * @return true if "info" messages will be logged
                 */
                virtual bool isInfoEnabled() const { return false; };

                /**
                 * Log a warn message.
                 *
                 * @param message the message
                 */
                virtual void warn( const QString & /* message */ ) {};

                /**
                 * Determine if messages of priority "warn" will be logged.
                 *
                 * @return true if "warn" messages will be logged
                 */
                virtual bool isWarnEnabled() const { return false; };

                /**
                 * Log a error message.
                 *
                 * @param message the message
                 */
                virtual void error( const QString & /* message */ ) {};

                /**
                 * Determine if messages of priority "error" will be logged.
                 *
                 * @return true if "error" messages will be logged
                 */
                virtual bool isErrorEnabled() const { return false; };

                /**
                 * Log a fatalError message.
                 *
                 * @param message the message
                 */
                virtual void fatalError( const QString & /* message */ ) {};

                /**
                 * Determine if messages of priority "fatalError" will be
                 * logged.
                 *
                 * @return true if "fatalError" messages will be logged
                 */
                virtual bool isFatalErrorEnabled() const { return false; };

                /**
                 * Create a new child logger.
                 * The name of the child logger is
                 * [current-loggers-name].[passed-in-name]
                 *
                 * @param name the subname of this logger
                 * @return the new logger
                 */
                virtual LoggerPtr getChildLogger( const QString & /* name */ ) {
                    return nullPtr ();
                }

                /**
                 * Destructor
                 */
                virtual ~Logger () {};
        };

} // namespace lt

#endif /* _LOGGER_HH */

// vim: set et ts=4 sw=4:
// $Id: Logger.hh,v 1.3 2004/02/24 14:21:50 mman Exp $
