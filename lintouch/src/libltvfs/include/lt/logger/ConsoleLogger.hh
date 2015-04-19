// $Id: ConsoleLogger.hh,v 1.1 2003/11/11 15:32:37 mman Exp $
//
//   FILE: ConsoleLogger.hh -- 
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

#ifndef _CONSOLELOGGER_HH
#define _CONSOLELOGGER_HH

#include "lt/logger/Logger.hh"

namespace lt {

        /**
         * The default ConsoleLogger implementation.
         *
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class ConsoleLogger : public Logger
        {

            public:

                enum ConsoleLogLevel {
                    LEVEL_DEBUG = 0,
                    LEVEL_INFO,
                    LEVEL_WARN,
                    LEVEL_ERROR,
                    LEVEL_FATAL,
                    LEVEL_DISABLED
                };

            public:

            /**
             * Create a logger with specified log-level.
             */
            ConsoleLogger ( const QString & name = "", 
                    const ConsoleLogLevel & level = LEVEL_DEBUG );

            virtual ~ConsoleLogger ();

            /**
             * Copy Constructor.
             */
            ConsoleLogger (
                    const ConsoleLogger & other );

            /**
             * Assignment operator.
             */
            ConsoleLogger & operator = (
                    const ConsoleLogger & other );
    
            /**
             * Log a debug message.
             *
             * @param message the message
             */
            virtual void debug( const QString & message );

            /**
             * Determine if messages of priority "debug" will be logged.
             *
             * @return true if "debug" messages will be logged
             */
            virtual bool isDebugEnabled() const;

            /**
             * Log a info message.
             *
             * @param message the message
             */
            virtual void info( const QString & message );

            /**
             * Determine if messages of priority "info" will be logged.
             *
             * @return true if "info" messages will be logged
             */
            virtual bool isInfoEnabled() const;

            /**
             * Log a warn message.
             *
             * @param message the message
             */
            virtual void warn( const QString & message );

            /**
             * Determine if messages of priority "warn" will be logged.
             *
             * @return true if "warn" messages will be logged
             */
            virtual bool isWarnEnabled() const;

            /**
             * Log a error message.
             *
             * @param message the message
             */
            virtual void error( const QString & message );

            /**
             * Determine if messages of priority "error" will be logged.
             *
             * @return true if "error" messages will be logged
             */
            virtual bool isErrorEnabled() const;

            /**
             * Log a fatalError message.
             *
             * @param message the message
             */
            virtual void fatalError( const QString & message );

            /**
             * Determine if messages of priority "fatalError" will be logged.
             *
             * @return true if "fatalError" messages will be logged
             */
            virtual bool isFatalErrorEnabled() const;

            /**
             * Create a new child logger.
             * The name of the child logger is
             * [current-loggers-name].[passed-in-name]
             *
             * @param name the subname of this logger
             * @return the new logger
             */
            virtual LoggerPtr getChildLogger( const QString & name );

            private:

            struct ConsoleLoggerPrivate;
            ConsoleLoggerPrivate *d;

        };

} // namespace lt

#endif /* _CONSOLELOGGER_HH */

// vim: set et ts=4 sw=4:
// $Id: ConsoleLogger.hh,v 1.1 2003/11/11 15:32:37 mman Exp $
