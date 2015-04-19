// $Id: ProjectWindow.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectWindow.hh --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 13 July 2004
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

#ifndef _LTEDITORPROJECTWINDOW_HH
#define _LTEDITORPROJECTWINDOW_HH

#include <qfileinfo.h>

#include <lt/LTMap.hh>
#include <lt/templates/Template.hh>
#include <lt/configuration/DefaultConfiguration.hh>

#include "CommandType.hh"
#include "ProjectWindowBase.h"

using namespace lt;

class QMessageBox;
class QPopupMenu;
class ProjectWindow;
class MainWindow;
class Command;
class QMouseEvent;

namespace lt
{
    class EditorProject;
}

using namespace lt;

/**
 * TODO
 */
class ProjectWindow : public ProjectWindowBase
{
    Q_OBJECT

public:
    /**
    * TODO
    */
    ProjectWindow( EditorProject* p, QWidget* parent, MainWindow* mw,
            double zoom = 1.0, LoggerPtr logger = Logger::nullPtr(),
            const char* name = 0,
            WFlags fl = Qt::WType_TopLevel | Qt::WDestructiveClose );
    /**
     * TODO
     */
    virtual ~ProjectWindow();

    /**
     * TODO
     */
    virtual bool close( bool alsoDelete );

    /**
     * TODO
     */
    EditorProject* project() const;

    /**
     * Returns the name of the currently selected view. QString::null when
     * there is no view at all.
     */
    QString currentView() const;

    /**
     * Returns the name of the currently selected panel. QString::null when
     * there is no view a/o panel at all.
     */
    QString currentPanel() const;

    /**
     * TODO
     */
    void setProjectFileInfo( QFileInfo fi );

    /**
     * TODO
     */
    QFileInfo projectFileInfo() const;

    /**
     * Returns unsaved flag of the project. True for unsaved, false for
     * clean.
     */
    bool unsaved() const;

    /**
     * Clears the undo stack and set the project unsaved.
     */
    void forceUnsaved();

public:
    /**
     * Set the grid color for this project window.
     */
    void setGridColor( const QColor & color );

    /**
     * Return the grid color of this project window.
     */
    QColor gridColor() const;

    /**
     * Set the horizontal grid spacing for this project window.
     */
    void setGridXSpacing( unsigned spacing );

    /**
     * Return the horizontal grid spacing of this project window.
     */
    unsigned gridXSpacing() const;

    /**
     * Set the vertical grid spacing for this project window.
     */
    void setGridYSpacing( unsigned spacing );

    /**
     * Return the vertical grid spacing of this project window.
     */
    unsigned gridYSpacing() const;

    /**
     * Set the grid highlight for this project window.
     * Every Nth line of the grid will be painted by thicker pen.
     */
    void setGridHighlight( unsigned n );

    /**
     * Return the grid hightlight of this project window.
     */
    unsigned gridHighlight() const;

    /**
     * Show/Hide the grid in this project window.
     */
    void setGridVisible( bool on );

    /**
     * Is the grid in this project window visible?.
     */
    bool gridVisible() const;

    /**
     * Activate/Deactivate Snap To Grid in this project window.
     * Grid can be hidden while snap to grid is activated.
     */
    void setGridSnap( bool on );

    /**
     * Is the Snap To Grid in this project window activated?.
     */
    bool gridSnap() const;

    /**
     * Adds a Command into Undo Stack. Use this method to put new
     * Command on stack. Do NOT access the stack directly in
     * EditorProject besauce it may move and this method always knows where
     * to find the Stack.
     *
     * \param cmd The command to add.
     */
    void executeCommand( Command* cmd );

    /**
     * Adds a Command into Undo Stack and executes it on
     * QTimer::singleShot(0) time. Use this method to execute commands that
     * can't be executed imediately. Like Rename* commands.
     *
     * \param cmd The command to add.
     */
    void executeCommandLater( Command* cmd );

    /**
     * Returns the last UndoCommand that has been performed upon this
     * ProjectWindow. It may be NULL, if there has not been any
     * commmand performed yet.
     *
     * \returns the last UndoCommand that has been performed upon this
     * ProjectWindow. It may be NULL, if there has not been any
     * commmand performed yet.
     */
    Command* lastCommand() const;

    /**
     * Sets the given Command as the last command performed upon the
     * ProjectWindow.
     *
     * \param cmd The last command.
     * \see lastCommand()
     */
    void setLastCommand( Command* cmd );

    /**
     * Gather all saveable information of Editor to serializeable
     * ConfigurationPtr class.
     */
    ConfigurationPtr saveEditorSettings() const;

    /**
     * Gather all saveable information of Editor to serializeable
     * ConfigurationPtr class.
     */
    void applyEditorSettings(ConfigurationPtr es);

public slots:
    /**
     * TODO
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

    /**
     * Display given view, notify all dialog of this change.
     * \param viewName The view name to display.
     */
    void displayView( const QString& viewName );

    /**
     * Display given panel, notify all dialog of this change. Does nothing
     * if the panel doesn't exist in current view.
     * \param panelName The panel name to display.
     */
    void displayPanel( const QString& panelName );

    /**
     * Connected to Project/Save menu item.
     */
    bool saveProjectSlot();

protected slots:

    /**
     * Connected to Project/Save As menu item.
     */
    bool saveProjectAsSlot();

    /**
     * Connected to Project/Revert to Saved menu item.
     */
    bool revertToSavedSlot();

    /**
     * Connected to Edit/Cut menu item.
     */
    void cutSlot();

    /**
     * Connected to Edit/Copy menu item.
     */
    void copySlot();

    /**
     * Connected to Edit/Paste menu item.
     */
    void pasteSlot();

    /**
     * Connected to Edit/Duplicate menu item.
     */
    void duplicateSlot();

    /**
     * Connected to Edit/Delete menu item.
     */
    void deleteSlot();

    /**
     * Connected to Edit/Select All menu item.
     */
    void selectAllSlot();

    /**
     * Connected to Edit/Unselect All menu item.
     */
    void unselectAllSlot();

    /**
     * Connected to Edit/Invert Selection menu item.
     */
    void invertSelectionSlot();

    /**
     * Connected to View/Zoom In menu item.
     */
    void zoomInSlot();

    /**
     * Connected to View/Zoom Out menu item.
     */
    void zoomOutSlot();

    /**
     * Connected to EPVC.
     */
    void zoomInSlotP( const QPoint & );

    /**
     * Connected to EPVC.
     */
    void zoomOutSlotP( const QPoint & );

    /**
     * Connected to View/Fit to Window menu item.
     */
    void zoomFitToWindowSlot();

    /**
     * Connected to View/Zoom/25% menu item.
     */
    void zoom25Slot();

    /**
     * Connected to View/Zoom/50% menu item.
     */
    void zoom50Slot();

    /**
     * Connected to View/Zoom/100% menu item.
     */
    void zoom100Slot();

    /**
     * Connected to View/Zoom/200% menu item.
     */
    void zoom200Slot();

    /**
     * Connected to View/Zoom/400% menu item.
     */
    void zoom400Slot();

    /**
     * Connected to Selection/One Layer Up menu item.
     */
    void layerUpSlot();

    /**
     * Connected to Selection/One Layer Down menu item.
     */
    void layerDownSlot();

    /**
     * Connected to Selection/Brig to Front menu item.
     */
    void bringToFrontSlot();

    /**
     * Connected to Selection/Send to Back menu item.
     */
    void sendToBottomSlot();

    /**
     * Connected to Selection/Align/Left menu item.
     */
    void alignLeftSlot();

    /**
     * Connected to Selection/Align/Center menu item.
     */
    void alignCenterSlot();

    /**
     * Connected to Selection/Align/Right menu item.
     */
    void alignRightSlot();

    /**
     * Connected to Selection/Align/Bottom menu item.
     */
    void alignBottomSlot();

    /**
     * Connected to Selection/Align/Middle menu item.
     */
    void alignMiddleSlot();

    /**
     * Connected to Selection/Align/Top menu item.
     */
    void alignTopSlot();

    /**
     * Connected to Selection/Align/Lay Out Horiz menu item.
     */
    void alignLayOutHrizontallySlot();

    /**
     * Connected to Selection/Align/Lay Out Vert menu item.
     */
    void alignLayOutVerticallySlot();

    /**
     * Connected to Selection/Align/Adjacent menu item.
     */
    void alignAdjacentSlot();

    /**
     * Connected to Selection/Align/Stacked menu item.
     */
    void alignStackedSlot();

    /**
     * Reorganize the contents of the window as a result of selection of
     * another view. Connected with view selection combo through the Base.ui
     * file.
     */
    virtual void viewSelectedSlot( const QString & viewName );

    /**
     * Reorganize the contents of the window as a result of selection of
     * another panel. Connected with EditablePanelViewContainer.
     */
    virtual void panelSelectedSlot( const QString & panelName );

    /**
     * Change zoom factor, connected to zoom selection combo.
     */
    virtual void zoomChangedSlot( const QString & factor );

    /**
     * Turn on/off the snap to grid feature. Connected with the snap to grid
     * check box through the Base.ui file.
     */
    virtual void snapToGridToggledSlot( bool on );

    /**
     * Selection at currentPanel() has been changed. connected to EPVC.
     */
    virtual void selectionChangedSlot();

    /**
     * Selection at currentPanel() has been double clicked. connected to
     * EPVC.
     */
    virtual void selectionDoubleClickedSlot(QMouseEvent*);

    /**
     * Selection at currentPanel() has been moved. connected to
     * EPVC.
     */
    virtual void selectionMovedSlot( const LTMap <QRect> & origRects );

    /**
     * Selection at currentPanel() has been resized. connected to
     * EPVC.
     */
    virtual void selectionResizedSlot( TemplateDict & templates,
            const LTMap <QRect> & origRects );

    /**
     * Template at currentPanel() has been placed (within Place mode).
     * connected to EPVC.
     */
    virtual void templatePlacedSlot( const QRect & pos );

    /**
     * Called when status of QtUndoStack changed, ie when is emptied of not.
     */
    void undoStackChangedSlot( bool );

    virtual void languageChange();

    /**
     * Called when a mouse release event happens in EPVC. Show global menu.
     */
    void showGlobalMenuSlot(QMouseEvent* e);

    /**
     * Executes one stored undo-command slot.
     */
    void executeCommandLaterSlot();

protected:
    /**
     * All events go through this method. We check for WindowActivate that
     * has no special method.
     *
     * \param e The event
     * \return Check Qt doc
     */
    virtual bool event( QEvent* e );

    /**
     * Popup menu on mouse release.
     */
    virtual void mouseReleaseEvent( QMouseEvent * e );


    /**
     * Filter events in preview && magnify mode.
     */
    virtual bool eventFilter( QObject * o, QEvent * e );

    /**
     * set up PW after setting d->project.
     */
    virtual void setup();

    /**
     * reload project and set to d->project, returns true if the reload is
     * successfull, false otherwise.
     */
    virtual bool reload();

signals:
    /**
     * TODO
     */
    void projectWindowActivated( ProjectWindow* );

protected:
    class ProjectWindowProtected;
    ProjectWindowProtected* d;

public:

    /**
     * Counter for numbering newly added views.
     */
    int numViews;
    /**
     * Counter for numbering newly added panels.
     */
    int numPanels;

    friend class LintouchMainWindow;
};

#endif /* _LTEDITORPROJECTWINDOW_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectWindow.hh 1169 2005-12-12 15:22:15Z modesto $
