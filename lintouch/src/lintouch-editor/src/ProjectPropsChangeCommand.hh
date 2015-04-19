// $Id: ProjectPropsChangeCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectPropsChangeCommand.hh -- 
// AUTHOR: abdul <henry@swac.cz>
//   DATE: 10 August 2004
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

#ifndef _LTEDITORPROJECTPROPSCHANGECOMMAND_HH
#define _LTEDITORPROJECTPROPSCHANGECOMMAND_HH

#include <lt/templates/Info.hh>

#include "Command.hh"

using namespace lt;

class ProjectWindow;
class MainWindow;

/**
 * This command class provides redo/undo support for
 * change in project info.
 */
class ProjectPropsChangeCommand : public Command
{
public:
    /**
     * Constructs ProjectPropsChangeCommand.
     * \param oldInfo is the old project Info. 
     * \param newInfo is the changed project Info. 
     * \param pw points to the project window the info of whose project is
     * being changed. 
     * \param mw points to the editor main window.
     */
    ProjectPropsChangeCommand( const Info oldInfo,
            const Info newInfo, ProjectWindow* pw,
            MainWindow* mw );

    /**
     * Destroys the class
     */
    virtual ~ProjectPropsChangeCommand();

    /**
     * Sets newInfo as the new project Information
     */
    virtual void redo();

    /**
     * Resets oldInfo as the project Information
     */
    virtual void undo();

    /**
     * returns class' RTTI id
     */
    virtual CommandType rtti() const;

private:
    typedef struct ProjectPropsChangeCommandPrivate;
    ProjectPropsChangeCommandPrivate* d;
};

#endif /* _LTEDITORPROJECTPROPSCHANGECOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPropsChangeCommand.hh 1169 2005-12-12 15:22:15Z modesto $
