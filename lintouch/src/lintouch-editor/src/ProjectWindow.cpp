// $Id: ProjectWindow.cpp 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: ProjectWindow.cpp --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 13 July 2004
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

#include <qaction.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <lt/ui/Undo.hh>

#include <lt/project/Panel.hh>
#include <lt/project/View.hh>

#include "AddDelPanelCommand.hh"
#include "AddDelSelectionCommand.hh"
#include "AddDelViewCommand.hh"
#include "Command.hh"
#include "CopyPasteBuffer.hh"
#include "EditablePanelViewContainer.hh"
#include "EditablePanelView.hh"
#include "EditorProjectBuilder.hh"
#include "EditorProject.hh"
#include "EditorProjectSerializer.hh"
#include "MainWindow.hh"
#include "MoveNResizeSelectionCommand.hh"
#include "PanelOrderChangeCommand.hh"
#include "PanelZoomSelector.hh"
#include "RenamePanelCommand.hh"
#include "RenameTemplateCommand.hh"
#include "RenameViewCommand.hh"
#include "ReorderViewCommand.hh"
#include "ResizeViewCommand.hh"
#include "TemplateTabOrderChangeCommand.hh"

#include "ProjectWindow.hh"

using namespace lt;


#include <ProjectWindowProtected.hh>

ProjectWindow::ProjectWindow( EditorProject* p,
        QWidget* parent, MainWindow* mw,
        double zoom /* = 1.0 */, LoggerPtr logger /* = Logger::nullPtr() */,
        const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel | Qt::WDestructiveClose*/ ) :
    ProjectWindowBase( parent, name, fl ),
    d( new ProjectWindowProtected ),
    numViews( 0 ), numPanels( 0 )
{
    Q_CHECK_PTR( d );
    Q_ASSERT(mw);

    d->project = p;
    d->mw = mw;

    // inital zoom
    d->zoom = zoom;
    zoomLevel->setZoomCoefficient( (int)(zoom * 100 + .5) );
    connect( zoomLevel, SIGNAL( activated( const QString & ) ),
            this, SLOT( zoomChangedSlot( const QString & ) ) );

    d->logger = logger;

    // QtUndoManager::manager()->associateView( this, p->undoStack() );
    d->undoStack = new UndoStack( this );
    Q_CHECK_PTR(d->undoStack);

    d->project->setUndoStack(d->undoStack);
    undoStackChangedSlot(true);

    // connect undostack content change to know when the PW is changed
    connect( d->undoStack, SIGNAL(cleanChanged(bool)),
            this, SLOT(undoStackChangedSlot(bool)) );

    // Quit dialog
    d->quitDialog = new QMessageBox( "XXX",
            tr("Project contains unsaved changes. "
                "Do You really want to close it?"),
            QMessageBox::Warning,
            QMessageBox::Cancel | QMessageBox::Escape,  // Cancel
            QMessageBox::Ignore,                        // Discard Changes
            QMessageBox::Retry | QMessageBox::Default,  // Save
            this );
    Q_CHECK_PTR( d->quitDialog );
    d->quitDialog->setButtonText( QMessageBox::Cancel, tr("Cancel") );
    d->quitDialog->setButtonText( QMessageBox::Ignore, tr("Discard Changes") );
    d->quitDialog->setButtonText( QMessageBox::Retry, tr("Save") );

    // project main popup menu
    d->projectWindowMenu = new QPopupMenu( this, "ProjectWindowMenu" );
    Q_CHECK_PTR( d->projectWindowMenu );

    // project submenu
    QPopupMenu* popupProject = new QPopupMenu( this, "popupProject" );
    Q_CHECK_PTR( popupProject );
    popupProject->insertItem( tr("&New"),
            d->mw, SLOT(createProjectSlot()), CTRL+Key_N );
    popupProject->insertItem( tr("&Open..."),
            d->mw, SLOT(openProjectSlot()), CTRL+Key_O );
    popupProject->insertItem( tr("&Save"),
            this, SLOT( saveProjectSlot()), CTRL+Key_S );
    popupProject->insertItem( tr("Save &As..."),
            this, SLOT( saveProjectAsSlot()), CTRL+SHIFT+Key_S );
    popupProject->insertSeparator();
    popupProject->insertItem( tr("&Revert to Saved"),
            this, SLOT( revertToSavedSlot()) );
    popupProject->insertSeparator();
    popupProject->insertItem( tr("&Close"),
            this, SLOT(close()), CTRL+Key_W );

    // edit submenu
    QPopupMenu* popupEdit = new QPopupMenu( this, "popupEdit" );
    Q_CHECK_PTR( popupEdit );
    QAction* a = d->undoStack->createUndoAction(this);
    Q_CHECK_PTR( a );
    a->setAccel(QKeySequence("Ctrl+Z"));
    a->addTo( popupEdit );
    // redo
    a = d->undoStack->createRedoAction(this);
    Q_CHECK_PTR( a );
    a->setAccel(QKeySequence("Shift+Ctrl+Z"));
    a->addTo( popupEdit );
    popupEdit->insertSeparator();
    popupEdit->insertItem( tr("&Copy"),
            this, SLOT( copySlot() ), CTRL+Key_C );
    popupEdit->insertItem( tr("Cu&t"),
            this, SLOT( cutSlot() ), CTRL+Key_X );
    popupEdit->insertItem( tr("&Paste"),
            this, SLOT( pasteSlot() ), CTRL+Key_V );
    popupEdit->insertItem( tr("Dup&licate"),
            this, SLOT( duplicateSlot() ), CTRL+Key_D );
    popupEdit->insertItem( tr("&Delete"),
            this, SLOT( deleteSlot() ), Key_Delete );
    popupEdit->insertSeparator();
    popupEdit->insertItem( tr("Select &All"),
            this, SLOT( selectAllSlot() ), CTRL+Key_A );
    popupEdit->insertItem( tr("U&nselect All"),
            this, SLOT( unselectAllSlot() ), SHIFT+CTRL+Key_A );
    popupEdit->insertItem( tr("Inve&rt Selection"),
            this, SLOT( invertSelectionSlot() ), CTRL+Key_I );

    // Align submenu
    QPopupMenu* popupAlign = new QPopupMenu( this, "popupAlign" );
    Q_CHECK_PTR( popupAlign );
    popupAlign->insertItem( tr("&Left"),
            this, SLOT( alignLeftSlot() ) );
    popupAlign->insertItem( tr("&Center"),
            this, SLOT( alignCenterSlot() ) );
    popupAlign->insertItem( tr("&Right"),
            this, SLOT( alignRightSlot() ) );
    popupAlign->insertItem( tr("&Bottom"),
            this, SLOT( alignBottomSlot() ) );
    popupAlign->insertItem( tr("&Middle"),
            this, SLOT( alignMiddleSlot() ) );
    popupAlign->insertItem( tr("&Top"),
            this, SLOT( alignTopSlot() ) );
    popupAlign->insertSeparator();
    popupAlign->insertItem( tr("Lay Out &Horizontally"),
            this, SLOT( alignLayOutHrizontallySlot() ) );
    popupAlign->insertItem( tr("Lay Out &Vertically"),
            this, SLOT( alignLayOutVerticallySlot() ) );
    popupAlign->insertSeparator();
    popupAlign->insertItem( tr("&Adjacent"),
            this, SLOT( alignAdjacentSlot() ) );
    popupAlign->insertItem( tr("&Stacked"),
            this, SLOT( alignStackedSlot() ) );

    // Selection submenu
    QPopupMenu* popupSelection = new QPopupMenu( this, "popupSelection" );
    Q_CHECK_PTR( popupSelection );
    popupSelection->insertItem( tr("One Layer &Up"),
            this, SLOT( layerUpSlot() ), CTRL+Key_F );
    popupSelection->insertItem( tr("One Layer &Down"),
            this, SLOT( layerDownSlot() ), CTRL+Key_B );
    popupSelection->insertItem( tr("Bring to &Front"),
            this, SLOT( bringToFrontSlot() ), SHIFT+CTRL+Key_F );
    popupSelection->insertItem( tr("Send to &Bottom"),
            this, SLOT( sendToBottomSlot() ), SHIFT+CTRL+Key_B );
    //FIXME: postponed for release 1.3
    //popupSelection->insertSeparator();
    //popupSelection->insertItem( tr("&Group") );
    //popupSelection->insertItem( tr("U&ngroup") );
    popupSelection->insertSeparator();
    popupSelection->insertItem( tr("&Align"), popupAlign );
    popupSelection->insertItem( tr("&Properties"),
            d->mw, SLOT(showTemplatePropertiesDialogSlot()) );

    // Zoom submenu
    QPopupMenu* popupZoom = new QPopupMenu( this, "popupZoom" );
    Q_CHECK_PTR( popupZoom );
    popupZoom->insertItem( tr("25%"),
            this, SLOT( zoom25Slot() ), ALT+Key_4 );
    popupZoom->insertItem( tr("50%"),
            this, SLOT( zoom50Slot() ), ALT+Key_5 );
    popupZoom->insertItem( tr("100%"),
            this, SLOT( zoom100Slot() ), ALT+Key_1 );
    popupZoom->insertItem( tr("200%"),
            this, SLOT( zoom200Slot() ), ALT+Key_2 );
    popupZoom->insertItem( tr("400%"),
            this, SLOT( zoom400Slot() ), ALT+Key_3 );

    // View submenu
    QPopupMenu* popupView = new QPopupMenu( this, "popupView" );
    Q_CHECK_PTR( popupView );
    popupView->insertItem( tr("Zoom &In"),
            this, SLOT( zoomInSlot() ), CTRL+Key_Plus );
    popupView->insertItem( tr("Zoom &Out"),
            this, SLOT( zoomOutSlot() ), CTRL+Key_Minus );
    popupView->insertItem( tr("&Zoom"), popupZoom );
    popupView->insertSeparator();
    popupView->insertItem( tr("&Fit to Window"),
            this, SLOT( zoomFitToWindowSlot() ), CTRL+Key_E );
    popupView->insertSeparator();
    popupView->insertItem( tr("&Grid Settings..."),
            d->mw, SLOT(showProjectGridSettingsDialogSlot()) );

    // Dialogs submenu
    d->popupDialogs = new QPopupMenu( this, "popupDialogs" );
    Q_CHECK_PTR( d->popupDialogs );
    d->popupDialogs->insertItem( tr("&Templates"),
            d->mw, SLOT(showProjectTemplatesDialogSlot()) );
    d->popupDialogs->insertItem( tr("&Panels"),
            d->mw, SLOT(showProjectPanelsDialogSlot()), CTRL+Key_L );
    d->popupDialogs->insertItem( tr("&Views"),
            d->mw, SLOT(showProjectViewsDialogSlot()) );
    d->popupDialogs->insertSeparator();
    d->popupDialogs->insertItem( tr("Varia&bles"),
            d->mw, SLOT(showProjectVariablesDialogSlot()), CTRL+Key_K );
    d->popupDialogs->insertItem( tr("&Connections"),
            d->mw, SLOT(showProjectConnectionsDialogSlot()) );
    d->popupDialogs->insertItem( tr("&Server Logging"),
            d->mw, SLOT(showProjectServerLoggingDialogSlot()) );
    d->popupDialogs->insertSeparator();
    d->popupDialogs->insertItem( tr("&Resources"),
            d->mw, SLOT(showProjectResourcesDialogSlot()), CTRL+Key_R );
    d->popupDialogs->insertSeparator();
    d->popupDialogs->insertItem( tr("Pr&eview"),
            d->mw, SLOT(showProjectPreviewDialogSlot()), CTRL+Key_P );
    d->popupDialogs->insertSeparator();
    d->popupDialogs->insertItem( tr("Pr&oject Properties..."),
            d->mw, SLOT(showProjectPropertiesDialogSlot()) );

    QPopupMenu* popupHelp = new QPopupMenu( this, "popupHelp" );
    Q_CHECK_PTR( popupHelp );
    popupHelp->insertItem( tr("&Manual"),
            d->mw, SLOT(showHelpWindowSlot()), Key_F1 );
    popupHelp->insertSeparator();
    popupHelp->insertItem( tr("&About..."),
            d->mw, SLOT(showAboutDialogSlot()) );

    // Tools submenu
    d->popupTools = new QPopupMenu( this, "popupTools" );
    Q_CHECK_PTR( d->popupTools );
    d->popupTools->insertItem( tr("&Modify"),
            d->mw, SLOT( switchToModifyModeSlot() ), ALT+Key_M );
    d->popupTools->insertItem( tr("Ma&gnify"),
            d->mw, SLOT( switchToMagnifyModeSlot() ), ALT+Key_G );
    d->popupTools->insertItem( tr("&Pan"),
            d->mw, SLOT( switchToPanModeSlot() ), ALT+Key_P );
    d->popupTools->insertItem( tr("Pre&view"),
            d->mw, SLOT( switchToPreviewModeSlot() ), ALT+Key_V );
    d->popupTools->insertItem( tr("&Online"),
            d->mw, SLOT( goOnlineSlot() ), ALT+Key_O );

    // fillin project menu popup
    d->projectWindowMenu->insertItem( tr("&Project"), popupProject );
    d->projectWindowMenu->insertItem( tr("&Edit"), popupEdit );
    d->projectWindowMenu->insertItem( tr("&Selection"), popupSelection );
    d->projectWindowMenu->insertItem( tr("&View"), popupView );
    d->projectWindowMenu->insertItem( tr("&Dialogs"), d->popupDialogs );
    d->projectWindowMenu->insertItem( tr("&Tools"), d->popupTools );
    d->projectWindowMenu->insertItem( tr("&Help"), popupHelp );

    // receive notifications about panel changes originating within EPVC
    connect( m_viewContainer, SIGNAL( panelDisplayed( const QString & ) ),
            this, SLOT( panelSelectedSlot( const QString & ) ) );

    // receive notifications about selection changes originating within EPVC
    connect( m_viewContainer, SIGNAL( selectionChanged() ),
            this, SLOT( selectionChangedSlot() ) );
    connect( m_viewContainer, SIGNAL( selectionDoubleClicked(QMouseEvent*) ),
            this, SLOT( selectionDoubleClickedSlot(QMouseEvent*) ) );
    connect( m_viewContainer,
            SIGNAL( selectionMoved( const LTMap <QRect> & ) ),
            this,
            SLOT( selectionMovedSlot( const LTMap <QRect> & ) ) );
    connect( m_viewContainer, SIGNAL( selectionResized(
                    TemplateDict &, const LTMap <QRect> & ) ),
            this, SLOT( selectionResizedSlot(
                    TemplateDict &, const LTMap <QRect> & ) ) );
    connect( m_viewContainer, SIGNAL( templatePlaced( const QRect & ) ),
            this, SLOT( templatePlacedSlot( const QRect & ) ) );
    connect( m_viewContainer, SIGNAL( zoomInRequested( const QPoint & ) ),
            this, SLOT( zoomInSlotP( const QPoint & ) ) );
    connect( m_viewContainer, SIGNAL( zoomOutRequested( const QPoint & ) ),
            this, SLOT( zoomOutSlotP( const QPoint & ) ) );

    // give the menu to the container
    connect( m_viewContainer, SIGNAL(contentsMouseReleaseSignal(QMouseEvent*)),
            this, SLOT(showGlobalMenuSlot(QMouseEvent*)));

    // filter events for preview + magnify mode
    installEventFilter( this );

    // populate combo + view container
    setup();
}

void ProjectWindow::setup()
{
    // initialize view selection combo
    m_viewSelector->clear();
    const ViewMap & views = d->project->views();
    for( unsigned i = 0; i < views.count(); i++ ) {
        m_viewSelector->insertItem( views.keyAt( i ) );
    }

    // check for project without any views
    if( d->project->views().count() == 0 ) {
        // set up no view
        m_viewSelector->setEnabled( false );
    } else {
        // set up initial view properly
        m_viewSelector->setEnabled( true );
        viewSelectedSlot( m_viewSelector->currentText() );
    }
}

ProjectWindow::~ProjectWindow()
{
//    QtUndoManager::manager()->disassociateView( this );
    delete d->project; // we own the EditorProject instance!
    delete d;
}

bool ProjectWindow::close( bool /*alsoDelete*/ )
{
    Q_ASSERT(d);
    Q_ASSERT(d->undoStack);

    if( !d->undoStack->isClean() ) {
        Q_ASSERT( d->quitDialog );
        d->quitDialog->setCaption(
            tr("Close %1 - Lintouch Editor")
            .arg( d->projectFileInfo.baseName() ) );
        int result = d->quitDialog->exec();
        if( result == QMessageBox::Cancel ) {
            // Not quitting
            return false;
        } else if( result == QMessageBox::Retry ) {
            if( saveProjectSlot() == false ) {
                // Not quitting
                return false;
            }
        }
    }

    // pass true because we want the widget deleted.
    return ProjectWindowBase::close( true );
}

bool ProjectWindow::event( QEvent* e )
{
    if( e && e->type() == QEvent::WindowActivate ) {
        //TODO: check if the emit can be before the event is passed to the
        //base class
        emit( projectWindowActivated( this ) );
    }
    return ProjectWindowBase::event(e);
}

void ProjectWindow::mouseReleaseEvent( QMouseEvent * e )
{
    showGlobalMenuSlot(e);
    ProjectWindowBase::mouseReleaseEvent( e );
}

bool ProjectWindow::saveProjectSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->project );

    if( d->project->isNew() ) {
        // ask for name
        return saveProjectAsSlot();
    }

    d->projectFileInfo.refresh();
    if (d->projectLastModified < d->projectFileInfo.lastModified())
    {
        int res = QMessageBox::question(this, tr("Save - Lintouch Editor"),
                tr( "The file %1 has changed since editing started.\n"
                    "What do you want to do?")
                .arg(d->projectFileInfo.filePath()),
                tr("Force Overwrite"),tr("Reload Project"),tr("Cancel"),
                0, 0 );

        if( res == 2) {
            // Cancel
            return false;
        } else if( res == 1 ) {
            // revert without question
            reload();
            return true;
        }
        Q_ASSERT(res==0);
        // Yes, Overwrite
    }

    if (!EditorProjectSerializer(d->logger->getChildLogger(
        QString("Serializer(%1)").arg(d->projectFileInfo.filePath())))
        .saveEditorProject(*d->project, d->projectFileInfo.filePath(),
            saveEditorSettings()))
    {
        QMessageBox::critical(this,
                tr("Save - Lintouch Editor"),
                tr("Unable to save the project %1.")
                .arg(d->projectFileInfo.filePath()) );
        return false;
    }

    setProjectFileInfo(d->projectFileInfo);

    // the undostack is empty, there is no lastCommand now
    setLastCommand(NULL);

    return true;
}

bool ProjectWindow::saveProjectAsSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->project );

    QString fileExt = ".ltp";

    QString file = QFileDialog::getSaveFileName(
            d->projectFileInfo.filePath(),
            tr("Lintouch Project Files (*.ltp);;All files (*.*)"),
            this, "saveProjectAsDialog",
            tr("Save Project - Lintouch Editor") );

    if( file.isNull() ) {
        return false;
    }

    if( !file.contains( '.'  ) ) {
        file.append( fileExt );
    }

    if( QFile::exists(file) ) {
        if( QMessageBox::warning( this, tr("Save As - Lintouch Editor"),
                    tr("The file %1 already exists.\n"
                        "Do you want to overwrite its contents?")
                    .arg(file),
                    tr("No"), tr("Yes, Overwrite"), QString::null,
                    0, 0 ) == 0 )
        {
            // no overwrite
            return false;
        }
    }

    if (!EditorProjectSerializer(
                d->logger->getChildLogger(QString("Serializer(%1)").arg(file)))
        .saveEditorProject(*d->project, file, saveEditorSettings()))
    {
        QMessageBox::critical(this,
                tr("Save As - Lintouch Editor"),
                tr("Unable to save the project %1.")
                .arg(file) );
        return false;
    }

    setProjectFileInfo(QFileInfo(file));

    d->mw->addRecentProject( file );

    // the undostack is empty, there is no lastCommand now
    setLastCommand(NULL);

    return true;
}

bool ProjectWindow::revertToSavedSlot()
{
    if( d->project->isNew() ) {
        // do nothing if the project was not save yet
        return false;
    }

    if (QMessageBox::question(this,
        tr("Question - Lintouch Editor"),
        tr("Do you want to revert %1 to %2? "
            "You will loose all your changes.")
            .arg(d->projectFileInfo.fileName())
            .arg(d->projectFileInfo.filePath()),
        tr("No"), tr("Yes, Revert"), NULL, 0, 0)
        == 0)
    {
        return false;
    }

    return reload();
}

bool ProjectWindow::reload()
{
    ConfigurationPtr es;

    EditorProject* prj = new EditorProject(
            EditorProjectBuilder(d->logger->getChildLogger(
                    QString("Builder(%1)").arg(d->projectFileInfo.filePath())))
            .buildEditorProject(d->projectFileInfo.filePath(),
                d->mw->templateManager(), d->mw->pluginManager(),
                d->mw->serverLoggingManager(), es));
    if (!prj->isValid())
    {
        QMessageBox::critical(this,
                tr("Open - Lintouch Editor"),
                tr("Unable to revert the project %1.\n"
                    "File read error or file format error.")
                .arg(d->projectFileInfo.filePath())
                );
        return false;
    }

    // actualize last modified timestamp
    setProjectFileInfo( d->projectFileInfo );

    // apply saved settings
    applyEditorSettings(es);

    // remember new project
    d->project = prj;

    // pass him undo stack so that we he knows about dirty states
    d->project->setUndoStack( d->undoStack );

    // simulate there is no PW, all dialogs will clear their contents
    d->mw->projectChangedSlot(NULL, LTE_CMD_NOOP);

    // setup us correctly
    setLastCommand(NULL);
    d->undoStack->clear();
    setup();

    // and now simulate we are focused, all dialogs will repopulate
    d->mw->projectChangedSlot(this, LTE_CMD_NOOP);

    return true;
}

void ProjectWindow::cutSlot()
{
    if( m_viewContainer->operationMode() == EditablePanelView::Modify ) {
        copySlot();
        deleteSlot();
    }
}

void ProjectWindow::copySlot()
{
    if( m_viewContainer->operationMode() == EditablePanelView::Modify ) {
        const TemplateDict & l =
            m_viewContainer->currentPanelView()->currentSelection();
        if( l.count() > 0 ) {
            d->mw->copyPasteBuffer()->copy( l, currentView(), currentPanel(),
                    d->project);
        }
    }
}

void ProjectWindow::pasteSlot()
{
    if( d->mw->copyPasteBuffer()->isEmpty() == false &&
            m_viewContainer->operationMode() == EditablePanelView::Modify ) {

        BindingsList bl;

        //FIXME: use proper logger category
        TemplateDict newTemplates = d->mw->copyPasteBuffer()->paste(
             d->project, bl, d->logger );

        // now newly placed templates has the same z-coord as their origin
        // we try first to find the lowest in the new set
        double lowest = 0;
        bool lowest_set = false;
        for( unsigned kk = 0; kk < newTemplates.count(); kk++ ) {
            if( ! lowest_set || lowest > newTemplates[ kk ]->z() ) {
                lowest_set = true;
                lowest = newTemplates[ kk ]->z();
            }
        }

        // if we found anything at the panel, we will try to move the newly
        // placed templates on top while keeping their inter-z-relations
        if( lowest_set ) {
            //compute highest z-coordinate on the panel and use
            //one-level-more for the newly placed template
            double z = 0;
            if( m_viewContainer->currentPanelView() ) {
                const TemplateDict & tdd =
                    m_viewContainer->currentPanelView()->templates();
                for( unsigned k = 0; k < tdd.count(); k++ ) {
                    if( tdd[ k ]->z() >= z ) z = tdd[ k ]->z() + 1;
                }
            }

            // now shift all templates in the new set
            double shift = z - lowest;
            for( unsigned kkk = 0; kkk < newTemplates.count(); kkk++) {
                newTemplates[ kkk ]->setZ(
                        newTemplates[ kkk ]->z() + shift );
            }
        }

        // prepare undo/redo command and execute it
        AddDelSelectionCommand * cmd =
            new AddDelSelectionCommand( newTemplates, bl,
                    this, d->mw );

        Q_CHECK_PTR( cmd );
        executeCommand( cmd );
    }
}

void ProjectWindow::duplicateSlot()
{
    if( m_viewContainer->operationMode() == EditablePanelView::Modify ) {
        copySlot();
        pasteSlot();
    }
}

void ProjectWindow::deleteSlot()
{
    if( m_viewContainer->operationMode() == EditablePanelView::Modify ) {

        // prepare undo/redo command and execute it
        AddDelSelectionCommand * cmd =
            new AddDelSelectionCommand(
                    this, d->mw );

        Q_CHECK_PTR( cmd );
        executeCommand( cmd );
    }
}

void ProjectWindow::selectAllSlot()
{
    if( m_viewContainer->operationMode() == EditablePanelView::Modify ) {
        m_viewContainer->currentPanelView()->selectAllTemplates();
    }
}

void ProjectWindow::unselectAllSlot()
{
    if( m_viewContainer->operationMode() == EditablePanelView::Modify ) {
        m_viewContainer->currentPanelView()->unselectAllTemplates();
    }
}

void ProjectWindow::invertSelectionSlot()
{
    if( m_viewContainer->operationMode() == EditablePanelView::Modify ) {
        m_viewContainer->currentPanelView()->invertSelection();
    }
}

void ProjectWindow::zoomInSlot()
{
    d->zoom *= 2;
    if( d->zoom > 10.0 ) d->zoom = 10.0;
    zoomLevel->setZoomCoefficientList( QStringList() );
    zoomLevel->setZoomCoefficient( (int)(d->zoom * 100 + .5) );
    m_viewContainer->resizePanelViews( d->zoom );
}

void ProjectWindow::zoomOutSlot()
{
    d->zoom /= 2;
    if( d->zoom < 0.1 ) d->zoom = 0.1;
    zoomLevel->setZoomCoefficientList( QStringList() );
    zoomLevel->setZoomCoefficient( (int)(d->zoom * 100 + .5) );
    m_viewContainer->resizePanelViews( d->zoom );
}

void ProjectWindow::zoomInSlotP( const QPoint & p )
{
    //qWarning( "zoom in requested at panel %d, %d",
    //        p.x(), p.y() );
    zoomInSlot();
    EditablePanelView * epv = NULL;
    if( ( epv = m_viewContainer->currentPanelView() ) != NULL ) {
        QPoint mapped = epv->matrix().map( p );
        //qWarning( "mapped to %d, %d",
        //        mapped.x(), mapped.y() );
        epv->center( mapped.x(), mapped.y() );
    }
}

void ProjectWindow::zoomOutSlotP( const QPoint & p )
{
    //qWarning( "zoom out requested at panel %d, %d",
    //        p.x(), p.y() );
    zoomOutSlot();
    EditablePanelView * epv = NULL;
    if( ( epv = m_viewContainer->currentPanelView() ) != NULL ) {
        QPoint mapped = epv->matrix().map( p );
        //qWarning( "mapped to %d, %d",
        //        mapped.x(), mapped.y() );
        epv->center( mapped.x(), mapped.y() );
    }
}

void ProjectWindow::zoomFitToWindowSlot()
{
    m_viewContainer->resizePanelViews( -1.0 );

    zoomLevel->setZoomCoefficientList( QStringList() );

    if( m_viewContainer->currentPanelView() ) {
        zoomLevel->setZoomCoefficientList( QStringList() );
        QSize real = m_viewContainer->currentPanelView()->size();
        QSize virt = m_viewContainer->currentPanelView()->virtualSize();
        d->zoom = (double)real.width() / (double)virt.width();
        zoomLevel->setZoomCoefficient( (int)(d->zoom * 100 + .5) );
    }
}

void ProjectWindow::zoom25Slot()
{
    d->zoom = 0.25;
    zoomLevel->setZoomCoefficient( (int)(d->zoom * 100 + .5) );
    m_viewContainer->resizePanelViews( d->zoom );
}

void ProjectWindow::zoom50Slot()
{
    d->zoom = 0.50;
    zoomLevel->setZoomCoefficient( (int)(d->zoom * 100 + .5) );
    m_viewContainer->resizePanelViews( d->zoom );
}

void ProjectWindow::zoom100Slot()
{
    d->zoom = 1.00;
    zoomLevel->setZoomCoefficient( (int)(d->zoom * 100 + .5) );
    m_viewContainer->resizePanelViews( d->zoom );
}

void ProjectWindow::zoom200Slot()
{
    d->zoom = 2.00;
    zoomLevel->setZoomCoefficient( (int)(d->zoom * 100 + .5) );
    m_viewContainer->resizePanelViews( d->zoom );
}

void ProjectWindow::zoom400Slot()
{
    d->zoom = 4.00;
    zoomLevel->setZoomCoefficient( (int)(d->zoom * 100 + .5) );
    m_viewContainer->resizePanelViews( d->zoom );
}

void ProjectWindow::layerUpSlot()
{
    // obtain the selection
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selection is nonempty
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );

    // now prepare new locations for all templates
    for( unsigned j = 0; j < newLayers.count(); j ++ ) {
        newLayers[ j ] = newLayers[ j ] + 1;
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::layerDownSlot()
{
    // obtain the selection
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selection is nonempty
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );

    // now prepare new locations for all templates
    for( unsigned j = 0; j < newLayers.count(); j ++ ) {
        newLayers[ j ] = newLayers[ j ] - 1;
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::bringToFrontSlot()
{
    // obtain the selection
    TemplateDict selected(
            m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selection is nonempty
    if( selected.count() <= 0 ) return;

    // obtain all templates
    const TemplateDict & all =
        m_viewContainer->currentPanelView()->templates();

    unsigned j;

    // obtain unselected once
    TemplateDict unselected;
    for( j = 0; j < all.count(); j++ ) {

        if ( !selected.contains( all.keyAt( j ) ) ) {
            unselected.insert( all.keyAt( j ), all[ j ] );
        }
    }

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( selected );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( selected );

    double layerTopMostUnselected = 0, layerBottomMostSelected = 0;

    //obtain the topmost unselected layer
    LTMap <double> usLayers =
            MoveNResizeSelectionCommand::layers2Map( unselected );
    for( j = 0; j < usLayers.count(); j ++ ) {
        if ( ( layerTopMostUnselected == 0 )
                || usLayers[ j ] > layerTopMostUnselected ){
            layerTopMostUnselected = usLayers[ j ];
        }
    }

    //now obtain the bottom-most selected
    for( j = 0; j < newLayers.count(); j ++ ) {
        if ( ( layerBottomMostSelected == 0 )
                || ( newLayers[ j ] < layerBottomMostSelected ) ){
            layerBottomMostSelected = newLayers[ j ];
        }
    }

    // now prepare new locations for all templates
    for( j = 0; j < newLayers.count(); j ++ ) {
        double shift = layerTopMostUnselected - layerBottomMostSelected + 1;
        newLayers[ j ] = newLayers[ j ] + shift;
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( selected, newRects,
                newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::sendToBottomSlot()
{
    // obtain the selection
    TemplateDict selected(
            m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selection is nonempty
    if( selected.count() <= 0 ) return;

    // obtain all templates
    const TemplateDict & all =
        m_viewContainer->currentPanelView()->templates();

    unsigned j;

    // obtain unselected once
    TemplateDict unselected;
    for( j = 0; j < all.count(); j++ ) {

        if ( !selected.contains( all.keyAt( j ) ) ) {
            unselected.insert( all.keyAt( j ), all[ j ] );
        }
    }

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( selected );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( selected );

    double layerTopMostSelected = 0, layerBottomMostUnselected = 0;

    //obtain the bottom most unselected layer
    LTMap <double> usLayers =
            MoveNResizeSelectionCommand::layers2Map( unselected );
    for( j = 0; j < usLayers.count(); j ++ ) {
        if ( ( layerBottomMostUnselected == 0 )
                || usLayers[ j ] < layerBottomMostUnselected ){
            layerBottomMostUnselected = usLayers[ j ];
        }
    }

    //now obtain the top-most selected
    for( j = 0; j < newLayers.count(); j ++ ) {
        if ( ( layerTopMostSelected == 0 )
                || ( newLayers[ j ] > layerTopMostSelected ) ){
            layerTopMostSelected = newLayers[ j ];
        }
    }

    // now prepare new locations for all templates
    for( j = 0; j < newLayers.count(); j ++ ) {
        double shift = layerBottomMostUnselected + 1 + layerTopMostSelected;
        newLayers[ j ] = newLayers[ j ] - shift;
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( selected, newRects,
                newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignLeftSlot()
{
    // obtain the selection
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selection is nonempty
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );

    // find the left most template
    int edge = -1;
    for( unsigned i = 0; i < newRects.count(); i ++ ) {
        if( edge == -1 || newRects[ i ].left() < edge ) {
            edge = newRects[ i ].left();
        }
    }

    // now prepare new locations for all templates
    for( unsigned j = 0; j < newRects.count(); j ++ ) {
        QRect r = newRects[ j ];
        newRects[ j ] = QRect( edge, r.top(), r.width(), r.height() );
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignCenterSlot()
{
    // obtain all selected templates
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selected templates are present
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );
    int leftEdge = -1, rightEdge = -1;

    // find the left edge of the left most template
    unsigned i;
    for( i = 0; i < newRects.count(); i ++ ) {
        if( leftEdge == -1 || newRects[ i ].left() < leftEdge ) {
            leftEdge = newRects[ i ].left();
        }
    }
    //find the right edge of the bottom most template
    for( i = 0; i < newRects.count(); i ++ ) {
        if( rightEdge == -1 || newRects[ i ].right() > rightEdge ) {
            rightEdge = newRects[ i ].right();
        }
    }

    //obtain the middle position of the two edges
    int middle = ( leftEdge + rightEdge ) / 2;

    // now prepare new locations for all templates
    for( unsigned j = 0; j < newRects.count(); j ++ ) {
        QRect r = newRects[ j ];
        r.moveCenter( QPoint( middle, r.center().y() ) );
        newRects[ j ] = r;
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignRightSlot()
{
    // obtain the selection
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selection is nonempty
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );

    // find the right most template
    int edge = -1;
    for( unsigned i = 0; i < newRects.count(); i ++ ) {
        if( edge == -1 || newRects[ i ].right() > edge ) {
            edge = newRects[ i ].right();
        }
    }

    // now prepare new locations for all templates
    for( unsigned j = 0; j < newRects.count(); j ++ ) {
        QRect r = newRects[ j ];
        newRects[ j ] = QRect( ( edge - r.width() ), r.top(), r.width(),
                r.height() );
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignBottomSlot()
{
    // obtain the selection
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selection is nonempty
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );

    // find the bottom most template
    int edge = -1;
    for( unsigned i = 0; i < newRects.count(); i ++ ) {
        if( edge == -1 || newRects[ i ].bottom() > edge ) {
            edge = newRects[ i ].bottom();
        }
    }

    // now prepare new locations for all templates
    for( unsigned j = 0; j < newRects.count(); j ++ ) {
        QRect r = newRects[ j ];
        newRects[ j ] = QRect( r.left(), ( edge - r.height() ), r.width(),
                r.height() );
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignMiddleSlot()
{
    // obtain all selected templates
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selected templates are present
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );
    int topEdge = -1, bottomEdge = -1;

    // find the top edge of the top most template
    unsigned i;
    for( i = 0; i < newRects.count(); i ++ ) {
        if( topEdge == -1 || newRects[ i ].top() < topEdge ) {
            topEdge = newRects[ i ].top();
        }
    }
    //find the bottom edge of the bottom most template
    for( i = 0; i < newRects.count(); i ++ ) {
        if( bottomEdge == -1 || newRects[ i ].bottom() > bottomEdge ) {
            bottomEdge = newRects[ i ].bottom();
        }
    }

    //obtain the middle position of the two edges
    int middle = ( topEdge + bottomEdge ) / 2;

    // now prepare new locations for selected templates
    for( unsigned j = 0; j < newRects.count(); j ++ ) {
        QRect r = newRects[ j ];
        r.moveCenter( QPoint( r.center().x(), middle ) );
        newRects[ j ] = r;
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignTopSlot()
{
    // obtain the selection
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selection is nonempty
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );
    // obtain actual layers of selected templates

    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );
    // find the top most template

    int edge = -1;
    for( unsigned i = 0; i < newRects.count(); i ++ ) {
        if( edge == -1 || newRects[ i ].top() < edge ) {
            edge = newRects[ i ].top();
        }
    }

    // now prepare new locations for all templates
    for( unsigned j = 0; j < newRects.count(); j ++ ) {
        QRect r = newRects[ j ];
        newRects[ j ] = QRect( r.left(), edge, r.width(), r.height() );
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignLayOutHrizontallySlot()
{
    // obtain all selected templates
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selected templates are present
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates for sorting
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );
    unsigned j,k;

    // obtain actual layers of sorted, selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );

    // Sort the list on the left edge, such that the leftmost is first and
    // the right most is last
    for( j = 0; j < (t.count()-1); j++){

        for( k = j+1; k<t.count(); k++ ){

            if ( newRects[ j ].left() > newRects[ k ].left() ){
                //swap position
                QRect rcTmp = newRects[ j ];
                newRects[ j ] = newRects[ k ];
                newRects[ k ] = rcTmp;
                //swap layer
                double lTmp = newLayers[ j ];
                newLayers[ j ] = newLayers[ k ];
                newLayers[ k ] = lTmp;
                t.move( k, j );
            }
        }
    }

    // Obtain the x-axis shift
    // shift = ( center pt x of rightmost - center pt x of left most )
    // / ( no of templates - 1 )
    int shift = ( newRects[ t.count() - 1 ].center().x() -
            newRects[0].center().x() ) / (t.count()-1);

    // now prepare new locations for selected templates excluding the
    // left most and the right most one
    int xaxisCenterOfTemplateOnLeft = newRects[ 0 ].center().x();

    for( j = 1; j < (newRects.count() - 1); j ++ ) {
        QRect r = newRects[ j ];
        r.moveCenter( QPoint( ( xaxisCenterOfTemplateOnLeft + shift ),
                r.center().y() ) );
        newRects[ j ] = r;
        xaxisCenterOfTemplateOnLeft = newRects[ j ].center().x();
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );
    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignLayOutVerticallySlot()
{
    // obtain all selected templates
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selected templates are present
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates for sorting
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of sorted, selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );

    unsigned j,k;
    // Sort the list on the top edge, such that the bottommost is first and
    // the top most is last
    for( j = 0; j < (t.count()-1); j++){

        for( k = j+1; k<t.count(); k++ ){

            if ( newRects[ j ].top() > newRects[ k ].top() ){
                //swap position
                QRect rcTmp = newRects[ j ];
                newRects[ j ] = newRects[ k ];
                newRects[ k ] = rcTmp;
                //swap layer
                double lTmp = newLayers[ j ];
                newLayers[ j ] = newLayers[ k ];
                newLayers[ k ] = lTmp;
                t.move( k, j );
            }
        }
    }

    // obtain the y-axis shift
    // shift = ( center pt y of topmost - center pt y of bottom most )
    // / ( no of templates - 1 )
    int shift = ( newRects[ t.count() - 1 ].center().y() -
            newRects[0].center().y() ) / (t.count()-1);

    // now prepare new locations for selected templates excluding the
    // top most and the bottom most one
    int yaxisCenterOfTemplateOnTop = newRects[ 0 ].center().y();

    for( j = 1; j < (newRects.count() - 1); j ++ ) {
        QRect r = newRects[ j ];
        r.moveCenter( QPoint( r.center().x(),
                ( yaxisCenterOfTemplateOnTop + shift ) ) );
        newRects[ j ] = r;
        yaxisCenterOfTemplateOnTop = newRects[ j ].center().y();
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );
    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignAdjacentSlot()
{
    // obtain all selected templates
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selected templates are present
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates for sorting
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );

    unsigned j,k;
    // Sort the list on the left edge, such that the rightmost is first and
    // the left most is last
    for( j = 0; j < (t.count()-1); j++){

        for( k = j+1; k<t.count(); k++ ){

            if ( newRects[ j ].left() < newRects[ k ].left() ){
                //swap position
                QRect rcTmp = newRects[ j ];
                newRects[ j ] = newRects[ k ];
                newRects[ k ] = rcTmp;
                //swap layer
                double lTmp = newLayers[ j ];
                newLayers[ j ] = newLayers[ k ];
                newLayers[ k ] = lTmp;
                t.move( j, k );
            }
        }
    }

    // now prepare new locations for selected templates excluding the
    // right most, ie, right most stays
    int leftEdgeOfRightHandTemplate = newRects[ 0 ].left();

    for( j = 1; j < newRects.count() ; j ++ ) {
        QRect r = newRects[ j ];
        newRects[ j ] = QRect( ( leftEdgeOfRightHandTemplate - r.width() ),
                r.top(), r.width(), r.height() );

        QRect rcView( QPoint( 0, 0 ),
                m_viewContainer->currentPanelView()->virtualSize() );
        // on overflow reset and quit

        if ( ! rcView.contains( newRects[ j ] ) ){
            newRects[ j ] = r;
            break;
        }

        leftEdgeOfRightHandTemplate = newRects[ j ].left();
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );
    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::alignStackedSlot()
{
    // obtain all selected templates
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selected templates are present
    if( t.count() <= 0 ) return;

    // obtain actual rects of selected templates for sorting
    LTMap <QRect> newRects =
        MoveNResizeSelectionCommand::rects2Map( t );

    // obtain actual layers of sorted, selected templates
    LTMap <double> newLayers =
        MoveNResizeSelectionCommand::layers2Map( t );

    unsigned j,k;

    // Sort the list on the top edge, such that the topmost is first and
    // the bottom most is last
    for( j = 0; j < (t.count()-1); j++){
        for( k = j+1; k<t.count(); k++ ){
            if ( newRects[ j ].top() > newRects[ k ].top() ){
                //swap position
                QRect rcTmp = newRects[ j ];
                newRects[ j ] = newRects[ k ];
                newRects[ k ] = rcTmp;
                //swap layer
                double lTmp = newLayers[ j ];
                newLayers[ j ] = newLayers[ k ];
                newLayers[ k ] = lTmp;
                t.move( k, j );
            }
        }
    }

    // now prepare new locations for selected templates excluding the
    // right most, ie, right most stays
    int bottomEdgeOfTopHandTemplate = newRects[ 0 ].bottom();

    for( j = 1; j < newRects.count() ; j ++ ) {
        QRect r = newRects[ j ];
        newRects[ j ] = QRect( r.left(),
                bottomEdgeOfTopHandTemplate,
                r.width(), r.height() );

        QRect rcView( QPoint( 0, 0 ),
                m_viewContainer->currentPanelView()->virtualSize() );
        // on overflow reset and quit

        if ( ! rcView.contains( newRects[ j ] ) ){
            newRects[ j ] = r;
            break;
        }
        bottomEdgeOfTopHandTemplate = newRects[ j ].bottom();
    }

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( t, newRects, newLayers,
                this, d->mw );
    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::viewSelectedSlot( const QString & viewName )
{
    // remember the currently selected view
    d->project->views().setCurrentKey( viewName );
    // and let the other dialogs (even us) know about the change
    d->mw->projectChangedSlot( this, LTE_CMD_VIEW_SELECT );
}

void ProjectWindow::panelSelectedSlot( const QString & panelName )
{
    // remember the currently selected view
    View & v = d->project->views().current();
    v.panels().setCurrentKey( panelName );
    // and let the other dialogs know about the change
    d->mw->projectChangedSlot( this, LTE_CMD_PANEL_SELECT );
}

void ProjectWindow::zoomChangedSlot( const QString & factor )
{
    //qWarning( "ProjectWindow::zoomChangedSlot( " + factor + " )" );

    // need to extract number from the string with % at the end
    int idx = factor.find( '%' );
    int num = 100;
    if( idx != -1 ) {
        num = factor.left( idx ).toInt();
    } else {
        num = factor.toInt();
    }

    //qWarning( "num is %d", num );

    d->zoom = num / 100.0;
    if( d->zoom > 10.0 ) d->zoom = 10.0;

    m_viewContainer->resizePanelViews( d->zoom );

    //FIXME: quick hack, only allow one custom value to be present in the
    //list of possible zoom ratios
    if( num != 25 && num != 50 && num != 100 && num != 200 && num != 400 ) {
        zoomLevel->setZoomCoefficientList( QStringList() );
        zoomLevel->setZoomCoefficient( (int)( d->zoom * 100 + .5) );
    }
}

void ProjectWindow::snapToGridToggledSlot( bool on )
{
    setGridSnap( on );
    // and let the other dialogs know about the change
    d->mw->projectChangedSlot( this, LTE_CMD_GRID_PROPS_CHANGE );
}

void ProjectWindow::selectionChangedSlot()
{
    // let the other dialogs know about the change
    d->mw->projectChangedSlot( this, LTE_CMD_SELECTION_CHANGE );
}

void ProjectWindow::selectionDoubleClickedSlot(QMouseEvent* e)
{
    if(!e) {
        return;
    }

    // display the template properties dialog
    d->mw->showTemplatePropertiesDialogSlot();
}

void ProjectWindow::selectionMovedSlot(
        const LTMap <QRect> & origRects )
{
    // obtain the selection
    TemplateDict t(m_viewContainer->currentPanelView()->currentSelection());

    // only continue when selection is nonempty
    if( t.count() <= 0 ) return;

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( origRects, t,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::selectionResizedSlot(
        TemplateDict & templates,
        const LTMap <QRect> & origRects )
{
    // obtain the templates to move
    TemplateDict & t = templates;

    // only continue when selection is nonempty
    if( t.count() <= 0 ) return;

    // prepare undo/redo command and execute it
    MoveNResizeSelectionCommand * cmd =
        new MoveNResizeSelectionCommand( origRects, t,
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );
}

void ProjectWindow::templatePlacedSlot(
        const QRect & rect )
{
    //qWarning( "ProjectWindow::templatePlacedSlot" );
    //FIXME: use proper logger category
    Template * t = d->mw->instantiateTemplate(
            d->project->localizator(), d->project->templateManager(),
            d->logger );
    t->setVirtualRect( rect );
    t->propertiesChanged();

    //compute highest z-coordinate on the panel and use one-level-more for
    //the newly placed template
    double z = 0;
    if( m_viewContainer->currentPanelView() ) {
        const TemplateDict & tdd =
            m_viewContainer->currentPanelView()->templates();
        for( unsigned k = 0; k < tdd.count(); k++ ) {
            if( tdd[ k ]->z() >= z ) z = tdd[ k ]->z() + 1;
        }
    }

    t->setZ( z );

    TemplateDict newTemplates;
    // we start with the name "Type %1", where %1 will be replaced by
    // undo/redo command to form safe name
    newTemplates.insert( t->type() + " %1", t );

    // prepare undo/redo command and execute it
    AddDelSelectionCommand * cmd =
        new AddDelSelectionCommand( newTemplates, BindingsList(),
                this, d->mw );

    Q_CHECK_PTR( cmd );
    executeCommand( cmd );

    d->mw->switchOutOfPlaceModeSlot();
}

EditorProject* ProjectWindow::project() const
{
    Q_ASSERT( d );
    return d->project;
}

QString ProjectWindow::currentView() const
{
    Q_ASSERT( d );
    return d->project->views().currentKey();
}

QString ProjectWindow::currentPanel() const
{
    Q_ASSERT( d );
    if( d->project->views().count() > 0 ) {
        return d->project->views().current().panels().currentKey();
    }
    return QString::null;
}

void ProjectWindow::setProjectFileInfo( QFileInfo fi )
{
    Q_ASSERT(d);
    d->projectFileInfo = fi;
    d->projectFileInfo.refresh();
    d->projectLastModified = d->projectFileInfo.lastModified();

    setCaption( QString(tr("%1 - Lintouch Editor" ).arg(fi.baseName()) ) );
}

QFileInfo ProjectWindow::projectFileInfo() const
{
    Q_ASSERT(d);
    return d->projectFileInfo;
}

void ProjectWindow::setGridColor( const QColor & color )
{
    // when invalid color is passed in we use Qt::cyan instead
    if( ! color.isValid() ) {
        d->gridColor = Qt::cyan;
    }

    // for good colors, save and notify project window
    d->gridColor = color;

    // let the EPVC know about changed grid settings
    m_viewContainer->setGridColor( color );
}

QColor ProjectWindow::gridColor() const
{
    return d->gridColor;
}

void ProjectWindow::setGridXSpacing( unsigned spacing )
{
    // we do nothing when the same value has already been specified
    if( d->gridXSpacing == spacing ) {
        return;
    }

    // save and notify project window
    d->gridXSpacing = spacing;

    // we can accept spacing of 1 pixel and more
    if( d->gridXSpacing == 0 ) {
        d->gridXSpacing = DEFAULT_GRID_XSPACING;
    }

    // let the EPVC know about changed grid settings
    m_viewContainer->setGridXSpacing( spacing );
}

unsigned ProjectWindow::gridXSpacing() const
{
    return d->gridXSpacing;
}

void ProjectWindow::setGridYSpacing( unsigned spacing )
{
    // we do nothing when the same value has already been specified
    if( d->gridYSpacing == spacing ) {
        return;
    }

    // save and notify project window
    d->gridYSpacing = spacing;

    // we can accept spacing of 1 pixel and more
    if( d->gridYSpacing == 0 ) {
        d->gridYSpacing = DEFAULT_GRID_YSPACING;
    }

    // let the EPVC know about changed grid settings
    m_viewContainer->setGridYSpacing( spacing );
}

unsigned ProjectWindow::gridYSpacing() const
{
    return d->gridYSpacing;
}

void ProjectWindow::setGridHighlight( unsigned n )
{
    // we do nothing when the same value has already been specified
    if( d->gridHighlight == n ) {
        return;
    }

    // save and notify project window
    d->gridHighlight = n;

    // we can higlight every line or more, use 5 when invalid was specified
    if( d->gridHighlight == 0 ) {
        d->gridHighlight = DEFAULT_GRID_HIGHLIGHT;
    }

    // let the EPVC know about changed grid settings
    m_viewContainer->setGridHighlight( n );
}

unsigned ProjectWindow::gridHighlight() const
{
    return d->gridHighlight;
}

void ProjectWindow::setGridVisible( bool on )
{
    if( d->gridVisible != on ) {
        // save and notify project window
        d->gridVisible = on;

        // let the EPVC know about changed grid settings
        m_viewContainer->setGridVisible( on );
    }
}

bool ProjectWindow::gridVisible() const
{
    return d->gridVisible;
}

void ProjectWindow::setGridSnap( bool on )
{
    // we update the check box state according to what we received. this is
    // because this method is called by MW when new PW is created. the
    // default value for on is taken from EditorPreferences.
    // When we are invoked as a result of QCheckBox::toggled, we cause no
    // harm since setChecked emits no signals.
    m_snapToGrid->setChecked( on );
    if( d->gridSnap != on ) {
        // save and notify project window
        d->gridSnap = on;

        // let the EPVC know about changed grid settings
        m_viewContainer->setGridSnap( on );
    }
}

bool ProjectWindow::gridSnap() const
{
    return d->gridSnap;
}

void ProjectWindow::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT( d );

    // only react to events related to our window
    if( pw != this ) return;

    //qWarning( "ProjectWindow::projectChangedSlot: "
    //        " pw = %d, t = %d", (unsigned)pw, t );

    // when we get LTE_CMD_NOOP and we are not yet constructed, we behave as
    // if we received LTE_CMD_VIEW_SELECT
    // when we received LTE_CMD_VIEW_SELECT we recreate EPVC
    if( ( t == LTE_CMD_VIEW_SELECT ||
            (t == LTE_CMD_NOOP && !d->constructed ) ) && pw == this ) {

        // refresh the view container
        m_viewContainer->setProjectView( d->project->views().current(),
                d->zoom );
        d->constructed = true;
        return;
    }

    // we ignore any panel select requestes, we should generate them in 100%
    // of the cases
    if( t == LTE_CMD_PANEL_SELECT ) return;

    // when we receive LTE_CMD_IOPINS_CHANGE we schedule repaint for the EPVC
    if( t == LTE_CMD_IOPINS_CHANGE && pw == this ) {
        m_viewContainer->updatePanelOnTop();
    }

    // when we receive view rename, we simply change the appropriate item in
    // the m_viewSelector
    if( t == LTE_CMD_VIEW_RENAME && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_VIEW_RENAME ) {

        // select the renamed item
        RenameViewCommand* cmd =
            (RenameViewCommand*)pw->lastCommand();

        int idx = pw->project()->views().indexOf( cmd->newViewName() );
        QString label = m_viewSelector->text( idx );

        Q_ASSERT( label == cmd->oldViewName() );

        m_viewSelector->changeItem( cmd->newViewName(), idx );

        displayView( cmd->newViewName() );
    }

    // when we receive view resize, we simply change the panel views'
    // dimmensions to the new view sizes
    if( t == LTE_CMD_VIEW_RESIZE && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_VIEW_RESIZE ) {

        // select the resized item
        ResizeViewCommand* cmd =
            (ResizeViewCommand*)pw->lastCommand();

        // simply redisplay the view, results in view rebuild
        displayView( cmd->viewName() );
    }

    //when we receive view delete, we remove the deleted item and possibly
    //choose another view
    if( t == LTE_CMD_VIEW_DELETE && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_VIEW_ADD_DEL ) {

        // select the deleted item
        AddDelViewCommand* cmd =
            (AddDelViewCommand*)pw->lastCommand();

        bool reselect = cmd->position() == m_viewSelector->currentItem();
        m_viewSelector->removeItem( cmd->position() );
        if( reselect ) {
            int newidx = pw->project()->views().currentIndex();

            if( newidx != -1 ) {
                m_viewSelector->setCurrentItem( newidx );
                viewSelectedSlot( m_viewSelector->currentText() );
            } else {
                m_viewSelector->clear();
                viewSelectedSlot( QString::null );
            }
        }

        if( pw->project()->views().count() == 0 ) {
            m_viewSelector->setEnabled( false );
        }
    }

    //when we receive view add, we add it to the correct position
    if( t == LTE_CMD_VIEW_ADD && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_VIEW_ADD_DEL ) {

        // select the added item
        AddDelViewCommand* cmd =
            (AddDelViewCommand*)pw->lastCommand();

        m_viewSelector->insertItem( cmd->viewName(), cmd->position() );
        if( m_viewSelector->isEnabled() == false ) {
            m_viewSelector->setEnabled( true );
        }
        m_viewSelector->setCurrentItem( cmd->position() );
        viewSelectedSlot( m_viewSelector->currentText() );
    }

    //when we receive view reorder, we remove & add to the current position
    if( t == LTE_CMD_VIEW_ORDER_CHANGE && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_VIEW_ORDER_CHANGE ) {

        // select the added item
        ReorderViewCommand* cmd =
            (ReorderViewCommand*)pw->lastCommand();

        bool reselectNew =
            (cmd->oldPosition() == m_viewSelector->currentItem() );
        bool reselectOld =
            (cmd->newPosition() == m_viewSelector->currentItem() );

        Q_ASSERT( cmd->viewName() ==
                m_viewSelector->text( cmd->oldPosition() ) );

        m_viewSelector->removeItem( cmd->oldPosition() );
        m_viewSelector->insertItem( cmd->viewName(), cmd->newPosition() );

        if( reselectNew ) {
            m_viewSelector->setCurrentItem( cmd->newPosition() );
            d->project->views().setCurrentIndex( cmd->newPosition() );
        }
        if( reselectOld ) {
            m_viewSelector->setCurrentItem( cmd->oldPosition() );
            d->project->views().setCurrentIndex( cmd->oldPosition() );
        }
    }

    //when we receive panel delete, we remove the deleted item, view
    //container will choose another panel for us automatically
    if( t == LTE_CMD_PANEL_DELETE && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_PANEL_ADD_DEL ) {

        // select the deleted item
        AddDelPanelCommand* cmd =
            (AddDelPanelCommand*)pw->lastCommand();

        if( cmd->viewName() != m_viewSelector->currentText() ) {
            // simply reselect the view, this results in view rebuild
            displayView( cmd->viewName() );
        } else {
            m_viewContainer->removePanelView( cmd->position() );
            d->mw->projectChangedSlot(this, LTE_CMD_SELECTION_CHANGE);
        }
    }

    //when we receive panel add, we add it to the correct position
    if( t == LTE_CMD_PANEL_ADD && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_PANEL_ADD_DEL ) {

        // select the added item
        AddDelPanelCommand* cmd =
            (AddDelPanelCommand*)pw->lastCommand();

        if( cmd->viewName() != m_viewSelector->currentText() ) {
            // simply reselect the view, this results in view rebuild
            displayView( cmd->viewName() );
        } else {
            m_viewContainer->addPanelView( cmd->panelName(), d->zoom,
                    cmd->position() );
        }

        // activate the most recently added panel
        m_viewContainer->setCurrentPage( cmd->position() );
    }

    //when we receive panel reorder, we remove & add to the current position
    if( t == LTE_CMD_PANEL_ORDER_CHANGE && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_PANEL_ORDER_CHANGE ) {

        // select the added item
        PanelOrderChangeCommand* cmd =
            (PanelOrderChangeCommand*)pw->lastCommand();

        // ensure correct view is selected, this will result in view rebuild
        if( cmd->viewName() != m_viewSelector->currentText() ) {
            displayView( cmd->viewName() );
        } else {

            bool reselectNew =
                (cmd->oldPos() == m_viewContainer->currentPageIndex() );
            bool reselectOld =
                (cmd->newPos() == m_viewContainer->currentPageIndex() );

            Q_ASSERT( cmd->newPanel() == m_viewContainer->tabLabel(
                        m_viewContainer->page( cmd->oldPos() ) ) );

            QWidget * w = m_viewContainer->page( cmd->oldPos() );
            m_viewContainer->removePage( w );
            m_viewContainer->insertTab( w, cmd->newPanel(), cmd->newPos() );

            if( reselectNew ) {
                m_viewContainer->showPage(
                        m_viewContainer->page( cmd->newPos() ) );
            }
            if( reselectOld ) {
                m_viewContainer->showPage(
                        m_viewContainer->page( cmd->oldPos() ) );
            }
        }
    }

    //when we receive panel rename, we simply change the appropriate tab
    if( t == LTE_CMD_PANEL_RENAME && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_PANEL_RENAME ) {

        // select the renamed item
        RenamePanelCommand* cmd =
            (RenamePanelCommand*)pw->lastCommand();

        // ensure correct view is selected
        if( cmd->viewName() != m_viewSelector->currentText() ) {
            displayView( cmd->viewName() );
        } else {

            m_viewContainer->renamePanelView( cmd->position(),
                    cmd->newPanelName(), d->zoom );

        }
        displayPanel( cmd->newPanelName() );
    }

    //when we receive selection move or resize we show the modified panel
    if( t == LTE_CMD_SELECTION_MOVE_RESIZE && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_SELECTION_MOVE_RESIZE ) {

        // select the moved items
        MoveNResizeSelectionCommand* cmd =
            (MoveNResizeSelectionCommand*)pw->lastCommand();

        // ensure correct view is selected
        if( cmd->viewName() != m_viewSelector->currentText() ) {
            displayView( cmd->viewName() );
        }

        // ensure the panel where modifications has been made is visible
        m_viewContainer->displayPanel( cmd->panelName() );

        // resize all templates
        TemplateDict td(cmd->selectedTemplates());
        m_viewContainer->currentPanelView()->resizeTemplates(td);

        // select the moved/resized templates
        m_viewContainer->currentPanelView()->selectTemplates(
                cmd->selectedTemplates() );

        m_viewContainer->updatePanelOnTop();
    }

    //when we add new template we select it
    if( t == LTE_CMD_SELECTION_ADD && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_SELECTION_ADD_DEL ) {

        // select the added item
        AddDelSelectionCommand* cmd =
            (AddDelSelectionCommand*)pw->lastCommand();

        // ensure correct view is selected
        if( cmd->viewName() != m_viewSelector->currentText() ) {
            displayView( cmd->viewName() );
        }

        // ensure the panel where modifications has been made is visible
        m_viewContainer->displayPanel( cmd->panelName() );

        // select the added templates
        m_viewContainer->currentPanelView()->selectTemplates(
                cmd->templates() );
    }

    //when we delete some templates we unselect all
    if( t == LTE_CMD_SELECTION_DEL && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_SELECTION_ADD_DEL ) {

        // select the added item
        AddDelSelectionCommand* cmd =
            (AddDelSelectionCommand*)pw->lastCommand();

        // ensure correct view is selected
        if( cmd->viewName() != m_viewSelector->currentText() ) {
            displayView( cmd->viewName() );
        }

        // ensure the panel where modifications has been made is visible
        m_viewContainer->displayPanel( cmd->panelName() );

        // unselect all templates
        m_viewContainer->currentPanelView()->unselectAllTemplates();
    }

    //when we receive template reorder, we remove & add to the current position
    if( t == LTE_CMD_TEMPLATE_RENAME && pw == this &&
            pw->lastCommand()->rtti() == LTE_CMD_TEMPLATE_RENAME ) {

        // select the added item
        RenameTemplateCommand* cmd =
            (RenameTemplateCommand*)pw->lastCommand();

        // ensure correct view is selected, this will possibly result in
        // view rebuild
        if( cmd->viewName() != m_viewSelector->currentText() ) {
            displayView( cmd->viewName() );
        }

        // ensure the panel where modifications has been made is visible
        m_viewContainer->displayPanel( cmd->panelName() );

        // rename
        m_viewContainer->currentPanelView()->renameTemplate(
                cmd->oldTemplateName(), cmd->newTemplateName() );
    }

    // when we receive LTE_CMD_SELECTION_PROPS_CHANGE we schedule repaint
    // for the EPVC
    if( t == LTE_CMD_SELECTION_PROPS_CHANGE && pw == this ) {
        m_viewContainer->updatePanelOnTop();
    }

    //when we receive template reorder, and we are in preview mode, we
    //reenter the focus mode
    if( ( t == LTE_CMD_TEMPLATE_TABORDER_CHANGE ||
                t == LTE_CMD_TEMPLATE_SHORTCUT_CHANGE ) && pw == this ) {

        if( m_viewContainer->operationMode() == EditablePanelView::Preview ) {
            m_viewContainer->setOperationMode( EditablePanelView::Modify );
            m_viewContainer->setOperationMode( EditablePanelView::Preview );
        }
    }

    // update the visible panel when resources changes
    if(t == LTE_CMD_RESOURCE_ADD ||
            t == LTE_CMD_RESOURCE_DELETE ||
            t == LTE_CMD_RESOURCE_RENAME)
    {
        m_viewContainer->fullUpdatePanelOnTop();
    }
}

void ProjectWindow::executeCommand( Command* cmd )
{
    Q_ASSERT(d);
    Q_ASSERT(d->project);

    d->undoStack->push(cmd);
}

void ProjectWindow::executeCommandLater( Command* cmd )
{
    Q_ASSERT(d);
    Q_ASSERT(cmd);
    Q_ASSERT(d->lateCmd == NULL);

    d->lateCmd = cmd;
    QTimer::singleShot(0, this, SLOT(executeCommandLaterSlot()));
}

void ProjectWindow::executeCommandLaterSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(d->lateCmd);

    d->undoStack->push(d->lateCmd);

    d->lateCmd = NULL;
}

Command* ProjectWindow::lastCommand() const
{
    Q_ASSERT(d);
    return d->lastCommand;
}

void ProjectWindow::setLastCommand( Command* cmd )
{
    Q_ASSERT(d);
    d->lastCommand = cmd;
}

void ProjectWindow::displayView( const QString& viewName )
{
    // update combo
    m_viewSelector->setCurrentItem( d->project->views().indexOf( viewName ) );
    // just forward the request
    viewSelectedSlot( viewName );
    m_viewContainer->resizePanelViews( d->zoom );
}

void ProjectWindow::displayPanel( const QString& panelName )
{
    Q_ASSERT( m_viewContainer );
    m_viewContainer->displayPanel( panelName );
}

bool ProjectWindow::eventFilter( QObject * /* o */, QEvent * e )
{
    //qWarning( "ProjectWindow::eventFilter object=0x%x, event %d",
    //        (unsigned)o, e->type() );
    //qWarning( "object is this: %d", o == this );
    if( e->type() == QEvent::KeyPress )
    if( m_viewContainer->currentPanelView() == NULL ) return false;

    if( m_viewContainer->operationMode() == EditablePanelView::Magnify ) {

        if( e->type() != QEvent::KeyPress &&
                e->type() != QEvent::KeyRelease )
            return false;

        QKeyEvent * ke = (QKeyEvent *)e;

        // check for shift and possibly exchange cursors
        if( e->type() == QEvent::KeyPress && ke->key() == Qt::Key_Shift ) {
            m_viewContainer->setMagnifyCursor( 0 );
        }
        if( e->type() == QEvent::KeyRelease && ke->key() == Qt::Key_Shift ) {
            m_viewContainer->setMagnifyCursor( 1 );
        }

        return false;
    }

    // in preview mode we eat all events and forward keyboard ones to the
    // current EPV
    if( m_viewContainer->operationMode() == EditablePanelView::Preview ) {
        switch( e->type() ) {
            case QEvent::Accel:
            case QEvent::AccelOverride:
                // only pass in tab key
                if( ((QKeyEvent*)e)->key() != Qt::Key_Tab ) break;
            case QEvent::KeyPress:
                m_viewContainer->currentPanelView()->keyPressEvent(
                        (QKeyEvent*)e );
                return true;
            case QEvent::KeyRelease:
                m_viewContainer->currentPanelView()->keyReleaseEvent(
                        (QKeyEvent*)e );
                return true;
            default:
                break;
        }
        return false;
    }
    return false;
}

void ProjectWindow::undoStackChangedSlot( bool clean )
{
    Q_ASSERT(m_unsavedLabel);
    if(clean) {
        m_unsavedLabel->setText(QString::null);
    } else {
        m_unsavedLabel->setText(tr("Project is unsaved"));
    }
}

bool ProjectWindow::unsaved() const
{
    Q_ASSERT(d);
    return !d->undoStack->isClean();
}

void ProjectWindow::forceUnsaved()
{
    Q_ASSERT(d);
    d->undoStack->clear();
}

void ProjectWindow::languageChange()
{
    const QString s = caption();
    ProjectWindowBase::languageChange();
    setCaption(s);
}

void ProjectWindow::showGlobalMenuSlot(QMouseEvent* e)
{
    if( e && (e->button() & Qt::RightButton)) {
        e->accept();
        // show menu
        d->projectWindowMenu->exec( e->globalPos() );
    }
}

ConfigurationPtr ProjectWindow::saveEditorSettings() const {

    ConfigurationPtr es = ConfigurationPtr(
            new DefaultConfiguration("editor-settings"));

    ConfigurationPtr gs = es->getChild("grid-settings", true);

    ConfigurationPtr pb = gs->getChild("visible", true);
    pb->setValue(gridVisible()?"true":"false");

    pb = gs->getChild("snap", true);
    pb->setValue(gridSnap()?"true":"false");

    pb = gs->getChild("color", true);
    pb->setValue(gridColor().name());

    pb = gs->getChild("x-spacing", true);
    pb->setValue(QString::number(gridXSpacing()));

    pb = gs->getChild("y-spacing", true);
    pb->setValue(QString::number(gridYSpacing()));

    pb = gs->getChild("highlight", true);
    pb->setValue(QString::number(gridHighlight()));

    return es;
}

void ProjectWindow::applyEditorSettings(ConfigurationPtr es)
{
    if(es.isNull()) {
        return;
    }

    ConfigurationPtr gs = es->getChild("grid-settings", false);
    if(gs.isNull()) {
        return;
    }

    ConfigurationPtr pb = gs->getChild("visible", false);
    if(!pb.isNull()) {
        setGridVisible(pb->getValueAsBool(gridVisible()));
    }

    pb = gs->getChild("snap", false);
    if(!pb.isNull()) {
        setGridSnap(pb->getValueAsBool(gridSnap()));
    }

    pb = gs->getChild("color", false);
    if(!pb.isNull()) {
        setGridColor(QColor(pb->getValue(gridColor().name())));
    }

    pb = gs->getChild("x-spacing", false);
    if(!pb.isNull()) {
        setGridXSpacing(pb->getValueAsInt(gridXSpacing()));
    }

    pb = gs->getChild("y-spacing", false);
    if(!pb.isNull()) {
        setGridYSpacing(pb->getValueAsInt(gridYSpacing()));
    }

    pb = gs->getChild("highlight", false);
    if(!pb.isNull()) {
        setGridHighlight(pb->getValueAsInt(gridHighlight()));
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectWindow.cpp 1564 2006-04-07 13:33:15Z modesto $
