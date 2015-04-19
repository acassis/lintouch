// $Id: ProjectConnectionsDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectConnectionsDialog.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 27 September 2004
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

#ifndef _LTEDITORPROJECTCONNECTIONSDIALOG_HH
#define _LTEDITORPROJECTCONNECTIONSDIALOG_HH

#include "ProjectConnectionsDialogBase.h"

#include "CommandType.hh"

class ProjectWindow;
class MainWindow;

/**
 * Dialog for per-Project Connections handling. You can add/delete/rename
 * and change a Connection here.
 */
class ProjectConnectionsDialog :
    public ProjectConnectionsDialogBase
{
    Q_OBJECT

public:
    /**
     * ctor
     */
    ProjectConnectionsDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );
    /**
     * dtor
     */
    virtual ~ProjectConnectionsDialog();

public slots:
    /**
     * TODO
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

protected slots:
    /**
     * Create a connection, let the user to choose a type and change it's
     * properties.
     */
    virtual void newConnectionSlot();

    /**
     * Edit a connection.
     */
    virtual void editConnectionSlot();

    /**
     * When an existing connection is imported.
     */
    virtual void importConnectionSlot();

    /**
     * When an existing connection is reimported.
     */
    virtual void reimportConnectionSlot();

    /**
     * When an existing connection is exported.
     */
    virtual void exportConnectionSlot();

    /**
     * Delete a connection, also delete all occurence of this connection in
     * whole Project.
     */
    virtual void deleteConnectionSlot();

    /**
     * Called when a selection in m_connections changed.
     */
    virtual void selectionChanged();

    /**
     * Called when an item is renamed.
     */
    virtual void itemRenamed( QListViewItem* i, int col );

    /**
     * Called when used doubleclicks the item.
     */
    virtual void connectionDoubleclicked(QListViewItem* i);

protected:
    /**
     * Fill the listview with connections from current EditorProject
     */
    void fillConnections();

private:
    struct ProjectConnectionsDialogPrivate;
    ProjectConnectionsDialogPrivate* d;
};

#endif /* _LTEDITORPROJECTCONNECTIONSDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectConnectionsDialog.hh 1169 2005-12-12 15:22:15Z modesto $
