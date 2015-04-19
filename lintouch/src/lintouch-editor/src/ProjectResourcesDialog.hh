// $Id: ProjectResourcesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectResourcesDialog.hh --
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

#ifndef _LTEDITORPROJECTRESOURCESDIALOG_HH
#define _LTEDITORPROJECTRESOURCESDIALOG_HH

#include "ProjectResourcesDialogBase.h"
#include "CommandType.hh"

class MainWindow;
class ProjectWindow;

/**
 * Dialog to maintain resources in a project.
 */
class ProjectResourcesDialog :
    public ProjectResourcesDialogBase
{

    friend class MainWindow;

    Q_OBJECT

public:
    /**
     * Constructor.
     */
    ProjectResourcesDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );
    /**
     * Destructor.
     */
    virtual ~ProjectResourcesDialog();

public slots:
    /**
     * Notify when project changes.
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

protected slots:
    /**
     * When apply button is pressed.
     */
    void applySlot();

    /**
     * When ok button is pressed.
     */
    void okSlot();

    /**
     * When new resource is added.
     */
    virtual void newResourceSlot();

    /**
     * When an existing resource is reimported.
     */
    virtual void reimportResourceSlot();

    /**
     * When an existing resource is exported.
     */
    virtual void exportResourceSlot();

    /**
     * When an existing resource is deleted.
     */
    virtual void deleteResourceSlot();

    /**
     * When resource name is changed.
     */
    virtual void itemRenamed( QListViewItem* i, int c);

    /**
     * When a resource is selected.
     */
    virtual void selectionChanged();

protected:
    /**
     * Refresh table from resource pool.
     */
    void refreshFromResourcePool();

    /**
     * TODO
     */
    void dumpResourceContents(const QString& header = tr(""), const QString&
            footer = tr(""));
private:
    struct ProjectResourcesDialogPrivate;
    ProjectResourcesDialogPrivate* d;
};

#endif /* _LTEDITORPROJECTRESOURCESDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectResourcesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
