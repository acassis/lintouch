// $Id: EditorProjectSerializer.cpp 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: EditorProjectSerializer.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 08 September 2004
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

#include <qfile.h>
#include <qurl.h>
#include <qregexp.h>

#include <lt/configuration/DefaultConfiguration.hh>
#include <lt/configuration/DefaultConfigurationBuilder.hh>
#include <lt/configuration/DefaultConfigurationSerializer.hh>

#include <lt/vfs/VFSZIP.hh>
#include <lt/vfs/VFSLocal.hh>
#include <lt/vfs/TempFiles.hh>

#include "ProjectWindow.hh"

#include "BindingSerializer.hh"
#include "ConnectionSerializer.hh"
#include "EditorProjectSerializer.hh"
#include "InfoSerializer.hh"
#include "ViewSerializer.hh"
#include "ServerLoggingSerializer.hh"

using namespace lt;

struct EditorProjectSerializer::EditorProjectSerializerPrivate
{
    /**
     * Remove all the current view, connection, or binding files from
     * the project.
     */
    bool removeSources(const ConfigurationPtr &parentNode, VFSPtr &vfs);
};

bool EditorProjectSerializer::EditorProjectSerializerPrivate::removeSources(
    const ConfigurationPtr &parentNode, VFSPtr &vfs)
{
    if (parentNode.isNull())
        return true;
    ConfigurationList children = parentNode->getChildren();
    ConfigurationList::const_iterator it;
    for (it = const_cast<ConfigurationList&>(children).begin();
            it != const_cast<ConfigurationList&>(children).end(); ++it)
    {
        QString filename = (*it)->getAttribute("src");
        if (filename.isEmpty())
            continue;
        vfs->unlink(filename);
    }
    return true;
}


EditorProjectSerializer::EditorProjectSerializer(
    LoggerPtr logger /* = Logger::nullPtr() */)
    : d(new EditorProjectSerializerPrivate)
{
    enableLogging(logger);
}

EditorProjectSerializer::~EditorProjectSerializer()
{
    delete d;
}

bool EditorProjectSerializer::saveEditorProject(EditorProject &prj,
        const QUrl& location, ConfigurationPtr es)
{
    if (prj.tempFilename().isNull())
        prj.setTempFilename(getTempFileName());

    VFSPtr temp(new VFSZIP(prj.tempFilename(), false,
        getLogger().getChildLogger("TemporaryZIP")));

    if (!temp->isValid())
    {
        getLogger().error(QString("Cannot open the temporary storage for "
            "the resource storage %1").arg(prj.tempFilename()));
        return false;
    }

    //build a configuration for project-descriptor
    ConfigurationPtr projectDescriptor;
    if (temp->exists("project-descriptor.xml"))
    {
        QIODevice *dev = temp->openFile("project-descriptor.xml");
        if (!dev->isOpen())
        {
            delete dev;
            getLogger().error("Could not open the project descriptor");
            return false;
        }

        QString err;
        int line, col;
        projectDescriptor =
            DefaultConfigurationBuilder().buildFromFile(dev, err, line,
            col);
        delete dev;
        if (projectDescriptor.isNull())
        {
            getLogger().warn("The project descriptor is invalid");
            getLogger().info(QString("Possible parse error:%1:%2: %3")
                .arg(line).arg(col).arg(err));
            return false;
        }
    }
    else
    {
        projectDescriptor = ConfigurationPtr(new DefaultConfiguration(
            "project-descriptor"));
    }

    ConfigurationPtr newDescriptor = ConfigurationPtr(
        new DefaultConfiguration("project-descriptor"));

    //serialize the info - always overwrite
    ConfigurationPtr pi = newDescriptor->getChild("project-info", true);
    if (pi.isNull())
    {   //very unlikely
        getLogger().error("Could not create the info node: 777");
        return false;
    }
    if (!InfoSerializer::saveToConfiguration(pi, prj.info(),
        getLogger().getChildLogger("Info")))
        return false;

    d->removeSources(projectDescriptor->getChild("project-views"), temp);

    ConfigurationPtr pv = newDescriptor->getChild("project-views", true);
    if (pv.isNull())
    {   //very unlikely
        getLogger().error("Could not create the project-views node");
        return false;
    }

    //save the views
    const ViewMap &vm = prj.views();
    unsigned it;
    for ( it = 0; it < vm.count(); it++ )
    {
        ConfigurationPtr node = pv->getChild("view", true);
        node->setAttribute("name", vm.keyAt( it ));
        QString filename = QString("view-%1").arg(vm.keyAt( it ));

        //get rid off invalid characters in the filename
        filename.replace(QRegExp ("[^A-Za-z0-9_.-]*"), "");

        //check if the filename does not exist
        QString validName = filename + ".xml";
        for (int i = 2; temp->exists(validName); i++)
            validName = QString("%1%2.xml").arg(filename).arg(i);
        filename = validName;

        node->setAttribute("src", filename);

        QIODevice *dev = temp->openFile(filename, IO_WriteOnly);
        if (!dev->isOpen())
        {
            delete dev;
            getLogger().error(QString(
                "Cannot open the view file %1 for saving.").arg(filename));
            return false;
        }

        if (!ViewSerializer::saveToFile(dev, vm[ it ],
            getLogger().getChildLogger(QString("View %1").arg(vm.keyAt( it )))))
        {
            delete dev;
            getLogger().error(QString("Cannot save the view %1")
                .arg(vm.keyAt( it )));
            return false;
        }
        delete dev;
    }

    //delete all the libraries from the project
    ConfigurationPtr pt = newDescriptor->getChild(
        "project-template-libraries", true);
    if (pt.isNull())
    {   //very unlikely
        getLogger().error("Could not create the project-template-libraries"
            " node");
        return false;
    }

    //find all the used libraries
    QStringList usedLibraries;
    const ViewMap views = prj.views();
    for( unsigned iv = 0; iv < views.count(); iv++ )
    {
        const PanelMap & panels = views[ iv ].panels();
        for( unsigned ip = 0; ip < panels.count(); ip++ )
        {
            const TemplateDict & templates = panels[ ip ].templates();
            for( unsigned it = 0; it < templates.count(); it++ )
            {
                QString library = templates[ it ]->library();
                if (!usedLibraries.contains(library))
                {
                    ConfigurationPtr node = pt->getChild(
                        "template-library", true);
                    if (node.isNull())
                    {
                        getLogger().error("Could not create the "
                            "template-library node");
                        return false;
                    }
                    node->setAttribute("name", library);

                    usedLibraries.append(library);
                }
            }
        }
    }

    d->removeSources(projectDescriptor->getChild("project-connections"),
        temp);

    ConfigurationPtr pc = newDescriptor->getChild("project-connections",
        true);
    if (pc.isNull())
    {   //very unlikely
        getLogger().error("Could not create the project-connections node");
        return false;
    }

    //save the connections
    ConnectionDict cd = prj.connections();
    //add the disabled connection to the list of all connections temporarily
    const ConnectionDict& ud = prj.disabledConnections();
    for (it = 0; it < ud.count(); it++) cd.insert(ud.keyAt(it), ud[it]);

    for ( it = 0; it < cd.count(); it++ )
    {
        ConfigurationPtr node = pc->getChild("connection", true);
        node->setAttribute("name", cd.keyAt( it ));
        QString filename = QString("connection-%1").arg(cd.keyAt( it ));

        //get rid off invalid characters in the filename
        filename.replace(QRegExp ("[^A-Za-z0-9_.-]*"), "");

        //check if the filename does not exist
        QString validName = filename + ".xml";
        for (int i = 2; temp->exists(validName); i++)
            validName = QString("%1%2.xml").arg(filename).arg(i);
        filename = validName;

        node->setAttribute("src", filename);

        QIODevice *dev = temp->openFile(filename, IO_WriteOnly);
        if (!dev->isOpen())
        {
            delete dev;
            getLogger().error(QString(
                "Cannot open the connection file %1 for saving.")
                .arg(filename));
            return false;
        }

        if (!ConnectionSerializer::saveToFile(dev, *cd[it],
            getLogger().getChildLogger(QString("Connection %1")
            .arg(cd.keyAt(it)))))
        {
            delete dev;
            getLogger().error(QString("Cannot save the connection %1")
                .arg(cd.keyAt(it)));
            return false;
        }
        delete dev;
    }

    d->removeSources(projectDescriptor->getChild("project-bindings"), temp);

    ConfigurationPtr pb = newDescriptor->getChild("project-bindings", true);
    if (pb.isNull())
    {   //very unlikely
        getLogger().error("Could not create the project-bindings node");
        return false;
    }

    //save the bindings
    for (unsigned int vi = 0; vi < vm.count(); vi++)
    {
        for (unsigned int ci = 0; ci < cd.count(); ci++)
        {
            ConfigurationPtr node = pb->getChild("binding", true);
            node->setAttribute("connection", cd.keyAt(ci));
            node->setAttribute("view", vm.keyAt(vi));
            QString filename = QString("binding-%1-%2")
              .arg(cd.keyAt(ci)).arg(vm.keyAt(vi));

            //get rid off invalid characters in the filename
            filename.replace(QRegExp ("[^A-Za-z0-9_.-]*"), "");

            //check if the filename does not exist
            QString validName = filename + ".xml";
            for (int i = 2; temp->exists(validName); i++)
                validName = QString("%1%2.xml").arg(filename).arg(i);
            filename = validName;

            node->setAttribute("src", filename);

            QIODevice *dev = temp->openFile(filename, IO_WriteOnly);
            if (!dev->isOpen())
            {
                delete dev;
                getLogger().error(QString(
                    "Cannot open the binding file %1 for saving.")
                    .arg(filename));
                return false;
            }

            if (!BindingSerializer::saveToFile(dev,
                cd.keyAt(ci), vm.keyAt(vi), prj,
                getLogger().getChildLogger(QString("Binding %1 %2")
                .arg(cd.keyAt(ci)).arg(vm.keyAt(vi)))))
            {
                delete dev;
                getLogger().error(QString("Cannot save the binding %1 %2")
                    .arg(cd.keyAt(ci)).arg(vm.keyAt(vi)));
                return false;
            }
            delete dev;
        }
    }

    // serialize server loggings
    d->removeSources(projectDescriptor->getChild("project-logging"), temp);

    ConfigurationPtr slpc = newDescriptor->getChild("project-logging", true);
    if (slpc.isNull())
    {   //very unlikely
        getLogger().error("Could not create the project-logging node");
        return false;
    }

    //save the server logging
    ServerLoggingDict sl = prj.serverLoggings();
    for ( it = 0; it < sl.count(); it++ )
    {
        ConfigurationPtr node = slpc->getChild("logging", true);
        node->setAttribute("name", sl.keyAt( it ));
        QString filename = QString("logging-%1").arg(sl.keyAt( it ));

        //get rid off invalid characters in the filename
        filename.replace(QRegExp ("[^A-Za-z0-9_.-]*"), "");

        //check if the filename does not exist
        QString validName = filename + ".xml";
        for (int i = 2; temp->exists(validName); i++)
            validName = QString("%1%2.xml").arg(filename).arg(i);
        filename = validName;

        node->setAttribute("src", filename);

        QIODevice *dev = temp->openFile(filename, IO_WriteOnly);
        if (!dev->isOpen())
        {
            delete dev;
            getLogger().error(QString(
                "Cannot open the server logging file %1 for saving.")
                .arg(filename));
            return false;
        }

        if (!ServerLoggingSerializer::saveToFile(dev, *sl[it],
            getLogger().getChildLogger(QString("ServerLogging %1")
            .arg(sl.keyAt(it)))))
        {
            delete dev;
            getLogger().error(QString("Cannot save the server logging %1")
                .arg(sl.keyAt(it)));
            return false;
        }
        delete dev;
    }

    //serialize the resources
    VFSPtr vr(temp->subVFS("resources"));
    if (vr->isValid())
    {
        QStringList resources = vr->ls(".", VFSFilter::Files);
        for (QStringList::Iterator it = resources.begin();
            it != resources.end(); ++it )
        {
            vr->unlink(*it);
        }
    }
    else
    {
        if (!temp->mkdir("resources"))
        {
            getLogger().error("Cannot create the directory for the "
                "resources.");
            return false;
        }
        vr = VFSPtr(temp->subVFS("resources"));
        if (!vr->isValid())
        {
            getLogger().error("Cannot open the directory for the "
                "resources.");
            return false;
        }
    }
    //pack all the unpackedResources to the temporary ZIP
    QStringList resources = prj.unpackedResources()->ls(".",
        VFSFilter::Files);
    for (QStringList::Iterator it1 = resources.begin();
        it1 != resources.end(); ++it1 )
    {
        QIODevice *dev = prj.unpackedResources()->openFile(*it1);
        if (!dev->isOpen())
        {
            getLogger().warn(QString("Cannot open the resource %1")
                .arg(*it1));
            delete dev;
            return false;
        }

        QIODevice *dest = vr->openFile(*it1, IO_WriteOnly);
        if (!dest->isOpen())
        {
            getLogger().warn(QString("Cannot copy the resource %1 to the "
                "temporary project ZIP file").arg(*it1));
            delete dev;
            delete dest;
            return false;
        }
        dest->writeBlock(dev->readAll());
        delete dev;
        delete dest;
    }

    //copy the garbage fragments from the old descriptor
    ConfigurationList tops = projectDescriptor->getChildren();
    for (ConfigurationList::const_iterator it2 =
            const_cast<ConfigurationList&>(tops).begin();
            it2 != const_cast<ConfigurationList&>(tops).end(); ++it2)
    {
        if ((*it2)->getName() == "project-info" ||
            (*it2)->getName() == "project-views" ||
            (*it2)->getName() == "project-connections" ||
            (*it2)->getName() == "project-bindings" ||
            (*it2)->getName() == "project-template-libraries" ||
            (*it2)->getName() == "project-logging")
            continue;
        newDescriptor->addChild(*it2);
    }

    //write the descriptor to the file
    QIODevice *dev = temp->openFile("project-descriptor.xml", IO_WriteOnly);
    if (!dev->isOpen())
    {
        delete dev;
        getLogger().error(
            "Could not open the project descriptor for writing");
        return false;
    }
    if (!DefaultConfigurationSerializer().serializeToFile(dev,
        newDescriptor))
    {
        delete dev;
        getLogger().error("Cannot write the project descriptor to the "
            "temporary file");
        return false;
    }
    delete dev;

    ///////////////// editor-settings ///////////////////////
    //write the descriptor to the file
    dev = temp->openFile("editor-settings.xml", IO_WriteOnly);
    if (!dev->isOpen())
    {
        delete dev;
        getLogger().error("Could not open the grid settings for writing");
        return false;
    }
    if (!DefaultConfigurationSerializer().serializeToFile(dev, es))
    {
        delete dev;
        getLogger().error("Cannot write the grid settings to the "
                "temporary file");
        return false;
    }
    delete dev;
    ////////////////////////////////////////////////////////


    //copy the temporary file over the original file
    QFile src(prj.tempFilename());
    if (!src.exists())
    {
        getLogger().error(QString("The temporary file %1 has been deleted. "
            "Cannot save the project.").arg(prj.tempFilename()));
        return false;
    }
    if (!src.open(IO_Raw | IO_ReadOnly))
    {
        getLogger().error(QString("Cannot open the temporary file %1"
            "Cannot save the project.").arg(prj.tempFilename()));
        return false;
    }

    QFile dest(location.path());
    if (!dest.open(IO_Raw | IO_WriteOnly | IO_Truncate))
    {
        getLogger().error(QString("Cannot write to the project %1")
            .arg(location.path()));
        return false;
    }
    dest.writeBlock(src.readAll());
    dest.close();
    src.close();

    prj.clearNew();
    prj.setDirty(false);
    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditorProjectSerializer.cpp 1564 2006-04-07 13:33:15Z modesto $
