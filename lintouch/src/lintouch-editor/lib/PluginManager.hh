// $Id: PluginManager.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: PluginManager.hh --
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 05 October 2004
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

#ifndef _PLUGINMANAGER_HH
#define _PLUGINMANAGER_HH

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>

#include <lt/logger/Logger.hh>

#include <lt/templates/IOPin.hh>
using namespace lt;

/**
 * Enumerates all installed lintouch-server plugin on the system and
 * collects information useful for lintouch-editor connections stuff.
 */
class PluginManager {

public:
    /**
     * ctor
     */
    PluginManager(LoggerPtr logger =
        Logger::nullPtr());

    /**
     * dtor
     */
    ~PluginManager();

    ////////////////////////////////////////////////////////////
    // API

    /**
     * Returns list of plugins/connections found.
     */
    const QStringList& plugins() const;

    /**
     * Returns all properties for given connection. The properties are
     * QString to QString map with property name as the key and property
     * default value as the value.
     */
    const QMap<QString,QString>& connectionProperties(const QString& c) const;

    /**
     * Returns list of all variables properties for given connection. The
     * properties can be different for different types of variables defined
     * by the connection. The properties are QString to QString map with
     * property name as the key and property default value as the value.
     *
     * @param c The connection name.
     * @param vartype Type of the newly created variable. When invalid, all
     * possible properties that a variable might have are returned.
     */
    const QMap<QString,QString>& variablesProperties(const QString& c,
            IOPin::Type vartype = IOPin::InvalidType) const;

    /**
     * Return the list of uids as defined by the plugin.
     */
    const QStringList& uids(const QString& c) const;

    /**
     * Succeeds for public plugins and private plugins with the correct
     * dongle inserted.
     */
    bool isAvailable(const QString& c) const;

private:

    /**
     * Prevent copying.
     */
    PluginManager( const PluginManager & );
    PluginManager & operator =( const PluginManager & );

    struct PluginManagerPrivate;
    PluginManagerPrivate* d;
};

#endif /* _PLUGINMANAGER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: PluginManager.hh 1169 2005-12-12 15:22:15Z modesto $
