// $Id: ResizeViewCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ResizeViewCommand.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 01 September 2004
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

#include "ResizeViewCommand.hh"

using namespace lt;

struct ResizeViewCommand::ResizeViewCommandPrivate {

    /*
     * new width/height of the resized view.
     */
    int oldWidth;
    int oldHeight;

    /*
     * old width/height of the resized view.
     */
    int newWidth;
    int newHeight;

    /*
     * resized view name.
     */
    QString viewName;

    /*
     * redo/undo flag. true on redo, false on undo.
     */
    bool redo;

    /**
     * The view we work with
     */
    View v;
};

ResizeViewCommand::ResizeViewCommand( const QString& viewName,
        int nw, int nh, ProjectWindow* pw, MainWindow* mw ) :
    Command( pw, mw, qApp->translate("ResizeViewCommand", "Resize View") ),
    d(new ResizeViewCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->viewName = viewName;

    Q_ASSERT(m_pw->project()->views().contains(d->viewName));
    d->v = m_pw->project()->views()[d->viewName];

    d->oldWidth = d->v.virtualWidth();
    d->oldHeight = d->v.virtualHeight();

    d->newWidth = nw;
    d->newHeight = nh;
}

ResizeViewCommand::~ResizeViewCommand()
{
    delete d;
}

void ResizeViewCommand::redo()
{
    Q_ASSERT(d);
    Q_ASSERT(m_pw->project()->views().contains(d->viewName));

    d->v.setVirtualWidth( d->newWidth );
    d->v.setVirtualHeight( d->newHeight );

    d->redo = true;

    notifyMainWindow( LTE_CMD_VIEW_RESIZE );
}

void ResizeViewCommand::undo()
{
    Q_ASSERT(d);
    Q_ASSERT(m_pw->project()->views().contains(d->viewName));

    d->v.setVirtualWidth( d->oldWidth );
    d->v.setVirtualHeight( d->oldHeight );

    d->redo = false;

    notifyMainWindow( LTE_CMD_VIEW_RESIZE );
}

CommandType ResizeViewCommand::rtti() const
{
    return LTE_CMD_VIEW_RESIZE;
}

QString ResizeViewCommand::viewName() const
{
    Q_ASSERT(d);
    return d->viewName;
}

int ResizeViewCommand::oldWidth() const
{
    Q_ASSERT(d);
    if(d->redo) {
        return d->oldWidth;
    } else {
        return d->newWidth;
    }
}
int ResizeViewCommand::oldHeight() const
{
    Q_ASSERT(d);
    if(d->redo) {
        return d->oldHeight;
    } else {
        return d->newHeight;
    }
}
int ResizeViewCommand::newWidth() const
{
    Q_ASSERT(d);
    if(d->redo) {
        return d->newWidth;
    } else {
        return d->oldWidth;
    }
}
int ResizeViewCommand::newHeight() const
{
    Q_ASSERT(d);
    if(d->redo) {
        return d->newHeight;
    } else {
        return d->oldHeight;
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ResizeViewCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
