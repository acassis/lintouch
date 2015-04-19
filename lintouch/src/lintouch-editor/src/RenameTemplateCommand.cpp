// $Id: RenameTemplateCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenameTemplateCommand.cpp -- 
// AUTHOR: abdul<henry@swac.cz>
//   DATE: 8th September 2004
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
#include <qmap.h>
#include <qdict.h>

#include <lt/templates/Info.hh>
#include <lt/templates/Property.hh>
#include <lt/templates/Template.hh>
#include <lt/project/Project.hh>
#include <lt/project/View.hh>

#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "RenameTemplateCommand.hh"

using namespace lt;

struct RenameTemplateCommand::RenameTemplateCommandPrivate {

    /**
     * TODO
     */
    QString oldTemplateName;

    /**
     * TODO
     */
    QString newTemplateName;

    /**
     * TODO
     */
    QString panelName;

    /**
     * TODO
     */
    QString viewName;

    /**
     * TODO
     */
    int position;

    /**
     * True on redo action, false on undo action.
     */
    bool redo;
};

RenameTemplateCommand::RenameTemplateCommand(
        const QString oldTemplateName,
        const QString newTemplateName,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("RenameTemplateCommand",
                "Rename Template") ),
    d(new RenameTemplateCommandPrivate)
{
    Q_ASSERT(oldTemplateName != newTemplateName);

    Q_CHECK_PTR(d);
    d->oldTemplateName = oldTemplateName;
    d->newTemplateName =  newTemplateName;
    d->viewName = m_pw->currentView();
    d->panelName = m_pw->currentPanel();

    const View& v = m_pw->project()->views()[d->viewName];

    const Panel& p = v.panels()[ d->panelName ];

    d->position = p.templates().indexOf( oldTemplateName );

    Q_ASSERT( d->position != -1 );
}

RenameTemplateCommand::~RenameTemplateCommand()
{
    delete d;
}

CommandType RenameTemplateCommand::rtti() const
{
    return LTE_CMD_TEMPLATE_RENAME;
}

void RenameTemplateCommand::redo()
{
    Q_ASSERT( d );

    d->redo = true;

    renameTemplate( d->oldTemplateName, d->newTemplateName);

    // inform the MW
    notifyMainWindow( LTE_CMD_TEMPLATE_RENAME );
}

void RenameTemplateCommand::undo()
{
    Q_ASSERT(d);

    d->redo = false;

    renameTemplate( d->newTemplateName, d->oldTemplateName);

    // inform the MW
    notifyMainWindow( LTE_CMD_TEMPLATE_RENAME );

}

void RenameTemplateCommand::renameTemplate(
        const QString& oldName, const QString& newName)
{
    View& v = m_pw->project()->views()[d->viewName];

    Panel& p = v.panels()[ d->panelName ];

    TemplateDict& td = p.templates();
    Q_ASSERT( td.contains( oldName ) );

    int position = td.indexOf( oldName );
    Q_ASSERT( position == d->position );

    // The template whose name is changed
    Template* tmp = td[ d->position ];

    td.remove( position );
    td.insert( newName, tmp, position );

    Q_ASSERT( td.keyAt( d->position ) == newName );

    // rename bindings
    m_pw->project()->bindings().renameTemplate(d->viewName, d->panelName,
            oldName, newName);
}

QString RenameTemplateCommand::oldTemplateName() const
{
    Q_ASSERT(d);
    if( d->redo == true ) {
        return d->oldTemplateName;
    } else {
        return d->newTemplateName;
    }
}

QString RenameTemplateCommand::newTemplateName() const
{
    Q_ASSERT(d);
    if( d->redo == false ) {
        return d->oldTemplateName;
    } else {
        return d->newTemplateName;
    }
}

QString RenameTemplateCommand::viewName() const
{
    Q_ASSERT(d);
    return d->viewName;
}

QString RenameTemplateCommand::panelName() const
{
    Q_ASSERT(d);
    return d->panelName;
}

int RenameTemplateCommand::position() const
{
    Q_ASSERT(d);
    return d->position;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenameTemplateCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
