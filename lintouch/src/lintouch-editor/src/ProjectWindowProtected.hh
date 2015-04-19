// $Id: ProjectWindowProtected.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectWindowProtected.hh --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 12 July 2004
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

#ifndef _PROJECTWINDOWPROTECTED_HH
#define _PROJECTWINDOWPROTECTED_HH

class MainWindow;
#include <ProjectWindow.hh>

#define DEFAULT_GRID_COLOR          Qt::cyan
#define DEFAULT_GRID_XSPACING       10
#define DEFAULT_GRID_YSPACING       10
#define DEFAULT_GRID_HIGHLIGHT      10
#define DEFAULT_GRID_VISIBLE        false
#define DEFAULT_GRID_SNAP           false

struct ProjectWindow::ProjectWindowProtected {
    ProjectWindowProtected() : mw(NULL), undoStack( NULL ),
        quitDialog( NULL ), project( NULL ),
        projectWindowMenu( NULL ), popupDialogs(NULL), popupTools(NULL),
        gridColor( DEFAULT_GRID_COLOR ),
        gridXSpacing( DEFAULT_GRID_XSPACING ),
        gridYSpacing( DEFAULT_GRID_YSPACING ),
        gridHighlight( DEFAULT_GRID_HIGHLIGHT ),
        gridVisible( DEFAULT_GRID_VISIBLE ), gridSnap( DEFAULT_GRID_SNAP ),
        lastCommand( NULL ), constructed( false ), zoom( 1.0 ),
        logger( Logger::nullPtr() ), lateCmd(NULL)
    {
    }

    MainWindow* mw;

    /**
     * ProjectWindow own undo/redo stack
     */
    lt::UndoStack *undoStack;

    /**
     * TODO
     */
    QMessageBox* quitDialog;

    /**
     * TODO
     */
    EditorProject* project;

    /**
     * TODO
     */
    QPopupMenu* projectWindowMenu;
    QPopupMenu* popupDialogs;
    QPopupMenu* popupTools;

    ////////// EditorProject specific non-saved settings //////////

    /**
     * File information about the project file.
     */
    QFileInfo projectFileInfo;

    /**
     * The time the project file was last modified.
     */
    QDateTime projectLastModified;

    /**
     * Grid color for this project window
     */
    QColor gridColor;

    /**
     * Horizontal Grid spacing.
     */
    unsigned gridXSpacing;

    /**
     * Vertical Grid spacing.
     */
    unsigned gridYSpacing;

    /**
     * Hilight every Nth grid line.
     */
    unsigned gridHighlight;

    /**
     * Should the grid be painted?
     */
    bool gridVisible;

    /**
     * Should we snap to grid?
     */
    bool gridSnap;

    /**
     * Last UndoCommand that has been performed upon this PW. Set by a
     * Command itself in undo()/redo() method, get by lastCommand() method
     * of PW.
     */
    Command* lastCommand;

    /**
     * Flag inidicating we have not yet finished the construction.
     */
    bool constructed;

    /**
     * current zoom factor.
     */
    double zoom;

    /**
     * logger
     */
    LoggerPtr logger;

    /**
     * Undo command for late-execute.
     */
    Command* lateCmd;
};


#endif

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectWindowProtected.hh 1169 2005-12-12 15:22:15Z modesto $
