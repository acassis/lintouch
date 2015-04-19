// $Id: LogEnabled.hh,v 1.3 2004/02/24 14:21:50 mman Exp $
//
//   FILE: LogEnabled.hh -- 
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

#ifndef _LOGENABLED_HH
#define _LOGENABLED_HH

#include "lt/logger/Logger.hh"

namespace lt {

        /**
         * Components that need to log can implement this interface to
         * be provided with Loggers.
         *
         * The configured logger is available via getLogger() method. By
         * default (without explicitly invoking enableLogging) the getLogger
         * will return an empty logger which does nothing.
         *
         * @author <a href="mailto:peter@apache.org">Peter Donald</a>
         * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
         * @author <a href="mailto:mman@swac.cz">Martin Man</a>
         */
        class LogEnabled
        {
            public:

                LogEnabled () : m_logger ( Logger::nullPtr () ) {};

                /**
                 * Provide component with a logger.
                 *
                 * @param logger the logger
                 */
                virtual void enableLogging( LoggerPtr logger ) {
                    m_logger = logger;
                }

                /**
                 * Return logger to the subclasses. Use this function if you
                 * need to debug/error/warn.
                 */
                Logger & getLogger () const {
                    return *m_logger;
                }

                /**
                 * Return logger to the subclasses. Use this function if you
                 * need to pass this logger to other LogEnabled components.
                 */
                LoggerPtr getLoggerPtr () const {
                    return m_logger;
                }

                /**
                 * destructor.
                 */
                virtual ~LogEnabled () {};

            private:

                LoggerPtr m_logger;

        };

} // namespace lt

#endif /* _LOGENABLED_HH */

// vim: set et ts=4 sw=4:
// $Id: LogEnabled.hh,v 1.3 2004/02/24 14:21:50 mman Exp $
