// $Id: RenameVariableCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenameVariableCommand.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 22 October 2004
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

#include <lt/templates/Connection.hh>
#include <lt/templates/Variable.hh>

#include "Binding.hh"
#include "Bindings.hh"
#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "RenameVariableCommand.hh"

using namespace lt;

struct RenameVariableCommand::RenameVariableCommandPrivate
{
    /**
     * Add/Delete flag.
     */
    bool redo;

    /**
     * Connection name
     */
    QString cn;

    /**
     * Connection name;
     */
    QString oldName;

    /**
     * Connection type;
     */
    QString newName;

    /**
     * Position of the Connection in the list.
     */
    int pos;
};

RenameVariableCommand::RenameVariableCommand(
        const QString& cn,
        const QString& oldName, const QString& newName,
        ProjectWindow* pw, MainWindow* mw ) :
    Command(pw, mw, qApp->translate("RenameVariableCommand",
                "Rename Variable")),
    d(new RenameVariableCommandPrivate)
{
    Q_CHECK_PTR(d);
    Q_ASSERT(!cn.isEmpty());
    Q_ASSERT(!oldName.isEmpty());
    Q_ASSERT(!newName.isEmpty());

    d->cn = cn;
    d->oldName = oldName;
    d->newName = newName;
}

RenameVariableCommand::~RenameVariableCommand()
{
    delete d;
}

void RenameVariableCommand::redo()
{
    Q_ASSERT(d);

    d->redo = true;

    renameVariable(d->oldName, d->newName);

    notifyMainWindow(LTE_CMD_VARIABLE_RENAME);
}

void RenameVariableCommand::undo()
{
    Q_ASSERT(d);

    d->redo = false;

    renameVariable(d->newName, d->oldName);

    notifyMainWindow(LTE_CMD_VARIABLE_RENAME);
}

void RenameVariableCommand::renameVariable(const QString& oldName,
        const QString& newName)
{
    // rename the connection in the project
    Connection* c = m_pw->project()->connections()[d->cn];
    Q_ASSERT(c);

    VariableDict& vd = c->variables();

    d->pos = vd.indexOf(oldName);
    Q_ASSERT(d->pos >= 0);

    Variable* v = vd[oldName];
    Q_ASSERT(v);

    vd.remove(oldName);
    Q_ASSERT(!vd.contains(oldName));

    Q_ASSERT(!vd.contains(newName));
    vd.insert(newName, v, d->pos);

    // rename bindings
    Bindings& b = m_pw->project()->bindings();

    b.renameVariable(d->cn, oldName, newName);
}

CommandType RenameVariableCommand::rtti() const
{
    return LTE_CMD_VARIABLE_RENAME;
}

const QString& RenameVariableCommand::connection() const
{
    Q_ASSERT(d);
    return d->cn;
}

const QString& RenameVariableCommand::oldName() const
{
    Q_ASSERT(d);
    if(d->redo) {
        return d->oldName;
    } else {
        return d->newName;
    }
}

const QString& RenameVariableCommand::newName() const
{
    Q_ASSERT(d);
    if(d->redo) {
        return d->newName;
    } else {
        return d->oldName;
    }
}

int RenameVariableCommand::position() const
{
    Q_ASSERT(d);
    return d->pos;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenameVariableCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
