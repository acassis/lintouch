// $Id: AddDelServerLoggingCommand.hh 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: AddDelServerLoggingCommand.hh -- 
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

#ifndef _LTEDITORADDDELSERVERLOGGINGCOMMAND_HH
#define _LTEDITORADDDELSERVERLOGGINGCOMMAND_HH

#include "Command.hh"

#include <qstring.h>
#include <qmap.h>

class ProjectWindow;
class MainWindow;

namespace lt {
    class ServerLogging;
}

/**
 * An undocommand class; Add or delete specified ServerLogging from the current
 * project.
 */
class AddDelServerLoggingCommand : public Command
{
public:
    /**
     * ctor for add ServerLogging.
     */
    AddDelServerLoggingCommand(const QString& name, const QString& type,
            QMap<QString,QString> props, ProjectWindow* pw,
            MainWindow* mw );

    /**
     * ctor for delete ServerLogging.
     */
    AddDelServerLoggingCommand(const QString& name,
             ProjectWindow* pw, MainWindow* mw );

    /**
     * dtor
     */
    virtual ~AddDelServerLoggingCommand();

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
     * Get the name of the ServerLogging that is handled by this command.
     */
    const QString& name() const;

    /**
     * Get the pos the ServerLogging is at in the dict of serverLoggings.
     */
    int position() const;

    /**
     * Get path of this ServerLogging. Returns QString::null if the path is not
     * known.
     */
    QString path() const;

private:
    struct AddDelServerLoggingCommandPrivate;
    AddDelServerLoggingCommandPrivate* d;
};

#endif /* _LTEDITORADDDELSERVERLOGGINGCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelServerLoggingCommand.hh 1564 2006-04-07 13:33:15Z modesto $
