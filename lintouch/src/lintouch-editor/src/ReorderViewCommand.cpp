// $Id: ReorderViewCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ReorderViewCommand.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 02 September 2004
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

#include <lt/project/View.hh>

#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "ReorderViewCommand.hh"

using namespace lt;

struct ReorderViewCommand::ReorderViewCommandPrivate {

    /*
     * Holds name of the view that is reordered.
     */
    QString viewName;

    /*
     * Holds old position of the view. Old from POV of redo.
     */
    int oldPosition;

    /*
     * Holds new position of the view. New from POV of redo.
     */
    int newPosition;

    /*
     * Holds true on redo.
     */
    bool redo;
};

ReorderViewCommand::ReorderViewCommand(
        const QString& viewName, bool up,
        ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw,
            qApp->translate("ReorderViewCommand", "Change View Position") ),
    d(new ReorderViewCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->viewName = viewName;

    // get view map
    const ViewMap& vm = m_pw->project()->views();

    // get old view position
    d->oldPosition = vm.indexOf( d->viewName );

    if( up ) {
        // make sure the view is not the topmost
        Q_ASSERT(d->oldPosition>0);

        d->newPosition = d->oldPosition - 1;
    } else {
        // make sure the view is not the topmost
        Q_ASSERT(d->oldPosition<((int)vm.count()-1));

        d->newPosition = d->oldPosition + 1;
    }
}

ReorderViewCommand::~ReorderViewCommand()
{
    delete d;
}

void ReorderViewCommand::redo()
{
    Q_ASSERT(d);
    Q_ASSERT(!d->viewName.isNull());

    d->redo = true;

    // get view map
    ViewMap& vm = m_pw->project()->views();

    // move the view
    vm.move( d->oldPosition, d->newPosition );

    // notify others

    notifyMainWindow( LTE_CMD_VIEW_ORDER_CHANGE );
}

void ReorderViewCommand::undo()
{
    Q_ASSERT(d);
    Q_ASSERT(!d->viewName.isNull());

    d->redo = false;

    // get view map
    ViewMap& vm = m_pw->project()->views();

    // check the view is still there
    Q_ASSERT(vm.keyAt(d->newPosition) == d->viewName);

    // move the view back
    vm.move( d->newPosition, d->oldPosition );

    // notify others
    notifyMainWindow( LTE_CMD_VIEW_ORDER_CHANGE );
}

CommandType ReorderViewCommand::rtti() const
{
    return LTE_CMD_VIEW_ORDER_CHANGE;
}

QString ReorderViewCommand::viewName() const
{
    Q_ASSERT(d);
    return d->viewName;
}

int ReorderViewCommand::oldPosition() const
{
    Q_ASSERT(d);
    if( d->redo ) {
        return d->oldPosition;
    } else {
        return d->newPosition;
    }
}

int ReorderViewCommand::newPosition() const
{
    Q_ASSERT(d);
    if( d->redo ) {
        return d->newPosition;
    } else {
        return d->oldPosition;
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ReorderViewCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
