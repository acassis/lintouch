// $Id: VariablePropsChangedCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: VariablePropsChangedCommand.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 21 January 2005
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

#include "VariablePropsChangedCommand.hh"

#include <lt/templates/Variable.hh>
#include <lt/templates/Connection.hh>
#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

struct VariablePropsChangedCommand::VariablePropsChangedCommandPrivate
{
    QString cn;
    QString vn;
    QMap<QString,QString> oldProps;
    QMap<QString,QString> newProps;
};

VariablePropsChangedCommand::VariablePropsChangedCommand(
        const QString& cn, const QString& vn,
        const QMap<QString,QString>& oldProps,
        const QMap<QString,QString>& newProps,
        ProjectWindow* pw, MainWindow* mw ) :
    Command(pw, mw,
            qApp->translate("VariablePropsChangedCommand",
                "Variable props change")),
    d(new VariablePropsChangedCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->cn = cn;
    d->vn = vn;

    d->oldProps = QMap<QString,QString>(oldProps);
    d->newProps = QMap<QString,QString>(newProps);
}

VariablePropsChangedCommand::~VariablePropsChangedCommand()
{
    delete d;
}

void VariablePropsChangedCommand::redo()
{
    Q_ASSERT(d);
    Q_ASSERT(m_pw);
    Q_ASSERT(m_pw->project()->connections().contains(d->cn));

    Connection* c = m_pw->project()->connections()[d->cn];
    Q_ASSERT(c);
    Q_ASSERT(c->variables().contains(d->vn));

    PropertyDict& pd = c->variables()[d->vn]->properties();

    for(QMap<QString,QString>::const_iterator it = d->newProps.begin();
            it != d->newProps.end(); ++it) {

        if(pd.contains(it.key())) {
            pd[it.key()]->setStringValue(it.data());
        } else {
            Property* p = new Property(it.key(), "string", it.data());
            Q_CHECK_PTR(p);
            pd.insert(it.key(), p);
        }
    }

    notifyMainWindow( LTE_CMD_VARIABLE_PROPS_CHANGE );
}

void VariablePropsChangedCommand::undo()
{
    Q_ASSERT(d);
    Q_ASSERT(m_pw);
    Q_ASSERT(m_pw->project()->connections().contains(d->cn));

    Connection* c = m_pw->project()->connections()[d->cn];
    Q_ASSERT(c);
    Q_ASSERT(c->variables().contains(d->vn));

    PropertyDict& pd = c->variables()[d->vn]->properties();

    for(QMap<QString,QString>::const_iterator it = d->oldProps.begin();
            it != d->oldProps.end(); ++it) {

        if(pd.contains(it.key())) {
            pd[it.key()]->setStringValue(it.data());
        } else {
            Property* p = new Property(it.key(), "string", it.data());
            Q_CHECK_PTR(p);
            pd.insert(it.key(), p);
        }
    }

    notifyMainWindow( LTE_CMD_VARIABLE_PROPS_CHANGE );
}

CommandType VariablePropsChangedCommand::rtti() const
{
    return LTE_CMD_VARIABLE_PROPS_CHANGE;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: VariablePropsChangedCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
