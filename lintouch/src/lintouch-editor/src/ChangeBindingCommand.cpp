// $Id: ChangeBindingCommand.cpp 1211 2006-01-06 12:11:28Z modesto $
//
//   FILE: ChangeBindingCommand.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 13 October 2004
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
#include <lt/templates/IOPin.hh>
#include <lt/templates/Template.hh>
#include <lt/templates/Variable.hh>
#include <lt/project/Panel.hh>
#include <lt/project/View.hh>

#include "Bindings.hh"
#include "CommandType.hh"
#include "EditorProject.hh"
#include "ProjectWindow.hh"

#include "ChangeBindingCommand.hh"

using namespace lt;

struct ChangeBindingCommand::ChangeBindingCommandPrivate
{
    bool redo;

    QString view;
    QString panel;
    QString templ;
    QString iopin;
    QString oldConnection;
    QString newConnection;
    QString oldVariable;
    QString newVariable;

    BindingPtr oldBinding;
    BindingPtr newBinding;
};

ChangeBindingCommand::ChangeBindingCommand(
        const QString& templ,
        const QString& iopin,
        const QString& oldConnection,
        const QString& newConnection,
        const QString& oldVariable,
        const QString& newVariable,
        ProjectWindow* pw, MainWindow* mw ) :
    Command(pw, mw,
            qApp->translate("ChangeBindingCommand", "Update binding"),false),
    d(new ChangeBindingCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->view = pw->currentView();
    d->panel = pw->currentPanel();
    d->templ = templ;
    d->iopin = iopin;
    d->oldConnection = oldConnection;
    d->newConnection = newConnection;
    d->oldVariable = oldVariable;
    d->newVariable = newVariable;

    Bindings& bs = pw->project()->bindings();
    // find & store the old binding; can be null
    d->oldBinding = bs.binding(oldConnection, oldVariable,
            d->view, d->panel, d->templ, d->iopin);

    // create the new binding
    d->newBinding = BindingPtr(new Binding(d->newConnection, d->newVariable,
            d->view, d->panel, d->templ, d->iopin));
    Q_ASSERT(!d->newBinding.isNull());
}

ChangeBindingCommand::~ChangeBindingCommand()
{
    delete d;
}

void ChangeBindingCommand::redo()
{
    Q_ASSERT(d);
    d->redo = true;

    switchBindings(d->oldBinding, d->newBinding);

    notifyMainWindow(LTE_CMD_BINDING_CHANGE);
}

void ChangeBindingCommand::undo()
{
    Q_ASSERT(d);
    d->redo = false;

    switchBindings(d->newBinding, d->oldBinding);

    notifyMainWindow(LTE_CMD_BINDING_CHANGE);
}

void ChangeBindingCommand::switchBindings(BindingPtr o, BindingPtr n)
{
    Bindings& bs = m_pw->project()->bindings();

    // remove the old binding
    if(!o.isNull()) {
        // delete the binding always
        bs.removeBinding(o);
    }

    if(!n.isNull()) {
        // // just update the binding; that means add bingind with new values
        Q_ASSERT(!n.isNull());
        bs.addBinding(n);
    }
}

CommandType ChangeBindingCommand::rtti() const
{
    return LTE_CMD_BINDING_CHANGE;
}

const BindingPtr ChangeBindingCommand::binding() const
{
    Q_ASSERT(d);
    if(d->redo) {
        return d->newBinding;
    } else {
        return d->oldBinding;
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ChangeBindingCommand.cpp 1211 2006-01-06 12:11:28Z modesto $
