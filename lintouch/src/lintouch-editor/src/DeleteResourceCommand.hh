// $Id: DeleteResourceCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: DeleteResourceCommand.hh --
// AUTHOR: shivaji <shivaji@swac.cz>
//   DATE: 10th Aug 2004
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

#ifndef _LTEDITORDELETERESOURCECOMMAND_HH
#define _LTEDITORDELETERESOURCECOMMAND_HH
#include "Command.hh"

class ProjectWindow;
class MainWindow;
/**
 *Command for deleting resource.
 */
class DeleteResourceCommand:public Command
{
public:
    /**
     * Constructor.
     */
    DeleteResourceCommand( const QString& key,
        ProjectWindow* pw, MainWindow* mw );

    /**
     * Destructor.
     */
    virtual ~DeleteResourceCommand();

    /**
     * Undo handler.
     */
    virtual void undo();

    /**
     * Redo handler.
     */
    virtual void redo();

    /**
     * Getter for runtime type information.
     */
    virtual CommandType rtti() const;

private:
    typedef struct DeleteResourceCommandPrivate;
    DeleteResourceCommandPrivate * d;
};
#endif /* _LTEDITORDELETERESOURCECOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: DeleteResourceCommand.hh 1169 2005-12-12 15:22:15Z modesto $

