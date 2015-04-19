// $Id: ProjectPanelsDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectPanelsDialog.hh --
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

#ifndef _LTEDITORPROJECTPANELSDIALOG_HH
#define _LTEDITORPROJECTPANELSDIALOG_HH

#include "ProjectPanelsDialogBase.h"
#include "CommandType.hh"

class MainWindow;
class ProjectWindow;

/**
 * TODO
 */
class ProjectPanelsDialog : public ProjectPanelsDialogBase
{

    friend class MainWindow;

    Q_OBJECT

public:
    /**
     * TODO
     */
    ProjectPanelsDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );
    /**
     * TODO
     */
    virtual ~ProjectPanelsDialog();

public slots:
    /**
     * TODO
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

protected slots:
    /**
     * TODO
     */
    virtual void newPanelSlot();

    /**
     * TODO
     */
    virtual void duplicatePanelSlot();

    /**
     * TODO
     */
    virtual void deletePanelSlot();

    /**
     * TODO
     */
    virtual void panelUpSlot();

    /**
     * TODO
     */
    virtual void panelDownSlot();

    /**
     * TODO
     */
    virtual void selectionChangedSlot();

    /**
     * TODO
     */
    virtual void itemRenamedSlot( QListViewItem* i, int col );

    /**
     * Called when user doubleclicks a panel name in list of panels.
     * \param i The doubleclicked panel.
     */
    virtual void panelDoubleClickedSlot( QListViewItem* i );

protected:
    /**
     * Select the view and ensures its visible. Take care of null item.
     */
    void showPanelItem( QListViewItem* i );

private:
    struct ProjectPanelsDialogPrivate;
    ProjectPanelsDialogPrivate* d;
};

#endif /* _LTEDITORPROJECTPANELSDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPanelsDialog.hh 1169 2005-12-12 15:22:15Z modesto $
