// $Id: ServerLoggingManager.cpp 1571 2006-04-13 11:38:54Z modesto $
//
//   FILE: ServerLoggingManager.cpp --
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

#include <qapplication.h>
#include <qdir.h>

#include <apr_hash.h>

#include <lt/server/logging.h>
#include "ServerLoggingManager.hh"

#include "config.h"

struct ServerLoggingManager::ServerLoggingManagerPrivate
{
    /**
     * Pool used to allocate loaded plugins.
     */
    apr_pool_t * pool;

    /**
     * all plugins found on the system
     */
    QStringList plugins;

    /**
     * Map of all plugin/serverLoggin properties
     */
    QMap<QString, QMap<QString,QString> > slProperties;

    /**
     * True for the public plugins or checked private plugins.
     */
    QMap<QString, bool> isAvailable;

    /**
     * Set the the logging properties of the plugin by
     * the plugin introspection.
     */
    void setPluginProperties(const QString &basename, const QString &name,
        LoggerPtr logger = Logger::nullPtr());

    /**
     * Create QMap from the python mapping. The python interpreter must have
     * have been initialized. No checks are performed with respect to the
     * mapping.
     */
    QMap<QString,QString> mappingToQMap(apr_hash_t*hash);
};

QMap<QString,QString>
ServerLoggingManager::ServerLoggingManagerPrivate::mappingToQMap(
        apr_hash_t *h)
{
    QMap<QString,QString> result;

    apr_pool_t * pool = NULL;
    apr_hash_index_t * hi = NULL;

    if( h == NULL ) return result;

    apr_pool_create( &pool, NULL );
    hi = apr_hash_first( pool, h );

    for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
        const char * key = NULL, * val = NULL;

        apr_hash_this( hi, (const void**) &key, NULL, (void**) &val );

        result.insert( QString( key ), QString( val ) );
    }

    apr_pool_destroy( pool );
    return result;
}

void ServerLoggingManager::ServerLoggingManagerPrivate::setPluginProperties(
        const QString &basename,
        const QString &name, LoggerPtr logger /* = Logger::nullPtr() */)
{
    QCString pluginfile = name.utf8();
    lt_logging_plugin_t * plugin = NULL;
    plugin = lt_logging_plugin_load( pluginfile, NULL, pool );
    if( plugin == NULL ) {
        logger->error(QString("Failed to load the logging plugin %1:")
                .arg(name));
        return;
    }

    logger->info(QString("Loaded the logging plugin %1" \
                " (registering under name '%2')")
            .arg(name).arg(basename));

    apr_hash_t * properties =
        lt_logging_plugin_default_properties_get( plugin );

    if( properties != NULL ) {
        slProperties[basename] = mappingToQMap(properties);
    } else {
        logger->warn( QString( "Logging plugin %1 " \
                    "does not define any properties").arg( basename ) );
    }

    plugins.append(basename);
    isAvailable[basename] = true;
}

ServerLoggingManager::ServerLoggingManager(
        LoggerPtr logger /* = Logger::nullPtr() */)
    : d(new ServerLoggingManagerPrivate)
{
    Q_CHECK_PTR(d);

    apr_pool_create( &d->pool, NULL );

    // where to look for plugins
    QValueList<QDir> pluginDirs;

    // application dir and prefix dir
    QDir appDir(qApp->applicationDirPath());
    QDir prefixDir(PREFIX_NATIVE);

    // where to look for plugins, prefix and appDir based
    pluginDirs.append(
            appDir.absFilePath("../../logging-plugins/llp-basic/.libs"));
    pluginDirs.append(appDir.absFilePath("../lib/lintouch/logging-plugins"));
    QString p1 = prefixDir.absFilePath( "lib/lintouch/logging-plugins" );
    if( ! pluginDirs.contains( p1 ) ) {
        pluginDirs.append( p1 );
    }

    for (QValueList<QDir>::Iterator pd = pluginDirs.begin();
        pd != pluginDirs.end(); ++pd)
    {
        // skip nonexistent dirs
        if( ! (*pd).exists() ) continue;
#ifdef Q_OS_WIN32
        QStringList plugins = (*pd).entryList("*.dll");
#else
        QStringList plugins = (*pd).entryList("*.so");
#endif
        // go for source files and byte compiled files
        QStringList registered;
        for (QStringList::Iterator it = plugins.begin(); it != plugins.end();
            ++it )
        {
            QString basename = QFileInfo(*it).baseName();
#ifdef Q_OS_WIN32
            // strip -0 from the autotools generated .dlls
            basename.remove( basename.length() - 2, 2 );
#endif
            if( ! registered.contains( basename ) ) {
                d->setPluginProperties( basename,
                        (*pd).absPath() + "/" + (*it), logger);
                registered.append( basename );
            }
        }
    }
}

ServerLoggingManager::~ServerLoggingManager()
{
    apr_pool_destroy( d->pool );
    delete d;
}

// API

const QStringList& ServerLoggingManager::plugins() const
{
    Q_ASSERT(d);
    return d->plugins;
}

const QMap<QString,QString> & ServerLoggingManager::serverLoggingProperties(
        const QString& c) const
{
    static QMap<QString,QString> dflt;
    Q_ASSERT(d);
    if( d->slProperties.contains( c ) ) {
        return d->slProperties[c];
    }
    return dflt;
}

bool ServerLoggingManager::isAvailable(const QString& c) const
{
    Q_ASSERT(d);
    return d->isAvailable[c];
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerLoggingManager.cpp 1571 2006-04-13 11:38:54Z modesto $
