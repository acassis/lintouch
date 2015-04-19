// $Id: ProjectServerLoggingPropertiesDialog.hh 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: ProjectServerLoggingPropertiesDialog.hh -- 
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

#ifndef _LTEDITORPROJECTSERVERLOGGINGPROPERTIESDIALOG_HH
#define _LTEDITORPROJECTSERVERLOGGINGPROPERTIESDIALOG_HH

#include "ProjectServerLoggingPropertiesDialogBase.h"

#include <qstringlist.h>

class ProjectWindow;
class MainWindow;

namespace lt {
    class ServerLogging;
}

/**
 *  Project ServerLoggings Properties Dialog allows Frank to specify properties
 *  and type of the newly created ServerLogging and to change properties of
 *  existing ServerLogging. The dialog is modal.
 */
class ProjectServerLoggingPropertiesDialog :
    public ProjectServerLoggingPropertiesDialogBase
{
    Q_OBJECT

public:
    /*
     * ctor
     */
    ProjectServerLoggingPropertiesDialog(MainWindow* mw,
            QWidget* parent, bool modal = true);
    /*
     * dtor
     */
    virtual ~ProjectServerLoggingPropertiesDialog();

    /**
     * Starts the dialog as modal.
     * \param ec The new ServerLogging.
     * \returns QDialog::Accepted or QDialog::Rejected
     */
    int runNew(const QString& cn, ProjectWindow* pw);

    /**
     * Starts the dialog as modal.
     * \param ec The edited ServerLogging.
     * \returns QDialog::Accepted or QDialog::Rejected
     */
    int runEdit(const QString& cn, ProjectWindow* pw);

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
     * Called when user selects a type from ServerLogging type combobox.
     */
    virtual void typeChangedSlot(const QString& t);

    /**
     * Called when a ServerLogging property's value has changed.
     */
    void propertyChangedSlot(const QString& c, const QString& p,
            const QString& v);

    /**
     * Override done to actually execute add-ServerLogging command here.
     */
    virtual void done(int r);

signals:
    /**
     * Signals all property edits to finish thier editation and save their
     * values.
     */
    void finishEdit();

private:
    struct ProjectServerLoggingPropertiesDialogPrivate;
    ProjectServerLoggingPropertiesDialogPrivate* d;
};

#endif /* _LTEDITORPROJECTSERVERLOGGINGPROPERTIESDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectServerLoggingPropertiesDialog.hh 1564 2006-04-07 13:33:15Z modesto $
