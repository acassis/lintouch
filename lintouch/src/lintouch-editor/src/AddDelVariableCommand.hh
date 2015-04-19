// $Id: AddDelVariableCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddDelVariableCommand.hh -- 
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

#ifndef _LTEDITORADDDELVARIABLECOMMAND_HH
#define _LTEDITORADDDELVARIABLECOMMAND_HH

#include <qstring.h>
#include <qmap.h>

#include "lt/templates/IOPin.hh"

#include "Command.hh"

class ProjectWindow;
class MainWindow;

namespace lt {
    class Variable;
}

/**
 * An undocommand class; Add or delete specified connection from the current
 * project.
 */
class AddDelVariableCommand : public Command
{
public:
    /**
     * ctor for add connection.
     *
     * \param cn Connection name
     * \param vn Variable name
     * \param vt Variable type
     * \param props Map of Variable properties
     */
    AddDelVariableCommand(const QString& cn, const QString& vn,
            const QString& vt, QMap<QString,QString> props,
            ProjectWindow* pw, MainWindow* mw );

    /**
     * ctor for delete connection.
     */
    AddDelVariableCommand(const QString& cn, const QString& vn,
             ProjectWindow* pw, MainWindow* mw );

    /**
     * dtor
     */
    virtual ~AddDelVariableCommand();

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
     * Get the name of the variable that is handled by this command.
     */
    const QString& connection() const;

    /**
     * Get the name of the variable that is handled by this command.
     */
    const QString& name() const;

    /**
     * Get the pos the connection is at in the dict of connections.
     */
    int position() const;

    /**
     * Get the Variable's type. Valid only when for add-redo and
     * delete-undo! Otherwise returns InvalidType.
     */
    lt::IOPin::Type variableType() const;

private:
    struct AddDelVariableCommandPrivate;
    AddDelVariableCommandPrivate* d;
};

#endif /* _LTEDITORADDDELVARIABLECOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelVariableCommand.hh 1169 2005-12-12 15:22:15Z modesto $
