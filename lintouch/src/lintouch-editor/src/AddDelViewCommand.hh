// $Id: AddDelViewCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: AddDelViewCommand.hh -- 
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 27 July 2004
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

#ifndef _LTEDITORADDDELVIEWCOMMAND_HH
#define _LTEDITORADDDELVIEWCOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class AddDelViewCommand : public Command
{
public:
    /**
     * ctor. Adds a new view with default generic name.
     */
    AddDelViewCommand( ProjectWindow* pw,
            MainWindow* mw );

    /**
     * ctor for duplicate
     * \param viewName A name of view to duplicate
     */
    AddDelViewCommand( const QString& viewName,
            ProjectWindow* pw, MainWindow* mw );

    /**
     * ctor for delete
     * \param viewName A name of view to delete.
     * \param del Any value to delete the view. Used only for distinguish
     * ctors between each other.
     */
    AddDelViewCommand( const QString& viewName, bool del,
            ProjectWindow* pw,
            MainWindow* mw );

    /**
     * dtor
     */
    virtual ~AddDelViewCommand();

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
     * Return name of the view that has been added/duplicated.
     * \Returns name of the view that has been added/duplicated.
     */
    QString viewName() const;

    /**
     * Returns potision of template. Usefull for delete/undo only. Returns
     * -1 otherwise.
     */
    int position() const;

private:
    struct AddDelViewCommandPrivate;
    AddDelViewCommandPrivate* d;
};

#endif /* _LTEDITORADDDELVIEWCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: AddDelViewCommand.hh 1169 2005-12-12 15:22:15Z modesto $
