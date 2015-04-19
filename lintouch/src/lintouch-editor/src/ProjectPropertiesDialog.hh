// $Id: ProjectPropertiesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectPropertiesDialog.hh --
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

#ifndef _LTEDITORPROJECTPROPERTIESDIALOG_HH
#define _LTEDITORPROJECTPROPERTIESDIALOG_HH

#include <lt/templates/Info.hh>

#include "CommandType.hh"
#include "ProjectPropertiesDialogBase.h"

using namespace lt;

class MainWindow;
class ProjectWindow;

/**
 * This class can be used to change info of the currently focused
 * project. The change comes with undo/redo support.
 */
class ProjectPropertiesDialog :
    public ProjectPropertiesDialogBase
{

    friend class MainWindow;

    Q_OBJECT

public:
    /**
     * Constructs the dialog with the main editor window as its parent.
     */
    ProjectPropertiesDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );
    /**
     * Destroys the dialog.
     */
     virtual ~ProjectPropertiesDialog();

protected:
    /**
    * This function returns true if there is a change in the info of the
    * currently focussed project otherwise it returns false.
    */
    bool isDirty( Info oldInfo, Info newInfo );

public slots:
    /**
     * This displays the info of the currently focussed project.
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

protected slots:
    /**
     * This applies the changed info of the focussed project
     */
    void applySlot();

    /**
     * This applies the changed project info and closes the dialog
     */
    void okSlot();

private:
    struct ProjectPropertiesDialogPrivate;
    ProjectPropertiesDialogPrivate* d;

};

#endif /* _LTEDITORPROJECTPROPERTIESDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPropertiesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
