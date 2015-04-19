// $Id: ProjectResourcesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectResourcesDialog.cpp --
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

#include "ProjectResourcesDialog.hh"
#include "MainWindow.hh"
#include "AddResourceCommand.hh"
#include "DeleteResourceCommand.hh"
#include "RenameResourceCommand.hh"
#include "ProjectWindow.hh"

#include <qpushbutton.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qfiledialog.h>

#include <EditorProject.hh>
using namespace lt;
/*
 * Specialized QListViewItem. Sets defaults and rtti.
 */
class ProjectResourceItem : public QListViewItem
{
public:
    enum {
        LTEDITORPROJECTRESOURCEITEM = 1003
    };
    /*
     * Same parameters as QListViewItem but sets the second column value to
     * the same value as column 1. Needed for Rename
     */
    ProjectResourceItem( QListView* parent, QListViewItem* after,
            QString label1 ) : QListViewItem( parent, after )
    {
        setRenameEnabled( 0, true );
        setCurrentValue( label1 );
        setOldValue( label1 );
    }

    void reset()
    {
        setText( 0, oldValue() );
    }

    QString oldValue() const
    {
        return text( 1 );
    }

    QString currentValue() const
    {
        return text( 0 );
    }

    void setCurrentValue( const QString& v )
    {
        setText( 0, v );
    }

    void setOldValue( const QString& v )
    {
        setText( 1, v );
    }

    virtual int rtti() const {
        return LTEDITORPROJECTRESOURCEITEM;
    }
protected:

    virtual void startRename( int col )
    {
        QListViewItem::startRename( col );
        QLineEdit* le = getInPlaceEditBox( col );
        Q_ASSERT( le );
        if ( le ){
            QString text = le->text();
            le->deselect();
            le->setSelection( 0, text.findRev('.') );
        }
    }
private:
    QLineEdit* getInPlaceEditBox(int col)
    {
        QListView* lv = listView();
        Q_ASSERT( lv );
        QRect rcCell = lv->itemRect( this );
        int y = rcCell.y();
        int x = rcCell.x();
        for ( int c = 0; c< col; c++ ){
            x += lv->columnWidth(c);
        }
        //qWarning("x:%d, y:%d", x, y );
        return (QLineEdit*)lv->viewport()->childAt( x, y );
    }

};


struct ProjectResourcesDialog::ProjectResourcesDialogPrivate{

    ProjectResourcesDialogPrivate() : pw(NULL) {}

    /**
     * Stores the reference of the current project window.
     */
    ProjectWindow* pw;
};

ProjectResourcesDialog::ProjectResourcesDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
        ProjectResourcesDialogBase( parent, name, fl ),
        d(new ProjectResourcesDialogPrivate)
{
    Q_CHECK_PTR(d);
    m_resources->setSorting(-1);
}

ProjectResourcesDialog::~ProjectResourcesDialog()
{
    delete d;
}

void ProjectResourcesDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t)
{
    Q_ASSERT(d);

    bool enable = (pw != NULL);
    m_resources->setEnabled( enable );
    m_buttonNew->setEnabled( enable );
    m_buttonReimport->setEnabled( enable );
    m_buttonExport->setEnabled( enable );
    m_buttonDelete->setEnabled( enable );

    if( d->pw ||  t == LTE_CMD_NOOP || t == LTE_CMD_RESOURCE_ADD
            || t == LTE_CMD_RESOURCE_DELETE || t == LTE_CMD_RESOURCE_RENAME ) {

        //store current project
        d->pw = pw;

        if (t != LTE_CMD_RESOURCE_RENAME){
            m_resources->clear();
        }

        if( d->pw ) {
            setCaption( QString( tr("%1 Resources - Lintouch Editor") )
                    .arg(pw->projectFileInfo().baseName()) );

            if (t != LTE_CMD_RESOURCE_RENAME){
                refreshFromResourcePool();
#ifdef __DUMP_RESOURCE_LV__
                dumpResourceContents( "refreshFromResourcePool",
                        "refreshFromResourcePool end" );
#endif //__DUMP_RESOURCE_LV__
            }

            if ( d->pw->lastCommand() ) {
                QString resourceNameToBeSelected;

                if ( d->pw->lastCommand()->rtti() ==
                        LTE_CMD_RESOURCE_RENAME ){
                    // select the renamed item
                    RenameResourceCommand* cmd =
                        (RenameResourceCommand*)pw->lastCommand();
                    Q_ASSERT(cmd);

#ifdef __DUMP_RESOURCE_LV__
                    dumpResourceContents( "LTE_CMD_RESOURCE_RENAME",
                            "LTE_CMD_RESOURCE_RENAME end");
#endif//__DUMP_RESOURCE_LV__
                    ProjectResourceItem* i =
                        (ProjectResourceItem*)m_resources->
                        findItem( cmd->oldResourceName(), 1 );

                    if ( i ){
                        resourceNameToBeSelected = cmd->newResourceName();
                        i->setCurrentValue( resourceNameToBeSelected );
                        i->setOldValue( resourceNameToBeSelected );
                    }
                } else if ( d->pw->lastCommand()->rtti() ==
                        LTE_CMD_RESOURCE_ADD){
                    AddResourceCommand* cmd =
                        (AddResourceCommand*)pw->lastCommand();
                    Q_ASSERT( cmd );
                    resourceNameToBeSelected = cmd->resourceKey();
                }
                QListViewItem * lv =
                    m_resources->findItem( resourceNameToBeSelected, 0 );

                if ( lv ){
                    m_resources->setSelected( lv, true );
                    m_resources->ensureItemVisible( lv );
                }
            }
            m_resources->setFocus();
        } else {
            setCaption( tr("Resources - Lintouch Editor") );
        }
    }
    selectionChanged();
}

void ProjectResourcesDialog::applySlot()
{
    //TODO
}

void ProjectResourcesDialog::okSlot()
{
    applySlot();
    close();
}

void ProjectResourcesDialog::newResourceSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );

    QStringList resources = QFileDialog::getOpenFileNames(
        "*.*", QString::null,
        this, NULL, tr("Add Resource - Lintouch Editor") );

    QStringList::iterator i = resources.begin();

    while( i != resources.end() ){
        QString newResource = *i;
        AddResourceCommand* cmd = new AddResourceCommand(
                newResource, d->pw, (MainWindow*)parent() );
        Q_CHECK_PTR( cmd );
        d->pw->executeCommand( cmd );
        ++i;
    }
}

void ProjectResourcesDialog::reimportResourceSlot()
{
    Q_ASSERT(d);

    ProjectResourceItem* i =
            (ProjectResourceItem*)m_resources->selectedItem();
    Q_ASSERT(i);

    QString fn =
        d->pw->project()->fullPathToResouce(i->currentValue());

    fn = QFileDialog::getOpenFileName( fn );

    if(fn.isNull()) {
        return;
    }

    AddResourceCommand* cmd = new AddResourceCommand(i->currentValue(),
        fn, d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );
}

void ProjectResourcesDialog::exportResourceSlot()
{
    Q_ASSERT(d);

    ProjectResourceItem* i =
            (ProjectResourceItem*)m_resources->selectedItem();
    Q_ASSERT(i);

    QString fn = QFileDialog::getSaveFileName(i->currentValue());
    if(fn.isNull()) {
        return;
    }

    QIODevice* raw = d->pw->project()->unpackedResources()
        ->openFile(i->currentValue());
    Q_ASSERT(raw);

    if(raw->isOpen()) {

        Q_LONG fs = -1;
        QFile fw(fn);
        if(fw.open(IO_WriteOnly|IO_Truncate)) {
            fs = fw.writeBlock(raw->readAll());

            if(fs != raw->size()) {
                // can't write all
                QMessageBox::warning( this,
                        tr("Error"),
                        tr("Unable to export selected "
                            "resource to the file %1.").arg(fn),
                        QMessageBox::Ok,
                        QMessageBox::NoButton );
            }
        } else {
            // can't open for write
            QMessageBox::warning( this,
                    tr("Error"),
                    tr("Unable to write to the file %1.").arg(fn),
                    QMessageBox::Ok,
                    QMessageBox::NoButton );
        }
    } else {
        // no such resource?
        qWarning("Unable to open selected resource %1.",
                i->currentValue().latin1());
    }
    delete raw;
}

void ProjectResourcesDialog::deleteResourceSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );

    ProjectResourceItem* i =
            (ProjectResourceItem*)m_resources->selectedItem();

    if ( i ) {
        DeleteResourceCommand* cmd =
                new DeleteResourceCommand( i->currentValue(), d->pw,
                (MainWindow*)parent() );
        Q_CHECK_PTR( cmd );
        d->pw->executeCommand( cmd );
    }
}

void ProjectResourcesDialog::refreshFromResourcePool()
{
    VFSPtr unpkdResource = d->pw->project()->unpackedResources();

    if ( ( unpkdResource.isNull() == false )
            && ( unpkdResource->isValid() ) ) {
        QStringList rl = unpkdResource->ls();
        QListViewItem* i = NULL;

        for( QStringList::const_iterator it = rl.begin();
                it != rl.end(); ++it ) {
            i = m_resources->findItem( *it, 0 );

            if ( i == NULL ){
                i = new ProjectResourceItem( m_resources, i, *it );
            }
        }
    }

}

void ProjectResourcesDialog::selectionChanged()
{
    QListViewItem* i = m_resources->selectedItem();

    bool enable = (i!=NULL);
    m_buttonExport->setEnabled( enable );
    m_buttonReimport->setEnabled(enable);
    m_buttonDelete->setEnabled( enable );
}


void ProjectResourcesDialog::itemRenamed( QListViewItem* i,
        int c )
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );

    Q_ASSERT( i );
    Q_ASSERT( c == 0 );
    Q_ASSERT( i->rtti() == 
            ProjectResourceItem::LTEDITORPROJECTRESOURCEITEM );

    ((ProjectResourceItem*)i)->setCurrentValue(
            ((ProjectResourceItem*)i)->currentValue().
            stripWhiteSpace() );

    if( ( (ProjectResourceItem*) i )->currentValue().isEmpty() ) {
        ( (ProjectResourceItem*) i )->reset(); // silently rename back
#ifdef __DUMP_RESOURCE_LV__
        dumpResourceContents( "itemRenamed:reset", "itemRenamed:reset end");
#endif //__DUMP_RESOURCE_LV__
        return;
    }

    if ( ((ProjectResourceItem*)i)->currentValue() ==
            ((ProjectResourceItem*)i)->oldValue() ){
#ifdef __DUMP_RESOURCE_LV__
        dumpResourceContents( "itemRenamed:equal", "itemRenamed:equal end");
#endif //__DUMP_RESOURCE_LV__
        return;
    }

    for( ProjectResourceItem *it =
            (ProjectResourceItem*)m_resources->firstChild();
            it; it = (ProjectResourceItem*)it->itemBelow() ) {

        if( it != i && it->currentValue() ==
                ((ProjectResourceItem*)i)->currentValue() ) {
            QMessageBox::warning( this,
                    tr("Error"),
                    QString(tr("The resource name %1"
                            " is already used within this project."
                            " Please choose a different name."))
                        .arg(((ProjectResourceItem*)i)->
                                currentValue()),
                    QMessageBox::Ok,
                    QMessageBox::NoButton );
            ((ProjectResourceItem*)i)->reset(); // rename back
            return;
        }

    }

#ifdef __DUMP_RESOURCE_LV__
    dumpResourceContents( "itemRenamed", "itemRenamed end");
#endif //__DUMP_RESOURCE_LV__
    RenameResourceCommand* cmd =
            new RenameResourceCommand(
            ((ProjectResourceItem*)i)->oldValue(), //old name
            ((ProjectResourceItem*)i)->currentValue(), //new name
            d->pw, (MainWindow*)parent() );
    Q_CHECK_PTR( cmd );
    d->pw->executeCommandLater( cmd );
}

void ProjectResourcesDialog::dumpResourceContents(const QString&
        header, const QString& footer )
{
    Q_ASSERT( m_resources );

    QListViewItemIterator lvi( m_resources );
    qWarning( header );

    while( lvi.current() )
    {
        ProjectResourceItem* i = (ProjectResourceItem*)
                lvi.current();
        QString info = QString("1. Current value : %1 ,\
                        \n2. Old value : %2").
                        arg( i->currentValue() ).
                        arg( i->oldValue() );
        qWarning( info );
        ++lvi;
    }
    qWarning( footer );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectResourcesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
