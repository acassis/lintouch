// $Id:PanelZoomSelector.cpp
//
//   FILE: PanelZoomSelector.cpp --
// AUTHOR: shivaji <shivaji@swac.cz>
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

#include "PanelZoomSelector.hh"
#include "MainWindow.hh"

#include <qwidget.h>
#include <qregexp.h>
#include <qvalidator.h>
#include <qlineedit.h> 
#include <qlistbox.h>

#define ZC_25         "25%"
#define ZC_50         "50%"
#define ZC_100        "100%"
#define ZC_200        "200%"
#define ZC_400        "400%"
#define SIZE_LIMIT    10
#define DEFAULT_COEF  100

PanelZoomSelector::PanelZoomSelector( QWidget * parent,
        const char * name):QComboBox( TRUE, parent, name ) {

    setSizeLimit( SIZE_LIMIT );
    setDuplicatesEnabled( FALSE );
    QRegExp rx( "[1-9][0-9]{0,4}[%]{0,1}" );
    QValidator *validator = new QRegExpValidator( rx, this );
    setValidator( validator );
    setAutoCompletion ( TRUE );

    QLineEdit* lnEdit = lineEdit();
    connect( lnEdit, SIGNAL( lostFocus() ), this,
            SLOT( focusOutSlot() ) );
    connect( lnEdit, SIGNAL( returnPressed() ), this,
            SLOT( pressedReturnSlot() ) );

    QStringList lstEmptyList;
    setZoomCoefficientList( lstEmptyList );
}

PanelZoomSelector::~PanelZoomSelector()
{
    //TODO
}

void PanelZoomSelector::
        setZoomCoefficientList( const QStringList & zoomCoefficients )
{
    QListBox *lstBox = listBox();
    Q_ASSERT( lstBox );

    clear();

    QStringList lstZoomcoefficients;
    lstZoomcoefficients << ZC_25 << ZC_50 << ZC_100 << ZC_200 << ZC_400;
    insertStringList( lstZoomcoefficients );

    if( zoomCoefficients.isEmpty() ) {
        setCurrentItem( -1 );
        return;
    }

    for( QStringList::const_iterator it = zoomCoefficients.begin();
            it != zoomCoefficients.end(); ++it ) {
        QString strZoomCoefficient = *it;
        int zoomCoeff = zoomCoefficient( strZoomCoefficient );

        if( zoomCoeff > 0 ) {
            QString strZoomCoef = 
                    QString( "%1%2" ).arg( zoomCoeff ).arg( "%" );

            if( !lstBox->findItem( strZoomCoef ) ) {
                lstBox->insertItem( strZoomCoef );
            }
        }
    }

    sortCoefficientList();
}

int PanelZoomSelector::
        zoomCoefficientList( QStringList & zoomCoefficientsList)
{
    zoomCoefficientsList.clear();
    QListBox *lstBox = listBox();
    Q_ASSERT( lstBox );

    QListBoxItem* lstItem = lstBox->firstItem();

    while ( lstItem ) {
        QString zoomCoeff = lstItem->text();
        zoomCoefficientsList.append( zoomCoeff );
        lstItem = lstItem->next();
   }

    return zoomCoefficientsList.count();
}

int PanelZoomSelector::zoomCoefficient() const
{
    int zoomCoeff;

    QString selCoeff = currentText();
    zoomCoeff = zoomCoefficient( selCoeff );

    return ( ( zoomCoeff >= 1)  ? zoomCoeff : DEFAULT_COEF );
}

void PanelZoomSelector::setZoomCoefficient( int zoomCoefficient )
{
    QString strZoom;

    if ( zoomCoefficient <= 0 ) {
        return;
    }

    strZoom = QString( "%1%2" ).arg( zoomCoefficient ).arg( "%" );
    setZoomCoefficient( strZoom );
}

void PanelZoomSelector::focusOutSlot()
{
    QString selCoeff = currentText();

    if ( selCoeff.isEmpty() ) {
        selCoeff = QString("%1").arg( DEFAULT_COEF );
    }

    if( !selCoeff.contains('%') ) {
        selCoeff.append( '%' );
    }

    setZoomCoefficient( selCoeff );
}

void PanelZoomSelector::pressedReturnSlot()
{
    QString selCoeff = currentText();
    QListBox *lstBox = listBox();
    Q_ASSERT( lstBox );

    QListBoxItem* lstBoxItem = lstBox->findItem( selCoeff );
    int indexOfItem = lstBox->index( lstBoxItem );

    if( !selCoeff.contains('%') ) {
        selCoeff.append( '%' );

        if( !lstBox->findItem( selCoeff ) ) {
            lstBox->changeItem( selCoeff, indexOfItem );
            sortCoefficientList();
            setCurrentText( selCoeff );
        } else {
            lstBox->removeItem( indexOfItem );
        }
    } else {

        if( !lstBoxItem->next() ) {
            sortCoefficientList();
            setCurrentText( selCoeff );
        }
    }
}

void PanelZoomSelector::
        setZoomCoefficient( QString zoomCoefficient )
{
    QListBox *lstBox = listBox();
    Q_ASSERT( lstBox );

    if( !lstBox->findItem( zoomCoefficient ) ) {
        lstBox->insertItem( zoomCoefficient );
        sortCoefficientList();
    }

    setCurrentText( zoomCoefficient );
}

int PanelZoomSelector::
        zoomCoefficient( QString strZoomCoeff ) const
{
    bool validZoomCoeff = false;
    int zoomCoeff = -1;

    if( !strZoomCoeff.isEmpty() ) {
        int indexPercentage = strZoomCoeff.find( "%" );
        QString selCoeff;

        if( indexPercentage > 0 ) {
            selCoeff = strZoomCoeff.left( indexPercentage );
        } else {
            selCoeff = strZoomCoeff;
        }

        bool Ok;
        int iZoomCoeff = selCoeff.toInt( &Ok, 10 );

        if( Ok ) {
            zoomCoeff  =  iZoomCoeff;
            validZoomCoeff = true;
        }
    }

    if( !validZoomCoeff ) {
        qWarning( tr( "Invalid Zoom Coefficient" ) );
    }

    return zoomCoeff;
}

void PanelZoomSelector::sortCoefficientList()
{
    QStringList lstZoomCoefficient;
    zoomCoefficientList( lstZoomCoefficient );

    int zoomCoeffcount = lstZoomCoefficient.count();

    for( int i = 0; i < zoomCoeffcount -1; i++ ) {

        for( int j = 0; j < zoomCoeffcount - i - 1; j++ ) {
            int coeffNumberFirst = zoomCoefficient(
                    lstZoomCoefficient[ j ] );
            int coeffNumberNext = zoomCoefficient(
                    lstZoomCoefficient[ j + 1 ] );

            if( coeffNumberFirst > 0 && coeffNumberNext > 0 &&
                    ( coeffNumberFirst > coeffNumberNext ) ) {
                QString tempKey = lstZoomCoefficient[ j ];
                lstZoomCoefficient[ j ] = lstZoomCoefficient[ j + 1 ];
                lstZoomCoefficient[ j + 1 ] = tempKey;
            }
        }
    }

    clear();
    insertStringList( lstZoomCoefficient );
    setCurrentItem( -1 );

}

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelZoomSelector.cpp 1169 2005-12-12 15:22:15Z modesto $

