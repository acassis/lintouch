// $Id: ProjectPropsChangeCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectPropsChangeCommand.cpp -- 
// AUTHOR: abdul <henry@swac.cz>
//   DATE: 10 August 2004
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
#include <qdatetime.h>

#include <lt/project/Project.hh>

#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "ProjectPropsChangeCommand.hh"

using namespace lt;

struct ProjectPropsChangeCommand::
        ProjectPropsChangeCommandPrivate {

    /**
     * This stores the old project info
     */
    Info oldInfo;

    /**
     * This stores the new project info
     */
    Info newInfo;

};

ProjectPropsChangeCommand::ProjectPropsChangeCommand(
        const Info oldInfo, const Info newInfo,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("ProjectPropsChangeCommand",
                "Change Project Properties") ),
    d(new ProjectPropsChangeCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->oldInfo = oldInfo;
    d->newInfo = newInfo;
}

ProjectPropsChangeCommand::~ProjectPropsChangeCommand()
{
    delete d;
}

CommandType ProjectPropsChangeCommand::rtti() const
{
    return LTE_CMD_PROJECT_PROPS_CHANGE;
}

void ProjectPropsChangeCommand::redo()
{
    Q_ASSERT(d);

    Info projectInfo = m_pw->project()->info();

    Q_ASSERT( projectInfo.author() == d->oldInfo.author() );
    Q_ASSERT( projectInfo.version() == d->oldInfo.version() );
    Q_ASSERT( projectInfo.date() == d->oldInfo.date() );
    Q_ASSERT( projectInfo.shortDescription() == 
            d->oldInfo.shortDescription() );
    Q_ASSERT( projectInfo.longDescription() == 
            d->oldInfo.longDescription() );

    m_pw->project()->setInfo( d->newInfo );

    notifyMainWindow( LTE_CMD_PROJECT_PROPS_CHANGE );
}

void ProjectPropsChangeCommand::undo()
{
    Q_ASSERT(d);

    Info projectInfo = m_pw->project()->info();

    Q_ASSERT( projectInfo.author() == d->newInfo.author() );
    Q_ASSERT( projectInfo.version() == d->newInfo.version() );
    Q_ASSERT( projectInfo.date() == d->newInfo.date() );
    Q_ASSERT( projectInfo.shortDescription() == 
            d->newInfo.shortDescription() );
    Q_ASSERT( projectInfo.longDescription() == 
            d->newInfo.longDescription() );

    m_pw->project()->setInfo( d->oldInfo );

    notifyMainWindow( LTE_CMD_PROJECT_PROPS_CHANGE );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPropsChangeCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
