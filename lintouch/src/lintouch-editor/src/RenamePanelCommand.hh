// $Id: RenamePanelCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenamePanelCommand.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 26 August 2004
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

#ifndef _LTEDITORRENAMEPANELCOMMAND_HH
#define _LTEDITORRENAMEPANELCOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class RenamePanelCommand : public Command
{
public:
    /**
     * ctor
     */
    RenamePanelCommand( const QString oldPanelName,
            const QString newPanelName,
            ProjectWindow* pw,
            MainWindow* mw );

    /**
     * dtor
     */
    virtual ~RenamePanelCommand();

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
     * Return the OLD name of the Panel with respect to undo/redo. On redo
     * the old name is really the oldname, on undo the old name is in fact
     * the new name.
     * \returns the OLD name of the Panel with respect to undo/redo.
     */
    QString oldPanelName() const;

    /**
     * Return the NEW name of the Panel with respect to undo/redo. On redo
     * the old name is really the oldname, on undo the old name is in fact
     * the new name.
     * \returns the NEW name of the Panel with respect to undo/redo.
     */
    QString newPanelName() const;

    /**
     * Return name of the view that has been the panel added to.
     * \Returns name of the view that has been the panel added to.
     */
    QString viewName() const;

    /**
     * Return position of the renamed item from PanelMap.
     * \Returns position of the renamed item from PanelMap.
     */
    int position() const;

protected:
    /**
     * Do the actual rename.
     */
    void renamePanel(const QString& oldName, const QString& newName);

private:
    struct RenamePanelCommandPrivate;
    RenamePanelCommandPrivate* d;
};

#endif /* _LTEDITORRENAMEPANELCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenamePanelCommand.hh 1169 2005-12-12 15:22:15Z modesto $
