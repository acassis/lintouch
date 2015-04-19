// $Id: ResizeViewCommand.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ResizeViewCommand.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 01 September 2004
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

#ifndef _LTEDITORRESIZEVIEWCOMMAND_HH
#define _LTEDITORRESIZEVIEWCOMMAND_HH

#include "Command.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class ResizeViewCommand : public Command
{
public:
    /**
     * ctor for resize.
     *
     * \param viewName The name of view to resize.
     * \param nw New width.
     * \param nh New height.
     * \param pw ProjectWindow.
     * \param mw MainWindow.
     */
    ResizeViewCommand( const QString& viewName, int nw, int nh,
            ProjectWindow* pw, MainWindow* mw );

    /**
     * dtor
     */
    virtual ~ResizeViewCommand();

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
     * Returns old view width. Takes undo/redo direction into account.
     */
    int oldWidth() const;

    /**
     * Returns old view height. Takes undo/redo direction into account.
     */
    int oldHeight() const;

    /**
     * Returns new view width. Takes undo/redo direction into account.
     */
    int newWidth() const;

    /**
     * Returns new view height. Takes undo/redo direction into account.
     */
    int newHeight() const;

private:
    struct ResizeViewCommandPrivate;
    ResizeViewCommandPrivate* d;
};

#endif /* _LTEDITORRESIZEVIEWCOMMAND_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ResizeViewCommand.hh 1169 2005-12-12 15:22:15Z modesto $
