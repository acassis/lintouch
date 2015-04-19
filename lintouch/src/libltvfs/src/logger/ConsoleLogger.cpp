// $Id: ConsoleLogger.cpp,v 1.1 2003/11/11 15:32:46 mman Exp $
//
//   FILE: ConsoleLogger.cpp -- 
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

#include "lt/logger/ConsoleLogger.hh"

using namespace lt;

struct ConsoleLogger::ConsoleLoggerPrivate {
    ConsoleLogLevel logLevel;
    QString name;
};

ConsoleLogger::ConsoleLogger ( const QString & name /* = "" */, 
        const ConsoleLogLevel & level /* = LEVEL_DEBUG */ ) {
    d = new ConsoleLoggerPrivate ();
    Q_CHECK_PTR ( d );
    d->name = name;
    d->logLevel = level;
}

ConsoleLogger::~ConsoleLogger () {
    delete d;
}

ConsoleLogger::ConsoleLogger (
        const ConsoleLogger & other ) : Logger ( other ) {
    d = new ConsoleLoggerPrivate ();
    Q_CHECK_PTR ( d );
    *d = *other.d;
}

ConsoleLogger & ConsoleLogger::operator = (
        const ConsoleLogger & other ) {
    *d = *other.d;
    return *this;
}

void ConsoleLogger::debug( const QString & message ) {
    if ( d->logLevel <= LEVEL_DEBUG ) {
        qWarning( "[DEBUG] " + d->name + " " + message );
    }
}

bool ConsoleLogger::isDebugEnabled() const {
    return d->logLevel <= LEVEL_DEBUG;
}

void ConsoleLogger::info( const QString & message ) {
    if ( d->logLevel <= LEVEL_INFO ) {
        qWarning( "[INFO] " + d->name + " " + message );
    }
}

bool ConsoleLogger::isInfoEnabled() const {
    return d->logLevel <= LEVEL_INFO;
}

void ConsoleLogger::warn( const QString & message ) {
    if ( d->logLevel <= LEVEL_WARN ) {
        qWarning( "[WARN] " + d->name + " " + message );
    }
}

bool ConsoleLogger::isWarnEnabled() const {
    return d->logLevel <= LEVEL_WARN;
}

void ConsoleLogger::error( const QString & message ) {
    if ( d->logLevel <= LEVEL_ERROR ) {
        qWarning( "[ERROR] " + d->name + " " + message );
    }
}

bool ConsoleLogger::isErrorEnabled() const {
    return d->logLevel <= LEVEL_ERROR;
}

void ConsoleLogger::fatalError( const QString & message ) {
    if ( d->logLevel <= LEVEL_FATAL ) {
        qWarning( "[FATAL] " + d->name + " " + message );
    }
}

bool ConsoleLogger::isFatalErrorEnabled() const {
    return d->logLevel <= LEVEL_FATAL;
}

LoggerPtr ConsoleLogger::getChildLogger( const QString & name ) {
    return LoggerPtr(new ConsoleLogger ( d->name + "." + name, d->logLevel ));
}

// vim: set et ts=4 sw=4:
// $Id: ConsoleLogger.cpp,v 1.1 2003/11/11 15:32:46 mman Exp $
