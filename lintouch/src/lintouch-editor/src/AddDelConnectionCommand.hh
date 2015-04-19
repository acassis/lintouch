// $Id: AddDelConnectionCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddDelConnectionCommand.hh -- 
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

#ifndef _LTEDITORADDDELCONNECTIONCOMMAND_HH
#define _LTEDITORADDDELCONNECTIONCOMMAND_HH

#include "Command.hh"

#include <qstring.h>
#include <qmap.h>

class ProjectWindow;
class MainWindow;

namespace lt {
    class Connection;
}

/**
 * An undocommand class; Add or delete specified connection from the current
 * project.
 */
class AddDelConnectionCommand : public Command
{
public:
    /**
     * ctor for add connection.
     */
    AddDelConnectionCommand(const QString& name, const QString& type,
            QMap<QString,QString> props, ProjectWindow* pw,
            MainWindow* mw );

    /**
     * ctor for import/reimport connection. It is reimport when name already
     * exists as a connection.
     */
    AddDelConnectionCommand(const QString& name, lt::Connection* c,
            const QString& path,
            ProjectWindow* pw, MainWindow* mw );

    /**
     * ctor for delete connection.
     */
    AddDelConnectionCommand(const QString& name,
             ProjectWindow* pw, MainWindow* mw );

    /**
     * dtor
     */
    virtual ~AddDelConnectionCommand();

    /**
     * Redo
     */
    virtual void redo();

    /**
     * Undo
     */
    virtual void undo();

    /**
     * RTTI
     */
    virtual CommandType rtti() const;

    /**
     * Get the name of the connection that is handled by this command.
     */
    const QString& name() const;

    /**
     * Get the pos the connection is at in the dict of connections.
     */
    int position() const;

    /**
     * Get path of this connection. Returns QString::null if the path is not
     * known.
     */
    QString path() const;

private:
    struct AddDelConnectionCommandPrivate;
    AddDelConnectionCommandPrivate* d;
};

#endif /* _LTEDITORADDDELCONNECTIONCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelConnectionCommand.hh 1169 2005-12-12 15:22:15Z modesto $
