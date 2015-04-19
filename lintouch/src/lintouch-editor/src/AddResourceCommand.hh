// $Id: AddResourceCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddResourceCommand.hh --
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
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

#ifndef _LTEDITORADDRESOURCECOMMAND_HH
#define _LTEDITORADDRESOURCECOMMAND_HH
#include "Command.hh"

class ProjectWindow;
class MainWindow;
/**
 * Command class for adding resource.
 */
class AddResourceCommand:public Command
{
public:
    /**
    * Constructor.
    */
    AddResourceCommand( const QString& newResource,
            ProjectWindow* pw, MainWindow* mw );

    /**
    * Constructor.
    */
    AddResourceCommand( const QString& key, const QString& path,
            ProjectWindow* pw, MainWindow* mw );

    /**
    * Destructor.
    */
    ~AddResourceCommand();

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
     * Returns the resource key of the added resource.
     */
    QString resourceKey() const;

private:
    typedef struct AddResourceCommandPrivate;
    AddResourceCommandPrivate * d;
};

#endif /* _LTEDITORADDRESOURCECOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddResourceCommand.hh 1169 2005-12-12 15:22:15Z modesto $

