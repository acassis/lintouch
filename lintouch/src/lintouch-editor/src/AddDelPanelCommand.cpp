// $Id: AddDelPanelCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddDelPanelCommand.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 25 August 2004
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

#include <lt/project/Panel.hh>
#include <lt/project/Project.hh>

#include "BindingsHelper.hh"
#include "Bindings.hh"
#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "AddDelPanelCommand.hh"

using namespace lt;

struct AddDelPanelCommand::AddDelPanelCommandPrivate {

    AddDelPanelCommandPrivate(): position(-1),del(false) {}

    /**
     * Updates link in given Panel according the src/dst names.
     */
    void updateLinks( Panel& p )
    {
        bool change;

        TemplateDict templates = p.templates();
        for( unsigned int i = 0; i < templates.count(); ++i ) {
            change=false;

            LTMap<PropertyDict> pg = templates[i]->propertyGroups();
            for( unsigned int j = 0; j < pg.count(); ++j ) {

                PropertyDict pd = pg[j];
                for( unsigned int k = 0; k < pd.count(); ++k ) {

                    if( pd[k]->type() == Property::LinkType ) {

                        if( pd[k]->encodeValue() == srcPanelName ) {
                            change=true;
                            pd[k]->decodeValue( dstPanelName );
                        }
                    }
                }
            }

            if( change ) {
                templates[i]->propertiesChanged();
            }
        }
    }

    /**
     * View name into that we add/duplicate/delete panel.
     */
    QString viewName;

    /**
     * Name of the panel that has been added/duplicated/deleted.
     */
    QString dstPanelName;

    /**
     * Name of the panel that is the source for duplicate.
     */
    QString srcPanelName;

    /**
     * Position of the panel in PanelMap.
     */
    int position;

    /**
     * Delete flag. True if the is a delete command.
     */
    bool del;

    /**
     * Holds deleted Panel.
     */
    Panel panel;

    /**
     * Holds Bindings saved from deleted panel
     */
    BindingsList bl;
};


AddDelPanelCommand::AddDelPanelCommand(
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("AddDelPanelCommand", "Add Panel") ),
    d(new AddDelPanelCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->viewName = pw->currentView();
    d->dstPanelName = Command::generateSafeName(
            qApp->translate("AddDelPanelCommand", "Panel %1" ),
            pw->numPanels,
            pw->project()->views()[d->viewName].panels().mapping() );
    pw->numPanels ++;
}

AddDelPanelCommand::AddDelPanelCommand(
        const QString& panelName,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("AddDelPanelCommand","Duplicate Panel") ),
    d(new AddDelPanelCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->viewName = pw->currentView();
    const View& v = pw->project()->views()[d->viewName];

    d->srcPanelName = panelName;
    d->position = v.panels().indexOf(panelName) + 1;
    d->dstPanelName = Command::generateSafeName(
            panelName + " " + qApp->translate("AddDelPanelCommand", "copy %1"),
            1, v.panels().mapping() );

    // save bindings to temporary list
    BindingsList bl = m_pw->project()->bindings().bindingsByPanel(
            d->viewName, d->srcPanelName);

    // update binding to the new panal name
    for(BindingsList::iterator it = bl.begin(); it != bl.end(); ++it) {
        Q_ASSERT((*it)->panel() == d->srcPanelName);
        // create new Binding class from old one but with
        // new/dstPanelName name
        d->bl.append(BindingPtr(new Binding(
                        (*it)->connection(),
                        (*it)->variable(),
                        (*it)->view(),
                        d->dstPanelName,
                        (*it)->templ(),
                        (*it)->iopin())));
    }

    // make a copy of the panel
    d->panel = v.panels()[d->srcPanelName].clone(
            m_pw->project()->localizator(),
            m_pw->project()->templateManager() );
}

AddDelPanelCommand::AddDelPanelCommand(
        const QString& panelName, bool /*del*/,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("AddDelPanelCommand", "Delete Panel") ),
    d(new AddDelPanelCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->viewName = pw->currentView();
    d->dstPanelName = panelName;
    d->del = true;

    const View& v = pw->project()->views()[d->viewName];

    // save the panel
    d->panel = v.panels()[d->dstPanelName];

    // store old position of deleted panel
    d->position = v.panels().indexOf(d->dstPanelName);

    // save bindings
    d->bl = BindingsHelper::saveTemplatesBindingAndUnbind(m_pw->project(),
            d->viewName, d->dstPanelName, d->panel.templates());
}

AddDelPanelCommand::~AddDelPanelCommand()
{
    delete d;
}

CommandType AddDelPanelCommand::rtti() const
{
    return LTE_CMD_PANEL_ADD_DEL;
}

void AddDelPanelCommand::redo()
{
    Q_ASSERT(d);

    d->bl.clear();

    View& v = m_pw->project()->views()[d->viewName];
    PanelMap& pm = v.panels();

    CommandType t;

    if( d->del ) {
        //delete

        // remove it from Project
        v.removePanel( d->dstPanelName );

        t = LTE_CMD_PANEL_DELETE;
    } else {
        // add/duplicate

        if( !d->srcPanelName.isNull() ) {
            // duplicate
            Q_ASSERT( pm.contains(d->srcPanelName) );

            // add prepared panel to the view
            v.addPanel( d->dstPanelName, d->panel );

            // move the panel just after the source panel
            pm.move( d->dstPanelName, d->position );

            // update all links!
            d->updateLinks( d->panel );

            // update bindings of the new panel
            BindingsHelper::restoreBindingsAndBind(m_pw->project(), d->bl);

        } else {
            // add
            Q_ASSERT( !d->dstPanelName.isNull() );
            Q_ASSERT( !pm.contains(d->dstPanelName) );

            // add the panel to current view
            v.addPanel( d->dstPanelName, d->panel );

            // get position of the added panel
            d->position = pm.indexOf(d->dstPanelName);
        }
        t = LTE_CMD_PANEL_ADD;
    }
    // notify all
    notifyMainWindow( t );
}

void AddDelPanelCommand::undo()
{
    Q_ASSERT(d);

    View& v = m_pw->project()->views()[d->viewName];
    PanelMap& pm = v.panels();

    CommandType t;

    if( d->del ) {
        //undelete
        Q_ASSERT( !pm.contains(d->dstPanelName) );

        // add deleted panel
        v.addPanel( d->dstPanelName, d->panel );

        // move the panel to it's old position
        pm.move( d->dstPanelName, d->position );

        // restore bindings
        BindingsHelper::restoreBindingsAndBind(m_pw->project(), d->bl);

        // notify all
        t = LTE_CMD_PANEL_ADD;
    } else {
        // unadd/unduplicate
        Q_ASSERT( pm.contains(d->dstPanelName) );

        // unbind and forget
        BindingsHelper::saveTemplatesBindingAndUnbind(m_pw->project(),
                d->viewName, d->dstPanelName, pm[d->dstPanelName].templates());

        // remove the panel
        // d->panel = v.panels()[ d->dstPanelName ];
        v.removePanel( d->dstPanelName );

        t = LTE_CMD_PANEL_DELETE;
    }
    // notify all
    notifyMainWindow( t );
}

QString AddDelPanelCommand::viewName() const
{
    Q_ASSERT(d);
    return d->viewName;
}

QString AddDelPanelCommand::panelName() const
{
    Q_ASSERT(d);
    return d->dstPanelName;
}

int AddDelPanelCommand::position() const
{
    Q_ASSERT(d);
    return d->position;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelPanelCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
