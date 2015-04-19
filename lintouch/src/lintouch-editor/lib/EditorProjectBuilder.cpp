// $Id: EditorProjectBuilder.cpp 1558 2006-04-06 12:21:31Z modesto $
//
//   FILE: EditorProjectBuilder.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 30 July 2004
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

#include <lt/configuration/Configuration.hh>
#include <lt/configuration/DefaultConfiguration.hh>
#include <lt/configuration/DefaultConfigurationBuilder.hh>

#include <lt/vfs/TempFiles.hh>
#include <lt/vfs/VFSLocal.hh>
#include <lt/vfs/VFSZIP.hh>
#include <lt/templates/TemplateManager.hh>
#include <lt/templates/VFSLocalizator.hh>
#include <lt/project/ProjectBuilderProtected.hh>
#include <lt/project/ViewBuilder.hh>

#include "EditorProjectBuilder.hh"
#include "ServerLoggingBuilder.hh"

using namespace lt;

struct EditorProjectBuilder::EditorProjectBuilderPrivate
{
    /**
     * Create a temporary directory on the local filesystem and return
     * it as VFS.
     */
    VFSPtr createTemporaryDir(LoggerPtr logger = Logger::nullPtr()) const;

    /**
     * Fill in the Bindings structure of the project.
     */
    bool buildBindings(EditorProject &project,
        LoggerPtr logger = Logger::nullPtr());

    /**
     * Look for the editor settings in the VFS and parse it. Return a null
     * ptr on failure.
     */
    ConfigurationPtr buildEditorSettings(VFSPtr &v,
            LoggerPtr logger = Logger::nullPtr())
    {
        if (!v->isValid()) {
            logger->warn(QString("The location %1 is invalid")
                    .arg(v->rootLocation()));
            return ConfigurationPtr(new DefaultConfiguration(""));
        }

        //build a configuration for project-descriptor
        QIODevice *dev = v->openFile("editor-settings.xml");
        if (!dev->isOpen()) {
            delete dev;
            logger->info("The editor settings not found");
            return ConfigurationPtr(new DefaultConfiguration(""));
        }
        QString err;
        int line, col;
        ConfigurationPtr es =
            DefaultConfigurationBuilder().buildFromFile(dev, err, line, col);
        delete dev;
        if (es.isNull()) {
            logger->info("The editor settings is invalid");
            logger->info(QString("Possible parse error:%1:%2: %3")
                    .arg(line).arg(col).arg(err));
            return ConfigurationPtr(new DefaultConfiguration(""));
        }

        logger->info("The editor settings are loaded");
        return es;
    }

    bool buildServerLogging( EditorProject &result,
            ConfigurationPtr projectDescriptor, const VFSPtr &v,
            LoggerPtr logger = Logger::nullPtr())
    {
        //load logging
        ConfigurationPtr node =
            projectDescriptor->getChild("project-logging");

        if (!node.isNull()) //no logging defined
        {
            const ConfigurationList& logging = node->getChildren();

            for (ConfigurationList::const_iterator it = logging.begin();
                    it != logging.end(); ++it)
            {
                QString name = (*it)->getAttribute("name");
                if (name.isEmpty())
                {
                    logger->debug(QString(
                                "The Server Logging %1 has no name, ignored")
                            .arg((*it)->getLocation()));
                    continue;
                }
                logger->debug(QString("Creating the Server Logging %1")
                        .arg(name));

                QIODevice *dev = v->openFile((*it)->getAttribute("src"));
                if (!dev->isOpen())
                {
                    delete dev;
                    logger->warn(QString("The Server Logging %1" \
                               " not found: %2")
                            .arg(name).arg(node->getLocation()));
                    continue;
                }
                ServerLogging *sl = ServerLoggingBuilder::buildFromFile(
                        dev, logger->getChildLogger("logging-" + name));
                delete dev;

                if (sl == NULL)
                {
                    logger->warn(QString("The Server Logging %1 is invalid")
                            .arg(name));
                    continue;
                }

                result.addServerLogging(name, sl);
            }
        }

        if (result.serverLoggings().count() == 0)
            logger->warn("No valid Server Logging defined");

        return true;
    }
};

VFSPtr
    EditorProjectBuilder::EditorProjectBuilderPrivate::createTemporaryDir(
    LoggerPtr logger /* = Logger::nullPtr() */) const
{
    QUrl tempName = getTempFileName();

    //create the directory
    VFSPtr tempParent(new VFSLocal(tempName.dirPath(), false, logger));
    if (!tempParent->isValid())
    {
        logger->error(QString("Cannot access the temporary directory "
            " for the resource storage %1").arg(tempName.dirPath()));
        return VFSPtr();
    }
    if (!tempParent->mkdir(tempName.fileName()))
    {
        logger->error(QString("Cannot create a temporary storage for the "
            "resource %1").arg(tempName));
        return VFSPtr();
    }
    VFSPtr result(tempParent->subVFS(tempName.fileName()));
    if (!result->isValid())
    {
        logger->error(QString("Cannot open the temporary storage for "
            "the resource storage %1").arg(tempName));
        return VFSPtr();
    }

    return result;
}

struct VariableLocation
{
  QString connection;

  QString variable;
};

bool EditorProjectBuilder::EditorProjectBuilderPrivate::buildBindings(
    EditorProject &project, LoggerPtr logger /* = Logger::nullPtr() */)
{
    QMap<const Variable *, VariableLocation> allvars;

    //fill in the hash map
    const ConnectionDict& cd = project.connections();
    for(unsigned i = 0; i < cd.count(); ++i ) {

        VariableLocation vl;
        vl.connection = cd.keyAt(i);

        //we need the temporary or else - anyway, the C++ realm here
        const VariableDict& variables = cd[i]->variables();
        for(unsigned j=0; j < variables.count(); ++j) {

            vl.variable = variables.keyAt(j);
            allvars.insert(variables[j], vl);
        }
    }

    //find all the iopins
    for (unsigned int i = 0; i < project.views().count(); i++)
    {
        QString viewname = project.views().keyAt(i);
        const View &view = project.views()[i];
        for (unsigned int j = 0; j < view.panels().count(); j++)
        {
            QString panelname = view.panels().keyAt(j);
            const Panel &panel = view.panels()[j];
            for (unsigned int k = 0; k < panel.templates().count(); k++)
            {
                QString templatename = panel.templates().keyAt(k);
                const Template &temp = *panel.templates()[k];
                for (unsigned int m = 0; m < temp.iopins().count(); m++)
                {
                    QString iopinname = temp.iopins().keyAt(m);
                    const IOPin &iopin = *temp.iopins()[m];

                    const Variable *var = iopin.boundTo();
                    if (var == NULL)
                        continue;

                    if (!allvars.contains(var))
                    {   //should not happen - implementation error
                        logger->error(QString("Binding registration error: "
                            "view %1, panel %2, template %3, iopin %4 not "
                            "found among the registered variables")
                            .arg(viewname).arg(panelname).arg(templatename)
                            .arg(iopinname));
                        return false;
                    }

                    project.bindings().addBinding(allvars[var].connection,
                        allvars[var].variable, viewname, panelname,
                        templatename, iopinname);
                }
            }
        }
    }

    return true;
}

EditorProjectBuilder::EditorProjectBuilder(
    LoggerPtr logger /* = Logger::nullPtr() */,
    QString locale /* = QString::null */)
    : ProjectBuilder(logger, locale), p(new EditorProjectBuilderPrivate)
{
}

EditorProjectBuilder::~EditorProjectBuilder()
{
    delete p;
}

EditorProject EditorProjectBuilder::buildEditorProject(const QUrl& location,
    const TemplateManager &manager, const PluginManager &pluginManager,
    const ServerLoggingManager &slManager, ConfigurationPtr& es)
{
    d->cancelled = false;
    EditorProject result(getLoggerPtr());

    result.setTempFilename(getTempFileName());

    result.setTemplateManager( manager );

    //copy the original ZIP file to the temporary file
    QFile src(location.path());
    if (!src.exists())
    {
        getLogger().error(QString("The specified file %1 does not exist")
            .arg(location.path()));
        return EditorProject();
    }
    if (!src.open(IO_Raw | IO_ReadOnly))
    {
        getLogger().error(QString("Cannot open the project file %1")
            .arg(location.path()));
        return EditorProject();
    }
    QByteArray data = src.readAll();
    src.close();

    QFile dest(result.tempFilename());
    if (!dest.open(IO_Raw | IO_WriteOnly | IO_Truncate))
    {
        getLogger().error(QString("Cannot create the temporary file %1")
            .arg(result.tempFilename()));
        return EditorProject();
    }
    dest.writeBlock(data);
    dest.close();

    //
    VFSPtr v(new VFSZIP(result.tempFilename(), true, getLoggerPtr()));
    if (!v->isValid())
        return EditorProject();

    ConfigurationPtr projectDescriptor = d->buildDescriptor(v);
    if (projectDescriptor.isNull())
        return EditorProject();

    if (!d->buildInfo(result, projectDescriptor))
        return EditorProject();

    // build connections
    if (!d->buildConnections(result, projectDescriptor, v))
        return EditorProject();

    // build Server Logging
    p->buildServerLogging(result, projectDescriptor, v, getLoggerPtr());
    // there may be no Server Logging so no check

    es = p->buildEditorSettings(v, getLoggerPtr());
    // we don't mind if there is no editor-settings, so no checks here

    //create a temporary dir for the resources
    VFSPtr ur(p->createTemporaryDir(
        getLogger().getChildLogger("UnpackedResources")));
    if (ur.isNull())
        return EditorProject();
    result.setUnpackedResources(ur);
    //unpack the resources
    VFSPtr vr(v->subVFS("resources"));
    if (!vr->isValid())
    {
        getLogger().info("The project contains no resources.");
    }
    QStringList resources = vr->ls(".", VFSFilter::Files);
    for (QStringList::Iterator it = resources.begin();
        it != resources.end(); ++it )
    {
        QIODevice *dev = vr->openFile(*it);
        if (!dev->isOpen())
        {
            getLogger().warn(QString("Cannot open the resource %1")
                .arg(*it));
            delete dev;
            continue;
        }

        QIODevice *dest = ur->openFile(*it, IO_WriteOnly);
        if (!dest->isOpen())
        {
            getLogger().warn(QString("Cannot copy the resource %1 to a "
                "temporary location").arg(*it));
            delete dev;
            delete dest;
            continue;
        }
        dest->writeBlock(dev->readAll());
        delete dev;
        delete dest;
    }

    //build localizator on the unpacked resources VFS
    VFSLocalizatorPtr localizator(new VFSLocalizator(ur,
        d->locale));
    result.setLocalizator(localizator);
    if (d->cancelled)
        return EditorProject();

    if (!d->buildViews(result, projectDescriptor, v, manager,
        localizator, *this))
        return EditorProject();

    result.clearNew();

    //create resource manager
    result.setResourceManager(ResourceManagerPtr(new ResourceManager(ur)));

    //create a temporary dir for the deleted resources (undo)
    VFSPtr dr(p->createTemporaryDir(
        getLogger().getChildLogger("DeletedResources")));
    if (dr.isNull())
        return EditorProject();
    result.setDeletedResources(dr);

    //build bindings - bind iopins to variables
    if (!d->buildBindings(result, projectDescriptor, v ))
        return EditorProject();

    //build bindings - populate the Bindings class
    if (!p->buildBindings(result, getLoggerPtr()))
        return EditorProject();

    //disable the unlicensed connections
    ConnectionDict &connections = result.connections();
    for (unsigned int i = 0; i < connections.count(); i++)
    {
        QString connectionType = connections[i]->type();
        bool isAvailable =
            pluginManager.plugins().contains(connectionType) &&
            pluginManager.isAvailable(connectionType);

        connections[i]->setAvailable(isAvailable);
        if (!isAvailable)
        {
            //move the connection to disabledConnections
            getLogger().debug(QString("The connection %1 has been disabled").
                arg(connections.keyAt(i)));

            bool wasAutoDelete = connections.autoDelete();
            connections.setAutoDelete(false);

            result.disabledConnections().insert(connections.keyAt(i),
                connections[i]);
            connections.remove(connections.keyAt(i));
            i--;

            connections.setAutoDelete(wasAutoDelete);

        }
    }

    return result;
}

EditorProject EditorProjectBuilder::buildEditorProject(
    const TemplateManager &manager)
{
    d->cancelled = false;
    EditorProject result(getLoggerPtr());

    result.setTemplateManager( manager );

    //create a temporary dir for the resources
    VFSPtr ur(p->createTemporaryDir(
        getLogger().getChildLogger("UnpackedResources")));
    if (ur.isNull())
        return EditorProject();
    result.setUnpackedResources(ur);

    //create localizator on the unpacked resources VFS
    VFSLocalizatorPtr localizator(new VFSLocalizator(ur, d->locale));
    result.setLocalizator(localizator);

    //create resource manager
    result.setResourceManager(ResourceManagerPtr(new ResourceManager(ur)));

    //create a temporary dir for the deleted resources (undo)
    VFSPtr dr(p->createTemporaryDir(
        getLogger().getChildLogger("DeletedResources")));
    if (dr.isNull())
        return EditorProject();
    result.setDeletedResources(dr);

    //validate the Project in order for the caller to find out the
    //result of this method
    result.setInfo(Info());

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditorProjectBuilder.cpp 1558 2006-04-06 12:21:31Z modesto $
