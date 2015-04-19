// $Id: ProjectViewResizeDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectViewResizeDialog.cpp -- 
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 03 August 2004
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

#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <lt/project/View.hh>

#include "EditorProject.hh"
#include "ProjectViewsDialog.hh"
#include "ProjectWindow.hh"
#include "ResizeViewCommand.hh"

#include "ProjectViewResizeDialog.hh"

using namespace lt;

struct ProjectViewResizeDialog::ProjectViewResizeDialogPrivate
{
    ProjectViewResizeDialogPrivate():pw(NULL),keepAspect(false) {}

    /*
     * ProjectWindow holder.
     */
    ProjectWindow* pw;

    /*
     * View name holder.
     */
    QString viewName;

    /*
     * flag that stores aspect checkbox value
     */
    bool keepAspect;

    /*
     * Aspect ratio value holder.
     */
    double aspect;
};

ProjectViewResizeDialog::ProjectViewResizeDialog(
        ProjectViewsDialog* parent, const char* name /*= 0*/,
        bool modal /*= true*/, WFlags fl /*= Qt::WType_TopLevel*/ ) :
    ProjectViewResizeDialogBase( parent, name, modal, fl ),
    d(new ProjectViewResizeDialogPrivate)
{
    Q_CHECK_PTR(d);
    m_keepAspect->setChecked(d->keepAspect);
}

ProjectViewResizeDialog::~ProjectViewResizeDialog()
{
    delete d;
}

int ProjectViewResizeDialog::exec( ProjectWindow* pw,
        const QString& viewName )
{
    Q_ASSERT(d);
    Q_ASSERT(pw);

    setCaption( tr("Resize View %1 - Lintouch Editor").arg(viewName) );

    d->pw = pw;
    d->viewName = viewName;

    const View& v = pw->project()->views()[viewName];

    unsigned vw = v.virtualWidth();
    unsigned vh = v.virtualHeight();

    m_origWidth->setText( QString::number(vw) );
    m_origHeight->setText( QString::number(vh) );

    // turn off width and height slots
    d->aspect = 0.0;

    m_newWidth->setValue( vw );
    m_newHeight->setValue( vh );

    d->aspect = (double)vw / vh;

    // set the aspect on as is the default
    m_keepAspect->setChecked(true);

    // run the dialog
    ProjectViewResizeDialogBase::exec();

    // clear stored values
    d->pw = NULL;
    d->viewName = QString::null;

    return QDialog::Accepted;
}

void ProjectViewResizeDialog::applySlot()
{
    ResizeViewCommand* cmd =
        new ResizeViewCommand( d->viewName, m_newWidth->value(),
            m_newHeight->value(), d->pw,
            (MainWindow*)(parent()->parent()) );
    Q_CHECK_PTR(cmd);

    d->pw->executeCommand( cmd );
}

void ProjectViewResizeDialog::closeSlot()
{
    accept();
}

void ProjectViewResizeDialog::okSlot()
{
    applySlot();
    accept();
}

void ProjectViewResizeDialog::widthValueChangedSlot(int val)
{
    Q_ASSERT(d);
    if( d->keepAspect && d->aspect != 0.0 ) {
        d->keepAspect = false;      // turn off autoupdate
        m_newHeight->setValue( (int)(val / d->aspect) );
        d->keepAspect = true;       // turn on autoupdate
    }
}

void ProjectViewResizeDialog::heightValueChangedSlot(int val)
{
    Q_ASSERT(d);
    if( d->keepAspect && d->aspect != 0.0 ) {
        d->keepAspect = false;      // turn off autoupdate
        m_newWidth->setValue( (int)(val * d->aspect) );
        d->keepAspect = true;       // turn on autoupdate
    }
}

void ProjectViewResizeDialog::aspectToggledSlot(bool a)
{
    Q_ASSERT(d);
    d->keepAspect = a;

    if( a ) {
        m_newHeight->setValue( (int)(m_newWidth->value() / d->aspect) );
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectViewResizeDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
