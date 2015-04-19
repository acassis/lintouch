// $Id: ProjectBuilder.cpp,v 1.45 2004/09/14 16:23:26 mman Exp $
//
//   FILE: ProjectBuilder.cpp --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 11 November 2003
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

#include <lt/configuration/Configuration.hh>

#include <lt/templates/TemplateManager.hh>
#include <lt/templates/VFSLocalizator.hh>
#include <lt/vfs/VFSZIP.hh>

#include "lt/project/ProjectBuilderProtected.hh"
#include "lt/project/ResourceManager.hh"
#include "lt/project/TemplateManagerBuilder.hh"
#include "lt/project/ViewBuilder.hh"

using namespace lt;

ProjectBuilder::ProjectBuilder(LoggerPtr logger /* = Logger::nullPtr() */,
    QString locale /* = QString::null */)
    : d(new ProjectBuilderProtected)
{
    enableLogging(logger);
    d->cancelled = false;
    d->vb = NULL;
    d->locale = locale;
    d->enableLogging(logger);
}

ProjectBuilder::~ProjectBuilder()
{
    delete d;
}

void ProjectBuilder::cancel()
{
    if (d->vb)
        d->vb->cancel();
    d->cancelled = true;
}

Project ProjectBuilder::buildRuntimeProject(const QUrl& location)
{
    d->cancelled = false;
    Project result;

    VFSPtr v(new VFSZIP(location, true, getLoggerPtr()));
    if (!v->isValid())
        return Project();

    ConfigurationPtr projectDescriptor = d->buildDescriptor(v);
    if (projectDescriptor.isNull())
        return Project();

    if (!d->buildInfo(result, projectDescriptor))
        return Project();

    // build connections
    if (!d->buildConnections(result, projectDescriptor, v))
        return Project();

    //build TM
    TemplateManager manager = TemplateManagerBuilder(
            getLogger().getChildLogger("TemplateManager")).buildFromNames(
            projectDescriptor->getChild("project-template-libraries"));
    if (d->cancelled)
        return Project();
    if (manager.templateLibraries().count() == 0)
        getLogger().warn("No valid templates defined");
    result.setTemplateManager( manager );

    //build the resource manager
    VFSPtr resVFS(VFSPtr(v->subVFS("resources")));
    if (!resVFS->isValid())
    {
        getLogger().warn("No resources in the project");
    }
    else
    {
        result.setResourceManager(ResourceManagerPtr(
            new ResourceManager(resVFS)));
    }

    //build localizator
    VFSLocalizatorPtr localizator(new VFSLocalizator(resVFS, d->locale));
    result.setLocalizator(localizator);
    if (d->cancelled)
        return Project();

    // build views
    if (!d->buildViews(result, projectDescriptor, v, manager,
        localizator, *this))
        return Project();

    // bind views to connections
    if (!d->buildBindings(result, projectDescriptor, v ))
        return Project();

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectBuilder.cpp,v 1.45 2004/09/14 16:23:26 mman Exp $
