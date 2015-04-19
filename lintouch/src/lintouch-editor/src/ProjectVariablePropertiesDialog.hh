// $Id: ProjectVariablePropertiesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectVariablePropertiesDialog.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 29 September 2004
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

#ifndef _LTEDITORPROJECTCONNECTIONPROPERTIESDIALOG_HH
#define _LTEDITORPROJECTCONNECTIONPROPERTIESDIALOG_HH

#include "ProjectVariablePropertiesDialogBase.h"

#include <qstringlist.h>

class ProjectWindow;
class MainWindow;

namespace lt {
    class Variable;
}

/**
 *  Project Variables Properties Dialog allows Frank to specify properties
 *  and type of the newly created connection and to change properties of
 *  existing connection. The dialog is modal.
 */
class ProjectVariablePropertiesDialog :
    public ProjectVariablePropertiesDialogBase
{
    Q_OBJECT

public:
    /*
     * ctor
     */
    ProjectVariablePropertiesDialog( QWidget* parent,
            bool modal = true);
    /*
     * dtor
     */
    virtual ~ProjectVariablePropertiesDialog();

    /**
     * Starts the dialog as modal.
     * \param ct The connection name
     * \returns QDialog::Accepted or QDialog::Rejected
     */
    int runNew(const QString& cn, ProjectWindow* pw, MainWindow* mw);

    /**
     * Starts the dialog as modal.
     * \param ct The connection name
     * \param vn The variable name
     * \returns QDialog::Accepted or QDialog::Rejected
     */
    int runEdit(const QString& cn, const QString& vn,
            ProjectWindow* pw, MainWindow* mw);

protected slots:
    /**
     * Called when user clicks <Ok> button. Saves the changes and closes the
     * dialog.
     */
    virtual void accept();

    /**
     * Called when user clicks <Apply> button.
     */
    virtual void apply();

    /**
     * Called when user selects a type from Variable type combobox.
     */
    virtual void typeChangedSlot(const QString& t);

    /**
     * Called when a connection property's value has changed.
     */
    void propertyChangedSlot(const QString& vn, const QString& p,
            const QString& v);

    /**
     * Override done to actually execute add-connection command here.
     */
    virtual void done(int r);

signals:
    /**
     * Signals all property edits to finish thier editation and save their
     * values.
     */
    void finishEdit();

private:
    struct ProjectVariablePropertiesDialogPrivate;
    ProjectVariablePropertiesDialogPrivate* d;
};

#endif /* _LTEDITORPROJECTCONNECTIONPROPERTIESDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectVariablePropertiesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
