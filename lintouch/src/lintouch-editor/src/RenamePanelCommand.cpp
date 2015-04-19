// $Id: RenamePanelCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenamePanelCommand.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 26 August 2004
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

#include "Bindings.hh"
#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "RenamePanelCommand.hh"

using namespace lt;

struct RenamePanelCommand::RenamePanelCommandPrivate {

    /**
     * Updates link in given View acording the new/old names.
     */
    void updateLinks( View v, const QString& oldName, const QString& newName )
    {
        bool change;

        const PanelMap & pm = v.panels();
        for( unsigned pi = 0; pi < pm.count(); pi++ ) {

            TemplateDict templates = pm[ pi ].templates();
            for( unsigned int i = 0; i < templates.count(); ++i ) {
                change=false;

                LTMap<PropertyDict> pg = templates[i]->propertyGroups();
                for( unsigned int j = 0; j < pg.count(); ++j ) {

                    PropertyDict pd = pg[j];
                    for( unsigned int k = 0; k < pd.count(); ++k ) {

                        if( pd[k]->type() == Property::LinkType ) {

                            // if( redo ) {
                            if( pd[k]->encodeValue() == oldName ) {
                                pd[k]->decodeValue(newName);
                                change=true;
                            }
                            // } else {
                            //     if( pd[k]->encodeValue() == newPanelName ) {
                            //         pd[k]->decodeValue(oldPanelName);
                            //         change=true;
                            //     }
                            // }
                        }
                    }
                }

                if( change ) {
                    templates[i]->propertiesChanged();
                }
            }
        }
    }

    /**
     * TODO
     */
    QString oldPanelName;

    /**
     * TODO
     */
    QString newPanelName;

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

RenamePanelCommand::RenamePanelCommand(
        const QString oldPanelName,
        const QString newPanelName,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("RenamePanelCommand", "Rename Panel") ),
    d(new RenamePanelCommandPrivate)
{
    Q_CHECK_PTR(d);
    d->oldPanelName = oldPanelName;
    d->newPanelName = newPanelName;
    d->viewName = m_pw->currentView();

    d->position =
        m_pw->project()->views()[d->viewName].panels().indexOf(d->oldPanelName);
}

RenamePanelCommand::~RenamePanelCommand()
{
    delete d;
}

CommandType RenamePanelCommand::rtti() const
{
    return LTE_CMD_PANEL_RENAME;
}

void RenamePanelCommand::redo()
{
    Q_ASSERT(d);

    d->redo = true;

    renamePanel(d->oldPanelName, d->newPanelName);

    // inform the MW
    notifyMainWindow( LTE_CMD_PANEL_RENAME );
}

void RenamePanelCommand::undo()
{
    Q_ASSERT(d);

    d->redo = false;

    renamePanel(d->newPanelName, d->oldPanelName);

    // inform the MW
    notifyMainWindow( LTE_CMD_PANEL_RENAME );
}

void RenamePanelCommand::renamePanel(
        const QString& oldName, const QString& newName)
{
    Q_ASSERT(d);

    View& v = m_pw->project()->views()[d->viewName];

    PanelMap& pm = v.panels();
    Q_ASSERT( pm.contains(oldName) );

    // backup the panel
    Panel panel( pm[oldName] );

    // remove the panel from View
    v.removePanel(oldName);

    // restore the panel with new name
    v.addPanel( newName, panel );

    // restore position
    pm.move( newName, d->position );
    Q_ASSERT( pm.keyAt(d->position) == newName );

    // updates link in given View acording the new/old names
    d->updateLinks(v, oldName, newName);

    // rename bindings
    m_pw->project()->bindings().renamePanel(d->viewName, oldName, newName);
}

QString RenamePanelCommand::oldPanelName() const
{
    Q_ASSERT(d);
    if( d->redo == true ) {
        return d->oldPanelName;
    } else {
        return d->newPanelName;
    }
}

QString RenamePanelCommand::newPanelName() const
{
    Q_ASSERT(d);
    if( d->redo == false ) {
        return d->oldPanelName;
    } else {
        return d->newPanelName;
    }
}

QString RenamePanelCommand::viewName() const
{
    Q_ASSERT(d);
    return d->viewName;
}

int RenamePanelCommand::position() const
{
    Q_ASSERT(d);
    return d->position;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenamePanelCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
