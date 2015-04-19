// $Id: ProjectConnectionsDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectConnectionsDialog.cpp --
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

#include <qlistview.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qvaluevector.h>

#include <lt/templates/Connection.hh>

#include "PluginManager.hh"
#include "CSV.hh"

#include "AddDelConnectionCommand.hh"
#include "Command.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectConnectionPropertiesDialog.hh"
#include "ProjectConnectionsDialog.hh"
#include "ProjectWindow.hh"
#include "RenameConnectionCommand.hh"

using namespace lt;

/*
 * Specialized QListViewItem. Sets defaults and rtti.
 */
class ProjectConnectionsItem : public QListViewItem
{
public:
    enum {
        LTEDITORPROJECTCONNECTIONSITEM = 1004
    };

    /*
     * Same parameters as QListViewItem but sets the second column value to
     * the same value as column 1. Needed for Rename
     */
    ProjectConnectionsItem( QListView* parent, QListViewItem* after,
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
        return LTEDITORPROJECTCONNECTIONSITEM;
    }
};

struct ProjectConnectionsDialog::
    ProjectConnectionsDialogPrivate
{
    ProjectConnectionsDialogPrivate() : pw(NULL), cp(NULL) {}

    Connection* connFromFile(const QString& fn)
    {
        QString data;
        QFile fr(fn);
        QFileInfo fi(fr);

        if(fr.open(IO_ReadOnly)) {
            data = QString(fr.readAll());
        } else {
            // can't open
            QMessageBox::warning( NULL,
                    tr("Error"),
                    tr("Unable to read from the file %1.").arg(fn),
                    QMessageBox::Ok,
                    QMessageBox::NoButton );
            return NULL;
        }

        Connection* c = csv2connection(QStringList::split("\n", data));
        if(!c) {
            QMessageBox::warning( NULL,
                    tr("Error"),
                    tr("%1 does not define valid connection.").arg(fn),
                    QMessageBox::Ok,
                    QMessageBox::NoButton );
            return NULL;
        }

        return c;
    }

    /*
     * pw
     */
    ProjectWindow* pw;

    /*
     * ConnectionPropertiesDialog
     */
    ProjectConnectionPropertiesDialog* cp;

    /**
     * Map of connection-paths. Used by Reimport connection command
     */
    QMap<EditorProject*, QMap<QString,QString> > connPath;
};

ProjectConnectionsDialog::ProjectConnectionsDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
    ProjectConnectionsDialogBase( parent, name, fl ),
    d(new ProjectConnectionsDialogPrivate)
{
    Q_CHECK_PTR(d);

    m_connections->setSorting(-1);

    d->cp = new ProjectConnectionPropertiesDialog(parent, this);
    Q_CHECK_PTR(d->cp);
}

ProjectConnectionsDialog::~ProjectConnectionsDialog()
{
    delete d;
}

void ProjectConnectionsDialog::newConnectionSlot()
{
    Q_ASSERT(d);

    d->cp->runNew( QString::null, d->pw);
}

void ProjectConnectionsDialog::deleteConnectionSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(m_connections->selectedItem());

    AddDelConnectionCommand* cmd =
        new AddDelConnectionCommand(
                m_connections->selectedItem()->text(0),
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR(cmd);
    d->pw->executeCommand(cmd);
}

void ProjectConnectionsDialog::editConnectionSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(m_connections->selectedItem());

    d->cp->runEdit( m_connections->selectedItem()->text(0), d->pw);
}

void ProjectConnectionsDialog::importConnectionSlot()
{
    QString fn = QFileDialog::getOpenFileName(QString::null,
            tr("Exported connections (*.csv);; All files (*.*);;"));
    if(fn.isNull()) {
        return;
    }

    Connection* c = d->connFromFile(fn);
    if(!c) {
        return;
    }
    QFileInfo fi(fn);

    if(!((MainWindow*)parent())->pluginManager().isAvailable(c->type())) {
        QMessageBox::warning( this,
                tr("Error"),
                tr("Unknown connection type %1.").arg(c->type()),
                QMessageBox::Ok,
                QMessageBox::NoButton );

        PropertyDict& pd = c->properties();
        pd.setAutoDelete(true);
        delete c;

        return;
    }

    // create local list of all connections keys
    QMap<QString,int> keys(d->pw->project()->connections().mapping());

    fn = Command::generateSafeName(
            QString("%1 %2").arg(fi.baseName()), 1, keys);

    AddDelConnectionCommand* cmd =
        new AddDelConnectionCommand(fn, c, fi.absFilePath(),
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR(cmd);
    d->pw->executeCommand(cmd);
}

void ProjectConnectionsDialog::reimportConnectionSlot()
{
    Q_ASSERT(d);

    QListViewItem* i = m_connections->selectedItem();
    Q_ASSERT(i);

    QString fn;
    if(d->connPath.contains(d->pw->project()) &&
            d->connPath[d->pw->project()].contains(i->text(0))) {
        fn = d->connPath[d->pw->project()][i->text(0)];
    }

    fn = QFileDialog::getOpenFileName(fn,
            tr("Exported connections (*.csv);; All files (*.*);;"));
    if(fn.isNull()) {
        return;
    }

    Connection* c = d->connFromFile(fn);
    if(!c) {
        return;
    }
    QFileInfo fi(fn);

    if(!((MainWindow*)parent())->pluginManager().isAvailable(c->type())) {
        QMessageBox::warning( this,
                tr("Error"),
                tr("Unknown connection type %1.").arg(c->type()),
                QMessageBox::Ok,
                QMessageBox::NoButton );

        PropertyDict& pd = c->properties();
        pd.setAutoDelete(true);
        delete c;

        return;
    }

    AddDelConnectionCommand* cmd =
        new AddDelConnectionCommand(i->text(0), c, fi.absFilePath(),
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR(cmd);
    d->pw->executeCommand(cmd);
}

void ProjectConnectionsDialog::exportConnectionSlot()
{
    QListViewItem* i = m_connections->selectedItem();
    Q_ASSERT(i);
    Q_ASSERT(d->pw->project()->connections().contains(i->text(0)));

    QString fn = QString("%1.csv").arg(i->text(0));
    fn = QFileDialog::getSaveFileName(fn);
    if(fn.isNull()) {
        return;
    }

    QStringList sc = connection2csv(
            d->pw->project()->connections()[i->text(0)]);

    QFile fw(fn);
    if(fw.open(IO_WriteOnly|IO_Truncate)) {
        for(QStringList::const_iterator it = sc.begin();
                it != sc.end(); ++it)
        {
            fw.writeBlock(*it, (*it).length());
        }
    } else {
        // can't open for write
        QMessageBox::warning( this,
                tr("Error"),
                tr("Unable to write to the file %1.").arg(fn),
                QMessageBox::Ok,
                QMessageBox::NoButton );
    }
}

void ProjectConnectionsDialog::selectionChanged()
{
    QListViewItem* i = m_connections->selectedItem();

    bool enable = (i!=NULL);
    m_buttonEdit->setEnabled( enable );
    m_buttonReimport->setEnabled(enable);
    m_buttonExport->setEnabled(enable);
    m_buttonDelete->setEnabled( enable );
}

void ProjectConnectionsDialog::itemRenamed( QListViewItem* i, int col )
{
    Q_ASSERT(i);
    Q_ASSERT(i->rtti() ==
            ProjectConnectionsItem::LTEDITORPROJECTCONNECTIONSITEM);
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

    if(!d->pw->project()->checkConnectionName(i->text(0))) {
        QMessageBox::warning( this,
                tr("Error"),
                QString(tr("The connection name %1"
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
    RenameConnectionCommand* cmd =
        new RenameConnectionCommand( i->text(2), //old name
                i->text(0), //new name
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );

    i->setText(0, i->text(2)); // rename back; refresh in projectChangedSlot()

    d->pw->executeCommandLater( cmd );
}

void ProjectConnectionsDialog::fillConnections()
{
    Q_ASSERT(d);
    Q_ASSERT(m_connections);

    // clear the m_connections
    m_connections->clear();

    if( d->pw == NULL || d->pw->project() == NULL ) {
        return;
    }

    const ConnectionDict& c = d->pw->project()->connections();
    if( c.count() < 1 ) {
        // no connections
        return;
    }

    ProjectConnectionsItem* i = NULL;
    for(unsigned j = 0; j < c.count(); ++j) {
        // qDebug(QString("connection to display: %1").arg(c.keyAt(j)));
        i = new ProjectConnectionsItem( m_connections, i,
                c.keyAt(j), c[j]->type() );
        Q_CHECK_PTR(i);
    }

    const ConnectionDict& dc = d->pw->project()->disabledConnections();
    if( dc.count() < 1 ) {
        // no connections
        return;
    }

    for(unsigned j = 0; j < dc.count(); ++j) {
        // qDebug(QString("connection to display: %1").arg(dc.keyAt(j)));
        i = new ProjectConnectionsItem( m_connections, i,
                dc.keyAt(j), dc[j]->type() );
        Q_CHECK_PTR(i);
        i->setEnabled(false);
    }
}

void ProjectConnectionsDialog::connectionDoubleclicked(
        QListViewItem* i)
{
    Q_ASSERT(i);
    m_connections->setSelected(i, true);
    editConnectionSlot();
}

void ProjectConnectionsDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT(d);

    bool enable = (pw != NULL);
    m_connections->setEnabled( enable );
    m_buttonNew->setEnabled( enable );
    m_buttonEdit->setEnabled( enable );
    m_buttonImport->setEnabled(enable);
    m_buttonReimport->setEnabled(enable);
    m_buttonExport->setEnabled(enable);
    m_buttonDelete->setEnabled( enable );

    if( d->pw != pw ) {

        //store current project
        d->pw = pw;

        // refresh from Connection dict of pw with clear
        fillConnections();
    }

    if( pw ) {
        setCaption( tr("%1 Connections - Lintouch Editor")
                .arg(pw->projectFileInfo().baseName()) );

        if(t == LTE_CMD_CONNECTION_DEL) {
            Q_ASSERT(pw->lastCommand()->rtti() == LTE_CMD_CONNECTION_ADD_DEL);
            // removed dynamic_cast because it segfaults when compiled as
            // ActiveQt server
            AddDelConnectionCommand* c =
                (AddDelConnectionCommand*)(pw->lastCommand());
            Q_ASSERT(c);
            ProjectConnectionsItem* i =
                (ProjectConnectionsItem*)m_connections->findItem(
                    c->name(), 0);
            Q_ASSERT(i);
            Q_ASSERT(i->rtti() ==
                ProjectConnectionsItem::LTEDITORPROJECTCONNECTIONSITEM);
            m_connections->takeItem(i);
            delete i;
            QListViewItem* x = m_connections->selectedItem();
            if(!x) {
                x = m_connections->firstChild();
                m_connections->setSelected(x, true);
            }
            m_connections->ensureItemVisible(x);

            // update connPath
            d->connPath.remove(pw->project());

        } else if(t == LTE_CMD_CONNECTION_ADD) {
            Q_ASSERT(pw->lastCommand()->rtti() == LTE_CMD_CONNECTION_ADD_DEL);
            // removed dynamic_cast because it segfaults when compiled as
            // ActiveQt server
            AddDelConnectionCommand* c =
                (AddDelConnectionCommand*)(pw->lastCommand());
            Q_ASSERT(c);
            fillConnections();
            ProjectConnectionsItem* i =
                (ProjectConnectionsItem*)m_connections->findItem(
                    c->name(), 0);
            Q_ASSERT(i);
            Q_ASSERT(i->rtti() ==
                ProjectConnectionsItem::LTEDITORPROJECTCONNECTIONSITEM);
            m_connections->setSelected(i, true);
            m_connections->ensureItemVisible(i);

            // update connPath
            if(c->path() != QString::null) {
                d->connPath[pw->project()].insert(c->name(), c->path());
            }

        } else if(t == LTE_CMD_CONNECTION_RENAME) {
            Q_ASSERT(pw->lastCommand()->rtti() == LTE_CMD_CONNECTION_RENAME);
            // removed dynamic_cast because it segfaults when compiled as
            // ActiveQt server
            RenameConnectionCommand* c =
                (RenameConnectionCommand*)(pw->lastCommand());
            Q_ASSERT(c);
            ProjectConnectionsItem* i =
                (ProjectConnectionsItem*)m_connections->findItem(
                    c->oldName(), 0);
            Q_ASSERT(i);
            Q_ASSERT(i->rtti() ==
                ProjectConnectionsItem::LTEDITORPROJECTCONNECTIONSITEM);
            i->setText(0, c->newName());
            i->setText(2, c->newName());
            m_connections->setSelected(i, true);
            m_connections->ensureItemVisible(i);

            // update connPath
            if(d->connPath.contains(pw->project())) {
                QMap<QString,QString> &m = d->connPath[pw->project()];
                if(m.contains(c->oldName())) {
                    QString path = m[c->oldName()];
                    m.remove(c->oldName());
                    m.insert(c->newName(), path);
                }
            }

        } else {
            //TODO: handle apropriate messages
        }
        m_connections->setFocus();
    } else {
        setCaption( tr("Connections - Lintouch Editor") );
    }

    selectionChanged();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectConnectionsDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
