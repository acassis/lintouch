// $Id: ProjectVariablesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectVariablesDialog.cpp --
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

#include <qcombobox.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlistbox.h>

#include <lt/templates/Connection.hh>
#include <lt/templates/Variable.hh>
#include <lt/templates/IOPin.hh>

#include "PluginManager.hh"
#include "AddDelVariableCommand.hh"
#include "Command.hh"
#include "EditorProject.hh"
#include "MainWindow.hh"
#include "ProjectVariablePropertiesDialog.hh"
#include "ProjectWindow.hh"
#include "RenameVariableCommand.hh"
#include "RenameConnectionCommand.hh"
#include "AddDelConnectionCommand.hh"

#include "ProjectVariablesDialog.hh"

using namespace lt;

/*
 * Specialized QListViewItem. Sets defaults and rtti.
 */
class ProjectVariablesItem : public QListViewItem
{
public:
    enum {
        LTEDITORPROJECTVARIABLESITEM = 1005
    };

    /*
     * Same parameters as QListViewItem but sets the second column value to
     * the same value as column 1. Needed for Rename
     */
    ProjectVariablesItem( QListView* parent, QListViewItem* after,
            QString label1, QString label2 ) : QListViewItem( parent, after )
    {
        setRenameEnabled( 0, true );

        // rename magic, 0. and 2. column contains the same name
        setText( 0, label1 );
        setText( 2, label1 );

        // the 1. column
        setText( 1, label2 );
    }

    virtual int rtti() const {
        return LTEDITORPROJECTVARIABLESITEM;
    }
};

struct ProjectVariablesDialog::
    ProjectVariablesDialogPrivate
{
    ProjectVariablesDialogPrivate() : pw(NULL), vp(NULL) {}

    /*
     * pw
     */
    ProjectWindow* pw;

    /**
     * Variable Properties dialog
     */
    ProjectVariablePropertiesDialog* vp;

    /**
     * Fill in the Connections combobox. Try to keep selected item.
     */
    void fillConnections(QComboBox* cb) {
        Q_ASSERT(cb);
        cb->clear();

        if(!pw) {
            return;
        }

        const ConnectionDict& cd = pw->project()->connections();
        for(unsigned i=0; i<cd.count(); ++i) {
            cb->insertItem(cd.keyAt(i));
        }
    }

    /**
     * Fill in the Variables list for the current Connection name. Current
     * in the means of LTDict::current().
     */
    void fillVariables(const QString& cn, QListView* lv) {
        Q_ASSERT(lv);
        Q_ASSERT(pw);

        const Connection* c = pw->project()->connections()[cn];
        Q_ASSERT(c);
        const VariableDict& vd = c->variables();

        lv->clear();
        ProjectVariablesItem* i = NULL;
        for(unsigned j=0; j<vd.count(); ++j) {
            IOPin::TypeList tl;
            tl.append(vd[j]->type());
            i = new ProjectVariablesItem( lv, i, vd.keyAt(j),
                    IOPin::encodeTypes(tl));
            Q_CHECK_PTR(i);
        }
    }
};

ProjectVariablesDialog::
    ProjectVariablesDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
    ProjectVariablesDialogBase( parent, name, fl ),
    d(new ProjectVariablesDialogPrivate)
{
    Q_CHECK_PTR(d);

    m_variables->setSorting(-1);

    d->vp = new ProjectVariablePropertiesDialog(this);
    Q_CHECK_PTR(d->vp);
}

ProjectVariablesDialog:: ~ProjectVariablesDialog()
{
    delete d;
}

void ProjectVariablesDialog::newVariableSlot()
{
    Q_ASSERT(d);

    const Connection* c = d->pw->project()->
        connections()[m_connection->currentText()];
    Q_ASSERT(c);

    d->vp->runNew( m_connection->currentText(), d->pw,
            (MainWindow*)parent() );
}

void ProjectVariablesDialog::editVariableSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(d->vp);

    variableDoubleClicked(m_variables->selectedItem());
}

void ProjectVariablesDialog::deleteVariableSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(m_variables->selectedItem());

    AddDelVariableCommand* cmd =
        new AddDelVariableCommand(m_connection->currentText(),
                m_variables->selectedItem()->text(0),
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR(cmd);
    d->pw->executeCommand(cmd);

}

void ProjectVariablesDialog::variableDoubleClicked(QListViewItem* i)
{
    Q_ASSERT(d);
    Q_ASSERT(i);

    const ConnectionDict& cd = d->pw->project()->connections();
    Q_ASSERT(cd.currentKey() == m_connection->currentText());

    d->vp->runEdit(cd.currentKey(), i->text(0), d->pw,
            (MainWindow*)parent());
}

void ProjectVariablesDialog::selectionChanged()
{
    QListViewItem* i = m_variables->selectedItem();

    bool enable = (i!=NULL);
    m_buttonEdit->setEnabled( enable );
    m_buttonDelete->setEnabled( enable );
}

void ProjectVariablesDialog::itemRenamed( QListViewItem* i, int col )
{
    Q_ASSERT(i);
    Q_ASSERT(i->rtti() ==
            ProjectVariablesItem::LTEDITORPROJECTVARIABLESITEM);
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

    const Connection* c = d->pw->project()->
        connections()[m_connection->currentText()];
    Q_ASSERT(c);

    if(c->variables().contains(i->text(0))) {
        QMessageBox::warning( this,
                tr("Error"),
                QString(tr("The variable name %1"
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
    RenameVariableCommand* cmd =
        new RenameVariableCommand(m_connection->currentText(),
                i->text(2), //old name
                i->text(0), //new name
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );

    i->setText(0, i->text(2)); // rename back; refresh in projectChangedSlot()

    d->pw->executeCommandLater( cmd );
}

void ProjectVariablesDialog::connectionChangedSlot(const QString& c)
{
    Q_ASSERT(d);
    Q_ASSERT(d->pw);

    ConnectionDict& cd = d->pw->project()->connections();
    Q_ASSERT(cd.contains(c));
    cd.setCurrentKey(c);

    d->fillVariables(c, m_variables);

    selectionChanged();

    Q_ASSERT(cd.currentKey() == m_connection->currentText());

    setCaption(tr("%1/%2 Variables - Lintouch Editor")
            .arg(d->pw->projectFileInfo().baseName())
            .arg(cd.currentKey()));
}

void ProjectVariablesDialog::editConnectionsSlot()
{
    ((MainWindow*)parent())->showProjectConnectionsDialogSlot();
}

void ProjectVariablesDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT(d);

    if( d->pw != pw ) {

        //store current project
        d->pw = pw;

        // clear
        m_connection->clear();
        m_variables->clear();
    }

    if(pw && pw->project()->connections().current()) {

        m_connection->setEnabled( true );
        m_variables->setEnabled( true );
        m_buttonNew->setEnabled( true );
        m_buttonDelete->setEnabled( true );

        QString cn = pw->project()->connections().currentKey();

        setCaption(tr("%1/%2 Variables - Lintouch Editor")
                .arg(pw->projectFileInfo().baseName())
                .arg(cn));

        if(t== LTE_CMD_NOOP ||
                t == LTE_CMD_CONNECTION_ADD || t == LTE_CMD_CONNECTION_DEL ||
                t == LTE_CMD_CONNECTION_RENAME)
        {
            QString vn;
            if(m_variables->selectedItem()) {
                vn = m_variables->selectedItem()->text(0);
            }

            d->fillConnections(m_connection);

            if(t == LTE_CMD_CONNECTION_RENAME) {
                Q_ASSERT(pw->lastCommand());
                Q_ASSERT(pw->lastCommand()->rtti() ==
                        LTE_CMD_CONNECTION_RENAME);

                // removed dynamic_cast because it segfaults when compiled as
                // ActiveQt server
                RenameConnectionCommand* c =
                    (RenameConnectionCommand*)(pw->lastCommand());
                Q_ASSERT(c);

                // select the connection
                cn = c->newName();

            } else if(t == LTE_CMD_CONNECTION_ADD) {
                Q_ASSERT(pw->lastCommand());
                Q_ASSERT(pw->lastCommand()->rtti() ==
                        LTE_CMD_CONNECTION_ADD_DEL);

                // removed dynamic_cast because it segfaults when compiled as
                // ActiveQt server
                AddDelConnectionCommand* c =
                    (AddDelConnectionCommand*)(pw->lastCommand());
                Q_ASSERT(c);

                cn = c->name();
            }

            // set m_connection to current connection
            m_connection->setCurrentText(cn);

            d->fillVariables(cn, m_variables);

            if(!vn.isEmpty()) {
                QListViewItem* i = m_variables->findItem(vn, 0);
                if(!i) {
                    i = m_variables->firstChild();
                }
                m_variables->setSelected(i, true);
                m_variables->ensureItemVisible(i);
            }

        } else if(t == LTE_CMD_VARIABLE_RENAME) {
            Q_ASSERT(pw->lastCommand());
            // removed dynamic_cast because it segfaults when compiled as
            // ActiveQt server
            RenameVariableCommand* c =
                (RenameVariableCommand*)(pw->lastCommand());
            Q_ASSERT(c);

            QListViewItem* i = NULL;

            // select the connection
            if(m_connection->listBox()->findItem(c->connection())) {
                m_connection->setCurrentText(c->connection());
                // this refreshes the m_variables!
                connectionChangedSlot(c->connection());
                i = m_variables->findItem(c->newName(), 0);
            } else {
                i = m_variables->findItem(c->oldName(), 0);
            }
            Q_ASSERT(i);
            m_variables->setSelected(i, true);
            m_variables->ensureItemVisible(i);

        } else if(t == LTE_CMD_VARIABLE_ADD) {
            Q_ASSERT(pw->lastCommand());
            // removed dynamic_cast because it segfaults when compiled as
            // ActiveQt server
            AddDelVariableCommand* c =
                (AddDelVariableCommand*)(pw->lastCommand());
            Q_ASSERT(c);

            // select the connection
            if(m_connection->listBox()->findItem(c->connection())) {
                m_connection->setCurrentText(c->connection());
                connectionChangedSlot(c->connection());
            }

            QListViewItem* i = m_variables->findItem(c->name(), 0);
            Q_ASSERT(i);
            m_variables->setSelected(i, true);
            m_variables->ensureItemVisible(i);

        } else if(t == LTE_CMD_VARIABLE_DEL) {
            Q_ASSERT(pw->lastCommand());
            // removed dynamic_cast because it segfaults when compiled as
            // ActiveQt server
            AddDelVariableCommand* c =
                (AddDelVariableCommand*)(pw->lastCommand());
            Q_ASSERT(c);

            // select the connection
            if(m_connection->listBox()->findItem(c->connection())) {
                m_connection->setCurrentText(c->connection());
                connectionChangedSlot(c->connection());
            }
        }

        m_variables->setFocus();
    } else {
        if( pw ) {
            setCaption(tr("%1 Variables - Lintouch Editor")
                    .arg(d->pw->projectFileInfo().baseName()) );
        } else {
            setCaption(tr("Variables - Lintouch Editor"));
        }

        m_connection->setEnabled( false );
        m_variables->setEnabled( false );
        m_buttonNew->setEnabled( false );
        m_buttonDelete->setEnabled( false );

        m_connection->clear();
        m_variables->clear();
    }

    selectionChanged();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectVariablesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
