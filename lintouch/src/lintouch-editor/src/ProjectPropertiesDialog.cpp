// $Id: ProjectPropertiesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectPropertiesDialog.cpp --
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

#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qtextedit.h>

#include <lt/project/Project.hh>

#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectPropertiesDialog.hh"
#include "ProjectPropsChangeCommand.hh"
#include "ProjectWindow.hh"

using namespace lt;

struct ProjectPropertiesDialog::
        ProjectPropertiesDialogPrivate {

    ProjectPropertiesDialogPrivate() : pw(NULL)
    {
    }

    /**
     * This points to the current project, the info of which
     * is displayed/changed.
     */
    ProjectWindow* pw;
};

ProjectPropertiesDialog::ProjectPropertiesDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
    ProjectPropertiesDialogBase( parent, name, fl ),
    d(new ProjectPropertiesDialogPrivate)
{
    Q_CHECK_PTR( d );

    connect( m_applyButton, SIGNAL( clicked() ),
            this, SLOT( applySlot() ) );
    connect( m_okButton, SIGNAL( clicked() ),
            this, SLOT( okSlot() ) );

}

ProjectPropertiesDialog::~ProjectPropertiesDialog()
{
    delete d;
}

void ProjectPropertiesDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT(d);

    if( pw != d->pw || t == LTE_CMD_NOOP ||
            t == LTE_CMD_PROJECT_PROPS_CHANGE ) {
        bool enable = (pw != NULL);

        infoTab->setEnabled( enable );
        m_okButton->setEnabled( enable );
        m_applyButton->setEnabled( enable );

        //store current project
        d->pw = pw;

        if( pw ) {
            setCaption( QString( tr("%1 Properties - Lintouch Editor") )
                    .arg( pw->projectFileInfo().baseName() ) );

            Info projectInfo = pw->project()->info();

            author->setText( projectInfo.author() );
            version->setText( projectInfo.version() );
            dateofCreation->setDate( projectInfo.date() );
            shortDescription->setText( projectInfo.shortDescription() );
            longDescription->setText( projectInfo.longDescription() );

        } else {
            setCaption( tr("Properties - Lintouch Editor") );

            // clear content on panel close
            Info projectInfo = Info();
            author->setText( QString::null );
            version->setText( QString::null );
            dateofCreation->setDate( QDate() );
            shortDescription->setText( QString::null );
            longDescription->setText( QString::null );
        }
    }
}

void ProjectPropertiesDialog::applySlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );

    Info newInfo;
    newInfo.setAuthor( author->text() );
    newInfo.setVersion( version->text() );
    newInfo.setDate( dateofCreation->date() );
    newInfo.setShortDescription( shortDescription->text() );
    newInfo.setLongDescription( longDescription->text() );

    Info oldInfo = d->pw->project()->info().clone();

    if( !isDirty( oldInfo, newInfo ) ) {
        return;
    }

    ProjectPropsChangeCommand* cmd;
    cmd = new ProjectPropsChangeCommand( oldInfo, newInfo,
                d->pw, ( MainWindow* )parent() );

    Q_CHECK_PTR( cmd );

    d->pw->executeCommand( cmd );

}

void ProjectPropertiesDialog::okSlot()
{
    applySlot();
    close();
}

bool ProjectPropertiesDialog::
        isDirty( Info oldInfo, Info newInfo )
{
    if( oldInfo.author() != newInfo.author() ) {
        return TRUE;
    } else if ( oldInfo.version() != newInfo.version() ) {
        return TRUE;
    } else if ( oldInfo.date() != newInfo.date() ) {
        return TRUE;
    } else if ( oldInfo.shortDescription() !=
            newInfo.shortDescription() ) {
        return TRUE;
    } else if ( oldInfo.longDescription() !=
            newInfo.longDescription() ) {
        return TRUE;
    }

    return FALSE;

}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPropertiesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
