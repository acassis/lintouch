// $Id: ProjectBuilderProtected.hh,v 1.3 2004/09/14 16:23:26 mman Exp $
//
//   FILE: ProjectBuilderProtected.hh --
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

#ifndef _PROJECTBUILDERPROTECTED_HH
#define _PROJECTBUILDERPROTECTED_HH

#include <lt/logger/Logger.hh>
#include <lt/configuration/Configuration.hh>

#include <lt/templates/TemplateManager.hh>
#include <lt/templates/VFSLocalizator.hh>
#include <lt/vfs/VFS.hh>

#include "lt/project/Project.hh"
#include "lt/project/ProjectBuilder.hh"

namespace lt
{
    class ViewBuilder;

    struct ProjectBuilder::ProjectBuilderProtected
        : public LogEnabled
    {
        /**
        * True if the building was cancelled.
        */
        bool cancelled;

        /**
        * The current selected locale.
        */
        QString locale;

        /**
        * This instance is valid and non-NULL only when views are being
        * created.
        */
        ViewBuilder *vb;

        /**
        * Look for the descriptor in the VFS and parse it. Return a null
        * ptr on failure.
        */
        ConfigurationPtr buildDescriptor(const VFSPtr &v);

        /**
        * Load the info from the descriptor and add it to the project. Return
        * success of the operation.
        */
        bool buildInfo(Project &result, ConfigurationPtr projectDescriptor);

        /**
        * Load and build the project connections of the project. Return
        * success of the operation.
        */
        bool buildConnections(Project &result,
            ConfigurationPtr projectDescriptor, const VFSPtr &v);

        /**
        * Load, build, and instantiate views, panels, and templates. Use
        * the passed template manager. Return success of the operation.
        *
        * The projectBuilder is used in signaling a termination in progress;
        * this hack helps from using the class ProjectBuilderPrivate as
        * a QObject descendant - that would require putting the class
        * in another file.
        */
        bool buildViews(Project &result, ConfigurationPtr projectDescriptor,
            const VFSPtr &v, const TemplateManager &manager,
            VFSLocalizatorPtr localizator,
            ProjectBuilder &projectBuilder);

        /**
        * Bind templates to the variables.
        * Return success of the operation.
        */
        bool buildBindings(Project &result,
                ConfigurationPtr projectDescriptor, const VFSPtr &v );
    };
}

#endif /* _PROJECTBUILDERPROTECTED_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectBuilderProtected.hh,v 1.3 2004/09/14 16:23:26 mman Exp $
