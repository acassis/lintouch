// $Id: EditorProject.cpp 1558 2006-04-06 12:21:31Z modesto $
//
//   FILE: EditorProject.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 23 July 2004
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


#include <EditorProject.hh>
using namespace lt;

#include <lt/ui/Undo.hh>
#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qdict.h>

struct EditorProject::EditorProjectPrivate
{
    bool newFlag;

    /**
     * UndoStack pointer from ProjectWindow.
     */
    UndoStack* undoStack;

    /**
     * The temporary filename for the project file.
     */
    QString tempFilename;

    /**
     * The temporary directory for the unpacked resources.
     */
    VFSPtr unpackedResources;

    /**
     * The temporary directory for the deleted resources - for the undo.
     */
    VFSPtr deletedResources;

    /**
     * This reffer to the EditorProject logger
     */
    LoggerPtr logger;

    /**
     * Remove the local VFS together with its content.
     */
    void deleteVFS(const VFSPtr &v);

    EditorProjectPrivate()
        : newFlag(true), undoStack(NULL)
    {
    }

    /**
     * Destructor.
     */
    ~EditorProjectPrivate();

    /**
     * Bindings.
     **/
    Bindings bindings;

    /**
     * Holds the unlicensed connections and the connections for which
     * no plugins have been found.
     */
    ConnectionDict disabledConnections;

    /**
     * Map of resouce-paths. Used by Reimport resource command
     */
    QMap<QString,QString> resPath;

    /**
     * Map of resouce-paths of deleted resources. We need them for undo.
     */
    QMap<QString,QString> resPathDel;

    ServerLoggingDict loggings;
};

void EditorProject::EditorProjectPrivate::deleteVFS(const VFSPtr &v)
{
    if ( v.isNull() || !v->isValid() )
    {
        return;
    }

    QStringList lstContents = v->ls();
    QStringList::iterator i = lstContents.begin();

    while( i != lstContents.end() ) {
        QString absFilePath = v->rootLocation().path() + *i;

        if( QFileInfo( absFilePath ).isFile() ) {

            if( !v->unlink( *i ) ) {
                v->getLogger().debug( QString(
                    "Unable to delete the temporary project file %1").
                    arg( absFilePath ) );
             }
        } else {
            VFSPtr tempSub( v->subVFS(*i) );
            deleteVFS( tempSub );
        }
        ++i;
    }

    if ( !QDir().rmdir( v->rootLocation().path() ) ) {
        v->getLogger().debug(
                QString("Unable to delete the temporary project folder %1").
                arg( v->rootLocation().path() ) );
    } else {
        v->getLogger().debug(
                QString("The temporary project folder %1 removed").
                arg( v->rootLocation().path() ) );
    }

}

EditorProject::EditorProjectPrivate::~EditorProjectPrivate()
{
    QFile temp(tempFilename);
    if (temp.exists())
    {
        if( !temp.remove() ) {
            if( logger ) {
                logger->debug( QString(
                         "The temporary project file %1 not removed" ).
                        arg( tempFilename ) );
            }
        } else {
            if( logger ) {
                logger->debug( QString(
                        "The temporary project file %1 removed").
                        arg(tempFilename ) );
            }
        }
    } else {
        if( logger ) {
            logger->debug( QString(
                    "The temporary project file %1 does not exists").
                    arg( tempFilename) );
        }
    }

    deleteVFS(unpackedResources);
    deleteVFS(deletedResources);
}

EditorProject::EditorProject(LoggerPtr logger /* = Logger::nullPtr() */)
    : d(new EditorProjectPrivate)
{
    enableLogging(logger);
    d->logger = logger;
}

EditorProject::EditorProject ( const EditorProject & p )
    : Project(p), LogEnabled(p), d ( p.d )
{
}

EditorProject & EditorProject::operator = ( const EditorProject & p ) {

    (Project &) *this = p;
    (LogEnabled &) *this = p;
    d = p.d;
    return *this;
}

EditorProject::~EditorProject()
{
    //TODO:Remove this code
    //d->deleteVFS( d->unpackedResources );
    //d->deleteVFS( d->deletedResources );
}

void EditorProject::setUndoStack( UndoStack* undoStack )
{
    Q_ASSERT(d);
    d->undoStack = undoStack;
}

bool EditorProject::isDirty() const
{
    Q_ASSERT(d);
    return d->undoStack == NULL && !d->undoStack->isClean();
}

void EditorProject::setDirty(bool flag)
{
    Q_ASSERT(d);
    if (d->undoStack != NULL && !flag)
//        d->undoStack->setClean();
        d->undoStack->clear();
}

bool EditorProject::isNew() const
{
    return d->newFlag;
}

void EditorProject::clearNew()
{
    d->newFlag = false;
}

UndoStack *EditorProject::undoStack() const
{
    Q_ASSERT(d);
    return d->undoStack;
}

void EditorProject::setTempFilename(const QString &filename)
{
    QFile temp(d->tempFilename);
    if (temp.exists())
    {
        temp.remove();
    }
    d->tempFilename = filename;
}

QString EditorProject::tempFilename() const
{
    return d->tempFilename;
}

void EditorProject::setUnpackedResources(const VFSPtr &vfs)
{
    d->unpackedResources = vfs;
}

VFSPtr EditorProject::unpackedResources() const
{
    return d->unpackedResources;
}

void EditorProject::setDeletedResources(const VFSPtr &vfs)
{
    d->deletedResources = vfs;
}

VFSPtr EditorProject::deletedResources() const
{
    return d->deletedResources;
}

const Bindings &EditorProject::bindings() const
{
  return d->bindings;
}

Bindings &EditorProject::bindings()
{
  return d->bindings;
}

QString EditorProject::addResource(const QString &filename)
{
    if (d->unpackedResources.isNull() || !d->unpackedResources->isValid())
    {
        getLogger().error("Attempting to add a resource to an invalid VFS");
        return QString();
    }
    QFileInfo fi(filename);
    if (!QFileInfo(filename).exists())
    {
        getLogger().error(QString("The specified file %1 does not exist")
            .arg(filename));
        return QString();
    }

    //generate a unique name
    QString result = fi.fileName();
    for (int i = 0; d->unpackedResources->exists(result); i++)
    {
        result = QString("%1%2.%3").arg(fi.baseName()).arg(i)
            .arg(fi.extension());
    }

    return addResource(filename, result);
}

QString EditorProject::addResource(const QString &filename, const QString& key)
{
    if (d->unpackedResources.isNull() || !d->unpackedResources->isValid())
    {
        getLogger().error("Attempting to add a resource to an invalid VFS");
        return QString();
    }

    QFileInfo fi(filename);
    if (!QFileInfo(filename).exists())
    {
        getLogger().error(QString("The specified file %1 does not exist")
            .arg(filename));
        return QString();
    }

    //copy the source to the new unique name
    QFile src(filename);
    if (!src.open(IO_ReadOnly))
    {
        getLogger().error(QString("Cannot open the resource %1").
            arg(filename));
        return QString();
    }
    QIODevice *dest = d->unpackedResources->openFile(key, IO_WriteOnly);
    if (!dest->isOpen())
    {
        getLogger().error(QString("Cannot copy the resource %1 to a "
            "temporary location %2").arg(filename)
            .arg(d->unpackedResources->rootLocation() + "/" + key));
        delete dest;
        return QString();
    }
    dest->writeBlock(src.readAll());
    delete dest;

    // save the full path to the resource
    d->resPath.insert(key, filename);

    return key;
}

QString EditorProject::deleteResource(const QString &key)
{
    if (d->unpackedResources.isNull() || !d->unpackedResources->isValid())
    {
        getLogger().error("Attempting to remove a resource from an invalid"
            " VFS");
        return QString();
    }
    if (d->deletedResources.isNull() || !d->deletedResources->isValid())
    {
        getLogger().error("Attempting to remove a resource to an invalid"
            " VFS (unable to undo the operation)");
        return QString();
    }

    //generate a pseudo-unique name for the undo
    QFileInfo fi(key);
    QString result = key;
    for (int i = 0; d->deletedResources->exists(result); i++)
    {
        result = QString("%1-%2.%3").arg(fi.baseName()).arg(i)
            .arg(fi.extension());
    }

    //copy the resource to the trash
    QIODevice *src = d->unpackedResources->openFile(key);
    if (!src->isOpen())
    {
        getLogger().error(QString("Cannot open the resource %1").arg(key));
        delete src;
        return QString();
    }
    QIODevice *dest = d->deletedResources->openFile(result, IO_WriteOnly);
    if (!dest->isOpen())
    {
        getLogger().error(QString("Cannot copy the resource %1 to the "
            "temporary location %2").arg(key).arg(result));
        delete dest;
        delete src;
        return QString();
    }
    dest->writeBlock(src->readAll());
    delete dest;
    delete src;

    //remove the source resource
    if (!d->unpackedResources->unlink(key))
    {
        getLogger().warn(QString("Cannot remove the resource %1")
            .arg(key));
    }

    // remove the saved full path to the resource
    d->resPathDel.insert(key, d->resPath[key]);
    d->resPath.remove(key);

    return result;
}

bool EditorProject::undeleteResource(const QString &key,
    const QString &undoname)
{
    if (d->unpackedResources.isNull() || !d->unpackedResources->isValid())
    {
        getLogger().error("Attempting to restore a resource from an invalid"
            " VFS");
        return false;
    }
    if (d->deletedResources.isNull() || !d->deletedResources->isValid())
    {
        getLogger().error("Attempting to restore a resource to an invalid"
            " VFS (unable to undo the operation)");
        return false;
    }

    if (d->unpackedResources->exists(key))
    {
        getLogger().error(QString("The temporary resource storage is "
            "corrupted. It should not contain the file %1")
            .arg(key));
        return false;
    }

    //restore the resource from the trash
    QIODevice *src = d->deletedResources->openFile(undoname);
    if (!src->isOpen())
    {
        getLogger().error(QString("Cannot open the resource %1")
            .arg(undoname));
        delete src;
        return false;
    }
    QIODevice *dest = d->unpackedResources->openFile(key, IO_WriteOnly);
    if (!dest->isOpen())
    {
        getLogger().error(QString("Cannot restore the resource %1 from the "
            "temporary location %2").arg(key).arg(undoname));
        delete dest;
        delete src;
        return false;
    }
    dest->writeBlock(src->readAll());
    delete dest;
    delete src;

    //remove the resource from the trash
    if (!d->deletedResources->unlink(undoname))
    {
        getLogger().warn(QString("Cannot remove the resource %1 from the"
            " trash").arg(undoname));
    }

    // add the saved full path to the resource
    d->resPath.insert(key, d->resPathDel[key]);
    d->resPathDel.remove(key);

    return true;
}

bool EditorProject::renameResource(const QString &oldname,
    const QString &newname)
{
    if (d->unpackedResources.isNull() || !d->unpackedResources->isValid())
    {
        getLogger().error("Attempting to rename a resource in an invalid"
            " VFS");
        return false;
    }

    //FIXME: speed issues - implement the rename method within VFS and
    //use it
    //copy the resource from one file to another
    QIODevice *src = d->unpackedResources->openFile(oldname);
    if (!src->isOpen())
    {
        getLogger().error(QString("Cannot open the resource %1")
            .arg(oldname));
        delete src;
        return false;
    }
    QIODevice *dest = d->unpackedResources->openFile(newname, IO_WriteOnly);
    if (!dest->isOpen())
    {
        getLogger().error(QString("Cannot rename the resource %1 to %2")
            .arg(oldname).arg(newname));
        delete dest;
        delete src;
        return false;
    }
    dest->writeBlock(src->readAll());
    delete dest;
    delete src;

    //remove the old resource
    if (!d->unpackedResources->unlink(oldname))
    {
        getLogger().warn(QString("Cannot remove the resource %1 from the"
            " resources storage").arg(oldname));
    }

    // Update the resPath as well
    d->resPath.insert(newname, d->resPath[oldname]);
    d->resPath.remove(oldname);

    return true;
}

QString EditorProject::fullPathToResouce(const QString& key)
{
    Q_ASSERT(d);
    if(d->resPath.contains(key)) {
        return d->resPath[key];
    }
    return QString::null;
}

const ConnectionDict& EditorProject::disabledConnections() const
{
    return d->disabledConnections;
}

ConnectionDict& EditorProject::disabledConnections()
{
    return d->disabledConnections;
}

bool EditorProject::checkConnectionName(const QString& cn) const
{
    return !connections().contains(cn) && !d->disabledConnections.contains(cn);
}

bool EditorProject::addServerLogging ( const QString & key,
        const ServerLogging * l)
{
    if ( d->loggings.contains( key ) ) {
        return false;
    }
    d->loggings.insert ( key, l );
    return true;
}

bool EditorProject::removeServerLogging ( const QString & key )
{
    if ( d->loggings.contains( key ) ) {
        d->loggings.remove ( key );
        return true;
    }
    return false;
}

const ServerLoggingDict & EditorProject::serverLoggings () const
{
    return d->loggings;
}

ServerLoggingDict & EditorProject::serverLoggings ()
{
    return d->loggings;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditorProject.cpp 1558 2006-04-06 12:21:31Z modesto $
