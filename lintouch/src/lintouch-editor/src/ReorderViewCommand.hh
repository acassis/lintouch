// $Id: ReorderViewCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ReorderViewCommand.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
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

#ifndef _LTEDITORREORDERVIEWCOMMAND_HH
#define _LTEDITORREORDERVIEWCOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class ReorderViewCommand : public Command
{
public:
    /**
     * ctor
     * \param viewName A name of view to delete.
     * \param up True if the view should be moved up, false if down.
     */
    ReorderViewCommand( const QString& viewName, bool up,
            ProjectWindow* pw,
            MainWindow* mw );

    /**
     * dtor
     */
    virtual ~ReorderViewCommand();

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
     * Return name of the view that has been moved up/down. Takes care of
     * undo/redo.
     */
    QString viewName() const;

    /**
     * Returns potision of view before the move.
     */
    int oldPosition() const;

    /**
     * Returns potision of view after the move. Takes care of undo/redo.
     */
    int newPosition() const;

private:
    struct ReorderViewCommandPrivate;
    ReorderViewCommandPrivate* d;
};

#endif /* _LTEDITORREORDERVIEWCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ReorderViewCommand.hh 1169 2005-12-12 15:22:15Z modesto $
