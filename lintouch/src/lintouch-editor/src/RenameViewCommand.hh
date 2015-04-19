// $Id: RenameViewCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenameViewCommand.hh -- 
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 06 August 2004
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

#ifndef _LTEDITORRENAMEVIEWCOMMAND_HH
#define _LTEDITORRENAMEVIEWCOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class RenameViewCommand : public Command
{
public:
    /**
     * ctor
     */
    RenameViewCommand( const QString oldViewName,
            const QString newViewName,
            ProjectWindow* pw,
            MainWindow* mw );

    /**
     * dtor
     */
    virtual ~RenameViewCommand();

    /**
     * TODO
     */
    virtual void redo();

    /**
     * TODO
     */
    virtual void undo();

    /**
     * RTTI
     */
    virtual CommandType rtti() const;

    /**
     * Return the OLD name of the view with respect to undo/redo. On redo
     * the old name is really the oldname, on undo the old name is in fact
     * the new name.
     * \returns the OLD name of the view with respect to undo/redo.
     */
    QString oldViewName() const;

    /**
     * Return the NEW name of the view with respect to undo/redo. On redo
     * the old name is really the oldname, on undo the old name is in fact
     * the new name.
     * \returns the NEW name of the view with respect to undo/redo.
     */
    QString newViewName() const;

    /**
     * Return position of the renamed item from ViewMap.
     * \Returns position of the renamed item from ViewMap.
     */
    int position() const;

protected:
    /**
     * Actually rename the view
     */
    void renameView(const QString& oldName, const QString& newName);

private:
    struct RenameViewCommandPrivate;
    RenameViewCommandPrivate* d;
};

#endif /* _LTEDITORRENAMEVIEWCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenameViewCommand.hh 1169 2005-12-12 15:22:15Z modesto $
