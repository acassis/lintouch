// $Id: AddDelConnectionCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddDelConnectionCommand.cpp -- 
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

#include "AddDelConnectionCommand.hh"

using namespace lt;

struct AddDelConnectionCommand::AddDelConnectionCommandPrivate
{
    // AddDelConnectionCommand modes
    enum { Add, Del, Import, Reimport };

    // ctor
    AddDelConnectionCommandPrivate() : mode(Add), canDelete(false), c(NULL),
                                       c_old(NULL) {}

    ~AddDelConnectionCommandPrivate() {
        // delete what we created
        if(canDelete && c) {
            PropertyDict& pd = c->properties();
            pd.setAutoDelete(true);
            delete c;
        }
        if(!canDelete && c_old) {
            PropertyDict& pd = c_old->properties();
            pd.setAutoDelete(true);
            delete c_old;
        }
    }

    /**
     * command mode
     */
    int mode;

    /**
     * True if the commmand owns the created Connection and therefor can
     * delete it.
     */
    bool canDelete;

    /**
     * Connection we work with.
     */
    Connection* c;

    /**
     * Old connection after reimport.
     */
    Connection* c_old;

    /**
     * Connection name;
     */
    QString cn;

    /**
     * Connection type;
     */
    QString ct;

    /**
     * Position of the Connection in the list.
     */
    int pos;

    /**
     * Properties of new Connection.
     */
    QMap<QString,QString> props;

    /**
     * Connection bindings.
     */
    BindingsList bl;

    /**
     * Old bindings after reimport
     */
    BindingsList bl_old;

    /**
     * Path to the reimported connection
     */
    QString path;
};

AddDelConnectionCommand::AddDelConnectionCommand(
        const QString& name, const QString& type,
        QMap<QString,QString> props,
        ProjectWindow* pw, MainWindow* mw ) :
    Command(pw, mw,
            qApp->translate("AddDelConnectionCommand", "Add Connection")),
    d(new AddDelConnectionCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->mode = AddDelConnectionCommandPrivate::Add;
    d->cn = name;
    d->ct = type;
    d->props = props;

    PropertyDict pd;
    for(QMap<QString,QString>::const_iterator it = d->props.begin();
            it != d->props.end(); ++it) {

        Property* p = new Property(it.key(), "string", it.data());
        Q_CHECK_PTR(p);
        pd.insert(it.key(), p);
    }
    d->c = new Connection(d->ct, pd);
    Q_CHECK_PTR(d->c);

    d->canDelete = true;
}

AddDelConnectionCommand::AddDelConnectionCommand(const QString& name,
        Connection* c, const QString& path,
        ProjectWindow* pw, MainWindow* mw ) :
    Command(pw, mw, QString::null),
    d(new AddDelConnectionCommandPrivate)
{
    Q_CHECK_PTR(d);

    Q_ASSERT(c);
    d->c = c;
    d->cn = name;
    d->path = path;

    ConnectionDict& cd = pw->project()->connections();

    if(cd.contains(name)) {
        d->mode = AddDelConnectionCommandPrivate::Reimport;
        setDescription(qApp->translate("AddDelConnectionCommand",
                    "Reimport Connection"));

        EditorProject* p = m_pw->project();
        Q_ASSERT(p);

        d->pos = cd.indexOf(d->cn);
        d->c_old = cd[d->cn];
        Q_ASSERT(d->c_old);

        // qWarning(">>> ctor consistencyCheck 1");
        // BindingsHelper::consistencyCheck(p);
        // qWarning("<<< ctor consistencyCheck 1");

        // merge the two connections
        if(d->c->type() == d->c_old->type()) {
            // atleast same type

            // save and remove bindings from Bindings class
            d->bl_old = BindingsHelper::saveVariableBindings(p, d->cn);

            const VariableDict& vd = d->c->variables();

            // walk through the old variables and compare
            const VariableDict& ovd = d->c_old->variables();
            // qWarning("c_old has %d vars", ovd.count());

            for(unsigned i = 0; i < ovd.count(); ++i) {
                QString key = ovd.keyAt(i);
                // qWarning("comparing %s, type: %d", key.latin1(),
                //         ovd[key]->type());

                if(vd.contains(key) && vd[key]->type() == ovd[key]->type()) {
                    // bind
                    d->bl +=
                        BindingsHelper::saveVariableBindingsAndUnbind(p,
                                d->cn, key);
                }
            }
            // qWarning("keep %d bindings", d->bl.count());

            // remove bindings rest of the bindings and unbind
            BindingsHelper::saveVariableBindingsAndUnbind(p, d->cn);
        } else {
            //completely different connection
            // qWarning("completely different connection (%s != %s)",
            //         d->c->type().latin1(), d->c_old->type().latin1());

            // save and remove bindings from Bindings class
            d->bl_old = BindingsHelper::saveVariableBindingsAndUnbind(p, d->cn);
        }
    } else {
        d->mode = AddDelConnectionCommandPrivate::Import;
        setDescription(qApp->translate("AddDelConnectionCommand",
                    "Import Connection"));
        //nothing more to be done
    }
    d->canDelete = false;

    // qWarning(">>> ctor consistencyCheck 2");
    // BindingsHelper::consistencyCheck(pw->project());
    // qWarning("<<< ctor consistencyCheck 2");
}

AddDelConnectionCommand::AddDelConnectionCommand(
        const QString& name, ProjectWindow* pw,
        MainWindow* mw ) :
    Command(pw, mw, qApp->translate("AddDelConnectionCommand",
                "Delete connection")),
    d(new AddDelConnectionCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->mode = AddDelConnectionCommandPrivate::Del;
    d->cn = name;

    EditorProject* p = m_pw->project();
    Q_ASSERT(p);

    // save and remove bindings from Bindings class
    d->bl = BindingsHelper::saveVariableBindingsAndUnbind(p, d->cn);

    ConnectionDict& cd = p->connections();
    Q_ASSERT(cd.contains(d->cn));
    d->c = cd[d->cn];
    Q_ASSERT(d->c);
    d->pos = cd.indexOf(d->cn);
    Q_ASSERT(d->pos >= 0);
}

AddDelConnectionCommand::~AddDelConnectionCommand()
{
    delete d;
}

void AddDelConnectionCommand::redo()
{
    Q_ASSERT(d);

    EditorProject* p = m_pw->project();
    Q_ASSERT(p);

    switch(d->mode) {
        case AddDelConnectionCommandPrivate::Del:

            p->removeConnection(d->cn);

            notifyMainWindow( LTE_CMD_CONNECTION_DEL );

            // we now own the Connection; we must delete it in dtor
            d->canDelete = true;
            break;

        case AddDelConnectionCommandPrivate::Add:
        case AddDelConnectionCommandPrivate::Import:

            Q_CHECK_PTR(d->c);
            p->addConnection(d->cn, d->c);

            notifyMainWindow( LTE_CMD_CONNECTION_ADD );

            // we don't own the Connection; we must NOT delete it in dtor
            d->canDelete = false;
            break;

        case AddDelConnectionCommandPrivate::Reimport:

            // unbind the whole connection
            BindingsHelper::saveVariableBindingsAndUnbind(p, d->cn);

            // remove old connection
            p->removeConnection(d->cn);

            // add the fresh new one
            p->addConnection(d->cn, d->c);

            // qWarning(">>> redo consistencyCheck 1");
            // BindingsHelper::consistencyCheck(p);
            // qWarning("<<< redo consistencyCheck 1");

            // bind acording the d->bl
            BindingsHelper::restoreBindingsAndBind(p, d->bl);

            // qWarning(">>> redo consistencyCheck 2");
            // BindingsHelper::consistencyCheck(p);
            // qWarning("<<< redo consistencyCheck 2");

            d->canDelete = false;

            notifyMainWindow( LTE_CMD_CONNECTION_ADD );
            break;
    }
}

void AddDelConnectionCommand::undo()
{
    Q_ASSERT(d);

    EditorProject* p = m_pw->project();
    Q_ASSERT(p);

    ConnectionDict& cd = p->connections();

    switch(d->mode) {
        case AddDelConnectionCommandPrivate::Del:
            //undo delete, ie add
            Q_ASSERT(d->c);
            Q_ASSERT(!d->cn.isEmpty());

            p->addConnection(d->cn, d->c);
            Q_ASSERT(cd.contains(d->cn));

            // move at old pos
            cd.move(d->cn, d->pos);
            Q_ASSERT(cd.indexOf(d->cn) == d->pos);

            // bind acording the d->bl
            BindingsHelper::restoreBindingsAndBind(p, d->bl);

            notifyMainWindow( LTE_CMD_CONNECTION_ADD );

            // we don't own the Connection; we must NOT delete it in dtor
            d->canDelete = false;
            break;

        case AddDelConnectionCommandPrivate::Add:
        case AddDelConnectionCommandPrivate::Import:
            //undo add/import, ie delete
            Q_ASSERT(cd.contains(d->cn));

            p->removeConnection(d->cn);
            Q_ASSERT(!cd.contains(d->cn));

            notifyMainWindow( LTE_CMD_CONNECTION_DEL );

            // we own the Connection; we must delete it in dtor
            d->canDelete = true;
            break;

        case AddDelConnectionCommandPrivate::Reimport:
            Q_ASSERT(d->c_old);
            Q_ASSERT(!d->cn.isEmpty());

            // unbind the whole connection
            BindingsHelper::saveVariableBindingsAndUnbind(p, d->cn);

            // remove old connection
            p->removeConnection(d->cn);

            p->addConnection(d->cn, d->c_old);
            Q_ASSERT(cd.contains(d->cn));

            // move at old pos
            cd.move(d->cn, d->pos);
            Q_ASSERT(cd.indexOf(d->cn) == d->pos);

            // qWarning(">>> undo consistencyCheck 1");
            // BindingsHelper::consistencyCheck(p);
            // qWarning("<<< undo consistencyCheck 1");

            // bind acording the d->bl_old
            BindingsHelper::restoreBindingsAndBind(p, d->bl_old);

            // qWarning(">>> undo consistencyCheck 2");
            // BindingsHelper::consistencyCheck(p);
            // qWarning("<<< undo consistencyCheck 2");

            d->canDelete = true;

            notifyMainWindow( LTE_CMD_CONNECTION_ADD );
            break;
    }
}

CommandType AddDelConnectionCommand::rtti() const
{
    return LTE_CMD_CONNECTION_ADD_DEL;
}

const QString& AddDelConnectionCommand::name() const
{
    Q_ASSERT(d);
    return d->cn;
}

int AddDelConnectionCommand::position() const
{
    Q_ASSERT(d);
    return d->pos;
}

QString AddDelConnectionCommand::path() const
{
    Q_ASSERT(d);
    return d->path;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelConnectionCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
