// $Id: ProjectVariablesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectVariablesDialog.hh -- 
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

#ifndef _LTEDITORPROJECTVARIABLESDIALOG_HH
#define _LTEDITORPROJECTVARIABLESDIALOG_HH

#include "ProjectVariablesDialogBase.h"

#include "CommandType.hh"

class ProjectWindow;
class MainWindow;

/**
 * Project Variables Dialog allows Frank to add/remove variables to/from the
 * currently selected project connection.
 */
class ProjectVariablesDialog :
    public ProjectVariablesDialogBase
{
    Q_OBJECT

public:
    /**
     * ctor
     */
    ProjectVariablesDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );
    /**
     * dtor
     */
    virtual ~ProjectVariablesDialog();

public slots:
    /**
     * TODO
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

protected slots:
    /**
     * Called when user clicks the <New> button.
     */
    virtual void newVariableSlot();

    /**
     * Called when user clicks the <Edit> button.
     */
    virtual void editVariableSlot();

    /**
     * Called when user clicks the <Delete> button.
     */
    virtual void deleteVariableSlot();

    /**
     * Called when a selection in m_variables changed.
     */
    virtual void selectionChanged();

    /**
     * Called when an item is renamed.
     */
    virtual void itemRenamed( QListViewItem* i, int col );

    /**
     * Called when the user selects a Connection.
     */
    virtual void connectionChangedSlot(const QString&);

    /**
     * Called when user doubleclick a variable
     */
    virtual void variableDoubleClicked(QListViewItem*);

    /**
     * Called when user clicks the Edit Connections button.
     */
    virtual void editConnectionsSlot();

private:
    struct ProjectVariablesDialogPrivate;
    ProjectVariablesDialogPrivate* d;
};

#endif /* _LTEDITORPROJECTVARIABLESDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectVariablesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
