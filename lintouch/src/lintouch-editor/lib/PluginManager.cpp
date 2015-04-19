// $Id: PluginManager.cpp 1170 2005-12-12 15:53:16Z modesto $
//
//   FILE: PluginManager.cpp --
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

#include <qdir.h>

#include <apr_hash.h>

#include <lt/server/plugin.h>

#include <qapplication.h>
#include "PluginManager.hh"

#include "config.h"

// If defined, only Loopback and Generator are defined
//#define MINIMAL_PLUGINS

struct PluginManager::PluginManagerPrivate
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
     * Map of all plugin/connection properties
     */
    QMap<QString, QMap<QString,QString> > connectionProperties;

    /**
     * Map of all variables properties
     * [ connection -> [ vartype -> [ name, value ] ] ]
     */
    QMap<QString, QMap<IOPin::Type, QMap<QString,QString> > >
        variablesProperties;

    /**
     * True for the public plugins or checked private plugins.
     */
    QMap<QString, bool> isAvailable;

    /**
     * Set the variable and the connection properties of the plugin by
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

QMap<QString,QString> PluginManager::PluginManagerPrivate::mappingToQMap(
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

void PluginManager::PluginManagerPrivate::setPluginProperties(
        const QString &basename,
        const QString &name, LoggerPtr logger /* = Logger::nullPtr() */)
{
    QCString pluginfile = name.utf8();
    lt_server_plugin_t * plugin = NULL;
    plugin = lt_server_plugin_load( pluginfile, NULL,
            NULL, pool );
    if( plugin == NULL ) {
        logger->error(QString("Failed to load the plugin %1:").arg(name));
        return;
    }

    logger->info(QString("Loaded the plugin %1 (registering under name '%2')")
            .arg(name).arg(basename));

    const lt_server_plugin_config_t * config = NULL;
    config = lt_server_plugin_default_config_get( plugin );

    if( config != NULL ) {
        connectionProperties[basename] = mappingToQMap(config->properties);
        apr_hash_t * h = NULL;
        QMap <QString, QString> merged, tmp;

        if( config->varproperties ) {
            h = (apr_hash_t*)apr_hash_get( config->varproperties,
                "bit", APR_HASH_KEY_STRING );
        } else {
            h = NULL;
        }
        variablesProperties[basename][IOPin::BitType] =
            tmp = mappingToQMap(h);
        // merge in properties for bit-typed variables
        merged = tmp;

        if( config->varproperties ) {
            h = (apr_hash_t*)apr_hash_get( config->varproperties,
                    "number", APR_HASH_KEY_STRING );
        } else {
            h = NULL;
        }
        variablesProperties[basename][IOPin::NumberType] =
            tmp = mappingToQMap(h);
        // merge in properties for number-typed variables
        for( QMap<QString,QString>::Iterator i = tmp.begin();
                i != tmp.end(); ++i ) {
            if( merged.contains( i.key() ) ) continue;
            merged.insert( i.key(), i.data() );
        }

        if( config->varproperties ) {
            h = (apr_hash_t*)apr_hash_get( config->varproperties,
                    "string", APR_HASH_KEY_STRING );
        } else {
            h = NULL;
        }
        variablesProperties[basename][IOPin::StringType] =
            tmp = mappingToQMap(h);
        // merge in properties for string-typed variables
        for( QMap<QString,QString>::Iterator i = tmp.begin();
                i != tmp.end(); ++i ) {
            if( merged.contains( i.key() ) ) continue;
            merged.insert( i.key(), i.data() );
        }

        // merged properties of all types of variables for backward
        // compatibility with VariablesDialog
        variablesProperties[basename][IOPin::InvalidType] = merged;
    } else {
        logger->warn( QString( "Plugin %1 does not define any properties"
                             ).arg( basename ) );
    }

    plugins.append(basename);
    isAvailable[basename] = true;
}

PluginManager::PluginManager(LoggerPtr logger /* = Logger::nullPtr() */)
    : d(new PluginManagerPrivate)
{
    Q_CHECK_PTR(d);

    apr_pool_create( &d->pool, NULL );

#ifdef MINIMAL_PLUGINS
    d->plugins = QStringList::split(",", "Loopback,Generator");

    // fill the connection properties
    QMap<QString,QString> m, empty;

    // Loopback properties
    m["latency"] = "0.0";
    d->connectionProperties["Loopback"] = m;
    m.clear();
    d->variablesProperties["Loopback"] = empty;

    // Generator properties
    m["refresh"] = "0.1";
    d->connectionProperties["Generator"] = m;
    m.clear();
    m["min"] = "-10";
    m["max"] = "+10";
    m["step"] = "+1";
    m["random"] = "0";
    d->variablesProperties["Generator"] = m;
    m.clear();
#else
    // where to look for plugins
    QValueList<QDir> pluginDirs;

    // application dir and prefix dir
    QDir appDir(qApp->applicationDirPath());
    QDir prefixDir(PREFIX_NATIVE);

    // where to look for plugins, prefix and appDir based
    pluginDirs.append(
            appDir.absFilePath("../../server-plugins/lsp-basic/.libs"));
    pluginDirs.append(appDir.absFilePath("../lib/lintouch/server-plugins"));
    QString p1 = prefixDir.absFilePath( "lib/lintouch/server-plugins" );
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
#endif
}

PluginManager::~PluginManager()
{
    apr_pool_destroy( d->pool );
    delete d;
}

// API

const QStringList& PluginManager::plugins() const
{
    Q_ASSERT(d);
    return d->plugins;
}

const QMap<QString,QString> & PluginManager::connectionProperties(
        const QString& c) const
{
    static QMap<QString,QString> dflt;
    Q_ASSERT(d);
    if( d->connectionProperties.contains( c ) ) {
        return d->connectionProperties[c];
    }
    return dflt;
}

const QMap<QString,QString>& PluginManager::variablesProperties(
        const QString& c, IOPin::Type type /* = IOPin::InvalidType */) const
{
    Q_ASSERT(d);
    switch( type ) {
        case IOPin::BitType:
        case IOPin::NumberType:
        case IOPin::StringType:
            return d->variablesProperties[c][type];
        default:
            return d->variablesProperties[c][IOPin::InvalidType];
    }
}

bool PluginManager::isAvailable(const QString& c) const
{
    Q_ASSERT(d);
    return d->isAvailable[c];
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: PluginManager.cpp 1170 2005-12-12 15:53:16Z modesto $
