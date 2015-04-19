// $Id: AddResourceCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddResourceCommand.cpp --
// AUTHOR: Shivaji Basu<shivaji@swac.cz>
//   DATE: 10th Aug 2004
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

#include "AddResourceCommand.hh"

#include "CommandType.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"
#include <EditorProject.hh>


struct AddResourceCommand::AddResourceCommandPrivate{
        AddResourceCommandPrivate()
        {}

        AddResourceCommandPrivate(const QString& n) : resourceName(n)
        {}

        ~AddResourceCommandPrivate()
        {}

    //Resource key
    QString key;

    //Resource name
    QString resourceName;

    // store the old resouce eventually; used for reimport
    QString undoname;
};

AddResourceCommand::AddResourceCommand(const QString& name,
        ProjectWindow* pw, MainWindow* mw):
        Command(pw, mw, qApp->translate("AddResourceCommand", "Add Resource")),
        d(new AddResourceCommandPrivate(name)){
    Q_CHECK_PTR(d);
}

AddResourceCommand::AddResourceCommand(const QString& key, const QString& path,
            ProjectWindow* pw, MainWindow* mw) :
    Command(pw, mw, qApp->translate("AddResourceCommand", "Reimport Resource")),
    d(new AddResourceCommandPrivate)
{
    d->key = key;
    d->resourceName = path;
}

AddResourceCommand::~AddResourceCommand()
{
    delete d;
}

CommandType AddResourceCommand::rtti() const
{
    return LTE_CMD_RESOURCE_ADD;
}

void AddResourceCommand::undo()
{
    Q_ASSERT(d);
    Q_ASSERT(d->key.isNull() == false);

    m_pw->project()->deleteResource(d->key);

    if(!d->undoname.isEmpty()) {
        // we are reimporting
        m_pw->project()->undeleteResource(d->key, d->undoname);
    }

    Q_ASSERT(d->key.isNull() == false);

    notifyMainWindow(LTE_CMD_RESOURCE_DELETE);
}

void AddResourceCommand::redo()
{
    Q_ASSERT(d);
    Q_ASSERT(d->resourceName.isNull() == false);

    EditorProject* p = m_pw->project();

    if(!d->key.isEmpty() &&
            p->unpackedResources()->exists(d->key))
    {
        // we are reimporting
        d->undoname = p->deleteResource(d->key);
        Q_ASSERT(!p->unpackedResources()->exists(d->key));

        d->key = p->addResource(d->resourceName, d->key);
    } else {
        d->key = p->addResource(d->resourceName);
    }

    Q_ASSERT(d->key.isNull() == false);

    notifyMainWindow(LTE_CMD_RESOURCE_ADD);
}

QString AddResourceCommand::resourceKey() const
{
    Q_ASSERT(d);

    return d->key;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddResourceCommand.cpp 1169 2005-12-12 15:22:15Z modesto $
