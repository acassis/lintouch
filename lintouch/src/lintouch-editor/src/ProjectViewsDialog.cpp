// $Id: ProjectViewsDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectViewsDialog.cpp --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 14 July 2004
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

#include "ProjectViewsDialog.hh"

#include "ProjectViewResizeDialog.hh"
#include "MainWindow.hh"
#include "AddDelViewCommand.hh"
#include "RenameViewCommand.hh"
#include "ReorderViewCommand.hh"
#include "ProjectWindow.hh"

#include <qpushbutton.h>
#include <qlistview.h>
#include <qmessagebox.h>

#include <EditorProject.hh>
using namespace lt;

/*
 * Specialized QListViewItem. Sets defaults and rtti.
 */
class ProjectViewsItem : public QListViewItem
{
public:
    enum {
        LTEDITORPROJECTVIEWSITEM = 1001
    };

    /*
     * Same parameters as QListViewItem but sets the second column value to
     * the same value as column 1. Needed for Rename
     */
    ProjectViewsItem( QListView* parent, QListViewItem* after,
            QString label1 ) : QListViewItem( parent, after )
    {
        setRenameEnabled( 0, true );
        setText( 0, label1 );
        setText( 1, label1 );
    }

    virtual int rtti() const {
        return LTEDITORPROJECTVIEWSITEM;
    }
};

struct ProjectViewsDialog::ProjectViewsDialogPrivate {

    ProjectViewsDialogPrivate() : pw(NULL),resizeDlg(NULL) {}

    void selectViewByName( QListView* lv, const QString& vn ) {

        Q_ASSERT(lv);
        Q_ASSERT(!vn.isEmpty());

        lv->clearSelection();

        QListViewItem * item = lv->findItem( vn, 0 );
        if( item ) {
            lv->setSelected( item, true );
            lv->ensureItemVisible( item );
        }
    }

    /**
     * TODO
     */
    ProjectWindow* pw;

    /**
     * Resize dialog holder.
     */
    ProjectViewResizeDialog* resizeDlg;
};

ProjectViewsDialog::ProjectViewsDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
    ProjectViewsDialogBase( parent, name, fl ),
    d(new ProjectViewsDialogPrivate)
{
    Q_CHECK_PTR(d);
    m_views->setSorting(-1);

    d->resizeDlg = new ProjectViewResizeDialog( this );
    Q_CHECK_PTR(d->resizeDlg);
}

ProjectViewsDialog::~ProjectViewsDialog()
{
    delete d;
}

void ProjectViewsDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT(d);

    bool enable = (pw != NULL);
    m_views->setEnabled( enable );
    m_buttonNew->setEnabled( enable );
    m_buttonDup->setEnabled( enable );
    m_buttonResize->setEnabled( enable );
    m_buttonDelete->setEnabled( enable );

    //qWarning( "ProjectViewsDialog::projectChangedSlot: "
    //        " pw = %d, t = %d", (unsigned)pw, t );

    if( d->pw != pw ) {

        //store current project
        d->pw = pw;

        refreshByViewMap();

        Q_ASSERT(d->resizeDlg);
        if( d->resizeDlg->isVisible() ) {
            // close the resizedialog on project change
            d->resizeDlg->close();
        }
    }

    if( pw ) {

        setCaption( QString( tr("%1 Views - Lintouch Editor") )
                .arg(pw->projectFileInfo().baseName()) );

        if(t == LTE_CMD_NOOP || t == LTE_CMD_VIEW_ADD ||
                t == LTE_CMD_VIEW_DELETE || t == LTE_CMD_VIEW_RENAME ||
                t == LTE_CMD_VIEW_SELECT || t == LTE_CMD_VIEW_RESIZE ||
                t == LTE_CMD_VIEW_ORDER_CHANGE)
        {
            QListViewItem * i = NULL;

            if( t == LTE_CMD_VIEW_RENAME && pw->lastCommand() &&
                    pw->lastCommand()->rtti() == LTE_CMD_VIEW_RENAME ) {

                RenameViewCommand* cmd =
                    (RenameViewCommand*)pw->lastCommand();
                Q_ASSERT(cmd);
                i = m_views->findItem( cmd->oldViewName(), 1 );
                Q_ASSERT(i);
                i->setText(0, cmd->newViewName());
                i->setText(1, cmd->newViewName());
            } else if( pw->lastCommand() &&
                    pw->lastCommand()->rtti() == LTE_CMD_VIEW_ADD_DEL ) {

                // add,dup,delete => refresh
                refreshByViewMap();

                if( t == LTE_CMD_VIEW_ADD  ) {
                    // only on add,dup there is a view to select
                    AddDelViewCommand* cmd =
                        (AddDelViewCommand*)pw->lastCommand();
                    Q_ASSERT(cmd);
                    i = m_views->findItem( cmd->viewName(), 0 );
                } else {
                    // on delete just select current view
                    i = m_views->findItem( pw->currentView(), 0 );
                }
            } else if( t == LTE_CMD_VIEW_ORDER_CHANGE ) {

                refreshByViewMap();

                ReorderViewCommand* cmd =
                    (ReorderViewCommand*)pw->lastCommand();
                Q_ASSERT(cmd);
                i = m_views->findItem( cmd->viewName(), 0 );
            } else {
                // just select current view
                i = m_views->findItem( pw->currentView(), 0 );
            }
            showViewItem( i );
        }
        m_views->setFocus();
    } else {
        setCaption( tr("Views - Lintouch Editor") );
    }

    selectionChanged();
}

void ProjectViewsDialog::refreshByViewMap()
{
    Q_ASSERT(m_views);

    m_views->clear();
    if( d->pw ) {
        ViewMap& vm = d->pw->project()->views();
        QListViewItem* i = NULL;

        for( unsigned j = 0; j < vm.count(); j++ ) {
            i = new ProjectViewsItem( m_views, i, vm.keyAt( j ) );
        }
    }
}
void ProjectViewsDialog::showViewItem( QListViewItem* i )
{
    Q_ASSERT(m_views);

    if( i ) {
        m_views->setSelected( i, true );
        m_views->ensureItemVisible( i );
    }
}

void ProjectViewsDialog::newViewSlot()
{
    Q_ASSERT(d);
    Q_ASSERT( d->pw );

    AddDelViewCommand* cmd = new AddDelViewCommand( d->pw,
            (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );
}

void ProjectViewsDialog::duplicateViewSlot()
{
    Q_ASSERT(d);
    Q_ASSERT( d->pw );

    QListViewItem* i = m_views->selectedItem();
    if( !i ) {
        // no view selected
        return;
    }

    AddDelViewCommand* cmd =
        new AddDelViewCommand( i->text(0), d->pw,
            (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );
}

void ProjectViewsDialog::resizeViewSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(d->pw);
    Q_ASSERT(d->resizeDlg);

    QListViewItem* i = m_views->selectedItem();
    if( !i ) {
        // no view selected
        return;
    }

    d->resizeDlg->exec( d->pw, i->text(0) );
}

void ProjectViewsDialog::deleteViewSlot()
{
    Q_ASSERT(d);
    Q_ASSERT( d->pw );

    QListViewItem* i = m_views->selectedItem();
    if( !i ) {
        // no view selected
        return;
    }

    AddDelViewCommand* cmd =
        new AddDelViewCommand( i->text(0), true, d->pw,
            (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );
}

void ProjectViewsDialog::viewUpSlot()
{
    Q_ASSERT(d);
    Q_ASSERT( d->pw );

    QListViewItem* i = m_views->selectedItem();
    if( !i ) {
        // no view selected
        return;
    }

    ReorderViewCommand* cmd =
        new ReorderViewCommand( i->text(0), true, d->pw,
            (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );
}

void ProjectViewsDialog::viewDownSlot()
{
    Q_ASSERT(d);
    Q_ASSERT( d->pw );

    QListViewItem* i = m_views->selectedItem();
    if( !i ) {
        // no view selected
        return;
    }

    ReorderViewCommand* cmd =
        new ReorderViewCommand( i->text(0), false, d->pw,
            (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );
}

void ProjectViewsDialog::selectionChanged()
{
    QListViewItem* i = m_views->selectedItem();

    bool enable = (i!=NULL);
    m_buttonDup->setEnabled( enable );
    m_buttonResize->setEnabled( enable );
    m_buttonDelete->setEnabled( enable );

    // disable up on first item and down on last item
    m_buttonUp->setEnabled( false );
    m_buttonDown->setEnabled( false );
    if( enable && i->itemAbove() != NULL ) {
        m_buttonUp->setEnabled( true );
    }
    if( enable && i->itemBelow() != NULL ) {
        m_buttonDown->setEnabled( true );
    }
}

void ProjectViewsDialog::itemRenamed( QListViewItem* i, int col )
{
    Q_ASSERT(i);
    Q_ASSERT(i->rtti() == ProjectViewsItem::LTEDITORPROJECTVIEWSITEM);
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

    for( QListViewItem *it = m_views->firstChild();
            it; it = it->itemBelow() ) {

        if( it != i && it->text(0) == i->text(0) ) {
            QMessageBox::warning( this,
                    tr("Error"),
                    QString(tr("The view name %1"
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

    RenameViewCommand* cmd =
        new RenameViewCommand( i->text(1), //old name
                i->text(0), //new name
                d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );

    d->pw->executeCommandLater( cmd );
}

void ProjectViewsDialog::viewDoubleClickedSlot( QListViewItem* i )
{
    Q_ASSERT(d);
    Q_ASSERT(d->pw);
    Q_ASSERT(i);
    Q_ASSERT(!i->text(0).isEmpty());

    d->pw->displayView(i->text(0));
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectViewsDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
