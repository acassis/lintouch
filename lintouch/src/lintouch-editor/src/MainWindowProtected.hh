// $Id: MainWindowProtected.hh 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: MainWindowProtected.hh --
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

#ifndef _MAINWINDOWPROTECTED_HH
#define _MAINWINDOWPROTECTED_HH

#include <MainWindow.hh>
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
#include "ServerLoggingManager.hh"
#include "ProjectServerLoggingDialog.hh"

struct MainWindow::MainWindowProtected
{
    MainWindowProtected() :
        pluginManager(NULL), slManager(NULL),
        mode( EditablePanelView::Modify ), copyPasteBuffer( NULL ),
        prefs( NULL ), quitDialog(NULL),
        projectFilesCount( 0 ), lastProjectWindow(NULL),
        projectViewsDialog( NULL ),
        projectPanelsDialog( NULL ), projectTemplatesDialog( NULL ),
        projectPropertiesDialog( NULL ), templatePropertiesDialog( NULL ),
        projectGridSettingsDialog( NULL ), projectPreviewDialog( NULL ),
        templateLibrariesDialog( NULL ), projectResourcesDialog( NULL ),
        helpWindow( NULL ), aboutDialog( NULL ),
        MRUPopup(NULL), templatePalette( NULL ),
        templateLibrarySelectionCombo( NULL ), projectConnections(NULL),
        projectVariables(NULL), projectServerLogging(NULL)
    {
        projectWindowsList.setAutoDelete(false);
    }

    QString newProjectName() {
        // generic EditorProject name
        return QString(tr("LintouchProject%1.ltp")).arg( ++projectFilesCount );
    }

    /**
     * Create new ProjectWindow. Takes care of all connects and
     * counter stuff.
     * \return New ProjectWindow ready for show
     */
    ProjectWindow* newProjectWindow( QWidget* parent, MainWindow* mw,
            EditorProject* prj, double zoom ) {

        ProjectWindow* prw =
            new ProjectWindow( prj, parent, mw, zoom, logger );
        Q_CHECK_PTR(prw);

        // append to the PW list
        projectWindowsList.append( prw );

        // connect signals and slots!
        connect( prw,
                SIGNAL(projectWindowActivated(ProjectWindow*) ),
                mw,
                SLOT(projectWindowActivatedSlot(ProjectWindow*) ) );
        connect( mw, SIGNAL( projectChanged(
                        ProjectWindow*,CommandType)),
                prw, SLOT( projectChangedSlot(
                        ProjectWindow*,CommandType))
                );
        // connect to know when the PW is destroyed
        connect( prw, SIGNAL(destroyed(QObject*)),
                mw, SLOT(projectWindowClosedSlot(QObject*)) );

        return prw;
    }

    /**
     * Find a place in a popup and put new MRU popup there. The place in the
     * popup is defined as two separators one just after another.
     */
    void placeMRUPopup( MainWindow* parent, QPopupMenu* fileMenu ) {
        Q_ASSERT( fileMenu );

        bool lastWasSeparator=false;
        unsigned int i;
        for( i=0; i < fileMenu->count(); ++i ) {
            if( fileMenu->text(fileMenu->idAt(i)).isNull() ) {
                if( lastWasSeparator ) {
                    break;
                }
                lastWasSeparator=true;
            }
        }
        if( i == fileMenu->count() ) {
            qWarning( "Didn't find a place for MRU popup!" );
            return;
        }
        MRUPopup = new QPopupMenu( fileMenu );
        Q_CHECK_PTR( MRUPopup );
        connect( MRUPopup, SIGNAL(activated(int)),
                parent, SLOT(MRUProjectOpenSlot(int)) );

        fileMenu->insertItem( tr("Open &Recent"), MRUPopup, -1, ++i );

        updateMRUPopup();
    }

    /**
     * Updates content of MRUPopup popup. Must be called after
     * placeMRUPopup() which creates the MRUPopup.
     */
    void updateMRUPopup()
    {
        Q_ASSERT( MRUPopup );

        MRUPopup->clear();

        QStringList l = prefs->recentProjects();
        int i = 1;
        for( QStringList::const_iterator it = l.constBegin();
                it != l.constEnd(); ++it, ++i ) {

            if( i < 10 ) {
                MRUPopup->insertItem( QString( "&%1 %2" ).arg(i).arg(*it), i );
            } else {
                MRUPopup->insertItem( *it, i );
            }
        }
    }

    /**
     * TODO
     */
     LoggerPtr logger;

    /**
     * TODO
     */
    TemplateManager templateManager;

    /**
     * Holds PluginManager.
     */
    PluginManager *pluginManager;

    /**
     * Holds ServerLoggingManager
     */
    ServerLoggingManager* slManager;

    /**
     * Operation mode of the editor.
     */
    EditablePanelView::OperationMode mode;

    /**
     * Copy/Paste buffer.
     */
    CopyPasteBuffer * copyPasteBuffer;

    /**
     * Preferences Dialog.
     */
    PreferencesDialog * prefs;

    /**
     * TODO
     */
    QMessageBox* quitDialog;

    /**
     * TODO
     */
    int projectFilesCount;

    /**
     * TODO
     */
    QPtrList< ProjectWindow > projectWindowsList;

    /**
     * Pointer to the the last focused PW
     */
    ProjectWindow* lastProjectWindow;

    /**
     * TODO
     */
    ProjectViewsDialog* projectViewsDialog;

    /**
     * TODO
     */
    ProjectPanelsDialog* projectPanelsDialog;

    /**
     * TODO
     */
    ProjectTemplatesDialog* projectTemplatesDialog;

    /**
     * TODO
     */
    ProjectPropertiesDialog* projectPropertiesDialog;

    /**
     * TODO
     */
    TemplatePropertiesDialog* templatePropertiesDialog;
    /**
     * TOOD
     */
    ProjectGridSettingsDialog* projectGridSettingsDialog;

    /**
     * TODO
     */
    ProjectPreviewDialog* projectPreviewDialog;

    /**
     * TODO
     */
    TemplateLibrariesDialog* templateLibrariesDialog;

    /**
     * TODO
     */
    ProjectResourcesDialog* projectResourcesDialog;

    /**
     * Help window.
     */
    HelpWindow * helpWindow;

    /**
     * About dialog.
     */
    AboutDialog * aboutDialog;

    /**
     * TODO
     */
    QPopupMenu* MRUPopup;

    /**
     * TODO
     */
    TemplatePalette* templatePalette;

    /**
     * TODO
     */
    TemplateLibrarySelectionCombo* templateLibrarySelectionCombo;

    /**
     * Connections Dialog holder.
     */
    ProjectConnectionsDialog* projectConnections;

    /**
     * Connections Dialog holder.
     */
    ProjectVariablesDialog* projectVariables;

    /**
     * Connections Dialog holder.
     */
    ProjectServerLoggingDialog* projectServerLogging;

    /**
     * List of project filenames given at the command line.
     */
    QStringList prjs;
};

#endif

// vim: set et ts=4 sw=4 tw=76:
// $Id: MainWindowProtected.hh 1564 2006-04-07 13:33:15Z modesto $
