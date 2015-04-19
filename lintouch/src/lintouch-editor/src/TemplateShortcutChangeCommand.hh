// $Id: TemplateShortcutChangeCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateShortcutChangeCommand.hh -- 
// AUTHOR: abdul <henry@swac.cz>
//   DATE: 4th September 2004
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

#ifndef _LTEDITORTEMPLATESHORTCUTCHANGECOMMAND_HH
#define _LTEDITORTEMPLATESHORTCUTCHANGECOMMAND_HH

#include <lt/templates/Template.hh>

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * This command class provides redo/undo support for
 * change in shortcut of Templates in panel.
 */

class TemplateShortcutChangeCommand : public Command
{
public:
    /**
     * Constructs TemplateShortcutChangeCommand. 
     */
    TemplateShortcutChangeCommand( const QString& selectedTemplate,
            const QString& shortCut , const QString& stolenFromTemplate,
            ProjectWindow* pw, MainWindow* mw );
    /**
     * Destroys the class
     */
    virtual ~TemplateShortcutChangeCommand();

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
     * returns the panel of the current template
     */
    QString panelName();

    /**
     * returns the view of the current template
     */
    QString viewName();

    /**
     * returns the current template
     */
    QString currentTemplateName();

    /**
     * returns the name of the template ( if any ) from which the
     * shortcut key was stollen 
     */
    QString stolenFromTemplateName();

    /**
     * This returns the old stortcut key
     */
    QString oldShortCutKey();

    /**
     * This returns the new shortcut key
     */
    QString newShortCutKey();

private:
    typedef struct TemplateShortcutChangeCommandPrivate;
    TemplateShortcutChangeCommandPrivate* d;
};

#endif /* _LTEDITORTEMPLATESHORTCUTCHANGECOMMAND_HH */


// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateShortcutChangeCommand.hh 1169 2005-12-12 15:22:15Z modesto $
