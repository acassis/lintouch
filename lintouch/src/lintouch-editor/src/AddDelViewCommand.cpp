// $Id: AddDelViewCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddDelViewCommand.cpp -- 
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 27 July 2004
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
#include <lt/project/View.hh>

#include "BindingsHelper.hh"
#include "Bindings.hh"
#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "AddDelViewCommand.hh"

using namespace lt;

struct AddDelViewCommand::AddDelViewCommandPrivate {

    /**
     * ctor
     */
    AddDelViewCommandPrivate() : position(-1),del(false) {}

    /**
     * The newly created/duplicated view name
     */
    QString dstViewName;

    /**
     * The source view for duplication.
     */
    QString srcViewName;

    /**
     * Position of deleted item, -1 otherwise.
     */
    int position;

    /**
     * Deleted view.
     */
    View view;

    /**
     * true if this is delete command
     */
    bool del;

    /**
     * Holds Bindings saved from deleted view
     */
    BindingsList bl;
};

AddDelViewCommand::AddDelViewCommand(
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("AddDelViewCommand", "Add View") ),
    d(new AddDelViewCommandPrivate)
{
    Q_CHECK_PTR(d);
    d->dstViewName = Command::generateSafeName(
            qApp->translate("AddDelViewCommand", "View %1"),
            pw->numViews,
            pw->project()->views().mapping());
    pw->numViews++;

    // set View size from defaults
    d->view = m_mw->createDefaultView();
}

AddDelViewCommand::AddDelViewCommand(
        const QString& viewName, ProjectWindow* pw,
        MainWindow* mw ) :
    Command( pw, mw, qApp->translate("AddDelViewCommand", "Duplicate View") ),
    d(new AddDelViewCommandPrivate)
{
    Q_ASSERT(d);
    d->srcViewName = viewName;
    d->position = m_pw->project()->views().indexOf(viewName) + 1;
    d->dstViewName = Command::generateSafeName(
            viewName + " " + qApp->translate("AddDelViewCommand", "copy %1"), 1,
            pw->project()->views().mapping());

    EditorProject* ep = m_pw->project();
    const ViewMap& vm = ep->views();

    // check that we have the view
    Q_ASSERT( vm.contains(d->srcViewName) );

    // add new view to the project
    d->view = vm[d->srcViewName].clone(
            ep->localizator(),
            ep->templateManager() );

    // save bindings to temporary list
    BindingsList bl;
    PanelMap pm = d->view.panels();
    for(unsigned i = 0; i < pm.count(); ++i) {
        bl += ep->bindings().bindingsByPanel(
                d->srcViewName, pm.keyAt(i));
    }

    // update binding to the new view name
    for(BindingsList::iterator it = bl.begin(); it != bl.end(); ++it) {
        Q_ASSERT((*it)->view() == d->srcViewName);
        // create new Binding class from old one but with
        // new/dstPanelName name
        d->bl.append(BindingPtr(new Binding(
                        (*it)->connection(),
                        (*it)->variable(),
                        d->dstViewName,
                        (*it)->panel(),
                        (*it)->templ(),
                        (*it)->iopin())));
    }
}

AddDelViewCommand::AddDelViewCommand(
        const QString& viewName, bool /*del*/,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("AddDelViewCommand", "Delete View") ),
    d(new AddDelViewCommandPrivate)
{
    Q_ASSERT(d);
    d->dstViewName = viewName;
    d->position = m_pw->project()->views().indexOf(viewName);
    d->del = true;

    EditorProject* ep = m_pw->project();
    const ViewMap& vm = ep->views();

    // backup the view
    d->view = vm[d->dstViewName];

    // save bindings
    PanelMap pm = d->view.panels();
    for(unsigned i = 0; i < pm.count(); ++i) {
        d->bl += BindingsHelper::saveTemplatesBindingAndUnbind(
                ep, d->dstViewName, pm.keyAt(i), pm[i].templates());
    }
}

AddDelViewCommand::~AddDelViewCommand()
{
    delete d;
}

CommandType AddDelViewCommand::rtti() const
{
    return LTE_CMD_VIEW_ADD_DEL;
}

void AddDelViewCommand::redo()
{
    Q_ASSERT(d);

    d->bl.clear();

    // prepare often used vars
    EditorProject* ep = m_pw->project();
    ViewMap& vm = ep->views();

    if( d->del ) {
        //delete

        // remove the view from Project
        ep->removeView(d->dstViewName);

        notifyMainWindow( LTE_CMD_VIEW_DELETE );
    } else {
        // add or duplicate

        if( !d->srcViewName.isNull() ) {
            // let's duplicate

            ep->addView( d->dstViewName, d->view );

            // move the view just after the duplicated/source view
            vm.move( d->dstViewName, d->position );

            // update bindings of the new panel
            BindingsHelper::restoreBindingsAndBind(ep, d->bl);

        } else {

            ep->addView( d->dstViewName, d->view );

            // get position of the added view
            d->position = vm.indexOf(d->dstViewName);
        }

        notifyMainWindow( LTE_CMD_VIEW_ADD );
    }
}

void AddDelViewCommand::undo()
{
    Q_ASSERT(d);

    // prepare often used vars
    EditorProject* ep = m_pw->project();
    ViewMap& vm = ep->views();

    if( d->del ) {
        //undelete

        // make sure the deleted name is not there yet
        Q_ASSERT( !vm.contains(d->dstViewName) );

        // add the deleted view back
        ep->addView( d->dstViewName, d->view );

        // move the view to its old position
        vm.move( d->dstViewName, d->position );

        // update bindings of the new panel
        BindingsHelper::restoreBindingsAndBind(ep, d->bl);

        notifyMainWindow( LTE_CMD_VIEW_ADD );
    } else {
        //unadd or unduplicate

        // unbind and forget
        PanelMap pm = d->view.panels();
        for(unsigned i = 0; i < pm.count(); ++i) {
            BindingsHelper::saveTemplatesBindingAndUnbind(
                    ep,
                    d->dstViewName, pm.keyAt(i),
                    pm[i].templates());
        }

        ep->removeView( d->dstViewName );

        notifyMainWindow( LTE_CMD_VIEW_DELETE );
    }
}

QString AddDelViewCommand::viewName() const
{
    Q_ASSERT(d);
    return d->dstViewName;
}

int AddDelViewCommand::position() const
{
    Q_ASSERT(d);
    return d->position;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelViewCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
