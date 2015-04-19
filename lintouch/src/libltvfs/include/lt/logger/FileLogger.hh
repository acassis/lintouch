// $Id: FileLogger.hh 1276 2006-01-11 12:42:49Z modesto $
//
//   FILE: FileLogger.hh -- 
// AUTHOR: Patrik Modesto <modesto@swa.cz>
//   DATE: Wed Jan 11
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

#ifndef _FILELOGGER_HH
#define _FILELOGGER_HH

#include "lt/logger/Logger.hh"

namespace lt {

        /**
         * The default FileLogger implementation.
         *
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class FileLogger : public Logger
        {

            public:

                enum FileLogLevel {
                    LEVEL_DEBUG = 0,
                    LEVEL_INFO,
                    LEVEL_WARN,
                    LEVEL_ERROR,
                    LEVEL_FATAL,
                    LEVEL_DISABLED
                };

            protected:

            /**
             * Create a logger with specified QIODevice and log-level.
             */
            FileLogger ( QIODevice* io, const QString & name = "",
                    const FileLogLevel & level = LEVEL_DEBUG );

            public:

            /**
             * Create a logger with specified log-level.
             */
            FileLogger ( const QString & filename,
                    const QString & name = "",
                    const FileLogLevel & level = LEVEL_DEBUG );

            virtual ~FileLogger ();

            /**
             * Copy Constructor.
             */
            FileLogger (
                    const FileLogger & other );

            /**
             * Assignment operator.
             */
            FileLogger & operator = (
                    const FileLogger & other );

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

            struct FileLoggerPrivate;
            FileLoggerPrivate *d;

        };

} // namespace lt

#endif /* _FILELOGGER_HH */

// vim: set et ts=4 sw=4:
// $Id: FileLogger.hh 1276 2006-01-11 12:42:49Z modesto $
