// $Id: ProjectGridSettingsDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectGridSettingsDialog.hh --
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

#ifndef _LTEDITORPROJECTGRIDSETTINGSDIALOG_HH
#define _LTEDITORPROJECTGRIDSETTINGSDIALOG_HH

#include "ProjectGridSettingsDialogBase.h"
#include "CommandType.hh"

class ProjectWindow;
class MainWindow;

/**
 * Using this class grid settings related to the currently focused project
 * can be changed.
 */
class ProjectGridSettingsDialog :
        public ProjectGridSettingsDialogBase
{

    friend class MainWindow;

    Q_OBJECT

public:
    /**
     * Constructs the class
     */
    ProjectGridSettingsDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );
    /**
     * Destroys this class
     */
     virtual ~ProjectGridSettingsDialog();

public slots:
    /**
     * This is called whenever there is a change in the currently focussed
     * project. The controls in this dialog are refreshed with the grid
     * settings of the newly focussed project.
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

protected slots:
    /**
     * This applies the changed settings
     */
    void applySlot();

    /**
     * This applies the changed settings and close the dialog
     */
    void okSlot();

    /**
    * This allows selection of grid lines color from QColorDialog
    */
    virtual void gridColorSlot();

private:
    struct ProjectGridSettingsPrivate;
    ProjectGridSettingsPrivate* d;

};

#endif /* _LTEDITORPROJECTGRIDSETTINGSDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectGridSettingsDialog.hh 1169 2005-12-12 15:22:15Z modesto $
