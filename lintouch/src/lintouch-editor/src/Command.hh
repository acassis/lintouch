// $Id: Command.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: Command.hh -- 
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 26 July 2004
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

#ifndef _LTEDITORCOMMAND_HH
#define _LTEDITORCOMMAND_HH

#include <lt/ui/Undo.hh>
#include <qmap.h>

#include "CommandType.hh"

class MainWindow;
class ProjectWindow;


/**
 * TODO
 */
class Command : public lt::UndoCommand
{
public:
    /**
     * TODO
     */
    Command( ProjectWindow* pw, MainWindow* mw,
            const QString& description = QString::null,
            bool canMerge = false );

    virtual ~Command();

    //TODO: The API

    /**
     * RTTI identifies the particular class with easy enum-definned type.
     * \returns class' RTTI id
     */
    virtual CommandType rtti() const = 0;

    /**
     * Sets this command as the last command that happend on the
     * ProjectWindow and calls MainWindow's slot that
     * informs all dialogs about that an command has been performed.
     *
     * \param type The type of the command as defined in
     * CommandType.
     */
    virtual void notifyMainWindow( CommandType type );

public: //static
    /**
     * Generates safe name from pattern by replacing %1 with increasing
     * number from startAt until unique name is found.
     *
     * \param pattern Pattern where %1 replaced
     * \param startAt Starting point/number for replace
     * \param existing A list of existing names
     */
    static QString generateSafeName( const QString& pattern, int startAt,
            const QMap<QString,int>& existing );

protected:
    /**
     * TODO
     */
    ProjectWindow* m_pw;

    /**
     * TODO
     */
    MainWindow* m_mw;
};

#endif /* _LTEDITORCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Command.hh 1169 2005-12-12 15:22:15Z modesto $
