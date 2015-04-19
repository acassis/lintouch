// $Id: AddDelServerLoggingCommand.cpp 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: AddDelServerLoggingCommand.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: Fri, 07 Apr 2006
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

#include "Binding.hh"
#include "BindingsHelper.hh"
#include "Bindings.hh"
#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "ServerLogging.hh"
#include "AddDelServerLoggingCommand.hh"

using namespace lt;

struct AddDelServerLoggingCommand::AddDelServerLoggingCommandPrivate
{
    // AddDelServerLoggingCommand modes
    enum { Add, Del };

    // ctor
    AddDelServerLoggingCommandPrivate() : mode(Add), canDelete(false), sl(NULL),
                                       sl_old(NULL) {}

    ~AddDelServerLoggingCommandPrivate() {
        // delete what we created
        if(canDelete && sl) {
            PropertyDict& pd = sl->properties();
            pd.setAutoDelete(true);
            delete sl;
        }
        if(!canDelete && sl_old) {
            PropertyDict& pd = sl_old->properties();
            pd.setAutoDelete(true);
            delete sl_old;
        }
    }

    /**
     * command mode
     */
    int mode;

    /**
     * True if the commmand owns the created ServerLogging and therefor can
     * delete it.
     */
    bool canDelete;

    /**
     * ServerLogging we work with.
     */
    ServerLogging* sl;

    /**
     * ServerLogging we work with.
     */
    ServerLogging* sl_old;

    /**
     * ServerLogging name;
     */
    QString cn;

    /**
     * ServerLogging type;
     */
    QString ct;

    /**
     * Position of the ServerLogging in the list.
     */
    int pos;

    /**
     * Properties of new ServerLogging.
     */
    QMap<QString,QString> props;
};

AddDelServerLoggingCommand::AddDelServerLoggingCommand(
        const QString& name, const QString& type,
        QMap<QString,QString> props,
        ProjectWindow* pw, MainWindow* mw ) :
    Command(pw, mw,
            qApp->translate("AddDelServerLoggingCommand", "Add ServerLogging")),
    d(new AddDelServerLoggingCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->mode = AddDelServerLoggingCommandPrivate::Add;
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
    d->sl = new ServerLogging(d->ct, pd);
    Q_CHECK_PTR(d->sl);

    d->canDelete = true;
}

AddDelServerLoggingCommand::AddDelServerLoggingCommand(
        const QString& name, ProjectWindow* pw,
        MainWindow* mw ) :
    Command(pw, mw, qApp->translate("AddDelServerLoggingCommand",
                "Delete ServerLogging")),
    d(new AddDelServerLoggingCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->mode = AddDelServerLoggingCommandPrivate::Del;
    d->cn = name;

    EditorProject* p = m_pw->project();
    Q_ASSERT(p);

    ServerLoggingDict& cd = p->serverLoggings();
    Q_ASSERT(cd.contains(d->cn));
    d->sl = cd[d->cn];
    Q_ASSERT(d->sl);
    d->pos = cd.indexOf(d->cn);
    Q_ASSERT(d->pos >= 0);
}

AddDelServerLoggingCommand::~AddDelServerLoggingCommand()
{
    delete d;
}

void AddDelServerLoggingCommand::redo()
{
    Q_ASSERT(d);

    EditorProject* p = m_pw->project();
    Q_ASSERT(p);

    switch(d->mode) {
        case AddDelServerLoggingCommandPrivate::Del:

            p->removeServerLogging(d->cn);

            notifyMainWindow( LTE_CMD_SERVERLOGGING_DEL );

            // we now own the ServerLogging; we must delete it in dtor
            d->canDelete = true;
            break;

        case AddDelServerLoggingCommandPrivate::Add:

            Q_CHECK_PTR(d->sl);
            p->addServerLogging(d->cn, d->sl);

            notifyMainWindow( LTE_CMD_SERVERLOGGING_ADD );

            // we don't own the ServerLogging; we must NOT delete it in dtor
            d->canDelete = false;
            break;
    }
}

void AddDelServerLoggingCommand::undo()
{
    Q_ASSERT(d);

    EditorProject* p = m_pw->project();
    Q_ASSERT(p);

    ServerLoggingDict& cd = p->serverLoggings();

    switch(d->mode) {
        case AddDelServerLoggingCommandPrivate::Del:
            //undo delete, ie add
            Q_ASSERT(d->sl);
            Q_ASSERT(!d->cn.isEmpty());

            p->addServerLogging(d->cn, d->sl);
            Q_ASSERT(cd.contains(d->cn));

            // move at old pos
            cd.move(d->cn, d->pos);
            Q_ASSERT(cd.indexOf(d->cn) == d->pos);

            notifyMainWindow( LTE_CMD_SERVERLOGGING_ADD );

            // we don't own the ServerLogging; we must NOT delete it in dtor
            d->canDelete = false;
            break;

        case AddDelServerLoggingCommandPrivate::Add:
            //undo add, ie delete
            Q_ASSERT(cd.contains(d->cn));

            p->removeServerLogging(d->cn);
            Q_ASSERT(!cd.contains(d->cn));

            notifyMainWindow( LTE_CMD_SERVERLOGGING_DEL );

            // we own the ServerLogging; we must delete it in dtor
            d->canDelete = true;
            break;
    }
}

CommandType AddDelServerLoggingCommand::rtti() const
{
    return LTE_CMD_SERVERLOGGING_ADD_DEL;
}

const QString& AddDelServerLoggingCommand::name() const
{
    Q_ASSERT(d);
    return d->cn;
}

int AddDelServerLoggingCommand::position() const
{
    Q_ASSERT(d);
    return d->pos;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelServerLoggingCommand.cpp 1564 2006-04-07 13:33:15Z modesto $
