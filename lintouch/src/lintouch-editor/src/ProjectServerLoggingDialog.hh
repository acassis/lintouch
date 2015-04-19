// $Id: ProjectServerLoggingDialog.hh 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: ProjectServerLoggingDialog.hh -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: Fri, 07 Apr 2006
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

#ifndef _LTEDITORPROJECTSERVERLOGGINGDIALOG_HH
#define _LTEDITORPROJECTSERVERLOGGINGDIALOG_HH

#include "ProjectServerLoggingDialogBase.h"

#include "CommandType.hh"

class ProjectWindow;
class MainWindow;

/**
 * Dialog for per-Project Server Logging handling. You can add/delete/rename
 * and change a Server Logging here.
 */
class ProjectServerLoggingDialog :
    public ProjectServerLoggingDialogBase
{
    Q_OBJECT

public:
    /**
     * ctor
     */
    ProjectServerLoggingDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );
    /**
     * dtor
     */
    virtual ~ProjectServerLoggingDialog();

public slots:
    /**
     * TODO
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

protected slots:
    /**
     * Create a Server Logging, let the user to choose a type and change it's
     * properties.
     */
    virtual void newServerLoggingSlot();

    /**
     * Edit a Server Logging.
     */
    virtual void editServerLoggingSlot();

    /**
     * Delete a Server Logging, also delete all occurence of this Server
     * Logging in whole Project.
     */
    virtual void deleteServerLoggingSlot();

    /**
     * Called when a selection in m_Server Logging changed.
     */
    virtual void selectionChanged();

    /**
     * Called when an item is renamed.
     */
    virtual void itemRenamed( QListViewItem* i, int col );

    /**
     * Called when used doubleclicks the item.
     */
    virtual void serverLoggingDoubleclicked(QListViewItem* i);

protected:
    /**
     * Fill the listview with Server Logging from current EditorProject
     */
    void fillServerLogging();

private:
    struct ProjectServerLoggingDialogPrivate;
    ProjectServerLoggingDialogPrivate* d;
};

#endif /* _LTEDITORPROJECTSERVERLOGGINGDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectServerLoggingDialog.hh 1564 2006-04-07 13:33:15Z modesto $
