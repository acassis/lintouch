// $Id: MainWindow.cpp 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: MainWindow.cpp --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 12 July 2004
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
#include <qapplication.h>
#include <qdir.h>
#include <qdir.h>
#include <qiconset.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qptrlist.h>
#include <qscrollview.h>
#include <qsize.h>
#include <qstatusbar.h>
#include <qstringlist.h>
#include <qtimer.h>

#include <stdlib.h>

#include <lt/vfs/TempFiles.hh>
#include <lt/templates/TemplateManager.hh>
#include <lt/templates/TemplateLibrary.hh>
#include <lt/project/Panel.hh>
#include <lt/project/TemplateManagerBuilder.hh>

#include "AboutDialog.hh"
#include "CommandType.hh"
#include "CopyPasteBuffer.hh"
#include "EditablePanelViewContainer.hh"
#include "EditorProjectBuilder.hh"
#include "EditorProjectSerializer.hh"
#include "EditorProject.hh"
#include "HelpWindow.hh"
#include "MainWindow.hh"
#include "PreferencesDialog.hh"
#include "ProjectConnectionsDialog.hh"
#include "ProjectGridSettingsDialog.hh"
#include "ProjectPanelsDialog.hh"
#include "ProjectPreviewDialog.hh"
#include "ProjectPropertiesDialog.hh"
#include "ProjectResourcesDialog.hh"
#include "ProjectTemplatesDialog.hh"
#include "ProjectVariablesDialog.hh"
#include "ProjectViewsDialog.hh"
#include "ProjectWindow.hh"
#include "TemplateLibrariesDialog.hh"
#include "TemplateLibrarySelectionCombo.hh"
#include "TemplatePalette.hh"
#include "TemplatePropertiesDialog.hh"
#include "ProjectServerLoggingDialog.hh"

// uncomment when using OutputDebugStringA
// #include <windows.h>

using namespace lt;

// comment or undef when you don't want the dummy project inserted
// #define CREATE_DUMMY_PROJECT

#undef CREATE_DUMMY_PROJECT
#ifdef CREATE_DUMMY_PROJECT
#include "DummyProjectBuilder.hh"
#endif

// comment or undef when you don't want the dummy connections inserted
// #define CREATE_DUMMY_CONNECTIONS

#undef CREATE_DUMMY_CONNECTIONS
#ifdef CREATE_DUMMY_CONNECTIONS
#include <lt/templates/Connection.hh>
#include <lt/templates/Property.hh>
#endif

#include <MainWindowProtected.hh>

MainWindow::MainWindow(
        const QString & helpFile,
        const QStringList prjs,
        LoggerPtr l,
        QWidget* parent /*= 0*/,
        const char* name /*= 0*/, WFlags fl /*= 0*/ ) :
    MainWindowBase( parent, name, fl ),
    d( new MainWindowProtected )
{
    Q_CHECK_PTR( d );

    // save the project list
    d->prjs = prjs;

    // toolbar icons
    mainModifyAction->setIconSet(
            QIconSet( QPixmap::fromMimeSource( "modify.xpm" ) ) );
    mainMagnifyAction->setIconSet(
            QIconSet( QPixmap::fromMimeSource( "magnify.xpm" ) ) );
    mainPanningAction->setIconSet(
            QIconSet( QPixmap::fromMimeSource( "pan.xpm" ) ) );
    mainPreviewAction->setIconSet(
            QIconSet( QPixmap::fromMimeSource( "preview.xpm" ) ) );
    onlineAction->setIconSet(
            QIconSet( QPixmap::fromMimeSource( "online.xpm" ) ) );

    // copy paste buffer
    d->copyPasteBuffer = new CopyPasteBuffer( this );
    Q_CHECK_PTR( d->copyPasteBuffer );

    // Help Window
    d->helpWindow = new HelpWindow( helpFile );
    Q_CHECK_PTR( d->helpWindow );

    // About Dialog
    d->aboutDialog = new AboutDialog( this );
    Q_CHECK_PTR( d->aboutDialog );

    // get  logger
    d->logger = l;

    // create TemplateManager
    TemplateManagerBuilder* tmb =
        new TemplateManagerBuilder( d->logger );
    Q_CHECK_PTR(tmb);
    d->templateManager = tmb->buildFromDefaultDirs();
    delete tmb;

    // create PluginManager
    d->pluginManager = new PluginManager(d->logger->getChildLogger(
        "PluginManager"));

    // create ServerLoggingManager
    d->slManager = new ServerLoggingManager(d->logger->getChildLogger(
        "ServerLoggingManager"));

    // remove status bar; Qt Designer adds it to every QMainWindow by
    // default and I've found no other way how to remove it.
    statusBar()->close();

    // Quit dialog
    d->quitDialog = new QMessageBox( tr("Quit - Lintouch Editor"),
            tr("Unsaved Projects exist. "
                "Do You want to Exit Lintouch Editor?"),
            QMessageBox::Warning,
            QMessageBox::Cancel | QMessageBox::Escape | QMessageBox::Default,
            QMessageBox::Ok,
            QMessageBox::NoButton,
            this );
    Q_CHECK_PTR( d->quitDialog );
    d->quitDialog->setButtonText( QMessageBox::Cancel, tr("Cancel") );
    d->quitDialog->setButtonText( QMessageBox::Ok, tr("Quit") );

    // ProjectPreferences Dialog
    d->prefs = new PreferencesDialog( this );
    Q_CHECK_PTR( d->prefs );

    // ProjectViews Dialog
    d->projectViewsDialog =
        new ProjectViewsDialog( this );
    Q_CHECK_PTR( d->projectViewsDialog );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectViewsDialog, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // ProjectPanels Dialog
    d->projectPanelsDialog = new ProjectPanelsDialog( this );
    Q_CHECK_PTR( d->projectPanelsDialog );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectPanelsDialog, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // ProjectTemplates Dialog
    d->projectTemplatesDialog = new ProjectTemplatesDialog( this );
    Q_CHECK_PTR( d->projectTemplatesDialog );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectTemplatesDialog, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // ProjectProperties Dialog
    d->projectPropertiesDialog = new ProjectPropertiesDialog( this );
    Q_CHECK_PTR( d->projectPropertiesDialog );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectPropertiesDialog, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // TemplateProperties Dialog
    d->templatePropertiesDialog = new TemplatePropertiesDialog( this );
    Q_CHECK_PTR( d->templatePropertiesDialog );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->templatePropertiesDialog, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // GridSettings Dialog
    d->projectGridSettingsDialog =
        new ProjectGridSettingsDialog( this );
    Q_CHECK_PTR( d->projectGridSettingsDialog );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectGridSettingsDialog, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // ProjectPreview Dialog
    d->projectPreviewDialog = new ProjectPreviewDialog( this );
    Q_CHECK_PTR( d->projectPreviewDialog );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectPreviewDialog, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // TemplateLibraries Dialog
    d->templateLibrariesDialog =
        new TemplateLibrariesDialog( d->templateManager, this );
    Q_CHECK_PTR( d->templateLibrariesDialog );

    // ProjectResources dialog
    d->projectResourcesDialog = new ProjectResourcesDialog( this );
    Q_ASSERT( d->projectResourcesDialog );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectResourcesDialog, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // create MRU popup
    d->placeMRUPopup( this, fileMenu );

    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    QHBoxLayout* h = new QHBoxLayout(centralWidget(), -1, -1, "QHBoxLayout");
    QVBoxLayout* v = new QVBoxLayout( h, -1, "QVBoxLayout" );

    // Template library selection combo
    d->templateLibrarySelectionCombo =
        new TemplateLibrarySelectionCombo( d->templateManager,
        centralWidget() );
    Q_CHECK_PTR( d->templateLibrarySelectionCombo );
    connect( d->templateLibrarySelectionCombo,
           SIGNAL(libraryChanged( const QString & ) ),
           this,
           SLOT(templateLibraryChanged( const QString & ) ) );
    v->addWidget( d->templateLibrarySelectionCombo );

    // Template button pallete
    d->templatePalette = new TemplatePalette(d->templateManager,
            centralWidget());
    Q_CHECK_PTR( d->templatePalette );
    v->addWidget( d->templatePalette );

    if ( d->templateLibrarySelectionCombo->count() > 0 ){
        d->templateLibrarySelectionCombo->setCurrentItem( 0 );
    }

    //connect interesting signals here
    connect( d->templatePalette, SIGNAL( templateSelected() ),
            this, SLOT( switchToPlaceModeSlot() ) );

    connect( d->templatePalette, SIGNAL( templateUnselected() ),
            this, SLOT( switchOutOfPlaceModeSlot() ) );

    // ProjectConnections Dialog
    d->projectConnections = new ProjectConnectionsDialog( this );
    Q_CHECK_PTR( d->projectConnections );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectConnections, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // ProjectVariables Dialog
    d->projectVariables =
        new ProjectVariablesDialog( this );
    Q_CHECK_PTR( d->projectVariables );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectVariables, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    // ProjectServerLoggingDialog Dialog
    d->projectServerLogging = new ProjectServerLoggingDialog( this );
    Q_CHECK_PTR( d->projectServerLogging );
    connect( this, SIGNAL( projectChanged(
                    ProjectWindow*,CommandType)),
            d->projectServerLogging, SLOT( projectChangedSlot(
                    ProjectWindow*,CommandType))
           );

    if( !d->prjs.isEmpty() ) {
        QTimer::singleShot( 0, this, SLOT( startup() ) );
    }
}

MainWindow::~MainWindow()
{
    delete d->slManager;
    delete d->pluginManager;
    delete d->helpWindow;
    delete d;
}

void MainWindow::startup()
{
    for(QStringList::const_iterator it = d->prjs.begin();
            it != d->prjs.end(); ++it) {
        openProjectFileInfo( QFileInfo( *it ) );
    }
}

void MainWindow::switchToPlaceModeSlot()
{
    mainModifyAction->setOn( true );
    d->mode = EditablePanelView::Place;
    setOperationModeOfPWs( d->mode );
}

void MainWindow::switchOutOfPlaceModeSlot()
{
    if( d->mode == EditablePanelView::Place ) {
        switchToModifyModeSlot();
    }
}

void MainWindow::switchToModifyModeSlot()
{
    mainModifyAction->setOn( true );
    d->mode = EditablePanelView::Modify;
    setOperationModeOfPWs( d->mode );
    d->templatePalette->clearSelection();
}

void MainWindow::switchToMagnifyModeSlot()
{
    mainMagnifyAction->setOn( true );
    d->mode = EditablePanelView::Magnify;
    setOperationModeOfPWs( d->mode );
    d->templatePalette->clearSelection();
}

void MainWindow::switchToPanModeSlot()
{
    mainPanningAction->setOn( true );
    d->mode = EditablePanelView::Pan;
    setOperationModeOfPWs( d->mode );
    d->templatePalette->clearSelection();
}

void MainWindow::switchToPreviewModeSlot()
{
    mainPreviewAction->setOn( true );
    d->mode = EditablePanelView::Preview;
    setOperationModeOfPWs( d->mode );
    showProjectPreviewDialogSlot();
    d->templatePalette->clearSelection();
}

void MainWindow::goOnlineSlot()
{
    // when we have a pw, we save the project to a temp file and exec it
    // with runtime
    ProjectWindow * pw = d->lastProjectWindow;
    if( pw != NULL ) {
        EditorProject * ep = pw->project();

        if( ! pw->saveProjectSlot() ) return;

        // generate safe temporary filename
        QString file = getTempFileName() + ".ltp";

        if (!EditorProjectSerializer(d->logger->getChildLogger(
                        QString("Serializer(%1)").arg(file)))
                .saveEditorProject(*ep, file, pw->saveEditorSettings()))
        {
            QMessageBox::critical(this,
                    tr("Save As - Lintouch Editor"),
                    tr("Unable to save the project %1.")
                    .arg(file) );
            return;
        }

        // fork runtime
        RuntimeController * rc = new RuntimeController( this );
        connect( rc, SIGNAL( runtimeExited( RuntimeController *, int ) ),
                this, SLOT( runtimeExited( RuntimeController *, int ) ) );
        rc->setProject( file );
        if(!rc->isReady()) {
            QMessageBox::critical(this,
                    tr("Error"),
                    tr("Unable to execute Lintouch Runtime."));
            return;
        }
        rc->start();
    }
}

void MainWindow::runtimeExited( RuntimeController * rc, int /* retcode */ )
{
    // no controller, no action
    if( rc == NULL ) return;

    // delete project
    QFile::remove( rc->project() );

    // and get rid of the controller
    rc->deleteLater();
}

void MainWindow::showPreferencesSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->prefs );

    d->prefs->raise();
    d->prefs->setActiveWindow();
    d->prefs->show();
}

bool MainWindow::close( bool alsoDelete )
{
    Q_ASSERT( d );

    bool unsaved = false;

    QPtrListIterator<ProjectWindow> it(d->projectWindowsList);
    for(;it.current() && !unsaved; ++it ) {
        Q_ASSERT(*it);
        unsaved |= (*it)->unsaved();
    }

    if( unsaved ) {
        Q_ASSERT( d->quitDialog );
        if( d->quitDialog->exec() == QMessageBox::Cancel ) {
            // Not quitting
            return false;
        }
    }

    // have to make a copy because on every close the MW gets notified and
    // removed the PW from d->projectWindowsList
    QPtrList<ProjectWindow> pl = d->projectWindowsList;

    it = QPtrListIterator<ProjectWindow>(pl);
    for(;it.current(); ++it ) {
        Q_ASSERT(*it);
        (*it)->forceUnsaved();
        (*it)->close(true);
    }

    return MainWindowBase::close( alsoDelete );
}

void MainWindow::createProjectSlot()
{
    Q_ASSERT( d );

#ifdef CREATE_DUMMY_PROJECT
    DummyProjectBuilder dpb( d->logger, "" );
    EditorProject * prj = dpb.buildDummyProject(
            2, 10, 10, d->templateManager, "LTTest", "LTLTest" );
            //2, 10, 10, d->templateManager, "Lamp", "mytemplatelibrary" );
    Q_CHECK_PTR(prj);
    // pass template manager to the project (used for cloning
    // view/panel/template cloning)
    prj->setTemplateManager( d->templateManager );
#else
    EditorProject* prj = new EditorProject(
            EditorProjectBuilder(d->logger->getChildLogger("BuilderNew"))
            .buildEditorProject(d->templateManager));
    Q_CHECK_PTR(prj);
    if (!prj->isValid())
    {
        QMessageBox::critical(this,
                tr("New - Lintouch Editor"),
                tr("Unable to create a new project."));
                //TODO:true reason!
        return;
    }
    prj->addView(tr("View"), createDefaultView());
#endif
    // fetch some defaults and set them to the project
    Info i = prj->info();
    i.setAuthor( d->prefs->projectAuthor() );
    i.setDate( QDate::currentDate() );

#ifdef CREATE_DUMMY_CONNECTIONS
    PropertyDict pd;
    pd.insert( "refresh", new Property( "Refresh", "float", "0.1" ) );
    Q_ASSERT(pd.count()==1);
    Connection* c = new Connection( "Generator", pd );
    Q_CHECK_PTR(c);
    prj->addConnection( "Generator", c );

    c = new Connection( "Loopback", PropertyDict() );
    Q_CHECK_PTR(c);
    prj->addConnection( "Loopback", c );
#endif

    // create new PW and show it!
    ProjectWindow* lte = d->newProjectWindow( this, this, prj,
            (double)(d->prefs->projectZoomLevel() / 100.0) );
    Q_ASSERT( lte );
    lte->setProjectFileInfo(QFileInfo(QDir::home(),d->newProjectName()));
    // take default grid settings and set them to the created window
    lte->setGridColor( d->prefs->gridColor() );
    lte->setGridXSpacing( d->prefs->gridXSpacing() );
    lte->setGridYSpacing( d->prefs->gridYSpacing() );
    lte->setGridHighlight( d->prefs->gridHighlight() );
    lte->setGridVisible( d->prefs->projectShowGrid() );
    lte->setGridSnap( d->prefs->projectSnapToGrid() );
    // set initial mode
    lte->m_viewContainer->setOperationMode( d->mode );
    // and show the project window
    lte->show();

    // switch to modify mode
    switchToModifyModeSlot();
}

void MainWindow::openProjectSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->prefs );

    QStringList files = QFileDialog::getOpenFileNames(
            tr("Lintouch Project Files (*.ltp);;All files (*.*)"),
            QString::null,
            this, "openProjectDialog",
            tr("Open Project - Lintouch Editor") );

    if( !files.count() ) {
        return;
    }

    for( QStringList::const_iterator it = files.constBegin();
            it != files.constEnd(); ++it )
    {
        openProjectFileInfo( *it );
        qApp->processEvents();
    }

    d->updateMRUPopup();
}

void MainWindow::projectWindowActivatedSlot( ProjectWindow* pw )
{
    Q_ASSERT( d );
    Q_ASSERT( pw );

    if( pw != d->lastProjectWindow ) {
        d->lastProjectWindow = pw;

        if( pw && pw != d->projectWindowsList.first() ) {
            // remove the PW from it's position in the list
            bool ok = d->projectWindowsList.remove( pw );
            Q_ASSERT( ok );

            // and set it as the first one
            ok = d->projectWindowsList.insert( 0, pw );
            Q_ASSERT( ok );
        }

        emit( projectChangedSlot(pw,LTE_CMD_NOOP) );
    }
}

void MainWindow::projectWindowClosedSlot( QObject* o )
{
    Q_ASSERT( d );

    // remove from internal list
    // (can't use dynamic_cast<> because it always returns NULL :-(((
    ProjectWindow* pw = (ProjectWindow*)o;
    int pos = d->projectWindowsList.find( pw );
    Q_ASSERT( pos >= 0 );
    d->projectWindowsList.remove( pos );

    // last is NULL now
    d->lastProjectWindow = NULL;

    if( d->projectWindowsList.count() != 0 ) {
        // activate another PW in list if there is one
        pw = d->projectWindowsList.first();
        Q_ASSERT(pw);
        pw->raise();
        pw->setActiveWindow();
        pw->show();
        // no emit, the setActiveWindow should do it
    } else {
        // signal empty ProjectWindow if last PW closed
        emit( projectChangedSlot(NULL,LTE_CMD_NOOP) );
    }
}

void MainWindow::showProjectViewsDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->projectViewsDialog );
    d->projectViewsDialog->raise();
    d->projectViewsDialog->setActiveWindow();
    d->projectViewsDialog->show();
}

void MainWindow::showProjectPanelsDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->projectPanelsDialog );
    d->projectPanelsDialog->raise();
    d->projectPanelsDialog->setActiveWindow();
    d->projectPanelsDialog->show();
}

void MainWindow::showProjectTemplatesDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->projectTemplatesDialog );
    d->projectTemplatesDialog->raise();
    d->projectTemplatesDialog->setActiveWindow();
    d->projectTemplatesDialog->show();
}

void MainWindow::showProjectPropertiesDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->projectPropertiesDialog );
    d->projectPropertiesDialog->raise();
    d->projectPropertiesDialog->setActiveWindow();
    d->projectPropertiesDialog->show();
}

void MainWindow::showTemplatePropertiesDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->templatePropertiesDialog );
    d->templatePropertiesDialog->raise();
    d->templatePropertiesDialog->setActiveWindow();
    d->templatePropertiesDialog->show();
}

void MainWindow::showProjectGridSettingsDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->projectGridSettingsDialog );
    d->projectGridSettingsDialog->raise();
    d->projectGridSettingsDialog->setActiveWindow();
    d->projectGridSettingsDialog->show();
}

void MainWindow::showProjectPreviewDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->projectPreviewDialog );
    d->projectPreviewDialog->raise();
    d->projectPreviewDialog->setActiveWindow();
    d->projectPreviewDialog->show();
}

void MainWindow::showTemplateLibrariesDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->templateLibrariesDialog );
    d->templateLibrariesDialog->raise();
    d->templateLibrariesDialog->setActiveWindow();
    d->templateLibrariesDialog->show();
}

void MainWindow::showProjectResourcesDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->projectResourcesDialog );
    d->projectResourcesDialog->raise();
    d->projectResourcesDialog->setActiveWindow();
    d->projectResourcesDialog->show();
}

void MainWindow::showAboutDialogSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->aboutDialog );
    d->aboutDialog->raise();
    d->aboutDialog->setActiveWindow();
    d->aboutDialog->show();
}

void MainWindow::showHelpWindowSlot()
{
    Q_ASSERT( d );
    Q_ASSERT( d->helpWindow );
    d->helpWindow->raise();
    d->helpWindow->setActiveWindow();
    d->helpWindow->show();
}

void MainWindow::projectChangedSlot( ProjectWindow* pw,
        CommandType t )
{
    Q_ASSERT(d);
    emit( projectChanged(pw,t) );
}

EditablePanelView::OperationMode MainWindow::operationMode() const
{
    return d->mode;
}

CopyPasteBuffer * MainWindow::copyPasteBuffer()
{
    return d->copyPasteBuffer;
}

Template * MainWindow::instantiateTemplate(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ )
{
    Template * t = d->templatePalette->instantiate(logger);
    t->enableLogging( logger );
    t->compose( tm );
    t->localize( loc );
    return t;
}

void MainWindow::setOperationModeOfPWs(
        EditablePanelView::OperationMode m )
{
    // switch all project windows into given state
    ProjectWindow * i = NULL;
    for( i = d->projectWindowsList.first(); i != NULL;
            i = d->projectWindowsList.next() ) {
        i->m_viewContainer->setOperationMode( m );
    }
    projectChangedSlot( d->lastProjectWindow, LTE_CMD_NOOP );
}

void MainWindow::MRUProjectOpenSlot( int id )
{
    Q_ASSERT( d );
    Q_ASSERT( d->prefs );

    int pos = d->MRUPopup->indexOf(id);

    QStringList l = d->prefs->recentProjects();

    openProjectFileInfo( l[pos] );
    qApp->processEvents();
}

void MainWindow::openProjectFileInfo( QFileInfo fi )
{
    ConfigurationPtr es;

    //load the EditorProject
    EditorProject* prj = new EditorProject(
            EditorProjectBuilder(d->logger->getChildLogger(
                    QString("Builder(%1)").arg(fi.filePath())))
            .buildEditorProject(fi.filePath(), d->templateManager,
                    *d->pluginManager, *d->slManager, es));
    Q_CHECK_PTR(prj);

    if (!prj->isValid())
    {
        QMessageBox::critical(this,
                tr("Open - Lintouch Editor"),
                tr("Unable to open the project %1.\n"
                    "File read error or file format error.")
                .arg(fi.filePath())
                );
        return;
    }

    // create new PW and show it!
    ProjectWindow* lte = d->newProjectWindow( this, this, prj,
            (double)(d->prefs->projectZoomLevel() / 100.0) );
    Q_ASSERT( lte );
    lte->setProjectFileInfo( fi );
    // take default grid settings and set them to the created window
    lte->setGridColor( d->prefs->gridColor() );
    lte->setGridXSpacing( d->prefs->gridXSpacing() );
    lte->setGridYSpacing( d->prefs->gridYSpacing() );
    lte->setGridHighlight( d->prefs->gridHighlight() );
    lte->setGridVisible( d->prefs->projectShowGrid() );
    lte->setGridSnap( d->prefs->projectSnapToGrid() );
    // set initial mode
    lte->m_viewContainer->setOperationMode( d->mode );

    // apply saved settings
    lte->applyEditorSettings(es);

    // and show the project window
    lte->show();

    addRecentProject( fi.filePath() );
}

void MainWindow::templateLibraryChanged( const QString& libraryName )
{
    if ( libraryName.isEmpty() ){
        return;
    }
    Q_ASSERT(d);
    Q_ASSERT(d->templatePalette);
    Q_ASSERT(d->templateManager.templateLibraries().contains(libraryName));

    d->templatePalette->currentLibraryChangedSlot(libraryName);
}

View MainWindow::createDefaultView() const
{
    Q_ASSERT(d);
    Q_ASSERT(d->prefs);

    QSize s = d->prefs->projectViewSize();

    View v;
    v.setVirtualWidth( s.width() );
    v.setVirtualHeight( s.height() );

    Panel p;
    v.addPanel(tr("Panel"), p);

    return v;
}

const TemplateManager& MainWindow::templateManager() const
{
    return d->templateManager;
}

void MainWindow::addRecentProject( const QString & filename )
{
    d->prefs->addRecentProject( filename );
    d->updateMRUPopup();
}

void MainWindow::showProjectConnectionsDialogSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(d->projectConnections);
    d->projectConnections->raise();
    d->projectConnections->setActiveWindow();
    d->projectConnections->show();
}

void MainWindow::showProjectVariablesDialogSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(d->projectVariables);
    d->projectVariables->raise();
    d->projectVariables->setActiveWindow();
    d->projectVariables->show();
}

void MainWindow::showProjectServerLoggingDialogSlot()
{
    Q_ASSERT(d);
    Q_ASSERT(d->projectServerLogging);
    d->projectServerLogging->raise();
    d->projectServerLogging->setActiveWindow();
    d->projectServerLogging->show();
}

const PluginManager& MainWindow::pluginManager() const
{
    Q_ASSERT(d);
    return *d->pluginManager;
}

const ServerLoggingManager& MainWindow::serverLoggingManager() const
{
    Q_ASSERT(d);
    return *d->slManager;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: MainWindow.cpp 1564 2006-04-07 13:33:15Z modesto $
