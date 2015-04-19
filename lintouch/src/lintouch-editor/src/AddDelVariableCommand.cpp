// $Id: AddDelVariableCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddDelVariableCommand.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 19 October 2004
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

#include "Binding.hh"
#include "BindingsHelper.hh"
#include "Bindings.hh"
#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "AddDelVariableCommand.hh"

using namespace lt;

struct AddDelVariableCommand::AddDelVariableCommandPrivate
{
    AddDelVariableCommandPrivate() : add(false), canDelete(false), v(NULL) {}

    ~AddDelVariableCommandPrivate() {

        if(canDelete && v) {
            // delete properties
            v->properties().setAutoDelete(true);
            delete v;
        }
    }

    /**
     * Add/Delete flag.
     */
    bool add;

    /**
     * True if the commmand owns the created Connection and therefor can
     * delete it.
     */
    bool canDelete;

    /**
     * Variable we work with.
     */
    Variable* v;

    /**
     * Connection name;
     */
    QString cn;

    /**
     * Variable name;
     */
    QString vn;

    /**
     * Variable type;
     */
    QString vt;

    /**
     * Position of the Variable in the list.
     */
    int pos;

    /**
     * Properties of new Variable.
     */
    QMap<QString,QString> props;

    /**
     * Variable bindings.
     */
    BindingsList bl;
};

AddDelVariableCommand::AddDelVariableCommand(
        const QString& cn, const QString& vn, const QString& vt,
        QMap<QString,QString> props,
        ProjectWindow* pw, MainWindow* mw ) :
    Command(pw, mw, qApp->translate("AddDelVariableCommand", "Add Variable")),
    d(new AddDelVariableCommandPrivate)
{
    Q_CHECK_PTR(d);

    Q_ASSERT(!cn.isEmpty());
    Q_ASSERT(!vn.isEmpty());
    Q_ASSERT(!vt.isEmpty());

    d->add = true;
    d->cn = cn;
    d->vn = vn;
    d->vt = vt;
    d->props = props;

    PropertyDict pd;
    for(QMap<QString,QString>::const_iterator it = d->props.begin();
            it != d->props.end(); ++it) {

        Property* p = new Property(it.key(), "string", it.data());
        Q_CHECK_PTR(p);
        pd.insert(it.key(), p);
    }
    d->v = new Variable(d->vt, pd);
    Q_CHECK_PTR(d->v);

    // we own the Variable; we must delete it in dtor
    d->canDelete = true;
}

AddDelVariableCommand::AddDelVariableCommand(
        const QString& cn, const QString& vn,
        ProjectWindow* pw, MainWindow* mw ) :
    Command(pw, mw, qApp->translate("AddDelVariableCommand", "Delete Variable")),
    d(new AddDelVariableCommandPrivate)
{
    Q_CHECK_PTR(d);
    Q_ASSERT(!vn.isEmpty());
    Q_ASSERT(!cn.isEmpty());

    d->add = false;
    d->cn = cn;
    d->vn = vn;

    Connection* c = m_pw->project()->connections()[d->cn];
    Q_ASSERT(c);

    Q_ASSERT(c->variables().contains(d->vn));
    d->v = c->variables()[d->vn];
    Q_ASSERT(d->v);

    d->pos = c->variables().indexOf(d->vn);
    Q_ASSERT(d->pos >= 0);

    // save and remove bindings from Bindings class
    d->bl = BindingsHelper::saveVariableBindingsAndUnbind(m_pw->project(),
            d->cn, d->vn);

    // we don't own the Variable; we must NOT delete it in dtor
    d->canDelete = false;
}

AddDelVariableCommand::~AddDelVariableCommand()
{
    delete d;
}

void AddDelVariableCommand::redo()
{
    Q_ASSERT(d);

    Q_ASSERT(m_pw->project()->connections().contains(d->cn));
    Connection* c = m_pw->project()->connections()[d->cn];

    Q_ASSERT(c);

    if(d->add) {
        // redo add
        Q_CHECK_PTR(d->v);

        Q_ASSERT(!c->variables().contains(d->vn));
        c->addVariable(d->vn, d->v);

        d->pos = c->variables().indexOf(d->vn);
        Q_ASSERT(d->pos >= 0);

        notifyMainWindow(LTE_CMD_VARIABLE_ADD);

        // we don't own the Variable; we must NOT delete it in dtor
        d->canDelete = false;
    } else {
        // redo delete
        Q_ASSERT(c->variables().contains(d->vn));
        c->removeVariable(d->vn);
        Q_ASSERT(!c->variables().contains(d->vn));

        notifyMainWindow(LTE_CMD_VARIABLE_DEL);

        // we own the Variable; we must delete it in dtor
        d->canDelete = true;
    }
}

void AddDelVariableCommand::undo()
{
    Q_ASSERT(d);

    Q_ASSERT(m_pw->project()->connections().contains(d->cn));
    Connection* c = m_pw->project()->connections()[d->cn];
    Q_ASSERT(c);

    if(d->add) {
        // undo add; ie delete
        Q_ASSERT(c->variables().contains(d->vn));
        Q_ASSERT(c->variables()[d->vn] == d->v);

        c->removeVariable(d->vn);
        Q_ASSERT(!c->variables().contains(d->vn));

        notifyMainWindow(LTE_CMD_VARIABLE_DEL);

        // we own the Variable; we must delete it in dtor
        d->canDelete = true;

    } else {
        // undo delete; ie add
        Q_ASSERT(d->v);
        c->addVariable(d->vn, d->v);

        c->variables().move(d->vn, d->pos);
        Q_ASSERT(c->variables().indexOf(d->vn) == d->pos);

        EditorProject* p = m_pw->project();
        Q_ASSERT(p);

        // bind acording the d->bl
        BindingsHelper::restoreBindingsAndBind(p, d->bl);

        notifyMainWindow(LTE_CMD_VARIABLE_ADD);

        // we don't own the Variable; we must NOT delete it in dtor
        d->canDelete = false;
    }
}

CommandType AddDelVariableCommand::rtti() const
{
    return LTE_CMD_VARIABLE_ADD_DEL;
}

const QString& AddDelVariableCommand::connection() const
{
    Q_ASSERT(d);
    return d->cn;
}

const QString& AddDelVariableCommand::name() const
{
    Q_ASSERT(d);
    return d->vn;
}

int AddDelVariableCommand::position() const
{
    Q_ASSERT(d);
    return d->pos;
}

IOPin::Type AddDelVariableCommand::variableType() const
{
    Q_ASSERT(d);
    if(d->v) {
        return d->v->type();
    }
    return IOPin::InvalidType;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelVariableCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
