// $Id: ProjectViewsDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectViewsDialog.hh --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 14 July 2004
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

#ifndef _LTEDITORPROJECTVIEWSDIALOG_HH
#define _LTEDITORPROJECTVIEWSDIALOG_HH

#include "ProjectViewsDialogBase.h"

#include "CommandType.hh"

class ProjectWindow;
class MainWindow;

/**
 * TODO
 */
class ProjectViewsDialog : public ProjectViewsDialogBase
{

    friend class MainWindow;

    Q_OBJECT

public:
    /**
     * TODO
     */
    ProjectViewsDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );
    /**
     * TODO
     */
    virtual ~ProjectViewsDialog();

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
    virtual void newViewSlot();

    /**
     * TODO
     */
    virtual void duplicateViewSlot();

    /**
     * TODO
     */
    virtual void resizeViewSlot();

    /**
     * TODO
     */
    virtual void deleteViewSlot();

    /**
     * TODO
     */
    virtual void viewUpSlot();

    /**
     * TODO
     */
    virtual void viewDownSlot();

    /**
     * TODO
     */
    virtual void selectionChanged();

    /**
     * TODO
     */
    virtual void itemRenamed( QListViewItem* i, int col );

    /**
     * Called when user doubleclicks list of views in this dialog.
     * \param i The doubleclicked item.
     */
    virtual void viewDoubleClickedSlot( QListViewItem* i );

protected:
    /**
     * TODO
     */
    void refreshByViewMap();

    /**
     * Select the view and ensures its visible. Take care of null item.
     */
    void showViewItem( QListViewItem* i );

private:
    struct ProjectViewsDialogPrivate;
    ProjectViewsDialogPrivate* d;
};

#endif /* _LTEDITORPROJECTVIEWSDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectViewsDialog.hh 1169 2005-12-12 15:22:15Z modesto $
