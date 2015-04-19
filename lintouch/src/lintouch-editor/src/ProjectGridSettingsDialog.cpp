// $Id: ProjectGridSettingsDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectGridSettingsDialog.cpp --
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

#include "ProjectGridSettingsDialog.hh"
#include "ProjectWindow.hh"
#include "MainWindow.hh"

#include <qcolordialog.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>

#include <EditorProject.hh>
using namespace lt;

struct ProjectGridSettingsDialog::
        ProjectGridSettingsPrivate {
    ProjectGridSettingsPrivate() : pw( NULL ) {
    }

    ~ProjectGridSettingsPrivate() {
    }

    /**
    * This points to the currently focussed project window
    */
    ProjectWindow* pw;
};

ProjectGridSettingsDialog::ProjectGridSettingsDialog(
        MainWindow* parent, const char* name /*= 0*/,
        WFlags fl /*= Qt::WType_TopLevel*/ ) :
    ProjectGridSettingsDialogBase( parent, name, fl ),
    d( new ProjectGridSettingsPrivate() )
{
    Q_CHECK_PTR( d );
    connect( m_applyButton, SIGNAL( clicked() ),
            this, SLOT( applySlot() ) );
    connect( m_okButton, SIGNAL( clicked() ),
            this, SLOT(okSlot()) );
}

ProjectGridSettingsDialog::~ProjectGridSettingsDialog()
{
    delete d;
}

void ProjectGridSettingsDialog::projectChangedSlot(
        ProjectWindow* pw, CommandType t )
{
    Q_ASSERT(d);

    if( pw != d->pw || t == LTE_CMD_NOOP ||
            t == LTE_CMD_GRID_PROPS_CHANGE ) {
        bool bEnable = (pw != NULL );
        showGrid->setEnabled( bEnable );
        snapToGrid->setEnabled( bEnable );
        x_size->setEnabled( bEnable );
        y_size->setEnabled( bEnable );
        majorLine->setEnabled( bEnable );
        gridColorButton->setEnabled( bEnable );
        m_applyButton->setEnabled( bEnable );
        m_okButton->setEnabled( bEnable );

        lblGridX->setEnabled( bEnable );
        lblGridY->setEnabled( bEnable );
        lblHighlight->setEnabled( bEnable );
        lblGridColor->setEnabled( bEnable );

        d->pw = pw;

        if( pw ) {
            setCaption(
                    QString( tr("%1 Grid Settings - Lintouch Editor") ).
                    arg(pw->projectFileInfo().baseName() ) );

            showGrid->setChecked( pw->gridVisible() );
            snapToGrid->setChecked( pw->gridSnap() ); 
            x_size->setValue( pw->gridXSpacing() );
            y_size->setValue( pw->gridYSpacing() );
            majorLine->setValue( pw->gridHighlight() );
            gridColorButton->setPaletteBackgroundColor( pw->gridColor() );
        } else {
            setCaption( tr("Grid Settings - Lintouch Editor") );
            d->pw = NULL;
            showGrid->setChecked( false );
            snapToGrid->setChecked( false );
            x_size->setValue( 1 );
            y_size->setValue( 1 );
            majorLine->setValue( 2 );
            gridColorButton->setPaletteBackgroundColor( Qt::lightGray );
        }
    }
}

void ProjectGridSettingsDialog::applySlot()
{
    Q_ASSERT(d);

    d->pw->setGridVisible( showGrid->isChecked() );
    d->pw->setGridSnap( snapToGrid->isChecked() );
    d->pw->setGridXSpacing( x_size->value() );
    d->pw->setGridYSpacing( y_size->value() );
    d->pw->setGridHighlight( majorLine->value() );
    d->pw->setGridColor( gridColorButton->paletteBackgroundColor() );
}

void ProjectGridSettingsDialog::okSlot()
{
    applySlot();
    close();
}

void ProjectGridSettingsDialog::gridColorSlot()
{
    QColor newColor = QColorDialog::getColor(
            gridColorButton->paletteBackgroundColor(), this );

    if( ( newColor.isValid() ) &&
            ( gridColorButton->paletteBackgroundColor() != newColor ) ) {
        gridColorButton->setPaletteBackgroundColor( newColor );
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectGridSettingsDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
