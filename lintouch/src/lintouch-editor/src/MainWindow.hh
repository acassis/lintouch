// $Id: MainWindow.hh 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: MainWindow.hh --
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

#ifndef _LTEDITORMAINWINDOW_HH
#define _LTEDITORMAINWINDOW_HH

#include <qfiledialog.h>

#include <lt/templates/TemplateManager.hh>
#include <lt/project/View.hh>
#include <lt/ui/RuntimeController.hh>

#include "CommandType.hh"
#include "EditablePanelView.hh"
#include "MainWindowBase.h"
#include "PluginManager.hh"
#include "ServerLoggingManager.hh"

class ProjectWindow;
class CopyPasteBuffer;

namespace lt
{
    class EditorProject;
}

using namespace lt;

/**
 * TODO
 */
class MainWindow : public MainWindowBase
{
    Q_OBJECT

public:
    /**
     * TODO
     */
    MainWindow( const QString & helpFile, const QStringList prjs, LoggerPtr l,
            QWidget* parent = 0, const char* name = 0,
            WFlags fl = 0 );
    /**
     * TODO
     */
    virtual ~MainWindow();

    /**
     * TODO
     */
    virtual bool close( bool alsoDelete );

public slots:

    /**
     * invoked after startup to possibly open files specified at
     * commandline.
     */
    void startup();

    /**
     * Connected to TemplateMatrix::pressed().
     */
    void switchToPlaceModeSlot();

    /**
     * Connected to TemplateMatrix::released().
     */
    void switchOutOfPlaceModeSlot();

    /**
     * Connected to Modify tool button.
     */
    void switchToModifyModeSlot();

    /**
     * Connected to Magnify tool button
     */
    void switchToMagnifyModeSlot();

    /**
     * Connected to Pan tool button
     */
    void switchToPanModeSlot();

    /**
     * Connected to Preview tool button
     */
    void switchToPreviewModeSlot();

    /**
     * Connected to Online Action.
     */
    void goOnlineSlot();

    /**
     * Connected to processExited of forked runtime.
     */
    void runtimeExited( RuntimeController * rc, int retcode );

    /**
     * Connected to Project/New.
     */
    void createProjectSlot();

    /**
     * Connected to Project/Open.
     */
    void openProjectSlot();

    /**
     * Loads project from fi and creates PW for it.
     */
    void openProjectFileInfo( QFileInfo fi );

    /**
     * Shows the Project Views Dialog.
     */
    void showProjectViewsDialogSlot();

    /**
     * Shows the Project Panels Dialog.
     */
    void showProjectPanelsDialogSlot();

    /**
     * Shows the Project Templates Dialog.
     */
    void showProjectTemplatesDialogSlot();

    /**
     * Shows the Project Properties Dialog.
     */
    void showProjectPropertiesDialogSlot();

    /**
     * Shows the Template Properties Dialog.
     */
    void showTemplatePropertiesDialogSlot();

    /**
     * Shows the Project Grid Settings Dialog.
     */
    void showProjectGridSettingsDialogSlot();

    /**
     * Shows the Project Preview Dialog.
     */
    void showProjectPreviewDialogSlot();

    /**
     * Shows the Project Resources Dialog.
     */
    void showProjectResourcesDialogSlot();

    /**
     * Shows the About Lintouch Editor Dialog.
     */
    void showAboutDialogSlot();

    /**
     * Shows the Help Window Dialog.
     */
    void showHelpWindowSlot();

    /**
     * Main dispatch routine distributing the pw related changes to all
     * project dependent dialogs.
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

    /**
     * Shows the Connections Dialog.
     */
    void showProjectConnectionsDialogSlot();

    /**
     * Shows the ConnectionVariables Dialog.
     */
    void showProjectVariablesDialogSlot();

    /**
     * Shows the Connections Dialog.
     */
    void showProjectServerLoggingDialogSlot();


public:

    /**
     * Returns the current operation mode of the editor.
     */
    EditablePanelView::OperationMode operationMode() const;

    /**
     * Returns the copy paste buffer.
     */
    CopyPasteBuffer * copyPasteBuffer();

    /**
     * Returns the new instance of currently selected template (at the
     * template tool button matrix
     */
    Template * instantiateTemplate( const LocalizatorPtr & loc,
            const TemplateManager & tm, LoggerPtr logger = Logger::nullPtr() );

    /**
     * Returns new view default size and one new empty panel with default
     * name.
     */
    View createDefaultView() const;

    /**
     * Return the reference to the current template manager.
     */
    const TemplateManager& templateManager() const;

    /**
     * Add given filename to the list of recent files and update MRU popup.
     */
    void addRecentProject( const QString & filename );

    /**
     * Returns PluginManager
     */
    const PluginManager& pluginManager() const;

    /**
     * Returns ServerLoggingManager
     */
    const ServerLoggingManager& serverLoggingManager() const;

protected:

    /**
     * Changes the operation mode of all open project windows.
     */
    void setOperationModeOfPWs( EditablePanelView::OperationMode m );

protected slots:

    /**
     * Shows the Editor Preferences Dialog.
     */
    void showPreferencesSlot();

    /**
     * TODO
     */
    void projectWindowActivatedSlot( ProjectWindow* pw );

    /**
     * TODO
     */
    void showTemplateLibrariesDialogSlot();

    /**
     * TODO
     */
    void projectWindowClosedSlot( QObject* o );

    /**
     * TODO
     */
    void MRUProjectOpenSlot( int id );

    /**
     * TODO
     */
    void templateLibraryChanged( const QString& libraryName );

signals:

    /**
     * TODO
     */
    void projectChanged( ProjectWindow*, CommandType );

protected:

    class MainWindowProtected;
    MainWindowProtected* d;
};

#endif /* _LTEDITORMAINWINDOW_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: MainWindow.hh 1564 2006-04-07 13:33:15Z modesto $
