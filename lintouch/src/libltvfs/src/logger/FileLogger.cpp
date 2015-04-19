// $Id: FileLogger.cpp 1288 2006-01-16 12:08:08Z modesto $
//
//   FILE: FileLogger.cpp -- 
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

#include "lt/logger/FileLogger.hh"

#include <qfile.h>
#include <qtextstream.h>

using namespace lt;

struct FileLogger::FileLoggerPrivate {
    FileLoggerPrivate() : f(NULL) {}
    ~FileLoggerPrivate() {
       if(!f) s.unsetDevice();
       delete f;
    }

    FileLoggerPrivate& operator=(const FileLoggerPrivate& src) {
        logLevel = src.logLevel;
        name = src.name;
        filename = src.filename;
        f = src.f;
        s.setDevice(src.s.device());

        return *this;
    }

    FileLogLevel logLevel;
    QString name;
    QString filename;
    QFile *f;
    QTextStream s;
};

// PROTECTED
FileLogger::FileLogger ( QIODevice* io, const QString & name /* = "" */,
        const FileLogLevel & level /* = LEVEL_DEBUG */ ) {

    d = new FileLoggerPrivate ();
    Q_CHECK_PTR ( d );

    d->logLevel = level;
    d->name = name;
    // intentionaly don't store the io, only the owner has to has it.
    d->s.setDevice(io);
}

FileLogger::FileLogger ( const QString & filename,
        const QString & name /* = "" */,
        const FileLogLevel & level /* = LEVEL_DEBUG */ ) {
    d = new FileLoggerPrivate ();
    Q_CHECK_PTR ( d );

    d->logLevel = level;
    d->name = name;
    d->filename = filename;

    d->f = new QFile(filename);
    Q_CHECK_PTR(d->f);
    if(!d->f->open(IO_Raw|IO_WriteOnly)) {
        qWarning("[FileLogger ERROR] Can not create %s", filename.latin1());
    } else {
        d->s.setDevice(d->f);
    }
}

FileLogger::~FileLogger () {
    delete d;
}

FileLogger::FileLogger (
        const FileLogger & other ) : Logger ( other ) {
    d = new FileLoggerPrivate ();
    Q_CHECK_PTR ( d );
    *d = *other.d;
}

FileLogger & FileLogger::operator = (
        const FileLogger & other ) {
    *d = *other.d;
    return *this;
}

void FileLogger::debug( const QString & message ) {
    if ( d->logLevel <= LEVEL_DEBUG ) {
        if(d->s.device() != NULL ) {
        d->s << ( "[DEBUG] " + d->name + " " + message + "\n" );
        }
    }
}

bool FileLogger::isDebugEnabled() const {
    return d->logLevel <= LEVEL_DEBUG;
}

void FileLogger::info( const QString & message ) {
    if ( d->logLevel <= LEVEL_INFO ) {
        if(d->s.device() != NULL ) {
            d->s << ( "[INFO] " + d->name + " " + message + "\n" );
        }
    }
}

bool FileLogger::isInfoEnabled() const {
    return d->logLevel <= LEVEL_INFO;
}

void FileLogger::warn( const QString & message ) {
    if ( d->logLevel <= LEVEL_WARN ) {
        if(d->s.device() != NULL ) {
            d->s << ( "[WARN] " + d->name + " " + message + "\n" );
        }
    }
}

bool FileLogger::isWarnEnabled() const {
    return d->logLevel <= LEVEL_WARN;
}

void FileLogger::error( const QString & message ) {
    if ( d->logLevel <= LEVEL_ERROR ) {
        if(d->s.device() != NULL ) {
            d->s << ( "[ERROR] " + d->name + " " + message + "\n" );
        }
    }
}

bool FileLogger::isErrorEnabled() const {
    return d->logLevel <= LEVEL_ERROR;
}

void FileLogger::fatalError( const QString & message ) {
    if ( d->logLevel <= LEVEL_FATAL ) {
        if(d->s.device() != NULL ) {
            d->s << ( "[FATAL] " + d->name + " " + message + "\n" );
        }
    }
}

bool FileLogger::isFatalErrorEnabled() const {
    return d->logLevel <= LEVEL_FATAL;
}

LoggerPtr FileLogger::getChildLogger( const QString & name ) {
    return LoggerPtr(new FileLogger ( d->s.device(),
                d->name + "." + name, d->logLevel ));
}

// vim: set et ts=4 sw=4:
// $Id: FileLogger.cpp 1288 2006-01-16 12:08:08Z modesto $
