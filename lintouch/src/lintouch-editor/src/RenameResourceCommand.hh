// $Id: RenameResourceCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: RenameResourceCommand.hh --
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
//   DATE: 11th Aug 2004
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

#ifndef _LTEDITORRENAMERESOURCECOMMAND_HH
#define _LTEDITORRENAMERESOURCECOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;
/**
 * Command class to rename resource.
 */
class RenameResourceCommand:public Command
{
public:
    /**
     * Constructor.
     */
    RenameResourceCommand( const QString& oldName,
            const QString& newName, ProjectWindow* pw,
            MainWindow* mw );

    /**
     * Destructor.
     */
    virtual ~RenameResourceCommand();

    /**
     * Undo handler.
     */
    virtual void undo();

    /**
     * Redo handler.
     */
    virtual void redo();

    /**
     * Getter for run time type information.
     */
    virtual CommandType rtti() const;

    /**
     * Returns the old resource name
     */
    QString oldResourceName() const;

    /**
     * Returns the new resource name
     */
    QString newResourceName() const;

private:

    typedef struct RenameResourceCommandPrivate;
    RenameResourceCommandPrivate * d; 

};
#endif /* _LTEDITORRENAMERESOURCECOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: RenameResourceCommand.hh 1169 2005-12-12 15:22:15Z modesto $

