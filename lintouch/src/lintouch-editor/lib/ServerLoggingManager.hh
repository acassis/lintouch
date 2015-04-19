// $Id: ServerLoggingManager.hh 1558 2006-04-06 12:21:31Z modesto $
//
//   FILE: ServerLoggingManager.hh --
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: Thu, 06 Apr 2006
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

#ifndef _SERVERLOGGINGMANAGER_HH
#define _SERVERLOGGINGMANAGER_HH

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>

#include <lt/logger/Logger.hh>

// #include <lt/templates/IOPin.hh>
using namespace lt;

/**
 * Enumerates all installed lintouch-server plugin on the system and
 * collects information useful for lintouch-editor connections stuff.
 */
class ServerLoggingManager {

public:
    /**
     * ctor
     */
    ServerLoggingManager(LoggerPtr logger = Logger::nullPtr());

    /**
     * dtor
     */
    ~ServerLoggingManager();

    ////////////////////////////////////////////////////////////
    // API

    /**
     * Returns list of plugins/connections found.
     */
    const QStringList& plugins() const;

    /**
     * Returns all properties for given server logging plugin. The
     * properties are QString to QString map with property name as the key
     * and property default value as the value.
     */
    const QMap<QString,QString>& serverLoggingProperties(
            const QString& c) const;

    /**
     * Succeeds for public plugins and private plugins with the correct
     * dongle inserted.
     */
    bool isAvailable(const QString& c) const;

private:

    /**
     * Prevent copying.
     */
    ServerLoggingManager( const ServerLoggingManager & );
    ServerLoggingManager & operator =( const ServerLoggingManager & );

private:

    struct ServerLoggingManagerPrivate;
    ServerLoggingManagerPrivate* d;
};

#endif /* _SERVERLOGGINGMANAGER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerLoggingManager.hh 1558 2006-04-06 12:21:31Z modesto $
