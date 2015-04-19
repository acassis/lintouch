// $Id: PanelOrderChangeCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: PanelOrderChangeCommand.cpp -- 
// AUTHOR: abdul<henry@swac.cz>
//   DATE: 2nd September 2004
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

#include <lt/project/Project.hh>
#include <lt/project/Panel.hh>

#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "PanelOrderChangeCommand.hh"

using namespace lt;

struct PanelOrderChangeCommand::
        PanelOrderChangeCommandPrivate {
    /**
     * This holds the old position of panel
     */
    int oldPosition;

    /**
     * This holds the new position of panel
     */
    int newPosition;

    /**
     * This holds the old name of panel
     */
    QString oldPanel;

    /**
     * This holds the new name of panel
     */
    QString newPanel;

    /**
     * View name into that we change the position of panel.
     */
    QString viewName;

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

PanelOrderChangeCommand::
        PanelOrderChangeCommand (
                QString oldPanel, QString newPanel,
                ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("PanelOrderChangeCommand",
                "Change Panel Position") ),
    d(new PanelOrderChangeCommandPrivate )
{
    Q_CHECK_PTR( d );
    Q_ASSERT( pw );

    d->oldPanel = oldPanel;
    d->newPanel = newPanel;
    d->viewName = pw->currentView();
    const View& v = pw->project()->views()[d->viewName];

    const PanelMap& pm = v.panels();

    d->oldPosition = pm.indexOf( d->newPanel );
    Q_ASSERT( d->oldPosition != -1 );
    d->newPosition = pm.indexOf( d->oldPanel );
    Q_ASSERT( d->newPosition != -1 );

    if( d->oldPosition > d->newPosition ) {
        d->flagMoveUp = true;
    } else {
        d->flagMoveUp = false;
    }

    d->flagRedo = true;    
}

PanelOrderChangeCommand::
        ~PanelOrderChangeCommand()
{
    delete d;
}

CommandType PanelOrderChangeCommand::rtti() const
{
    return LTE_CMD_PANEL_ORDER_CHANGE;
}

void PanelOrderChangeCommand::redo()
{
    d->flagRedo = true;

    Q_ASSERT( d );

    View& v = m_pw->project()->views()[d->viewName];

    PanelMap& pm = v.panels();

    Q_ASSERT( d->newPanel == pm.keyAt( d->oldPosition ) );
    Q_ASSERT( d->oldPanel == pm.keyAt( d->newPosition ) );

    if( d->flagMoveUp ) {
        pm.move( d->newPanel , d->newPosition );
    } else {
        pm.move( d->oldPanel , d->oldPosition );
    }

    notifyMainWindow( LTE_CMD_PANEL_ORDER_CHANGE );
}

void PanelOrderChangeCommand::undo()
{

    d->flagRedo = false;

    Q_ASSERT( d );

    View& v = m_pw->project()->views()[d->viewName];

    PanelMap& pm = v.panels();

    Q_ASSERT( d->oldPanel == pm.keyAt( d->oldPosition ) );
    Q_ASSERT( d->newPanel == pm.keyAt( d->newPosition ) );

    if( d->flagMoveUp ) {
        pm.move( d->oldPanel , d->newPosition );
    } else {
        pm.move( d->newPanel , d->oldPosition );
    }

    notifyMainWindow( LTE_CMD_PANEL_ORDER_CHANGE );
}

QString PanelOrderChangeCommand::viewName()
{
    Q_ASSERT( d );
    return d->viewName;
}

QString PanelOrderChangeCommand::newPanel()
{
    Q_ASSERT( d );
    return d->newPanel;
}

QString PanelOrderChangeCommand::oldPanel()
{
    Q_ASSERT( d );
    return d->oldPanel;
}
                                                                                                                            
int PanelOrderChangeCommand::newPos()
{
    Q_ASSERT( d );

    if( d->flagRedo ) { 
        return d->newPosition;
    } else {
        return d->oldPosition;
    }
}
                                                                                                                             
int PanelOrderChangeCommand::oldPos()
{
    Q_ASSERT( d );

    if( d->flagRedo ) {
        return d->oldPosition;
    } else {
        return d->newPosition;
    }
}


// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelOrderChangeCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
