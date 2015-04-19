// $Id: ProjectTemplatesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectTemplatesDialog.hh --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 15 July 2004
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

#ifndef _LTEDITORPROJECTTEMPLATESDIALOG_HH
#define _LTEDITORPROJECTTEMPLATESDIALOG_HH

#include <qdialog.h>

#include <lt/templates/Template.hh>

#include "CommandType.hh"
#include "ProjectTemplatesDialogBase.h"

class MainWindow;
class ProjectWindow;
class ProjectTemplatesDialog;

/**
 * This dialog captures the typed key combination and convert it
 * into a valid key sequence string that can be used as short cut keys
 */
class TemplatesShortCutKeyCapture : public QDialog
{

    Q_OBJECT

public:
    /**
     * Constructs a TemplatesShortCutKeyCapture with 
     * ProjectTemplatesDialog as parent.
     * \param keySequence points the string which contains the
     * captured short cut key combination
     */
    TemplatesShortCutKeyCapture(
            ProjectTemplatesDialog* parent,
            QString* keySequence, const char* name = 0 ,
            WFlags fl = 0 );

    /**
     * Destroys the dialog
     */
    virtual ~TemplatesShortCutKeyCapture( );

protected:
    /**
     * This receives the key press event and converts the key combination
     * typed into key sequence string that can be use as shortcut
     */
    virtual void keyPressEvent( QKeyEvent* e );

    /**
     * This closes the dialog when the dialog goes out of focus
     */
    virtual void focusOutEvent( QFocusEvent* );

private :
    typedef struct TemplatesShortCutKeyCapturePrivate;
    TemplatesShortCutKeyCapturePrivate* d;

};

/**
 * The ProjectTemplatesDialog class allows renaming of templates
 * at the current panel of the project. This dialog can also be used to 
 * change tab order and shortcut to templates.
 */
class ProjectTemplatesDialog :
        public ProjectTemplatesDialogBase
{

    friend class MainWindow;

    Q_OBJECT

public:
    /**
     * Constructs a ProjectTemplatesDialog
     */
    ProjectTemplatesDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );

    /**
     * Destroys the dialog
     */
    virtual ~ProjectTemplatesDialog();

    /**
     * Sets WFlags of this dialog to WStyle_StaysOnTop if 
     * param onTop is true else clear this style.
     */
    void bringDialogOnTop( bool onTop );

    /**
     * Called whenever user clicked a checkbox in the list of templates.
     */
    void changeTemplateLock(bool b, const QString& tn);

protected:
    /**
     * This function should be called to populate the panel combo with
     * the name of panels in the current view of the project. If there is
     * no current view or if there is no panel in the current view the 
     * panel combo is disabled
     */
    void populatePanelNames();

    /**
     * Clears panel name combo, template listview and disables all the
     * controls except the cancel button
     */
    void clearAndDisableControls();  

    /**
     * This finds the template having keySequence as shortcut. A null
     * string is returned if there is no such template
     */
    QString stealShortCutFromTemplate( QString keySequence );

    /**
     * Filters all events that goes to the QListView. We need to hack in and
     * enabled the F2 key for rename again
     */
    bool eventFilter( QObject *o, QEvent *e );

public slots:
    /**
     * This refreshes the panel combo when there is a change in current
     * project, or a change in current view of the current project. The
     * template list view is refreshed when a new panel in the current 
     * view is selected
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

protected slots:
    /**
     * This moves the selected template one line up in the focus order
     * hierarchy in the curent panel.
     */
    void moveUpSlot();

    /**
     * This moves the selected template one line down in the focus order
     * hierarchy in the curent panel.
     */
    void moveDownSlot();

    /**
    * This as input takes the name of a panel and populates the template
    * list view of the dialog with name and shortcut key sequence(if any)
    * of the templates in that panel
    */
    void panelSelectedSlot( const QString& panelName );

    /**
    * Enables / Disables movement of template one line up or one line down
    * the focus order hierarchy according to the position already occupied
    * by the template.
    */
    void templateSelectionChangedSlot( QListViewItem* lstViewItem );

    /**
     * This select the template in the panel view when col = 0
     * Initiate the change of shortcut key sequence of the
     * template when col=1.
     */
    void itemDoubleClickedSlot( QListViewItem* item,
            const QPoint& p ,int col );

    /**
     * This selects the template in the listview 
     */
    void itemClickedSlot( QListViewItem* item,
            const QPoint& p ,int col );

    /**
    * This validates the entered changed name or shortcut key sequence,
    * of the template and then implement the change.
    **/
    void itemRenamedSlot( QListViewItem* item, int col );

    /**
     * This internally call itemDoubleClickedSlot with col = 0 
     */
    void returnPressedSlot( QListViewItem* item);

private :
    typedef struct ProjectTemplatesDialogPrivate;
    ProjectTemplatesDialogPrivate* d;

};

#endif /* _LTEDITORPROJECTTEMPLATESDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectTemplatesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
