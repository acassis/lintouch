// $Id: PreferencesDialog.cpp 1212 2006-01-06 13:06:55Z modesto $
//
//   FILE: PreferencesDialog.cpp -- 
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

#include "PreferencesDialog.hh"
#include "PanelZoomSelector.hh"
#include "MRU.hh"

#include <qsettings.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qsize.h>
#include <qcolordialog.h>
#include <qpushbutton.h>
#include <qstringlist.h>

#define DOMAIN_NAME         "swac.de"
#define PRODUCT_NAME        "lintouch-editor"

#define GROUP_UI            "/ui"

#ifdef WE_CAN_DO_ON_TOP
#define DLG_ON_TOP          "/dialogs_on_top"
#endif

#ifdef QTUNDO_SUPPORTS_LIMIT
#define UNDO_LEVEL          "/undo_level"
#endif

#define RECENTLY_OPENED     "/recently_opened"

#define GROUP_GRID          "/grid"
#define GRID_X_SPACING      "/grid_x_spacing"
#define GRID_Y_SPACING      "/grid_y_spacing"
#define GRID_HIGHLIGHT      "/grid_highlight"
#define GRID_COLOR          "/grid_color"

#define GROUP_PROJECT       "/project"
#define PRJ_AUTHOR          "/author"
#define PRJ_VIEW_WIDTH      "/view_width"
#define PRJ_VIEW_HEIGHT     "/view_height"
#define PRJ_ZOOM_LEVEL      "/zoom"
#define PRJ_SHOW_GRID       "/show_grid"
#define PRJ_SNAP_TO_GRID    "/snap_to_grid"

#define MRU_PROJECT_LISTING "/recently_opened"
#define MRU_FILE            "/file"
#define FRONT_SLASH         "/"

#define DLG_ON_TOP_DEFAULT         0
#define UNDO_LEVEL_DEFAULT        30
#define RECENTLY_OPENED_DEFAULT   10

#define GRID_X_SPACING_DEFAULT    20
#define GRID_Y_SPACING_DEFAULT    20
#define GRID_HIGHLIGHT_DEFAULT    10
#define GRID_COLOR_DEFAULT     "lightgray"


#define PRJ_ZOOM_LEVEL_DEFAULT    100
#define PRJ_SHOW_GRID_DEFAULT       1
#define PRJ_SNAP_TO_GRID_DEFAULT    0

enum{ VIEW_DEFAULT_WIDTH = 800, VIEW_DEFAULT_HEIGHT = 600 };

struct PreferencesDialog::PreferenceDialogPrivate {

    PreferenceDialogPrivate() {
    }
    ~PreferenceDialogPrivate() {
    }

    /**
     * This stores the list of MRU and saves the recently opened file path
     * to the disc
     */
    MRU EditorMRU;
};

PreferencesDialog::
        PreferencesDialog( QWidget* parent /*= 0*/,
                const char* name /*= 0*/, bool modal /*= false*/,
                WFlags fl /*= 0*/ ) :
    PreferencesDialogBase( parent, name, modal, fl ),
    d( new PreferenceDialogPrivate() )
{
    loadSettings();
}

PreferencesDialog::~PreferencesDialog()
{
    delete d;
}

#ifdef WE_CAN_DO_ON_TOP
bool PreferencesDialog::dialogsOnTop() const
{
    return toolsOnTop->isChecked();
}
#endif

#ifdef QTUNDO_SUPPORTS_LIMIT
int PreferencesDialog::undoLevels() const
{
    return undoRedoLevel->value();
}
#endif

int PreferencesDialog::recentFiles() const
{
    int value = recentlyOpened->value();
    return value;
}

int PreferencesDialog::gridXSpacing() const
{
    return x_size->value();
}

int PreferencesDialog::gridYSpacing() const
{
    return y_size->value();
}

int PreferencesDialog::gridHighlight() const
{
    return majorLine->value();
}

QColor PreferencesDialog::gridColor() const
{
    return gridColorButton->paletteBackgroundColor();
}

QString PreferencesDialog::projectAuthor() const
{
    return author->text();
}

QSize PreferencesDialog::projectViewSize() const
{
    return QSize( viewWidth->value(), viewHeight->value() );
}

bool PreferencesDialog::projectShowGrid() const
{
    return showGrid->isChecked();
}

bool PreferencesDialog::projectSnapToGrid() const
{
    return snapToGrid->isChecked();
}

int PreferencesDialog::projectZoomLevel() const
{
    return zoomLevel->zoomCoefficient();
}

void PreferencesDialog::
        addRecentProject( const QString & filename )
{
    d->EditorMRU.addRecentProject( filename );
}

QStringList PreferencesDialog::recentProjects() const
{
    return d->EditorMRU.recentProjects();
}


void PreferencesDialog::saveSettings()
{
    QSettings settings;
    settings.setPath( DOMAIN_NAME, PRODUCT_NAME );

    QString topGroup = QString( "%1%2").arg( FRONT_SLASH ).
            arg( PRODUCT_NAME );
    settings.beginGroup( topGroup );

    //Saving UI settings
    settings.beginGroup ( GROUP_UI );
#ifdef WE_CAN_DO_ON_TOP
    settings.writeEntry( DLG_ON_TOP, dialogsOnTop() );
#endif
#ifdef QTUNDO_SUPPORTS_LIMIT
    settings.writeEntry( UNDO_LEVEL, undoLevels() );
#endif
    settings.writeEntry( RECENTLY_OPENED, recentFiles() );
    settings.endGroup();

    //Saving Grid settings
    settings.beginGroup ( GROUP_GRID );
    settings.writeEntry( GRID_X_SPACING, gridXSpacing() );
    settings.writeEntry( GRID_Y_SPACING, gridYSpacing() );
    settings.writeEntry( GRID_HIGHLIGHT, gridHighlight() );
    settings.writeEntry( GRID_COLOR, gridColor().name() );
    settings.endGroup();

    //Saving Project defaults
    settings.beginGroup ( GROUP_PROJECT );
    settings.writeEntry( PRJ_AUTHOR, projectAuthor() );
    QSize szView = projectViewSize();
    settings.writeEntry( PRJ_VIEW_WIDTH, szView.width() );
    settings.writeEntry( PRJ_VIEW_HEIGHT, szView.height() );
    settings.writeEntry( PRJ_ZOOM_LEVEL, projectZoomLevel() );
    settings.writeEntry( PRJ_SHOW_GRID, projectShowGrid() );
    settings.writeEntry( PRJ_SNAP_TO_GRID, projectSnapToGrid() );
    settings.endGroup();

    d->EditorMRU.setRecentFiles( recentFiles() );

    settings.endGroup();

    emit( preferencesChanged() );
}

void PreferencesDialog::loadSettings()
{
    QSettings settings;
    //load value of settings to the controls
    settings.setPath( DOMAIN_NAME, PRODUCT_NAME );

    QString topGroup = QString( "%1%2")
            .arg( FRONT_SLASH )
            .arg( PRODUCT_NAME );
    settings.beginGroup( topGroup );

    //UI settings
    settings.beginGroup( GROUP_UI );
#ifdef WE_CAN_DO_ON_TOP
    toolsOnTop->setChecked( settings.readBoolEntry( DLG_ON_TOP,
            DLG_ON_TOP_DEFAULT ) );
#endif
#ifdef QTUNDO_SUPPORTS_LIMIT
    undoRedoLevel->setValue( settings.readNumEntry( UNDO_LEVEL,
            UNDO_LEVEL_DEFAULT) );
#endif
    recentlyOpened->setValue( settings.readNumEntry( RECENTLY_OPENED,
            RECENTLY_OPENED_DEFAULT ) );
    settings.endGroup();

    //Grid settings
    settings.beginGroup( GROUP_GRID );
    x_size->setValue( settings.readNumEntry( GRID_X_SPACING,
            GRID_X_SPACING_DEFAULT ) );
    y_size->setValue( settings.readNumEntry( GRID_Y_SPACING,
            GRID_Y_SPACING_DEFAULT ) );
    majorLine->setValue( settings.readNumEntry( GRID_HIGHLIGHT,
            GRID_HIGHLIGHT_DEFAULT ) );
    QColor gridColor;
    gridColor.setNamedColor( settings.readEntry( GRID_COLOR,
            GRID_COLOR_DEFAULT ) );
    gridColorButton->setPaletteBackgroundColor( gridColor );
    settings.endGroup();

    //Project default settings
    settings.beginGroup( GROUP_PROJECT );
    author->setText( settings.readEntry( PRJ_AUTHOR ) );
    viewWidth->setValue( settings.readNumEntry( PRJ_VIEW_WIDTH,
            VIEW_DEFAULT_WIDTH ) );
    viewHeight->setValue( settings.readNumEntry( PRJ_VIEW_HEIGHT,
            VIEW_DEFAULT_HEIGHT ) );
    zoomLevel->setZoomCoefficient( settings.readNumEntry( PRJ_ZOOM_LEVEL,
            PRJ_ZOOM_LEVEL_DEFAULT ) );
    showGrid->setChecked( settings.readBoolEntry( PRJ_SHOW_GRID,
            PRJ_SHOW_GRID_DEFAULT ) );
    snapToGrid->setChecked( settings.readBoolEntry( PRJ_SNAP_TO_GRID,
            PRJ_SNAP_TO_GRID_DEFAULT ) );
    settings.endGroup();

    //Project MRU listing
    d->EditorMRU.clearList();
    d->EditorMRU.setRecentFiles( recentlyOpened->value() );
    d->EditorMRU.setPath( DOMAIN_NAME, PRODUCT_NAME );
    d->EditorMRU.setTopGroup( topGroup );
    d->EditorMRU.setEntryList( MRU_PROJECT_LISTING );
    d->EditorMRU.setSubKey( MRU_FILE );

    QStringList keys = settings.entryList( MRU_PROJECT_LISTING );
    sortKeys( keys );
    settings.beginGroup( MRU_PROJECT_LISTING );

    for ( QStringList::iterator i = keys.begin();
            i != keys.end(); ++i ){
        QString key = /*QString( MRU_PROJECT_LISTING ) + */
                QString( FRONT_SLASH ) + *i;
        QString value = settings.readEntry( key );

        if ( !value.isEmpty() ) {
            addRecentProject( value );
        } else {
            settings.removeEntry( key );
        }
    }
    settings.endGroup();

    d->EditorMRU.setLoadingComplete();

    settings.endGroup();
}

void PreferencesDialog::setGridColor()
{
    QColor newColor = QColorDialog::getColor(
     gridColorButton->paletteBackgroundColor(), this );

    if( ( newColor.isValid() ) &&
            ( gridColorButton->paletteBackgroundColor() != newColor ) ) {
        gridColorButton->setPaletteBackgroundColor( newColor );
    }
}

void PreferencesDialog::sortKeys( QStringList& lstKeys )
{
    int keyCount = lstKeys.count();

    for( int i = 0; i < keyCount -1; i++ ) {

        for( int j = 0; j < keyCount - i - 1; j++ ) {
            int fileNumberFirst = getFileNumber( lstKeys[ j ] );
            int fileNumberNext = getFileNumber( lstKeys[ j + 1 ] );

            if( fileNumberFirst > 0 && fileNumberNext > 0 &&
                    ( fileNumberFirst < fileNumberNext ) ) {
                QString tempKey = lstKeys[ j ];
                lstKeys[ j ] = lstKeys[ j + 1 ];
                lstKeys[ j + 1 ] = tempKey;
            }
        }
    }
}

int PreferencesDialog::getFileNumber ( QString key )
{
    bool ok;
    int iFileNameLength = QString(MRU_FILE).length() - 1;

    QString strFileNumber = key.mid( iFileNameLength );
    int iFileNumber = strFileNumber.toInt( &ok );

    if( ok ) {
        return iFileNumber;
    }
    else {
        return -1;
    }

}

// vim: set et ts=4 sw=4 tw=76:
// $Id: PreferencesDialog.cpp 1212 2006-01-06 13:06:55Z modesto $
