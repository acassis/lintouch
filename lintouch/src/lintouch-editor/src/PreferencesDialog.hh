// $Id: PreferencesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: PreferencesDialog.hh -- 
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

#ifndef _LTEDITORPREFERENCESDIALOG_HH
#define _LTEDITORPREFERENCESDIALOG_HH

#include "PreferencesDialogBase.h"

/**
 * This non modal dialog class allows configuration of permanent editor
 * settings. This dialog can be shown all the time while the editor is
 * running without blocking the input into other dialogs and windows.
 */
class PreferencesDialog : public PreferencesDialogBase
{
    Q_OBJECT

public:
    /**
     * Constructor
     */
    PreferencesDialog( QWidget* parent = 0, const char* name = 0,
            bool modal = false, WFlags fl = 0 );

    /**
     * Destructor
     */
    virtual ~PreferencesDialog();

#ifdef WE_CAN_DO_ON_TOP
    /**
     * This returns whether the Editor main window and all project
     * dependent windows stay above Editor Project window
     */
    bool dialogsOnTop() const;
#endif

#ifdef QTUNDO_SUPPORTS_LIMIT
    /**
     * This returns the number of undo redo operations allowed for each
     * projects
     */
    int undoLevels() const;
#endif

    /**
     * This returns the number of recent projects opened that will be
     * remembered 
     */
    int recentFiles() const;

    /**
     * This returns the horizental distance (in pixels) between
     * successive grid lines
     */
    int gridXSpacing() const;

    /**
     * This returns the vertical distance (in pixels) between 
     * successive grid lines
     */
    int gridYSpacing() const;

    /**
     * This returns every highlighted grid lines to be painted with
     * thicker pen
     */
    int gridHighlight() const;

    /**
     * This returns the color of the grid lines
     */
    QColor gridColor() const;

    /**
     * This returns the author of the project
     */
    QString projectAuthor() const;

    /**
     * This returns the size of the Project view
     */
    QSize projectViewSize() const;

    /**
     * This returns whether grid is shown in project window or not
     */
    bool projectShowGrid() const;

    /**
     * This returns whether operation on the panel are according to the
     * grid or not
     */
    bool projectSnapToGrid() const;

    /**
     * Returns the set zoom level for the project
     */
    int projectZoomLevel() const;

    /**
     * This appends opened project to the MRU list
     * \This also saves the path to the disc
     */
    void addRecentProject( const QString & filename );

    /**
     * This returns MRU list of opened project(s)
     */
    QStringList recentProjects() const;

signals:

    /**
     * emitted when settings are changed.
     */
    void preferencesChanged();

protected:
    /**
    * Sorts the keys of MRU saved in discs in accordance
    * to the order in which the project are opened ie the
    * File10 after File9 
    **/
    void sortKeys( QStringList& lstKeys );

    /**
    * Extracts the File number from the key of MRU
    * saved in disc
    */
    int getFileNumber ( QString key );

    protected slots:
    /**
    * Save all types( UI, Grid, Project defaults ) of settings
    */
    virtual void saveSettings();

    /**
    * Loads all types( UI, Grid, Project defaults ) of settings
    * from the external storage
    */
    virtual void loadSettings();

    /**
    * Sets the color of the grid lines. The color is selected with the help
    * of a QColorDialog
    */
    virtual void setGridColor();

    private:
    typedef struct PreferenceDialogPrivate;
    PreferenceDialogPrivate * d;    
};

#endif /* _LTEDITORPREFERENCESDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: PreferencesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
