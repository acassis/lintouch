// $Id: AddDelSelectionCommand.hh 1193 2005-12-22 10:58:02Z modesto $
//
//   FILE: AddDelSelectionCommand.hh -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 02 September 2004
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

#ifndef _LTEDITORADDDELSELECTIONCOMMAND_HH
#define _LTEDITORADDDELSELECTIONCOMMAND_HH

#include <lt/templates/Template.hh>

#include "Command.hh"
#include "Bindings.hh"

class ProjectWindow;
class MainWindow;

/**
 * Undo/Redo command that adds one or more templates to the panel or deletes
 * them from the panel.
 */
class AddDelSelectionCommand : public Command
{
    public:

        /**
         * Constructor. Adds given Templates to the panel.
         */
        AddDelSelectionCommand(
                const TemplateDict & templates,
                const BindingsList& bl,
                ProjectWindow* pw,
                MainWindow* mw );

        /**
         * Constructor. Deletes current selection from the panel.
         */
        AddDelSelectionCommand(
                ProjectWindow* pw,
                MainWindow* mw );

        /**
         * Destructor.
         */
        virtual ~AddDelSelectionCommand();

        /**
         * Applies the change.
         */
        virtual void redo();

        /**
         * Undoes the change.
         */
        virtual void undo();

        /**
         * RTTI
         */
        virtual CommandType rtti() const;

        /**
         * Returns the name of the panel where templates has been
         * added/deleted.
         */
        QString panelName() const;

        /**
         * Returns the name of the view where templates has been
         * added/deleted.
         */
        QString viewName() const;

        /**
         * Templates added or deleted to/from the panel.
         */
        const TemplateDict & templates() const;

        /**
         * Templates that should be selected.
         */
        const TemplateDict & selectedTemplates() const;

    private:

        struct AddDelSelectionCommandPrivate;
        AddDelSelectionCommandPrivate* d;
};

#endif /* _LTEDITORADDDELSELECTIONCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelSelectionCommand.hh 1193 2005-12-22 10:58:02Z modesto $
