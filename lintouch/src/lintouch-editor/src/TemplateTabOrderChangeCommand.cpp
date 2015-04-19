// $Id: TemplateTabOrderChangeCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateTabOrderChangeCommand.cpp -- 
// AUTHOR: abdul <henry@www.swac.cz>
//   DATE: 24th August 2004
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

#include <lt/project/Project.hh>
#include <lt/project/Panel.hh>
#include <lt/templates/Template.hh>

#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "TemplateTabOrderChangeCommand.hh"

using namespace lt;

struct TemplateTabOrderChangeCommand::
        TemplateTabOrderChangeCommandPrivate {
    /**
     * This holds the old position of template
     */
    int oldPosition;

    /**
     * This holds the new position of template
     */
    int newPosition;

    /**
     * This holds the name of template at new pos
     */
    QString oldTemplate;

    /**
     * This holds the name of template at old pos
     */
    QString newTemplate;

    /**
     * View name.
     */
    QString viewName;

    /**
     * Panel name.
     */
    QString panelName;

    /**
    * This keeps track whether the operation carried out is 
    * move up or move down the hierarchy line 
    */
     bool flagMoveUp;

    /**
     * TODO
     */
    bool flagRedo;

};

TemplateTabOrderChangeCommand::
        TemplateTabOrderChangeCommand (
                QString oldTemplate, QString newTemplate,
                ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw,
            qApp->translate("TemplateTabOrderChangeCommand",
                "Change Template Taborder") ),
    d(new TemplateTabOrderChangeCommandPrivate )
{
    Q_CHECK_PTR( d );
    Q_ASSERT( pw );

    d->oldTemplate = oldTemplate;
    d->newTemplate = newTemplate;
    d->viewName = m_pw->currentView();
    d->panelName = m_pw->currentPanel();

    const View& v = pw->project()->views()[d->viewName];

    const Panel& p = v.panels()[ d->panelName ];

    d->oldPosition = p.templates().indexOf( d->newTemplate );
    Q_ASSERT( d->oldPosition != -1 );
    d->newPosition = p.templates().indexOf( d->oldTemplate );
    Q_ASSERT( d->newPosition != -1 );

    if( d->oldPosition > d->newPosition ) {
        d->flagMoveUp = true;
    } else {
        d->flagMoveUp = false;
    }

    d->flagRedo = true;
}

TemplateTabOrderChangeCommand::
        ~TemplateTabOrderChangeCommand()
{
    delete d;
}

CommandType TemplateTabOrderChangeCommand::rtti() const
{
    return LTE_CMD_TEMPLATE_TABORDER_CHANGE;
}

void TemplateTabOrderChangeCommand::redo()
{
    d->flagRedo = true;

    Q_ASSERT( d );

    View& v = m_pw->project()->views()[d->viewName];

    Panel& p = v.panels()[ d->panelName ];

    Q_ASSERT( d->newTemplate == p.templates().keyAt( d->oldPosition ) );
    Q_ASSERT( d->oldTemplate == p.templates().keyAt( d->newPosition ) );

    if( d->flagMoveUp ) {
        p.templates().move( d->newTemplate , d->newPosition );
    } else {
        p.templates().move( d->oldTemplate , d->oldPosition );
    }

    notifyMainWindow( LTE_CMD_TEMPLATE_TABORDER_CHANGE );
}

void TemplateTabOrderChangeCommand::undo()
{

    d->flagRedo = false;

    Q_ASSERT( d );

    View& v = m_pw->project()->views()[d->viewName];

    Panel& p = v.panels()[ d->panelName ];

    Q_ASSERT( d->oldTemplate == p.templates().keyAt( d->oldPosition ) );
    Q_ASSERT( d->newTemplate == p.templates().keyAt( d->newPosition ) );

    if( d->flagMoveUp ) {
        p.templates().move( d->oldTemplate , d->newPosition );
    } else {
        p.templates().move( d->newTemplate , d->oldPosition );
    }

    notifyMainWindow( LTE_CMD_TEMPLATE_TABORDER_CHANGE );
}

QString TemplateTabOrderChangeCommand::viewName()
{
    Q_ASSERT( d );
    return d->viewName;
}

QString TemplateTabOrderChangeCommand::panelName()
{
    Q_ASSERT( d );
    return d->panelName;
}

QString TemplateTabOrderChangeCommand::newTemplate()
{
    Q_ASSERT( d );
    return d->newTemplate;
}

QString TemplateTabOrderChangeCommand::oldTemplate()
{
    Q_ASSERT( d );
    return d->oldTemplate;
}

int TemplateTabOrderChangeCommand::newPos()
{
    Q_ASSERT( d );

    if( d->flagRedo ) { 
        return d->newPosition;
    } else {
        return d->oldPosition;
    }
}

int TemplateTabOrderChangeCommand::oldPos()
{
    Q_ASSERT( d );

    if( d->flagRedo ) {
        return d->oldPosition;
    } else {
        return d->newPosition;
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateTabOrderChangeCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
