// $Id: RenameViewCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenameViewCommand.cpp -- 
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 06 August 2004
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
#include <lt/project/View.hh>

#include "Bindings.hh"
#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "RenameViewCommand.hh"

using namespace lt;

struct RenameViewCommand::RenameViewCommandPrivate {
    /**
     * TODO
     */
    QString oldViewName;

    /**
     * TODO
     */
    QString newViewName;

    /**
     * Position of the renamed item in the ViewMap
     */
    int position;

    /**
     * True on redo action, false on undo action.
     */
    bool redo;
};

RenameViewCommand::RenameViewCommand(
        const QString oldViewName,
        const QString newViewName,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("RenameViewCommand", "Rename View") ),
    d(new RenameViewCommandPrivate)
{
    Q_CHECK_PTR(d);
    d->oldViewName = oldViewName;
    d->newViewName = newViewName;
    d->position = pw->project()->views().indexOf(d->oldViewName);
}

RenameViewCommand::~RenameViewCommand()
{
    delete d;
}

CommandType RenameViewCommand::rtti() const
{
    return LTE_CMD_VIEW_RENAME;
}

void RenameViewCommand::redo()
{
    Q_ASSERT(d);

    d->redo = true;

    renameView(d->oldViewName, d->newViewName);

    // inform the MW
    notifyMainWindow( LTE_CMD_VIEW_RENAME );
}

void RenameViewCommand::undo()
{
    Q_ASSERT(d);

    d->redo = false;

    renameView(d->newViewName, d->oldViewName);

    // inform the MW
    notifyMainWindow( LTE_CMD_VIEW_RENAME );
}

void RenameViewCommand::renameView(
        const QString& oldName, const QString& newName)
{
    Q_ASSERT(d);

    ViewMap& vm = m_pw->project()->views();
    Q_ASSERT( vm.contains(oldName) );

    // backup the view
    View view( vm[oldName] );

    // remove the view from Project
    m_pw->project()->removeView(oldName);

    // restore the view with new name
    m_pw->project()->addView( newName, view );

    // restore position
    vm.move( newName, d->position );
    Q_ASSERT( vm.keyAt(d->position) == newName );

    // rename bindings
    m_pw->project()->bindings().renameView(oldName, newName);
}

QString RenameViewCommand::oldViewName() const
{
    Q_ASSERT(d);
    if( d->redo == true ) {
        return d->oldViewName;
    } else {
        return d->newViewName;
    }
}

QString RenameViewCommand::newViewName() const
{
    Q_ASSERT(d);
    if( d->redo == false ) {
        return d->oldViewName;
    } else {
        return d->newViewName;
    }
}

int RenameViewCommand::position() const
{
    Q_ASSERT(d);
    return d->position;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenameViewCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
