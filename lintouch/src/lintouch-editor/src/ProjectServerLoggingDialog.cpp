// $Id: ProjectServerLoggingDialog.cpp 1567 2006-04-10 12:16:16Z modesto $
//
//   FILE: ProjectServerLoggingDialog.cpp --
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

#include <qlistview.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qvaluevector.h>

#include "ServerLoggingManager.hh"
#include "CSV.hh"

#include "AddDelServerLoggingCommand.hh"
#include "Command.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectServerLoggingPropertiesDialog.hh"
#include "ProjectServerLoggingDialog.hh"
#include "ProjectWindow.hh"
#include "RenameServerLoggingCommand.hh"

using namespace lt;

/*
 * Specialized QListViewItem. Sets defaults and rtti.
 */
class ProjectServerLoggingItem : public QListViewItem
{
public:
    enum {
        LTEDITORPROJECTSERVERLOGGINGITEM = 1004
    };

    /*
     * Same parameters as QListViewItem but sets the second column value to
     * the same value as column 1. Needed for Rename
     */
    ProjectServerLoggingItem( QListView* parent, QListViewItem* after,
            QString label1, QString label2) : QListViewItem( parent, after )
    {
        setRenameEnabled( 0, true );

        // for rename magic
        setText( 0, label1 );
        setText( 2, label1 );

        // second column
        setText( 1, label2 );
    }

    virtual int rtti() const {
        return LTEDITORPROJECTSERVERLOGGINGITEM;
    }
};

struct ProjectServerLoggingDialog::
    ProjectServerLoggingDialogPrivate
{
    ProjectServerLoggingDialogPrivate() : pw(NULL), cp(NULL) {}

    /*
     * pw
     */
    ProjectWindow* pw;

    /*
     * ServerLoggingPropertiesDialog
     */
    ProjectServerLoggingPropertiesDialog* cp;
};

ProjectServerLoggingDialog::ProjectServerLoggingDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
    ProjectServerLoggingDialogBase( parent, name, fl ),
    d(new ProjectServerLoggingDialogPrivate)
{
    Q_CHECK_PTR(d);

    m_serverLogging->setSorting(-1);

    d->cp = new ProjectServerLoggingPropertiesDialog(parent, this);
    Q_CHECK_PTR(d->cp);
}

ProjectServerLoggingDialog::~ProjectServerLoggingDialog()
{
    delete d;
}

void ProjectServerLoggingDialog::newServerLoggingSlot()
{
    Q_ASSERT(d);

    d->cp->runNew( QString::null, d->pw);
}

void ProjectServerLoggingDialog::deleteServerLoggingSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(m_serverLogging->selectedItem());

    AddDelServerLoggingCommand* cmd =
        new AddDelServerLoggingCommand(
                m_serverLogging->selectedItem()->text(0),
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR(cmd);
    d->pw->executeCommand(cmd);
}

void ProjectServerLoggingDialog::editServerLoggingSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(m_serverLogging->selectedItem());

    QString key = m_serverLogging->selectedItem()->text(0);

    if(!((MainWindow*)parent())->serverLoggingManager()
            .isAvailable(m_serverLogging->selectedItem()->text(1))) {

        QMessageBox::warning( this,
                tr("Warning"),
                tr("The Server Logging plugin for the \"%1\""
                        " is not installed on the system."
                        "No editation is posible.").arg(key),
                QMessageBox::Ok,
                QMessageBox::NoButton,
                QMessageBox::NoButton );
    } else {
        d->cp->runEdit( key, d->pw);
    }
}

void ProjectServerLoggingDialog::selectionChanged()
{
    QListViewItem* i = m_serverLogging->selectedItem();

    bool enable = (i!=NULL);
    m_buttonEdit->setEnabled( enable );
    m_buttonDelete->setEnabled( enable );
}

void ProjectServerLoggingDialog::itemRenamed( QListViewItem* i, int col )
{
    Q_ASSERT(i);
    Q_ASSERT(i->rtti() ==
            ProjectServerLoggingItem::LTEDITORPROJECTSERVERLOGGINGITEM);
    Q_ASSERT(col == 0 );

    i->setText(0, i->text(0).stripWhiteSpace());

    if( i->text(0).isEmpty()) {
        i->setText(0, i->text(2)); // silently rename back
        return;
    }

    if( i->text(0) == i->text(2) ) {
        // just spaces has been changed. ignore.
        return;
    }

    if(d->pw->project()->serverLoggings().contains(i->text(0))) {
        QMessageBox::warning( this,
                tr("Error"),
                QString(tr("The Server Logging name %1"
                        " is already used within this project."
                        " Please choose a different name."))
                .arg(i->text(0)),
                QMessageBox::Ok,
                QMessageBox::NoButton,
                QMessageBox::NoButton );

        i->setText(0, i->text(2)); // rename back
        return;
    }

    // create a command and execute it
    RenameServerLoggingCommand* cmd =
        new RenameServerLoggingCommand( i->text(2), //old name
                i->text(0), //new name
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );

    i->setText(0, i->text(2)); // rename back; refresh in projectChangedSlot()

    d->pw->executeCommandLater( cmd );
}

void ProjectServerLoggingDialog::fillServerLogging()
{
    Q_ASSERT(d);
    Q_ASSERT(m_serverLogging);

    // clear the m_serverLogging
    m_serverLogging->clear();

    if( d->pw == NULL || d->pw->project() == NULL ) {
        return;
    }

    const ServerLoggingDict& c = d->pw->project()->serverLoggings();
    if( c.count() < 1 ) {
        // no Server Logging
        return;
    }

    ProjectServerLoggingItem* i = NULL;
    for(unsigned j = 0; j < c.count(); ++j) {
        // qDebug(QString("ServerLogging to display: %1").arg(c.keyAt(j)));
        i = new ProjectServerLoggingItem( m_serverLogging, i,
                c.keyAt(j), c[j]->type() );
        Q_CHECK_PTR(i);
    }
}

void ProjectServerLoggingDialog::serverLoggingDoubleclicked(
        QListViewItem* i)
{
    Q_ASSERT(i);
    m_serverLogging->setSelected(i, true);
    editServerLoggingSlot();
}

void ProjectServerLoggingDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT(d);

    bool enable = (pw != NULL);
    m_serverLogging->setEnabled( enable );
    m_buttonNew->setEnabled( enable );
    m_buttonEdit->setEnabled( enable );
    m_buttonDelete->setEnabled( enable );

    if( d->pw != pw ) {

        //store current project
        d->pw = pw;

        // refresh from ServerLogging dict of pw with clear
        fillServerLogging();
    }

    if( pw ) {
        setCaption( tr("%1 Server Logging - Lintouch Editor")
                .arg(pw->projectFileInfo().baseName()) );

        if(t == LTE_CMD_SERVERLOGGING_DEL) {
            Q_ASSERT(pw->lastCommand()->rtti() == LTE_CMD_SERVERLOGGING_ADD_DEL);
            // removed dynamic_cast because it segfaults when compiled as
            // ActiveQt server
            AddDelServerLoggingCommand* c =
                (AddDelServerLoggingCommand*)(pw->lastCommand());
            Q_ASSERT(c);
            ProjectServerLoggingItem* i =
                (ProjectServerLoggingItem*)m_serverLogging->findItem(
                    c->name(), 0);
            Q_ASSERT(i);
            Q_ASSERT(i->rtti() ==
                ProjectServerLoggingItem::LTEDITORPROJECTSERVERLOGGINGITEM);
            m_serverLogging->takeItem(i);
            delete i;
            QListViewItem* x = m_serverLogging->selectedItem();
            if(!x) {
                x = m_serverLogging->firstChild();
                m_serverLogging->setSelected(x, true);
            }
            m_serverLogging->ensureItemVisible(x);

        } else if(t == LTE_CMD_SERVERLOGGING_ADD) {
            Q_ASSERT(pw->lastCommand()->rtti() == LTE_CMD_SERVERLOGGING_ADD_DEL);
            // removed dynamic_cast because it segfaults when compiled as
            // ActiveQt server
            AddDelServerLoggingCommand* c =
                (AddDelServerLoggingCommand*)(pw->lastCommand());
            Q_ASSERT(c);
            fillServerLogging();
            ProjectServerLoggingItem* i =
                (ProjectServerLoggingItem*)m_serverLogging->findItem(
                    c->name(), 0);
            Q_ASSERT(i);
            Q_ASSERT(i->rtti() ==
                ProjectServerLoggingItem::LTEDITORPROJECTSERVERLOGGINGITEM);
            m_serverLogging->setSelected(i, true);
            m_serverLogging->ensureItemVisible(i);

        } else if(t == LTE_CMD_SERVERLOGGING_RENAME) {
            Q_ASSERT(pw->lastCommand()->rtti() == LTE_CMD_SERVERLOGGING_RENAME);
            // removed dynamic_cast because it segfaults when compiled as
            // ActiveQt server
            RenameServerLoggingCommand* c =
                (RenameServerLoggingCommand*)(pw->lastCommand());
            Q_ASSERT(c);
            ProjectServerLoggingItem* i =
                (ProjectServerLoggingItem*)m_serverLogging->findItem(
                    c->oldName(), 0);
            Q_ASSERT(i);
            Q_ASSERT(i->rtti() ==
                ProjectServerLoggingItem::LTEDITORPROJECTSERVERLOGGINGITEM);
            i->setText(0, c->newName());
            i->setText(2, c->newName());
            m_serverLogging->setSelected(i, true);
            m_serverLogging->ensureItemVisible(i);

        } else {
            //TODO: handle apropriate messages
        }
        m_serverLogging->setFocus();
    } else {
        setCaption( tr("Server Logging - Lintouch Editor") );
    }

    selectionChanged();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectServerLoggingDialog.cpp 1567 2006-04-10 12:16:16Z modesto $
