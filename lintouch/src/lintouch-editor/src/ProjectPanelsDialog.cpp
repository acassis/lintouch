// $Id: ProjectPanelsDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectPanelsDialog.cpp --
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

#include "ProjectPanelsDialog.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"
#include "AddDelPanelCommand.hh"
#include "RenamePanelCommand.hh"
#include "PanelOrderChangeCommand.hh"

#include <qpushbutton.h>
#include <qlistview.h>
#include <qmessagebox.h>

#include <EditorProject.hh>
using namespace lt;

/*
 * Specialized QListViewItem. Sets defaults and rtti.
 */
class ProjectPanelItem : public QListViewItem
{
public:
    enum {
        LTEDITORPROJECTPANELITEM = 1002
    };

    /*
     * Same parameters as QListViewItem but sets the second column value to
     * the same value as column 1. Needed for Rename
     */
    ProjectPanelItem( QListView* parent, QListViewItem* after,
            QString label1 ) : QListViewItem( parent, after )
    {
        setRenameEnabled( 0, true );
        setText( 0, label1 );
        setText( 1, label1 );
    }

    virtual int rtti() const {
        return LTEDITORPROJECTPANELITEM;
    }
};


struct ProjectPanelsDialog::ProjectPanelsDialogPrivate {

    ProjectPanelsDialogPrivate() : pw(NULL) {}
    /**
     * TODO
     */
    ProjectWindow* pw;

    /**
     * Fill in the m_panels QListView from current Project and current View.
     */
    void fillPanels( QListView* lv )
    {
        Q_ASSERT(lv);
        lv->clear();

        if( pw && pw->project()->views().count() > 0 ) {
            PanelMap pm = pw->project()->views()[pw->currentView()].panels();
            QListViewItem* i = NULL;

            for( unsigned j = 0; j < pm.count(); j++ ) {
                i = new ProjectPanelItem( lv, i, pm.keyAt( j ) );
            }
        }
    }
};

ProjectPanelsDialog::ProjectPanelsDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
    ProjectPanelsDialogBase( parent, name, fl ),
    d(new ProjectPanelsDialogPrivate)
{
    Q_CHECK_PTR(d);

    m_panels->setSorting(-1);
}

ProjectPanelsDialog::~ProjectPanelsDialog()
{
    delete d;
}

void ProjectPanelsDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT(d);

    bool enable = (pw != NULL);
    m_panels->setEnabled( enable );
    m_buttonNew->setEnabled( enable );
    m_buttonDuplicate->setEnabled( enable );
    m_buttonDelete->setEnabled( enable );

    //qWarning( "ProjectPanelsDialog::projectChangedSlot: "
    //        " pw = %d, t = %d", (unsigned)pw, t );

    if( pw != d->pw ) {
        //store current project
        d->pw = pw;

        d->fillPanels( m_panels );
    }

    if( pw ) {

        setCaption( QString( tr("%1/%2 Panels - Lintouch Editor") )
                .arg(pw->projectFileInfo().baseName())
                .arg(pw->currentView()) );

        if(t == LTE_CMD_PANEL_ADD || t == LTE_CMD_PANEL_DELETE ||
                t == LTE_CMD_PANEL_RENAME || t == LTE_CMD_PANEL_SELECT ||
                t == LTE_CMD_VIEW_ADD || t == LTE_CMD_VIEW_DELETE ||
                t == LTE_CMD_VIEW_RENAME || t == LTE_CMD_VIEW_SELECT ||
                t == LTE_CMD_PANEL_ORDER_CHANGE || t == LTE_CMD_NOOP ) {

            QListViewItem * i = NULL;

            if( pw->lastCommand() && t == LTE_CMD_PANEL_RENAME &&
                    pw->lastCommand()->rtti() == LTE_CMD_PANEL_RENAME ) {

                // select the renamed item
                RenamePanelCommand* cmd =
                    (RenamePanelCommand*)pw->lastCommand();
                Q_ASSERT(cmd);
                if( cmd->viewName() == pw->currentView() ) {
                    i = m_panels->findItem( cmd->oldPanelName(), 1 );
                    Q_ASSERT(i);
                    i->setText(0, cmd->newPanelName());
                    i->setText(1, cmd->newPanelName());
                }
            } else if( pw->lastCommand() &&
                    pw->lastCommand()->rtti() == LTE_CMD_PANEL_ADD_DEL ) {

                // add,dup,delete => refresh
                d->fillPanels( m_panels );

                if( t == LTE_CMD_PANEL_ADD ) {
                    // only on add,dup there is a panel to select
                    AddDelPanelCommand* cmd =
                        (AddDelPanelCommand*)pw->lastCommand();
                    Q_ASSERT(cmd);
                    if(pw->currentView() == cmd->viewName()) {
                        i = m_panels->findItem( cmd->panelName(), 0 );
                    }
                } else {
                    // on delete just select current panel
                    i = m_panels->findItem( pw->currentPanel(), 0 );
                }
            } else if( pw->lastCommand() &&
                    pw->lastCommand()->rtti() == LTE_CMD_PANEL_ORDER_CHANGE &&
                    t == LTE_CMD_PANEL_ORDER_CHANGE ) {

                // refrech all
                d->fillPanels( m_panels );

                // select the moved panel
                PanelOrderChangeCommand* cmd =
                    ( PanelOrderChangeCommand* )pw->lastCommand();
                Q_ASSERT(cmd);
                if(pw->currentView() == cmd->viewName()) {
                    i = m_panels->findItem( cmd->newPanel(), 0 );
                }
            } else if( t == LTE_CMD_VIEW_ADD || t == LTE_CMD_VIEW_DELETE ||
                    t == LTE_CMD_VIEW_RENAME || t == LTE_CMD_VIEW_SELECT ) {

                // refrech all
                d->fillPanels( m_panels );

                i = m_panels->findItem( pw->currentPanel(), 0 );
            } else {
                // just select current panel
                i = m_panels->findItem( pw->currentPanel(), 0 );
            }
            showPanelItem( i );
        }
        m_panels->setFocus();
    } else {
        setCaption( tr("Panels - Lintouch Editor") );
    }

    selectionChangedSlot();
}

void ProjectPanelsDialog::showPanelItem( QListViewItem* i )
{
    Q_ASSERT(m_panels);

    if( i ) {
        m_panels->setSelected( i, true );
        m_panels->ensureItemVisible( i );
    }
}

void ProjectPanelsDialog::newPanelSlot()
{
    Q_ASSERT(d);
    Q_ASSERT( d->pw );

    AddDelPanelCommand* cmd = new AddDelPanelCommand( d->pw,
            (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );
}

void ProjectPanelsDialog::duplicatePanelSlot()
{
    Q_ASSERT(d);
    Q_ASSERT( d->pw );

    QListViewItem* i = m_panels->selectedItem();
    if( !i ) {
        // no panel selected
        return;
    }

    AddDelPanelCommand* cmd =
        new AddDelPanelCommand( i->text(0), d->pw,
                (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );
}

void ProjectPanelsDialog::deletePanelSlot()
{
    Q_ASSERT(d);
    Q_ASSERT( d->pw );

    QListViewItem* i = m_panels->selectedItem();
    if( !i ) {
        // no panel selected
        return;
    }

    AddDelPanelCommand* cmd =
        new AddDelPanelCommand( i->text(0), true, d->pw,
                (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );
}

void ProjectPanelsDialog::panelUpSlot()
{
    QListViewItem* itemSelected = m_panels->selectedItem();
    Q_ASSERT( itemSelected );
                                                                                                                             
    QListViewItem* prevSibling = itemSelected->itemAbove();
    Q_ASSERT( prevSibling );
                                                                                                                             
    QString selectedPanel = itemSelected->text(0);
                
    QString prevPanel = prevSibling->text(0);

    PanelOrderChangeCommand* cmd;
    cmd = new PanelOrderChangeCommand( prevPanel ,selectedPanel,
                d->pw, (MainWindow*)parent() );
                                                                                                                             
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );

}

void ProjectPanelsDialog::panelDownSlot()
{
    QListViewItem* itemSelected = m_panels->selectedItem();
    Q_ASSERT( itemSelected );
                                                                                                                             
    QListViewItem* nextSibling = itemSelected->itemBelow();
    Q_ASSERT( nextSibling );
                                                                                                                             
    QString selectedPanel = itemSelected->text(0);
                                                                                                                           
    QString nextPanel = nextSibling->text(0);
                                                                                                                         
    PanelOrderChangeCommand* cmd;
    cmd = new PanelOrderChangeCommand( nextPanel ,selectedPanel,
                d->pw, (MainWindow*)parent() );
                                                                                                                             
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );

}

void ProjectPanelsDialog::selectionChangedSlot()
{
    QListViewItem* i = m_panels->selectedItem();

    bool enable = (i!=NULL);
    m_buttonDuplicate->setEnabled( enable );
    m_buttonDelete->setEnabled( enable );
    // m_buttonUp->setEnabled( enable );
    // m_buttonDown->setEnabled( enable );

    bool enableMoveUp = false;
    bool enableMoveDown = false;
                                                                                                                             
    if( i ) {
        QListViewItem* prevSibling = i->itemAbove();
        QListViewItem* nextSibling = i->itemBelow();
        enableMoveUp = ( prevSibling != 0 ) ? true : false ;
        enableMoveDown = ( nextSibling != 0 ) ? true : false ;
    }
                                                                                                                             
    m_buttonUp->setEnabled( enableMoveUp );
    m_buttonDown->setEnabled( enableMoveDown );

}

void ProjectPanelsDialog::itemRenamedSlot( QListViewItem* i, int col )
{
    Q_ASSERT(i);
    Q_ASSERT(i->rtti() == ProjectPanelItem::LTEDITORPROJECTPANELITEM);
    Q_ASSERT(col == 0 );

    i->setText(0, i->text(0).stripWhiteSpace());

    if( i->text(0).isEmpty()) {
        i->setText(0, i->text(1)); // silently rename back
        return;
    }

    if( i->text(0) == i->text(1) ) {
        // just spaces has been changed. ignore.
        return;
    }

    for( QListViewItem *it = m_panels->firstChild();
            it; it = it->itemBelow() ) {

        if( it != i && it->text(0) == i->text(0) ) {
            QMessageBox::warning( this,
                    tr("Error"),
                    QString(tr("The panel name %1"
                            " is already used within this project."
                            " Please choose a different name."))
                        .arg(i->text(0)),
                    QMessageBox::Ok,
                    QMessageBox::NoButton,
                    QMessageBox::NoButton );

            i->setText(0, i->text(1)); // rename back
            return;
        }
    }

    RenamePanelCommand* cmd =
        new RenamePanelCommand( i->text(1), //old name
                i->text(0), //new name
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );

    d->pw->executeCommandLater( cmd );
}

void ProjectPanelsDialog::panelDoubleClickedSlot( QListViewItem* i )
{
    Q_ASSERT(d);
    Q_ASSERT(d->pw);
    Q_ASSERT(i);
    d->pw->displayPanel( i->text(0) );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPanelsDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
