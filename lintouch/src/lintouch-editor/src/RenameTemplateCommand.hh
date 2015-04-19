// $Id: RenameTemplateCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenameTemplateCommand.hh -- 
// AUTHOR: abdul <henry@swac.cz>
//   DATE: 8th September 2004
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

#ifndef _LTEDITORRENAMETEMPLATECOMMAND_HH
#define _LTEDITORRENAMETEMPLATECOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class RenameTemplateCommand : public Command
{
public:
    /**
     * ctor
     */
    RenameTemplateCommand( const QString oldTemplateName,
            const QString newTemplateName,
            ProjectWindow* pw,
            MainWindow* mw );

    /**
     * dtor
     */
    virtual ~RenameTemplateCommand();

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
     * Return the OLD name of the Template with respect to undo/redo. 
     * On redo the old name is really the oldname, on undo the old name 
     * is in fact the new name.
     * \returns the OLD name of the Template with respect to undo/redo.
     */
    QString oldTemplateName() const;

    /**
     * Return the NEW name of the Template with respect to undo/redo. 
     * On redo the old name is really the oldname, on undo the old name 
     * is in fact the new name.
     * \returns the NEW name of the Template with respect to undo/redo.
     */
    QString newTemplateName() const;

    /**
     * Return name of the view that has the panel the template belongs to.
     * \Returns name of the view that has the panel the template belongs to.
     */
    QString viewName() const;

    /**
     * Return name of the the panel the template belongs to.
     * \Returns name of the panel the template belongs to.
     */
    QString panelName() const;

    /**
     * Return position of the renamed item from TemplateDict.
     * \Returns position of the renamed item from TemplateDict.
     */
    int position() const;

protected:
    /**
     * Do the actual rename.
     */
    void renameTemplate(const QString& oldName, const QString& newName);

private:
    struct RenameTemplateCommandPrivate;
    RenameTemplateCommandPrivate* d;
};

#endif /* _LTEDITORRENAMETEMPLATECOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenameTemplateCommand.hh 1169 2005-12-12 15:22:15Z modesto $
