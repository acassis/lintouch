// $Id: RenameServerLoggingCommand.cpp 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: RenameServerLoggingCommand.cpp -- 
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

#include "CommandType.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"

#include "RenameServerLoggingCommand.hh"
#include "ServerLogging.hh"

using namespace lt;

struct RenameServerLoggingCommand::RenameServerLoggingCommandPrivate
{
    /**
     * Add/Delete flag.
     */
    bool redo;

    /**
     * "ServerLogging.hh"
     */
    QString oldName;

    /**
     * ServerLogging type;
     */
    QString newName;

    /**
     * Position of the ServerLogging in the list.
     */
    int pos;
};

RenameServerLoggingCommand::RenameServerLoggingCommand(
        const QString& oldName, const QString& newName,
        ProjectWindow* pw, MainWindow* mw ) :
    Command(pw, mw,
            qApp->translate("RenameServerLoggingCommand", "Rename ServerLogging")),
    d(new RenameServerLoggingCommandPrivate)
{
    Q_CHECK_PTR(d);

    d->oldName = oldName;
    d->newName = newName;
}

RenameServerLoggingCommand::~RenameServerLoggingCommand()
{
    delete d;
}

void RenameServerLoggingCommand::redo()
{
    Q_ASSERT(d);

    d->redo = true;

    renameServerLogging(d->oldName, d->newName);

    notifyMainWindow(LTE_CMD_SERVERLOGGING_RENAME);
}

void RenameServerLoggingCommand::undo()
{
    Q_ASSERT(d);

    d->redo = false;

    renameServerLogging(d->newName, d->oldName);

    notifyMainWindow(LTE_CMD_SERVERLOGGING_RENAME);
}

void RenameServerLoggingCommand::renameServerLogging(const QString& oldName,
        const QString& newName)
{
    // rename the ServerLogging in the project
    ServerLoggingDict& cd = m_pw->project()->serverLoggings();
    Q_ASSERT(cd.contains(oldName));

    d->pos = cd.indexOf(oldName);
    Q_ASSERT(d->pos >= 0);

    ServerLogging* c = cd[oldName];
    Q_ASSERT(c);

    cd.remove(oldName);
    Q_ASSERT(!cd.contains(oldName));

    Q_ASSERT(!cd.contains(newName));
    cd.insert(newName, c, d->pos);
}

CommandType RenameServerLoggingCommand::rtti() const
{
    return LTE_CMD_SERVERLOGGING_RENAME;
}

const QString& RenameServerLoggingCommand::oldName() const
{
    Q_ASSERT(d);
    if(d->redo) {
        return d->oldName;
    } else {
        return d->newName;
    }
}

const QString& RenameServerLoggingCommand::newName() const
{
    Q_ASSERT(d);
    if(d->redo) {
        return d->newName;
    } else {
        return d->oldName;
    }
}

int RenameServerLoggingCommand::position() const
{
    Q_ASSERT(d);
    return d->pos;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenameServerLoggingCommand.cpp 1564 2006-04-07 13:33:15Z modesto $
