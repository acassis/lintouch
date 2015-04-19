// $Id: ProjectTemplatesDialog.cpp 1207 2006-01-05 15:08:45Z mman $
//
//   FILE: ProjectTemplatesDialog.cpp --
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

#include "ProjectTemplatesDialog.hh"
#include "MainWindow.hh"
#include "ProjectWindow.hh"
#include "TemplateTabOrderChangeCommand.hh"
#include "TemplateShortcutChangeCommand.hh"
#include "RenameTemplateCommand.hh"
#include "RenameViewCommand.hh"
#include "RenamePanelCommand.hh"
#include "PanelOrderChangeCommand.hh"
#include "AddDelPanelCommand.hh"

#include <qpushbutton.h>
#include <qcombobox.h>
#include <qdict.h> 
#include <qheader.h>
#include <qlabel.h> 
#include <qmap.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qgroupbox.h> 
#include <qmessagebox.h>
#include <qapplication.h>

#include <EditorProject.hh>
#include <EditablePanelView.hh>
#include <EditablePanelViewContainer.hh>

using namespace lt;

enum {
    COL_CHECK = 0,
    COL_NAME,
    COL_KEY
};

/*
 * Specialized QListViewItem. Sets defaults and rtti.
 */
class ProjectTemplateItem : public QCheckListItem
{
public:
    enum {
        LTEDITORPROJECTTEMAPLATEITEM = 1007
    };

    /*
     * Same parameters as QListViewItem but sets the second column value to
     * the same value as column 1. Needed for Rename
     */
    ProjectTemplateItem( QListView* parent, QListViewItem* after,
            QString label1, QString label2 ) :
        QCheckListItem( parent, after, QString::null, QCheckListItem::CheckBox)
    {
        setText( COL_NAME, label1 );
        setText( COL_KEY, label2 );

        setMultiLinesEnabled( false );
        setRenameEnabled( COL_NAME , true );
    }

    virtual int rtti() const {
        return LTEDITORPROJECTTEMAPLATEITEM;
    }

    static ProjectTemplatesDialog* tpd;
    static bool loading;
protected:
    virtual void stateChange(bool b)
    {
        if(loading) return;

        Q_ASSERT(tpd);
        tpd->changeTemplateLock(b, text(COL_NAME));
    }
};

ProjectTemplatesDialog* ProjectTemplateItem::tpd = NULL;
bool ProjectTemplateItem::loading = false;


struct TemplatesShortCutKeyCapture::
        TemplatesShortCutKeyCapturePrivate {
    TemplatesShortCutKeyCapturePrivate():
    keySequence( NULL )
    {
    }

    /**
     * This holds the shortcut key sequence
     */
    QString* keySequence;

    /**
     * Layout of the dialog
     */
    QGridLayout* shortCutKeyCaptureLayout;

    /**
     * Layout of the groupbox used in the dialog
     */
    QGridLayout* groupBoxLayout;

    /**
     * The label used to convey to the user that he should type in the key
     * combination
     */
    QLabel* label;

    /**
     * Group box used in the dialog
     */
    QGroupBox* groupBox;

};

TemplatesShortCutKeyCapture::TemplatesShortCutKeyCapture (
        ProjectTemplatesDialog* parent, QString* keySequence ,
        const char* name /*= 0*/, WFlags fl /* = 0 */) :
    QDialog( parent, name, true, fl ),
    d(new TemplatesShortCutKeyCapturePrivate )
{

    Q_ASSERT( keySequence );
    d->keySequence = keySequence;
    d->shortCutKeyCaptureLayout = new QGridLayout( this, 1, 1, 1, 6,
            "ShortCutDialogLayOut" );
    d->groupBox = new QGroupBox( this );
    d->groupBoxLayout = new QGridLayout( d->groupBox, 1, 1, 11, 6,
             "groupBoxLayOut" );
    d->label = new QLabel( d->groupBox );
    d->groupBoxLayout->addWidget( d->label, 0, 0 );
    d->label->setText( tr( "Please press the shortcut key combination" ) );
    d->label->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    d->shortCutKeyCaptureLayout->addWidget( d->groupBox, 0, 0 );

}

TemplatesShortCutKeyCapture::~TemplatesShortCutKeyCapture()
{
    delete d;
}

void TemplatesShortCutKeyCapture::keyPressEvent( QKeyEvent* e )
{

    Q_ASSERT( d->keySequence );

    if( e->key() == Key_Control || e->key() == Key_Meta || 
            e->key() == Key_Shift || e->key() == Key_Alt ||
            e->key() == Key_Menu ) {
        return;
    }

    if( e->key() != Key_Escape ) {
        *(d->keySequence) = ( QString ) Template::keyEvent2Sequence( e );
    }

    QDialog::keyPressEvent(e );
    done( 0 ); 

}

void TemplatesShortCutKeyCapture::focusOutEvent( QFocusEvent* e )
{
    QDialog::focusOutEvent( e );
    done( 0 );
}

struct ProjectTemplatesDialog::
        ProjectTemplatesDialogPrivate {
    ProjectTemplatesDialogPrivate() : pw( NULL )
    {
    }

    /**
     * Current project
     */
    ProjectWindow* pw;

    /**
    * Name of the current view
    */
    QString currentView;

    /**
     * Name of current panel
     */
    QString currentPanel;

    /**
    * This points to the template dict of the current panel
    **/
    TemplateDict* currentTemplateDict;

    /**
    * This holds the old name or old key sequence of the template
    * that's changed recently. This is used to revert back if validation
    * fails.
    */
    QString oldItemText;

};

ProjectTemplatesDialog::ProjectTemplatesDialog (
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
    ProjectTemplatesDialogBase( parent, name, fl ),
    d(new ProjectTemplatesDialogPrivate)
{
    Q_CHECK_PTR( d );

    connect( moveUp, SIGNAL( clicked() ),
            this, SLOT(moveUpSlot() ) );
    connect( moveDown, SIGNAL( clicked() ),
            this, SLOT( moveDownSlot() ) );
    connect( panel, SIGNAL( activated( const QString & ) ),
            this, SLOT( panelSelectedSlot( const QString & ) ) );
    connect( templateList, SIGNAL( selectionChanged( QListViewItem* ) ),
            this, SLOT( templateSelectionChangedSlot( QListViewItem* ) ) );
    connect( templateList,
            SIGNAL( doubleClicked( QListViewItem*,const QPoint&,int ) ),
            this, SLOT( itemDoubleClickedSlot( QListViewItem*,
                         const QPoint&, int ) ) );
    connect( templateList,
            SIGNAL( clicked( QListViewItem*,const QPoint&,int ) ),
            this, SLOT( itemClickedSlot( QListViewItem*,
                         const QPoint&, int ) ) );
    connect( templateList,
           SIGNAL( itemRenamed( QListViewItem*,int ) ),
           this,
           SLOT( itemRenamedSlot( QListViewItem*, int ) ) );
    connect( templateList,
           SIGNAL( returnPressed( QListViewItem* ) ),
           this,
           SLOT( returnPressedSlot( QListViewItem* ) ) );

    templateList->installEventFilter(this);

    templateList->setColumnWidth(0, 25);
    templateList->setSorting( -1 );
    templateList->header()->setClickEnabled( FALSE );
    templateList->setFocus();

    ProjectTemplateItem::tpd = this;
}

ProjectTemplatesDialog::~ProjectTemplatesDialog()
{
    delete d;
}

void ProjectTemplatesDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT(d);

    //qWarning( "ProjectTemplatesDialog::projectChangedSlot: "
    //        " pw = %d, t = %d", (unsigned)pw, t );

    if( !pw ) {
        d->pw = pw;
        setCaption( tr("Templates - Lintouch Editor") );
        clearAndDisableControls();
        return;
    }

    if( pw != d->pw ||
            t == LTE_CMD_SELECTION_ADD || t == LTE_CMD_SELECTION_DEL ) {

        d->pw = pw;
        d->currentView  = pw->currentView();
        d->currentPanel = pw->currentPanel();
        populatePanelNames();
        lblPanel->setEnabled( TRUE );

    } else if ( t == LTE_CMD_VIEW_SELECT ) {
        if( d->currentView != pw->currentView() ) {
            populatePanelNames();
            d->currentView = pw->currentView();
            d->currentPanel = pw->currentPanel();
        }
    } else if ( t == LTE_CMD_PANEL_SELECT ) {
        QString currentPanel = pw->currentPanel();
        if ( !currentPanel.isNull() ) {
            if( panel->listBox()->findItem( currentPanel,
                    Qt::ExactMatch ) ) {
                if( d->currentView == pw->currentView() ) {
                    panel->setCurrentText( currentPanel );
                    panelSelectedSlot( currentPanel );
                    d->currentPanel = pw->currentPanel();
                }
            }
        }
    }

    templateList->setFocus();

    if( t == LTE_CMD_SELECTION_CHANGE ) {

        EditablePanelView * panelView =
            pw->m_viewContainer->currentPanelView();

        if( panelView == NULL ) {
            clearAndDisableControls();
        } else {

            const TemplateDict & td = panelView->currentSelection();

            if( td.count() > 0 ) {
                // find the item to be selected and select it
                templateList->clearSelection();
                QListViewItem * i = templateList->findItem(
                        td.currentKey(), COL_NAME );

                //FIXME: could be Q_ASSERT( i ) instead of if, but I'm not able
                //to tell this right now
                if( i ) {
                    templateList->setSelected( i, true );
                    templateList->ensureItemVisible( i );
                }

            } else {
                // unselect all items
                templateList->clearSelection();
                templateSelectionChangedSlot( NULL );
            }
        }

        return;
    }

    if(!pw->lastCommand() ) {
        return;
    }

    if( t == LTE_CMD_TEMPLATE_TABORDER_CHANGE || 
            t == LTE_CMD_TEMPLATE_SHORTCUT_CHANGE ||
            t == LTE_CMD_TEMPLATE_RENAME  )  {

        QString templateViewName,templatePanelName,currentTemplate;

        if( t == LTE_CMD_TEMPLATE_TABORDER_CHANGE &&
                pw->lastCommand()->rtti() ==
                LTE_CMD_TEMPLATE_TABORDER_CHANGE  ) {
            TemplateTabOrderChangeCommand* cmd =
                    ( TemplateTabOrderChangeCommand* )
                     pw->lastCommand();
            Q_ASSERT( cmd );
            templateViewName = cmd->viewName();
            templatePanelName = cmd->panelName();
            currentTemplate = cmd->newTemplate();
            if( templateViewName == d->currentView && 
                    panel->currentText() == templatePanelName ) {
                //refresh the panel
                panelSelectedSlot( templatePanelName );
            } else {
                //fixme
            }
        }
        else if ( t == LTE_CMD_TEMPLATE_SHORTCUT_CHANGE && 
                pw->lastCommand()->rtti() == 
                LTE_CMD_TEMPLATE_SHORTCUT_CHANGE ) {
            TemplateShortcutChangeCommand* cmd =
                        ( TemplateShortcutChangeCommand* )
                        pw->lastCommand();
            Q_ASSERT( cmd );
            templateViewName = cmd->viewName();
            templatePanelName = cmd->panelName();
            currentTemplate = cmd->currentTemplateName();
            QString oldKey = cmd->oldShortCutKey();
            QString newKey = cmd->newShortCutKey();
            QString stolenTemplate = cmd->stolenFromTemplateName();
            if( templateViewName == d->currentView && 
                    panel->currentText() == templatePanelName ) {
                QListViewItem * lv = templateList->findItem(
                        currentTemplate , COL_NAME );
                if( lv && lv->text(COL_KEY) != newKey ) {
                    lv->setText( COL_KEY, ( QString ) QKeySequence( newKey ) );
                    if( !stolenTemplate.isEmpty() ) {
                        QListViewItem * slv =
                                templateList->findItem(
                               stolenTemplate, COL_NAME );
                        if( slv ) {
                            if( !( slv->text(COL_KEY).isEmpty() ) ) {
                                slv->setText( COL_KEY, 
                                    ( QString ) QKeySequence() );
                            } else {
                                slv->setText( COL_KEY,
                                    ( QString ) QKeySequence( oldKey ) );
                            }
                        }
                    }
                }
            } else {
                //fixme
            }
        } else if ( t == LTE_CMD_TEMPLATE_RENAME && 
                pw->lastCommand()->rtti() == 
                LTE_CMD_TEMPLATE_RENAME ) {
            RenameTemplateCommand* cmd =
                        ( RenameTemplateCommand* )
                        pw->lastCommand();
            Q_ASSERT( cmd );
            templateViewName = cmd->viewName();
            templatePanelName = cmd->panelName();
            QString oldName = cmd->oldTemplateName();
            QString newName  = cmd->newTemplateName();
            currentTemplate = newName;
            if( templateViewName == d->currentView && 
                    panel->currentText() == templatePanelName ) {
                QListViewItem * lv = templateList->findItem(
                        oldName , COL_NAME );
                if( lv ) {
                    lv->setText( COL_NAME, newName );
                }
            } else {
                //fixme
            }
        }
        QListViewItem * lv = templateList->findItem(
                currentTemplate , COL_NAME );
        if( lv ) {
            templateList->setSelected( lv, true );
            templateList->ensureItemVisible( lv );
            templateSelectionChangedSlot( lv );
        }
    } else if( t == LTE_CMD_VIEW_RENAME && 
            pw->lastCommand()->rtti() == LTE_CMD_VIEW_RENAME ) {
        RenameViewCommand* cmd =
                (RenameViewCommand*)pw->lastCommand();
        Q_ASSERT( cmd );

        if ( d->currentView == cmd->oldViewName() ) {
            d->currentView = cmd->newViewName();
            setCaption( QString( tr("%1/%2 Templates - Lintouch Editor") )
                .arg( d->pw->projectFileInfo().baseName() )
                .arg( d->currentView ) );            
        }
    } else if ( t == LTE_CMD_PANEL_RENAME && 
            pw->lastCommand()->rtti() == LTE_CMD_PANEL_RENAME ) {

        RenamePanelCommand* cmd =
                (RenamePanelCommand*)pw->lastCommand();
        Q_ASSERT( cmd );

        if ( d->currentView == cmd->viewName() ){
            QString oldPanelName = cmd->oldPanelName();
            QListBoxItem*  item =
                    panel->listBox()->findItem( oldPanelName, 
                    Qt::ExactMatch);
            Q_ASSERT( item );
            int index = panel->listBox()->index( item );
            panel->changeItem( cmd->newPanelName(), index );
            if( d->currentPanel == oldPanelName ) {
                d->currentPanel = cmd->newPanelName();
            }
        }
    } else if ( t == LTE_CMD_PANEL_ORDER_CHANGE &&
            pw->lastCommand()->rtti() == LTE_CMD_PANEL_ORDER_CHANGE ) {

        PanelOrderChangeCommand* cmd =
                (PanelOrderChangeCommand*)pw->lastCommand();
        Q_ASSERT(cmd);

        if ( d->currentView == cmd->viewName() ){

            QListBoxItem*  item =
                    panel->listBox()->findItem( cmd->newPanel(),
                    Qt::ExactMatch );
            Q_ASSERT( item );
            int index = panel->listBox()->index( item );
            Q_ASSERT( index == cmd->oldPos() );

            item = panel->listBox()->findItem(
                        cmd->oldPanel(),Qt::ExactMatch );
            Q_ASSERT( item );
            index = panel->listBox()->index( item );
            Q_ASSERT( index == cmd->newPos() );

            QString panelCurrentText = panel->currentText();
            bool panelCurrentTextChanged = ( panelCurrentText == 
                    cmd->oldPanel() ) ? true: false;

            panel->changeItem( cmd->newPanel(),
                    cmd->newPos() );
            panel->changeItem( cmd->oldPanel(),
                    cmd->oldPos() );
            if( panelCurrentTextChanged ) {
                panel->setCurrentText( cmd->oldPanel() );
            }
        }
    } else if ( t == LTE_CMD_PANEL_ADD || t == LTE_CMD_PANEL_DELETE ) {

        if ( pw->lastCommand()->rtti()
                == LTE_CMD_PANEL_ADD_DEL ){

            AddDelPanelCommand* cmd =
                            (AddDelPanelCommand*)pw->lastCommand();
            Q_ASSERT(cmd);
            if( d->currentView == cmd->viewName() ) {

                if ( t == LTE_CMD_PANEL_ADD ){
                    panel->insertItem( cmd->panelName(),
                            cmd->position() );
                    if( !panel->isEnabled() ) {
                        panel->setEnabled( true );
                        if( !templateList->isEnabled() ) {
                            templateList->setEnabled( true );
                        }
                        if( !lblPanel->isEnabled() ) {
                            lblPanel->setEnabled( true );
                        }
                    }
                } else if ( t == LTE_CMD_PANEL_DELETE ){
                    QListBoxItem*  item =
                            panel->listBox()->findItem( cmd->panelName(),
                            Qt::ExactMatch);
                    Q_ASSERT( item );
                    int index = panel->listBox()->index( item );
                    panel->removeItem( index );
                    if ( pw->project()->views()[
                            cmd->viewName() ].panels().count() == 0) {
                        clearAndDisableControls();
                    }
                }
            }
        }
    }
}

void ProjectTemplatesDialog::
        panelSelectedSlot( const QString& panelName )
{
    Q_ASSERT(d);
    Q_ASSERT(d->pw);

    templateList->clear();

    if( !panelName.isEmpty() ) {
        QString currentView,currentPanel;
        currentView = d->pw->currentView();
        currentPanel = d->pw->currentPanel();

        Q_ASSERT( !currentView.isEmpty() );

        View view =  d->pw->project()->views()[ currentView ];

        Panel panel = view.panels()[ panelName ];

        if( panelName != currentPanel ) {
            d->pw->m_viewContainer->displayPanel( panelName );
            return;
        }

        d->currentTemplateDict = &panel.templates();
        QString currentTemplate = d->currentTemplateDict->currentKey();

        ProjectTemplateItem* itemTemplateLast = 0;
        Template* itemTemplate = NULL;
        QString templateName;

        ProjectTemplateItem::loading = true;
        for( unsigned i = 0; i < d->currentTemplateDict->count(); i++ ) {

            itemTemplate =  ( *( d->currentTemplateDict ) )[ i ];
            Q_ASSERT( itemTemplate );

            templateName = d->currentTemplateDict->keyAt( i );
            Q_ASSERT( !templateName.isEmpty() );
            const QKeySequence& templateKey = itemTemplate->shortcut();

            itemTemplateLast = new ProjectTemplateItem( templateList,
                        itemTemplateLast, templateName, (QString)templateKey);

            Q_CHECK_PTR( itemTemplateLast );

            itemTemplateLast->setOn(!itemTemplate->isEnabled());
        }
        ProjectTemplateItem::loading = false;

        templateList->setEnabled( true );

        EditablePanelView * pv =
            d->pw->m_viewContainer->panelView( panelName );
        if( pv ) {
            const TemplateDict & td = pv->currentSelection();
            if( td.count() > 0 ) {
                QListViewItem * i = templateList->findItem(
                        td.currentKey(), COL_NAME );
                if( i ) {
                    templateList->setSelected( i, true );
                    templateList->ensureItemVisible( i );
                }
            } else {
                templateList->clearSelection();
                templateSelectionChangedSlot( NULL );
            }
        }

    } else {
        templateList->setEnabled( false );
        moveUp->setEnabled( false );
        moveDown->setEnabled( false );
    }
}

void ProjectTemplatesDialog::moveUpSlot()
{
    QListViewItem* itemSelected = templateList->selectedItem();
    Q_ASSERT( itemSelected );

    QListViewItem* prevSibling = itemSelected->itemAbove();
    Q_ASSERT( prevSibling );

    QString selectedTemplate = itemSelected->text(COL_NAME);
    int oldIndex = 
            d->currentTemplateDict->indexOf( selectedTemplate );
    Q_ASSERT( oldIndex != -1 );

    QString prevTemplate = prevSibling->text(COL_NAME);
    int newIndex = d->currentTemplateDict->indexOf( prevTemplate );
    Q_ASSERT( newIndex != -1 );

    TemplateTabOrderChangeCommand* cmd;
    cmd = new TemplateTabOrderChangeCommand( prevTemplate,
                selectedTemplate, d->pw,
                (MainWindow*)parent() );

    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );

}

void ProjectTemplatesDialog::moveDownSlot()
{
    QListViewItem* itemSelected = templateList->selectedItem();
    Q_ASSERT( itemSelected );

    QListViewItem* nextSibling = itemSelected->itemBelow();
    Q_ASSERT( nextSibling );

    QString selectedTemplate = itemSelected->text(COL_NAME);
    int newIndex =
            d->currentTemplateDict->indexOf( selectedTemplate );
    Q_ASSERT( newIndex != -1 );

    QString nextTemplate = nextSibling->text(COL_NAME);
    int oldIndex = d->currentTemplateDict->indexOf( nextTemplate );
    Q_ASSERT( oldIndex != -1 );

    TemplateTabOrderChangeCommand* cmd;
    cmd = new TemplateTabOrderChangeCommand( nextTemplate,
                selectedTemplate, d->pw,
                (MainWindow*)parent() );

    Q_CHECK_PTR( cmd );
    d->pw->executeCommand( cmd );

}

void ProjectTemplatesDialog::populatePanelNames()
{
    Q_ASSERT( d );
    Q_ASSERT( d->pw );

    panel->clear();

    QString currentPanel, currentView ;
    currentView = d->pw->currentView();

    if( !currentView.isNull() ) {
        currentPanel = d->pw->currentPanel();

        View view =  d->pw->project()->views()[ currentView ];

        PanelMap panelMap = view.panels();

        if ( panelMap.count() <= 0 ) {
            clearAndDisableControls();
            return;
        }

        panel->setEnabled( true );

        for( unsigned i = 0; i < panelMap.count(); i++ ) {
            panel->insertItem( panelMap.keyAt( i ) );
        }

        if( !currentPanel.isNull() ) {
            Q_ASSERT( panel->listBox()->findItem(
                    currentPanel,Qt::ExactMatch ) );
            panel->setCurrentText( currentPanel );
            panelSelectedSlot( currentPanel );
        } else {
            panel->setCurrentItem( -1 );
            currentPanel =  panel->currentText();
        }

        setCaption( QString( tr("%1/%2 Templates - Lintouch Editor") )
                .arg( d->pw->projectFileInfo().baseName() )
                .arg( currentView ) );
    } else {
        setCaption( QString( tr("%1 Templates - Lintouch Editor") )
                .arg( d->pw->projectFileInfo().baseName()) );
        clearAndDisableControls();
    } 

}

void ProjectTemplatesDialog::
        templateSelectionChangedSlot( QListViewItem* lstViewItem )
{
    bool enableMoveUp = false;
    bool enableMoveDown = false;

    if( lstViewItem ) {
        QListViewItem* prevSibling = lstViewItem->itemAbove();
        QListViewItem* nextSibling = lstViewItem->itemBelow();
        enableMoveUp = ( prevSibling != 0 ) ? true : false ;
        enableMoveDown = ( nextSibling != 0 ) ? true : false ;
         d->oldItemText = lstViewItem->text( COL_NAME );
    }

    moveUp->setEnabled( enableMoveUp );
    moveDown->setEnabled( enableMoveDown );

}

void ProjectTemplatesDialog::
        itemClickedSlot( QListViewItem* item,
                const QPoint& /*p*/ ,int col )
{
    if( item == NULL ) return;
    if( col == COL_NAME ) {
        QString templateName = item->text( COL_NAME );
        d->oldItemText = templateName.stripWhiteSpace();
    }
}

void ProjectTemplatesDialog::
        returnPressedSlot( QListViewItem* item)
{
    itemDoubleClickedSlot( item, QPoint(), 0 );
}

void ProjectTemplatesDialog::
        itemDoubleClickedSlot( QListViewItem* item,
                const QPoint& /*p*/ ,int col )
{
    Q_ASSERT(d);
    Q_ASSERT(d->pw);

    if( item ) {

        if( col == COL_NAME ) {
            QString templateName = item->text( COL_NAME );

            EditablePanelView* panelView =
                    d->pw->m_viewContainer->currentPanelView();
            Q_ASSERT( panelView );

            // make the unselect&select atomic
            panelView->blockSignals(true);
            panelView->unselectAllTemplates();
            panelView->blockSignals(false);

            Q_ASSERT( d->currentTemplateDict->contains( templateName ) );
            if((*d->currentTemplateDict)[templateName]->isEnabled()) {
                panelView->selectTemplate(
                    (*d->currentTemplateDict)[templateName], true );
            }

        } else if ( col == COL_KEY ) {
            WFlags fl = Qt::WStyle_Customize | Qt::WStyle_NoBorder ;
            QString keySequenceNew, keySequencePrev;
            keySequencePrev = item->text( COL_KEY );
            keySequencePrev = keySequencePrev.stripWhiteSpace();
            TemplatesShortCutKeyCapture keyCaptureDialog ( this ,
                    &keySequenceNew ,tr("Shortcut Capture"), fl );
            keyCaptureDialog.setActiveWindow();
            keyCaptureDialog.setFocusPolicy( QWidget::WheelFocus );
            keyCaptureDialog.exec();
            if( keySequenceNew != keySequencePrev ) {            
                QString stealFromTemplate = 
                        stealShortCutFromTemplate( keySequenceNew );
                QString templateName = item->text( COL_NAME );
                Q_ASSERT( d->currentTemplateDict->contains( templateName ) );
                if( templateName != stealFromTemplate ) {
                    TemplateShortcutChangeCommand* cmd =
                            new TemplateShortcutChangeCommand(
                            templateName,keySequenceNew, stealFromTemplate,d->pw,
                            (MainWindow*)parent() );

                    Q_CHECK_PTR( cmd );
                    d->pw->executeCommand( cmd );
                }
            }
            return;
        }
    }
}

void ProjectTemplatesDialog::
        itemRenamedSlot( QListViewItem* item, int col )
{
        Q_ASSERT( item );

        if(col != COL_NAME) {
            return;
        }

        bool textInvalid = false;

        QString newName = item->text( COL_NAME );
        newName = newName.stripWhiteSpace();

        if( newName == d->oldItemText ) {
            //Set the template name in the listVew after stripping
            //white space if any 
            item->setText( COL_NAME, newName );
            return;
        }

        if( newName.isEmpty() ||
                ( d->currentTemplateDict->indexOf( newName ) != -1 ) ) {
            textInvalid = true;
        }

        if( textInvalid ) {

            if( !newName.isEmpty() ) {
                QString strMsg = QString( "%1 %2 %3.\n%4" ).
                        arg( tr("The template name") ).arg(  newName ).
                        arg( tr( "is already used at this panel." ) ).
                        arg( tr("Please choose a different name.") ) ;

                QMessageBox::warning( this, tr( "Error" ),
                        strMsg, QMessageBox::Ok | QMessageBox::Default,
                         0, 0);
            }

            item->setText( COL_NAME, d->oldItemText );
        } else {
                QString oldName = d->oldItemText;

                RenameTemplateCommand* cmd =
                        new RenameTemplateCommand(
                        oldName, newName, d->pw,
                        (MainWindow*)parent() );
                Q_CHECK_PTR( cmd );
                d->pw->executeCommandLater( cmd );
        }

       templateSelectionChangedSlot( item );

}

QString ProjectTemplatesDialog::
        stealShortCutFromTemplate( QString keySequence )
{
        QString templateName = "";

        if( keySequence.isEmpty() ) {
            return templateName;
        }

        QListViewItem* item = 
                templateList->firstChild();
        while( item ) {

            if( item->text( COL_KEY ).stripWhiteSpace() == keySequence ) {
                templateName = item->text( COL_NAME );
                return templateName;
            }

            item = item->nextSibling();
        }
        return templateName;
}

void ProjectTemplatesDialog::clearAndDisableControls()
{
    panel->clear();
    templateList->clear();
    panel->setEnabled( FALSE );
    templateList->setEnabled( FALSE );
    moveUp->setEnabled( FALSE );
    moveDown->setEnabled( FALSE );
    lblPanel->setEnabled( FALSE );

}

void ProjectTemplatesDialog::bringDialogOnTop( bool onTop )
{
    WFlags f = getWFlags();
    if( onTop ) {
        clearWFlags( f );
        f |= WStyle_StaysOnTop;
        setWFlags( f );
    } else {
        clearWFlags( f );
        f |= ~ WStyle_StaysOnTop;
        setWFlags( f );
    }
}

void ProjectTemplatesDialog::changeTemplateLock(bool b, const QString& tn)
{
    Q_ASSERT(d);
    Q_ASSERT(d->currentTemplateDict);
    Q_ASSERT(d->currentTemplateDict->contains(tn));

    Template* t = (*(d->currentTemplateDict))[tn];
    Q_ASSERT(t);

    t->setEnabled(!b);

    EditablePanelView* panelView =
        d->pw->m_viewContainer->currentPanelView();
    Q_ASSERT( panelView );
    if(panelView->currentSelection().contains(tn)) {
        panelView->unselectTemplate(t, true);
    }
}

bool ProjectTemplatesDialog::eventFilter( QObject *o, QEvent *e )
{
    Q_ASSERT(o == templateList);
    if(!templateList->isRenaming() && e->type() == QEvent::KeyPress) {
        QKeyEvent* ke = (QKeyEvent*)e;
        if(ke->key() == Qt::Key_F2 && templateList->currentItem()) {
            templateList->currentItem()->startRename(COL_NAME);
            // eat the event
            return true;
        }
    }

    // pass through
    return false;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectTemplatesDialog.cpp 1207 2006-01-05 15:08:45Z mman $
