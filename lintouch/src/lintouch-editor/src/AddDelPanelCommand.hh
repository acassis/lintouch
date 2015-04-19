// $Id: AddDelPanelCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddDelPanelCommand.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 25 August 2004
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

#ifndef _LTEDITORADDDELPANELCOMMAND_HH
#define _LTEDITORADDDELPANELCOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class AddDelPanelCommand : public Command
{
public:
    /**
     * ctor. Adds a new panel with default generic name.
     */
    AddDelPanelCommand( ProjectWindow* pw,
            MainWindow* mw );

    /**
     * ctor for duplicate
     * \param panelName A name of panel to duplicate
     */
    AddDelPanelCommand( const QString& panelName,
            ProjectWindow* pw, MainWindow* mw );

    /**
     * ctor for delete
     * \param panelName A name of panel to delete.
     * \param del Any value to delete the panel. Used only for distinguish
     * ctors between each other.
     */
    AddDelPanelCommand( const QString& panelName, bool del,
            ProjectWindow* pw,
            MainWindow* mw );

    /**
     * dtor
     */
    virtual ~AddDelPanelCommand();

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
     * Return name of the panel that has been added.
     * \Returns name of the panel that has been added.
     */
    QString panelName() const;

    /**
     * Returns potision of template. Usefull for delete/undo only. Returns
     * -1 otherwise.
     */
    int position() const;

    /**
     * Return name of the view that has been the panel added to.
     * \Returns name of the view that has been the panel added to.
     */
    QString viewName() const;

private:
    struct AddDelPanelCommandPrivate;
    AddDelPanelCommandPrivate* d;
};

#endif /* _LTEDITORADDDELPANELCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelPanelCommand.hh 1169 2005-12-12 15:22:15Z modesto $
