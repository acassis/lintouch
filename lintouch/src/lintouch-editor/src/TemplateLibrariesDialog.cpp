// $Id: TemplateLibrariesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplateLibrariesDialog.cpp -- 
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

#include <qpushbutton.h>
#include <qlistview.h>
#include <qpixmap.h>

#include <lt/templates/TemplateLibrary.hh>

#include "ListViewToolTip.hh"
#include "TemplateLibrariesDialog.hh"

using namespace lt;

enum {
    COL_LIBLOADED = 0, COL_LIBNAME, COL_LIBDESC,
    COL_LIBLOCATION, MAX_COL
};

struct TemplateLibrariesDialog::
    TemplateLibrariesDialogPrivate{

    TemplateLibrariesDialogPrivate()
    :cellToolTip( NULL ){
        for(int c=0; c<MAX_COL; c++){
            maxContentsWidth[ c ] = 0;
        }
    }
    ~TemplateLibrariesDialogPrivate(){

        if (cellToolTip){
            delete cellToolTip;
        }
    }
    void setMaxContentsWidth( int c, int w )
    {
        if ( maxContentsWidth[c] < w ){
            maxContentsWidth[c] = w;
        }
    }
    //Tooltip
    ListViewToolTip* cellToolTip;
    //Stores the column wise maximum contents width
    int maxContentsWidth[ MAX_COL ];
};

TemplateLibrariesDialog::TemplateLibrariesDialog(
        const TemplateManager & tm, QWidget* parent,
        const char* name /*= 0*/, WFlags fl /*= Qt::WType_TopLevel*/ ):
        TemplateLibrariesDialogBase( parent, name, fl ),
        d( new TemplateLibrariesDialogPrivate() )
{
    Q_CHECK_PTR( d );

    d->cellToolTip = new ListViewToolTip( projectLibraries );

    //set alignment for column 'Loaded'
    projectLibraries->setColumnAlignment( COL_LIBLOADED, Qt::AlignCenter );

    projectLibraries->setColumnWidthMode( COL_LIBLOADED,
            QListView::Manual );
    projectLibraries->setColumnWidthMode( COL_LIBNAME, QListView::Manual );
    projectLibraries->setColumnWidthMode( COL_LIBDESC, QListView::Manual );
    projectLibraries->setColumnWidthMode( COL_LIBLOCATION,
            QListView::Manual );

    fillProjectLibrariesInfo( tm );
}

TemplateLibrariesDialog::~TemplateLibrariesDialog()
{
    delete d;
}

void TemplateLibrariesDialog::fillProjectLibrariesInfo(
        const TemplateManager & tm )
{
    TemplateLibraryDict libraries = tm.templateLibraries();

    for (unsigned i=0; i<libraries.count(); ++i ) {
        //Libary loaded
        //QPixmap pm = QPixmap::fromMimeSource(
        //    (i.current()->isValid())?"src/images/yes.png": 
        //    "src/images/no.png" );
        if( libraries.keyAt( i ) == "mytemplatelibrary" ) continue;
        QString pm = ( (libraries[i]->isValid() && libraries[i]->isAvailable() ) ?
                tr("yes") : tr("no") );
        QListViewItem* rowItem = new QListViewItem( projectLibraries );
        rowItem->setText( COL_LIBLOADED, pm );
        //Name
        rowItem->setText( COL_LIBNAME, libraries.keyAt(i) );
        d->setMaxContentsWidth( COL_LIBNAME, rowItem->width(
                projectLibraries->fontMetrics(), projectLibraries,
                COL_LIBNAME ) );

        //Description
        rowItem->setText( COL_LIBDESC,
                libraries[i]->info().shortDescription() );
        d->setMaxContentsWidth( COL_LIBDESC, rowItem->width(
                projectLibraries->fontMetrics(), projectLibraries,
                COL_LIBDESC ) );

        //Location
        rowItem->setText( COL_LIBLOCATION, libraries[i]->filename() );
        d->setMaxContentsWidth( COL_LIBLOCATION, rowItem->width(
                projectLibraries->fontMetrics(), projectLibraries,
                COL_LIBLOCATION ) );

        projectLibraries->insertItem( rowItem );
    }
}

int TemplateLibrariesDialog::minimumColumnWidth( int col )const
{
    Q_ASSERT( col >= COL_LIBLOADED && col <= COL_LIBLOCATION );

    return pixelWidthOf( projectLibraries->columnText( col ) ) +
        2*projectLibraries->itemMargin();
}

int TemplateLibrariesDialog::allowedColumnWidth( int col ) const
{
    Q_ASSERT( col >= COL_LIBLOADED && col <= COL_LIBLOCATION );

    int allowed = 0;
    int totalWidth = projectLibraries->viewport()->width();
    switch ( col ) {
    case COL_LIBLOADED:
        allowed = minimumColumnWidth( col );
        break;
    case COL_LIBNAME:
        allowed = totalWidth - ( knownColumnWidth( COL_LIBLOADED ) +
                minimumColumnWidth( COL_LIBDESC ) +
                minimumColumnWidth( COL_LIBLOCATION ) );

        if ( allowed > d->maxContentsWidth[ COL_LIBNAME ] ){
            allowed = d->maxContentsWidth[ COL_LIBNAME ];
        }

        if ( allowed < minimumColumnWidth( COL_LIBNAME ) ){
            allowed = minimumColumnWidth( COL_LIBNAME );
        }
        break;
    case COL_LIBDESC:
        allowed = totalWidth - ( knownColumnWidth( COL_LIBLOADED ) +
                knownColumnWidth( COL_LIBNAME ) +
                minimumColumnWidth( COL_LIBLOCATION ) );

        if ( allowed > d->maxContentsWidth[ COL_LIBDESC ] ){
            allowed = d->maxContentsWidth[ COL_LIBDESC ];
        }

        if ( allowed < minimumColumnWidth( COL_LIBDESC ) ){
            allowed = minimumColumnWidth( COL_LIBDESC );
        }
        break;
    case COL_LIBLOCATION:
        allowed = totalWidth - ( knownColumnWidth( COL_LIBLOADED ) +
                knownColumnWidth( COL_LIBNAME ) +
                knownColumnWidth( COL_LIBDESC ) );

        if ( allowed < minimumColumnWidth( COL_LIBLOCATION ) ){
            allowed = minimumColumnWidth( COL_LIBLOCATION );
        }
        break;
    }
    return allowed;
}

int TemplateLibrariesDialog::knownColumnWidth( int col ) const
{
    Q_ASSERT( col >= COL_LIBLOADED && col <= COL_LIBLOCATION );
    return projectLibraries->columnWidth( col );
}


int TemplateLibrariesDialog::pixelWidthOf(
        const QString& content ) const
{
    return projectLibraries->fontMetrics().width( content );
}

int TemplateLibrariesDialog::pixelWidthOf(
        QChar ch ) const
{
    return projectLibraries->fontMetrics().width( ch );
}

void TemplateLibrariesDialog::setColumnWidths()
{
    int columns[] = { COL_LIBLOADED, COL_LIBNAME, COL_LIBDESC,
            COL_LIBLOCATION };

    for (int c=0; c<MAX_COL; c++){
        projectLibraries->setColumnWidth( columns[c],
                allowedColumnWidth( columns[c] ) );
    }
}

void TemplateLibrariesDialog::resizeEvent( QResizeEvent * )
{
    setColumnWidths();
}

void TemplateLibrariesDialog::showEvent( QShowEvent * )
{
    setColumnWidths();
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateLibrariesDialog.cpp 1169 2005-12-12 15:22:15Z modesto $
