// $Id: TemplateManagerBuilder.cpp,v 1.8 2004/12/16 12:33:22 modesto Exp $
//
//   FILE: TemplateManagerBuilder.cpp --
//   AUTHOR: Jiri Barton <jbar@swac.cz>
//   DATE: 14 July 2004
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

#include <qlibrary.h>
#include <qfile.h>
#include <qdir.h>

#if defined(Q_OS_WIN32)
#include <qapplication.h>
#endif

#include <lt/templates/PluginTemplateLibraryLoader.hh>
#include <lt/vfs/TempFiles.hh>
#include <lt/vfs/VFSLocal.hh>

#include "lt/project/TemplateManagerBuilder.hh"
using namespace lt;

struct TemplateManagerBuilder::TemplateManagerBuilderPrivate
{
    /**
     * The list of directories to search template libraries in.
     */
    QStringList defaultDirs;

    /**
     * Add extensions, etc. to the libname in the directory dir.
     */
    void addDecoratedLibraryName(const QString &libname, const QString &dir,
            QStringList &names);
};

void TemplateManagerBuilder::TemplateManagerBuilderPrivate
::addDecoratedLibraryName(
        const QString &libname,
        const QString &dir,
        QStringList &names)
{
    // prepare directory where we will be looking
    QDir libdir( dir );
    // decorate libname with platform specific prefix & suffix
    QString decorated = QLibrary(libname).library();

    // cat them together
    QFileInfo fi( libdir, decorated );

    if( fi.isReadable() ) {
#ifdef QT_DEBUG
        qWarning( "FOUND: " + fi.filePath() );
#endif
        names.append( fi.filePath() );
    }
}

TemplateManagerBuilder::TemplateManagerBuilder(
        LoggerPtr logger /* = Logger::nullPtr() */ )
: d(new TemplateManagerBuilderPrivate)
{
    enableLogging(logger);

    d->defaultDirs = TemplateManager::templateLibraryDirs();
}

TemplateManagerBuilder::~TemplateManagerBuilder()
{
    delete d;
}

const QStringList& TemplateManagerBuilder::defaultDirs() const
{
    Q_ASSERT(d);
    return d->defaultDirs;
}

TemplateManager TemplateManagerBuilder::buildFromNames(
        ConfigurationPtr names)
{
    TemplateManager result;
    if (names.isNull()) {
        getLogger().warn( "Project does not use any template library." );
        return result;
    }

    ConfigurationList tl = names->getChildren();
    for (ConfigurationList::const_iterator it =
            const_cast<ConfigurationList&>(tl).begin();
            it != const_cast<ConfigurationList&>(tl).end(); ++it)
    {
        TemplateLibrary *library = NULL;
        QUrl filename;

        //search order of the template names
        QString libraryName = (*it)->getAttribute("name");
        if (libraryName.isEmpty())
        {
            getLogger().warn(QString(
                        "The library name in the project "
                        "descriptor is empty %1")
                    .arg((*it)->getLocation()));
            continue;
        }

        QStringList filesToTry;

        // decorate template library name with system specific locations
        QStringList::const_iterator dir;
        for( dir = const_cast<QStringList&>(d->defaultDirs).begin();
                dir != const_cast<QStringList&>(d->defaultDirs).end(); ++dir )
        {
            d->addDecoratedLibraryName( libraryName, *dir, filesToTry );
        }

        //let QT load it from platform-dependent locations with
        //platform-dependent specifics
        filesToTry.append( libraryName );

        // try to load one of the specified files
        QStringList::const_iterator file;
        for( file = const_cast<QStringList&>(filesToTry).begin();
                file != const_cast<QStringList&>(filesToTry).end(); ++file )
        {
#ifdef QT_DEBUG
            qWarning( "TRYING TO LOAD: " + *file );
#endif
            library = PluginTemplateLibraryLoader::loadFrom( *file,
                    libraryName, getLogger().getChildLogger( libraryName ));
            if (library->isValid())
            {
                getLogger().info(QString(
                            "Loaded template library from the file %1")
                        .arg( *file ));
                break;
            }
        }

        // report problems
        if (library == NULL || !library->isValid())
        {
            getLogger().warn(QString(
                        "The template library %1 not loaded: %2")
                    .arg(libraryName)
                    .arg((*it)->getLocation()));
            getLogger().debug(QString(
                        "Tried the following locations:\n    %1")
                    .arg(d->defaultDirs.join("\n    ")));
            continue;
        }

        if (library == NULL || !library->isValid())
            continue;

        library->setAvailable( library->isValid() );
        result.registerTemplateLibrary(libraryName, library);
    }

    return result;
}

TemplateManager TemplateManagerBuilder::buildFromDefaultDirs()
{
    TemplateManager result;

    const QStringList& dd = defaultDirs();

    for(QStringList::const_iterator st = dd.begin(); st != dd.end(); ++st)
    {
        VFSPtr v(new VFSLocal(*st, true,
                    getLogger().getChildLogger(QString("VFS %1").arg(*st))));
        QStringList filenames = v->ls(".", VFSFilter::Files);
        for (QStringList::const_iterator sf =
                const_cast<QStringList&>(filenames).begin();
                sf != const_cast<QStringList&>(filenames).end();
                ++sf)
        {
            //check if the name is feasable
#if defined(Q_OS_UNIX)
            if (!(*sf).endsWith(".so"))
                continue;
#elif defined(Q_OS_WIN32)
            if (!(*sf).endsWith(".dll", false))
                continue;
#endif

            //FIXME: get the library name
            QString libname = *sf;
#if defined(Q_OS_UNIX)
            libname = QUrl(*sf).fileName();
            libname = libname.left(libname.length() - 3);
            if (libname.startsWith("lib"))
                libname.remove(0, 3);
#elif defined(Q_OS_WIN32)
            libname = QUrl(*sf).fileName();
            libname = libname.left(libname.length() - 4);
#endif

            QString filename = QDir(*st).absFilePath(*sf);
            //load the library
            TemplateLibrary *library = PluginTemplateLibraryLoader
                ::loadFrom(filename, libname,
                        getLogger().getChildLogger(*sf));

            if (result.templateLibraries().contains(libname)) {
                if (library->isValid()) {
                    getLogger().info(QString(
                                "Replacing the library %1 with %2")
                            .arg(result.templateLibraries()[libname]
                                ->filename())
                            .arg(*sf));
                    library->setAvailable( true );
                    result.unregisterTemplateLibrary(libname);
                    result.registerTemplateLibrary(libname, library);
                }
            } else {
                getLogger().info(QString(
                            "Loaded %1 library %2 from the file %3")
                        .arg(library->isValid() ? "a valid" : "an invalid")
                        .arg(libname)
                        .arg(filename));
                library->setAvailable( library->isValid() );
                result.registerTemplateLibrary(libname, library);
            }
        }
    }
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateManagerBuilder.cpp,v 1.8 2004/12/16 12:33:22 modesto Exp $
