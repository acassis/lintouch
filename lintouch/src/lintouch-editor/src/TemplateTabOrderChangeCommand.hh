// $Id: TemplateTabOrderChangeCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateTabOrderChangeCommand.hh --
// AUTHOR: abdul <henry@swac.cz>
//   DATE: 24th August 2004
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

#ifndef _LTEDITORTEMPLATETABORDERCHANGECOMMAND_HH
#define _LTEDITORTEMPLATETABORDERCHANGECOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * This command class provides redo/undo support for
 * change in taborder of Templates in a panel.
 */

class TemplateTabOrderChangeCommand : public Command
{
public:
    /**
     * Constructs TemplateTabOrderChangeCommand.
     * \param oldName  The name of the template at the new position
     * \param newName The name of the template at the old position.
     */
    TemplateTabOrderChangeCommand( QString oldName,
            QString newName, ProjectWindow* pw,
            MainWindow* mw );

    /**
     * Destroys the class
     */
    virtual ~TemplateTabOrderChangeCommand();

    /**
     * The selected template is moved one position up / down
     * the hierarchy line depending upon the nature of operation
     * carried out by the user. The type of operation ie moveUp or 
     * moveDown is kept track off with the help of the flag d->flagMoveUp
     */
    virtual void redo();

    /**
     * The selected template is reverted back to its old position
     * in the hierarcy line.
     */
    virtual void undo();

    /**
     * \returns class' RTTI id
     */
    virtual CommandType rtti() const;

    /**
     * \returns The view name of the selected template.
     */
    QString viewName();

    /**
     * \returns The panel name of the selected template.
     */
    QString panelName();

    /**
     * \returns The template selected by user for moveUp / moveDown
     * operation.
     */
    QString newTemplate();

    /**
     *\returns The template at the position, the selected template is moved
     * to
     */
    QString oldTemplate();

    /**
     * \returns The position in the hierarcy line the selected template is
     * moved to.
     */
     int newPos();

    /**
     * \returns The position in the hierarcy line the selected template is
     * moved from.
     */
     int oldPos();

private:
    typedef struct TemplateTabOrderChangeCommandPrivate;
    TemplateTabOrderChangeCommandPrivate* d;
};

#endif /* _LTEDITORTEMPLATETABORDERCHANGECOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateTabOrderChangeCommand.hh 1169 2005-12-12 15:22:15Z modesto $


