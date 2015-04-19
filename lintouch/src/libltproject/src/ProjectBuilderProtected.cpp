// $Id: ProjectBuilderProtected.cpp,v 1.6 2004/10/14 10:17:00 modesto Exp $
//
//   FILE: ProjectBuilderProtected.cpp --
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

#include <qmap.h>
#include <qlibrary.h>

#include <lt/configuration/DefaultConfiguration.hh>
#include <lt/configuration/DefaultConfigurationBuilder.hh>

#include <lt/LTMap.hh>

#include <lt/templates/TemplateLibrary.hh>
#include <lt/templates/TemplateManager.hh>
#include <lt/templates/VFSLocalizator.hh>
#include <lt/vfs/VFS.hh>

#include "lt/project/ConnectionBuilder.hh"
#include "lt/project/InfoBuilder.hh"
#include "lt/project/ProjectBuilderProtected.hh"
#include "lt/project/TemplateManagerBuilder.hh"
#include "lt/project/ViewBuilder.hh"

using namespace lt;

ConfigurationPtr ProjectBuilder::ProjectBuilderProtected::buildDescriptor(
    const VFSPtr &v)
{
    if (!v->isValid())
    {
        getLogger().warn(QString("The location %1 is invalid")
                .arg(v->rootLocation()));
        return ConfigurationPtr(new DefaultConfiguration(""));
    }
    if (cancelled)
        return ConfigurationPtr(new DefaultConfiguration(""));

    //build a configuration for project-descriptor
    QIODevice *dev = v->openFile("project-descriptor.xml");
    if (!dev->isOpen())
    {
        delete dev;
        getLogger().warn("The project descriptor not found");
        return ConfigurationPtr(new DefaultConfiguration(""));
    }
    QString err;
    int line, col;
    ConfigurationPtr projectDescriptor =
        DefaultConfigurationBuilder().buildFromFile(dev, err, line, col);
    delete dev;
    if (projectDescriptor.isNull())
    {
        getLogger().warn("The project descriptor is invalid");
        getLogger().info(QString("Possible parse error:%1:%2: %3")
            .arg(line).arg(col).arg(err));
        return ConfigurationPtr(new DefaultConfiguration(""));
    }
    if (cancelled)
        return ConfigurationPtr(new DefaultConfiguration(""));

    return projectDescriptor;
}

bool ProjectBuilder::ProjectBuilderProtected::buildInfo(Project &result,
    ConfigurationPtr projectDescriptor)
{
    result.setInfo(InfoBuilder::buildFromConfiguration(
        projectDescriptor->getChild("project-info"), getLoggerPtr()));
    if (!result.info().date().isValid())
    {
        getLogger().warn("The project info is invalid");
        return false;
    }
    if (cancelled)
        return false;

    return true;
}

bool ProjectBuilder::ProjectBuilderProtected::buildConnections(
    Project &result, ConfigurationPtr projectDescriptor, const VFSPtr &v)
{
    //load connections
    ConfigurationPtr node = projectDescriptor->getChild("project-connections");
    if (!node.isNull()) //no connections defined
    {
        ConfigurationList connections = node->getChildren();
        for (ConfigurationList::const_iterator it =
                const_cast<ConfigurationList&>(connections).begin();
                it != const_cast<ConfigurationList&>(connections).end(); ++it)
        {
            QString name = (*it)->getAttribute("name");
            if (name.isEmpty())
            {
                getLogger().debug(QString(
                    "The connection %1 has no name, ignored")
                    .arg((*it)->getLocation()));
                continue;
            }
            getLogger().debug(QString("Creating the connection %1")
                    .arg(name));

            QIODevice *dev = v->openFile((*it)->getAttribute("src"));
            if (!dev->isOpen())
            {
                delete dev;
                getLogger().warn(QString("The connection %1 not found: %2")
                    .arg(name).arg(node->getLocation()));
                continue;
            }
            Connection *connection = ConnectionBuilder::buildFromFile(
                dev, getLogger().getChildLogger("connection-" + name));
            delete dev;

            if (connection == NULL)
            {
                getLogger().warn(QString("The connection %1 is invalid")
                    .arg(name));
                continue;
            }

            result.addConnection(name, connection);
        }
    }

    if (result.connections().count() == 0)
        getLogger().warn("No valid connections defined");
    if (cancelled)
        return false;

    return true;
}

bool ProjectBuilder::ProjectBuilderProtected::buildViews(Project &result,
    ConfigurationPtr projectDescriptor, const VFSPtr &v,
    const TemplateManager &manager,  VFSLocalizatorPtr localizator,
    ProjectBuilder &projectBuilder)
{
    ConfigurationPtr node = projectDescriptor->getChild("project-views");
    if (!node.isNull())
    {
        ConfigurationList views = node->getChildren();
        typedef LTMap<ConfigurationPtr> ConfigurationMap;
        ConfigurationMap configurations;
        int panelsTotal = 0;

        //build the list of all views and
        //count the number of panels in each of them (thus parse the views)
        ConfigurationList::const_iterator it;
        for (it = const_cast<ConfigurationList&>(views).begin();
                it != const_cast<ConfigurationList&>(views).end(); ++it)
        {
            QString name = (*it)->getAttribute("name");
            if (name.isEmpty())
            {
                getLogger().warn(QString(
                    "The view %1 has no name, ignored")
                    .arg((*it)->getLocation()));
                continue;
            }

            QString viewfilename = (*it)->getAttribute("src");
            if (viewfilename.isEmpty())
            {
                getLogger().warn(QString(
                    "The view %1 is assigned no filename, ignored")
                    .arg(name));
                continue;
            }

            QIODevice *dev = v->openFile(viewfilename);
            if (!dev->isOpen())
            {
                delete dev;
                getLogger().warn(QString( "The view %1 not found: %2 (%3)")
                    .arg(name).arg((*it)->getLocation()).arg(viewfilename));
                continue;
            }

            QString err;
            int line, col;
            ConfigurationPtr c =
                DefaultConfigurationBuilder().buildFromFile(
                    dev, err, line, col);
            delete dev;

            if (c.isNull())
            {
                getLogger().warn(QString("The view configuration %1"
                    " is invalid, possible error at line %2 col %3 (%4)")
                        .arg(name).arg(line).arg(col).arg(viewfilename));
                continue;
            }

            configurations.insert(name, c);

            //get the number of panels within this view
            node = c->getChild("panels");
            if (!node.isNull())
                panelsTotal += node->getChildren("panel").count();
            if (cancelled)
                return false;

        }

        //build the views
        vb = new ViewBuilder(panelsTotal, manager, localizator);
        connect(vb,
                SIGNAL(buildProgress(int, int)),
                &projectBuilder,
                SIGNAL(buildProgress(int, int)));
        for( unsigned it2 = 0; it2 < configurations.count(); it2++ )
        {
            getLogger().debug(QString("Creating the view %1")
                .arg(configurations.keyAt( it2 )));
            View view = vb->buildFromConfiguration(configurations[ it2 ],
                    getLogger().getChildLogger("view-" +
                        configurations.keyAt( it2 )));
            if (cancelled)
            {
                projectBuilder.disconnect(&projectBuilder);
                delete vb;
                vb = NULL;
                return false;
            }

            result.addView(configurations.keyAt( it2 ), view);
        }
        projectBuilder.disconnect(&projectBuilder);
        delete vb;
        vb = NULL;


    }
    if (result.views().count() == 0)
        getLogger().warn("No valid views defined");

    return true;
}

bool ProjectBuilder::ProjectBuilderProtected::buildBindings(Project &result,
        ConfigurationPtr projectDescriptor, const VFSPtr & v )
{
    int totalBindings = 0;
    ConfigurationPtr node = projectDescriptor->getChild("project-bindings");
    if (!node.isNull())
    {
        ConfigurationList bindings = node->getChildren();

        // for each binding file
        ConfigurationList::const_iterator it;
        for (it = const_cast<ConfigurationList&>(bindings).begin();
                it != const_cast<ConfigurationList&>(bindings).end(); ++it)
        {
            QString connection = (*it)->getAttribute("connection");
            if (connection.isEmpty())
            {
                getLogger().warn(QString(
                    "The binding %1 has no connection, ignored")
                    .arg((*it)->getLocation()));
                continue;
            }

            QString view = (*it)->getAttribute("view");
            if (view.isEmpty())
            {
                getLogger().warn(QString(
                    "The binding %1 has no view, ignored")
                    .arg((*it)->getLocation()));
                continue;
            }

            QString filename = (*it)->getAttribute("src");
            if (filename.isEmpty())
            {
                getLogger().warn(QString(
                    "The binding %1 is assigned no filename, ignored")
                    .arg((*it)->getLocation()));
                continue;
            }

            //check that project has the connection
            if( ! result.connections().contains( connection ) ) {
                getLogger().warn(QString("project does not contain "
                            "connection %1, ignoring")
                        .arg(connection));
                continue;
            }
            Connection * cc = result.connections()[ connection ];
            Q_ASSERT( cc != NULL );

            //check that project has the view
            if( ! result.views().contains( view ) ) {
                getLogger().warn(QString("project does not contain "
                            "view %1, ignoring")
                        .arg(view));
                continue;
            }
            View & vv = result.views()[ view ];

            QIODevice *dev = v->openFile(filename);
            if (!dev->isOpen())
            {
                delete dev;
                getLogger().warn(QString(
                            "The binding from connection %1 to view %2"
                            " not found: %3 (%4)")
                    .arg(connection).arg(view)
                    .arg((*it)->getLocation()).arg(filename));
                continue;
            }

            QString err;
            int line, col;
            ConfigurationPtr c =
                DefaultConfigurationBuilder().buildFromFile(
                    dev, err, line, col);
            delete dev;

            if (c.isNull())
            {
                getLogger().warn(QString("The binding configuration from "
                            "configuration %1 to view %2"
                    " is invalid, possible error at line %3 col %4 (%5)")
                        .arg(connection).arg(view)
                        .arg(line).arg(col).arg(filename));
                continue;
            }

            getLogger().debug(QString("Binding connection %1 to view %2")
                    .arg( connection ).arg( view ) );

            // parse given binging
            ConfigurationList panelNodes = c->getChildren("panel");
            int panels = 0;
            // for each panel
            for (ConfigurationList::const_iterator it =
                    const_cast<ConfigurationList&>(panelNodes).begin();
                    it != const_cast<ConfigurationList&>(panelNodes).end();
                    ++it)
            {
                QString panel = (*it)->getAttribute("id");
                if (panel.isEmpty())
                {
                    getLogger().warn(QString("binding %1 does not define "
                                "attribute id for panel nr. %2, ignoring")
                            .arg(filename).arg(panels));
                    continue;
                }

                //check that project has the panel
                if( ! vv.panels().contains(panel) ) {
                    getLogger().warn(QString("project does not contain "
                                "panel %1, ignoring")
                            .arg(panel));
                    continue;
                }
                Panel & pp = vv.panels()[ panel ];

                //parse variables
                int variables = 0;
                ConfigurationList varNodes = (*it)->getChildren("variable");
                // for each variable
                for (ConfigurationList::const_iterator it2 =
                        const_cast<ConfigurationList&>(varNodes).begin();
                        it2 != const_cast<ConfigurationList&>(varNodes).end();
                        ++it2)
                {
                    QString varname = (*it2)->getAttribute("name");
                    if (varname.isEmpty())
                    {
                        getLogger().warn(QString(
                                    "The binding %1 has no name, ignored")
                                .arg((*it2)->getLocation()));
                        continue;
                    }
                    QString tmname = (*it2)->getAttribute("template");
                    if (tmname.isEmpty())
                    {
                        getLogger().warn(QString(
                                    "The binding %1 has no template, ignored")
                                .arg((*it2)->getLocation()));
                        continue;
                    }
                    QString iopin = (*it2)->getAttribute("iopin");
                    if (iopin.isEmpty())
                    {
                        getLogger().warn(QString(
                                    "The binding %1 has no iopin, ignored")
                                .arg((*it2)->getLocation()));
                        continue;
                    }
                    if( ! pp.templates().contains( tmname ) ) {
                        getLogger().warn(QString("panel %1 does not contain "
                                    "template %2, ignoring")
                                .arg(panel).arg(tmname));
                        continue;
                    }
                    if( ! cc->variables().contains( varname ) ) {
                        getLogger().warn(QString("connection %1 "
                                    "does not contain "
                                    "variable %2, ignoring")
                                .arg(connection).arg(varname));
                        continue;
                    }
                    Variable * vv = cc->variables()[ varname ];
                    Q_ASSERT( vv != NULL );
                    Template * tt = pp.templates()[ tmname ];
                    Q_ASSERT( tt != NULL );

                    if( ! tt->iopins().contains( iopin ) ) {
                        getLogger().warn(QString("template %1 "
                                    "does not contain "
                                    "iopin %2, ignoring")
                                .arg(tmname).arg(iopin));
                        continue;
                    }
                    // do the binding
                    tt->iopins()[ iopin ]->bindToVariable( vv );
                    getLogger().debug(QString("Template %1 (iopin %2) at "
                                "panel %5 has been bound to "
                                "connection %3 (variable %4)")
                            .arg( tmname )
                            .arg( iopin )
                            .arg( connection )
                            .arg( varname )
                            .arg( panel ) );

                    variables++;
                }
                if( variables != 0 )
                    panels++;
            }

            // consider binding to be valid only when one or more panels has
            // been bound
            if( panels != 0 )
                totalBindings++;
        }
    }
    if (totalBindings == 0)
        getLogger().warn("No valid bindings defined");

    return true;
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectBuilderProtected.cpp,v 1.6 2004/10/14 10:17:00 modesto Exp $
