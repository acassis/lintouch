// $Id: PanelOrderChangeCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: PanelOrderChangeCommand.hh -- 
// AUTHOR: abdul <henry@swac.cz>
//   DATE: 2nd September 2004
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

#ifndef _LTEDITORPANELORDERCHANGECOMMAND_HH
#define _LTEDITORPANELORDERCHANGECOMMAND__HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * This command class provides redo/undo support for
 * change in order of panel in a view.
 */

class PanelOrderChangeCommand : public Command
{
public:
    /**
     * Constructs PanelOrderChangeCommand.
     * \oldName  is the name of the panel at the new position
     * \newName is the name of the panel at the old position. 
     */
    PanelOrderChangeCommand( QString oldName,
            QString newName, ProjectWindow* pw, 
            MainWindow* mw );

    /**
     * Destroys the class
     */
    virtual ~PanelOrderChangeCommand();

    /**
     * TODO
     */
    virtual void redo();

    /**
     * TODO
     */
    virtual void undo();

    /**
     * returns class' RTTI id
     */
    virtual CommandType rtti() const;

    /**
     *TODO
     */
    QString viewName();

    /**
     *TODO
     */
    QString newPanel();

    /**
     *TODO
     */
    QString oldPanel();

    /**
     * TODO
     */
     int newPos();

    /**
     * TODO
     */
     int oldPos();

private:
    typedef struct PanelOrderChangeCommandPrivate;
    PanelOrderChangeCommandPrivate* d;
};

#endif /* _LTEDITORPANELORDERCHANGECOMMAND__HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelOrderChangeCommand.hh 1169 2005-12-12 15:22:15Z modesto $
