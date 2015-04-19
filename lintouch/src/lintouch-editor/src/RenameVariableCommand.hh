// $Id: RenameVariableCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenameVariableCommand.hh -- 
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

#ifndef _LTEDITORRENAMEVARIABLECOMMAND_HH
#define _LTEDITORRENAMEVARIABLECOMMAND_HH

#include "Command.hh"

#include <qstring.h>
#include <qmap.h>

class ProjectWindow;
class MainWindow;

namespace lt {
    class Connection;
}

/**
 * An undocommand class; Rename specified connection from the current
 * project.
 */
class RenameVariableCommand : public Command
{
public:
    /**
     * ctor for rename connection.
     */
    RenameVariableCommand(const QString& cn,
            const QString& oldName,
            const QString& newName, ProjectWindow* pw,
            MainWindow* mw);

    /**
     * dtor
     */
    virtual ~RenameVariableCommand();

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
     * Get the old name of the connection with respect to undo/redo.
     */
    const QString& connection() const;

    /**
     * Get the old name of the connection with respect to undo/redo.
     */
    const QString& oldName() const;

    /**
     * Get the new name of the connection with respect to undo/redo.
     */
    const QString& newName() const;

    /**
     * Get the pos the connection is at in the dict of connections.
     */
    int position() const;

protected:
    /**
     * Do the actial rename.
     */
    void renameVariable(const QString& oldName, const QString& newName);

private:
    struct RenameVariableCommandPrivate;
    RenameVariableCommandPrivate* d;
};

#endif /* _LTEDITORRENAMEVARIABLECOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenameVariableCommand.hh 1169 2005-12-12 15:22:15Z modesto $
