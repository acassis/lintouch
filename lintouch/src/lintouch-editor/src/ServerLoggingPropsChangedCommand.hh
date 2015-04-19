// $Id: ServerLoggingPropsChangedCommand.hh 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: ServerLoggingPropsChangedCommand.hh -- 
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

#ifndef _SERVERLOGGINGPROPSCHANGEDCOMMAND_HH
#define _SERVERLOGGINGPROPSCHANGEDCOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class ServerLoggingPropsChangedCommand : public Command
{
public:
    /**
     * ctor for add ServerLogging.
     */
    ServerLoggingPropsChangedCommand(const QString& cn,
            const QMap<QString,QString>& oldProps,
            const QMap<QString,QString>& newProps,
            ProjectWindow* pw, MainWindow* mw );

    /**
     * dtor
     */
    virtual ~ServerLoggingPropsChangedCommand();

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

private:
    struct ServerLoggingPropsChangedCommandPrivate;
    ServerLoggingPropsChangedCommandPrivate* d;
};

#endif /* _SERVERLOGGINGPROPSCHANGEDCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ServerLoggingPropsChangedCommand.hh 1564 2006-04-07 13:33:15Z modesto $
