// $Id: ProjectPreviewTable.cpp 1210 2006-01-06 10:17:35Z modesto $
//
//   FILE: ProjectPreviewTable.cpp --
// AUTHOR: shivaji <shivaji@swac.cz>
//   DATE: 19 Aug 2004
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

#include "ProjectPreviewTable.hh"

#include <qlineedit.h>
#include <qstyle.h>
#include <qtabwidget.h>
#include <qevent.h>
using namespace lt;

enum
{
    COL_FIRST, COL_SECOND, COL_BIT,
    COL_NUMBER, COL_WITH_WIDGETS = COL_NUMBER, COL_STRING,
    COL_LAST = COL_STRING, MAX_COL
};

#define     PREVIEW_IOPINS      0x1
#define     PREVIEW_VARIABLES   0x2

struct ProjectPreviewTable::ProjectPreviewTablePrivate{
    ProjectPreviewTablePrivate():sortAscending( true ),
    lastSortColumn( -1 ), previewMode( PREVIEW_IOPINS ){

        for(int c=COL_FIRST; c<=COL_LAST; c++) {
            maxContentsWidth[ c ] = 0;
        }
    }

    ~ProjectPreviewTablePrivate(){
    }

    void setMaxContentsWidth( int c, int w )
    {
        if ( maxContentsWidth[c] < w ) {
            maxContentsWidth[c] = w;
        }
    }

    bool sortAscending;
    int lastSortColumn;
    int maxContentsWidth[ MAX_COL ];
    int previewMode;
};

ProjectPreviewTableItem::ProjectPreviewTableItem( bool shade,
        QTable* table, QTableItem::EditType editType, const QString& content,
        const QString& usrData ) :
    QTableItem( table, editType, content ),
    m_shade(shade), m_usrData(usrData)
{
}

ProjectPreviewTableItem::~ProjectPreviewTableItem()
{
}

void ProjectPreviewTableItem::paint( QPainter * p,
        const QColorGroup &cg, const QRect& cr, bool selected)
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, ( m_shade ) ? cg.light() : cg.base() );
    QTableItem::paint( p, g, cr, selected );
}

QString ProjectPreviewTableItem::getUserData()
{
    return m_usrData;
}

bool ProjectPreviewTableItem::isShade() const
{
    return m_shade;
}

void ProjectPreviewTableItem::swapShade(
        ProjectPreviewTableItem *i )
{
    Q_ASSERT( i );
    if ( m_shade != i->isShade() ){
        bool tmp = m_shade;
        m_shade = i->isShade();
        i->setShade( tmp );
    }
}

void ProjectPreviewTableItem::setShade( bool s )
{
    m_shade = s;
}

ProjectPreviewTable::ProjectPreviewTable(
        QWidget * parent /*= 0*/, const char * name /*= 0*/ ):
        QTable( parent, name ), d (
        new ProjectPreviewTablePrivate() ){
    Q_CHECK_PTR( d );
}

ProjectPreviewTable::ProjectPreviewTable(
        int numRows, int numCols, QWidget * parent /*= 0*/,
        const char * name /*= 0*/) : QTable( numRows, numCols,
        parent, name ), d( new ProjectPreviewTablePrivate() ){
    Q_CHECK_PTR( d );
}

void ProjectPreviewTable::init( int mode )
{
    Q_ASSERT( d );
    d->previewMode = mode;
    //first set the column labels
    if ( d->previewMode == PREVIEW_IOPINS ){
        setNumCols( numCols() + 1 );
        horizontalHeader()->setLabel( numCols() - 1,
                tr( "Template" ) );
        setNumCols( numCols() + 1 );
        horizontalHeader()->setLabel( numCols() - 1,
                tr( "IOPin" ) );
    } else if ( d->previewMode == PREVIEW_VARIABLES ){
        setNumCols( numCols() + 1 );
        horizontalHeader()->setLabel( numCols() - 1,
                tr( "Connection" ) );
        setNumCols( numCols() + 1 );
        horizontalHeader()->setLabel( numCols() - 1,
                tr( "Variable" ) );
    }
    
    setNumCols( numCols() + 1 );
    horizontalHeader()->setLabel( numCols() - 1,
            tr( "Bit" ) );
    setNumCols( numCols() + 1 );
    horizontalHeader()->setLabel( numCols() - 1,
            tr( "Number" ) );
    setNumCols( numCols() + 1 );
    horizontalHeader()->setLabel( numCols() - 1,
            tr( "String" ) );
    //set the default properties
    setNumRows( 0 );
    setNumCols( MAX_COL );
    setSelectionMode( QTable::NoSelection );
    setFocusStyle( QTable::FollowStyle );
    setSorting( true );

    //hide the vertical header
    verticalHeader()->hide();
    setLeftMargin( 0 );
}

ProjectPreviewTable::~ProjectPreviewTable()
{
    delete d;
}

QString ProjectPreviewTable::getUserData( int row, int col )
{
    Q_ASSERT( row >= 0 && row < numRows() );
    Q_ASSERT( col >= 0 && col < numCols() );

    return ((ProjectPreviewTableItem*)item( row, col
            ))->getUserData();
}

void ProjectPreviewTable::sortColumn( int col,
    bool , bool )
{
    Q_ASSERT( d );

    if ( col >= COL_BIT && col <= COL_STRING ){
        return;
    }

    d->sortAscending = !d->sortAscending;
    QTable::sortColumn( col, d->sortAscending, true );

    adjustCellWidgetsTabOrder();

    d->lastSortColumn = col;
}

void ProjectPreviewTable::swapRows( int row1, int row2,
        bool swapHeader /*= false*/ )
{
    Q_ASSERT( d );
    Q_ASSERT( sorting() );

    //Swap caption( used to store r*numCols()+c )
    QString caption1[COL_WITH_WIDGETS];
    QString caption2[COL_WITH_WIDGETS];
    int column[]={ COL_BIT, COL_NUMBER, COL_STRING };
    int i;

    for ( i = 0; i<COL_WITH_WIDGETS; i++){
        QWidget* w1 = cellWidget( row1, column[i] ),
               * w2 = cellWidget( row2, column[i] );

        if (  w1 &&  w2 ){
            caption1[i] = w1->caption();
            caption2[i] = w2->caption();

            //now swap
            w1->setCaption( caption2[i] );
            w2->setCaption( caption1[i] );

        }
    }

    //Swap shade(s) if required
    for ( int c=0; c<numCols(); c++ ){
        ProjectPreviewTableItem* i1 =
                (ProjectPreviewTableItem*)item( row1, c );
        ProjectPreviewTableItem* i2 =
                (ProjectPreviewTableItem*)item( row2, c );
        if ( i1 && i2 ){
            i1->swapShade( i2 );
        }
    }

    QTable::swapRows( row1, row2, swapHeader );
}

void ProjectPreviewTable::setCellWidget( int r, int c, QWidget* w )
{
    Q_ASSERT( d );
    Q_ASSERT( w );

    QTable::setCellWidget( r, c, w );
    QString caption = QString("%1").arg( indexOf( r, c ) );
    w->setCaption( caption );
    d->setMaxContentsWidth( c, w->sizeHint().width() );
}

void ProjectPreviewTable::setItem(int row, int col,
        QTableItem * item)
{
    QTable::setItem( row, col, item );
    d->setMaxContentsWidth( col, item->sizeHint().width() );
}

void ProjectPreviewTable::adjustCellWidgetsTabOrder()
{
    QWidget *last = NULL, *current = NULL;
    for( int r = 0; r < numRows(); r++) {
        for ( int c = COL_BIT; c <= COL_STRING; c++ ){
            current = cellWidget( r, c );
            if ( current ){
                if ( last ){
                    setTabOrder( last, current );
                }
                last = current;
            }
        }
    }
}

void ProjectPreviewTable::adjustToLastSortingOrder()
{
    Q_ASSERT( d );
    if ( d->lastSortColumn > -1 && d->lastSortColumn < numCols() ){
        QTable::sortColumn( d->lastSortColumn, d->sortAscending, true );
        adjustCellWidgetsTabOrder();
    }
}

void ProjectPreviewTable::setColumnWidths()
{
    for (int c=COL_FIRST; c<=COL_LAST; c++){
        setColumnWidth( c, allowedColumnWidth( c ) );
    }
}

int ProjectPreviewTable::minimumColumnWidth( int col ) const
{
    Q_ASSERT( col >= COL_FIRST && col <= COL_LAST );

    return pixelWidthOf( horizontalHeader()->label( col ) )
            + 2*horizontalHeader()->style().pixelMetric(
            QStyle::PM_HeaderMargin );
}

int ProjectPreviewTable::allowedColumnWidth( int col ) const
{
    Q_ASSERT( d );
    Q_ASSERT( col >= COL_FIRST && col <= COL_LAST );

    int allowed = 0;
    int totalWidth = width() -
        ( ( ( verticalScrollBar()->isVisible() )?
            verticalScrollBar()->width() : 0 ) + 4/*FIXME*/);

    switch ( col ) {
    case COL_FIRST:
        allowed = totalWidth - ( minimumColumnWidth( COL_SECOND ) +
                minimumColumnWidth( COL_BIT ) +
                minimumColumnWidth( COL_NUMBER ) +
                minimumColumnWidth( COL_STRING) );
        break;
    case COL_SECOND:
        allowed = totalWidth - ( knownColumnWidth( COL_FIRST ) +
                minimumColumnWidth( COL_BIT ) +
                minimumColumnWidth( COL_NUMBER ) +
                minimumColumnWidth( COL_STRING) );
        break;
    case COL_BIT:
        allowed = totalWidth - ( knownColumnWidth( COL_FIRST ) +
                knownColumnWidth( COL_SECOND ) +
                minimumColumnWidth( COL_NUMBER ) +
                minimumColumnWidth( COL_STRING) );
        break;
    case COL_NUMBER:
        allowed = totalWidth - ( knownColumnWidth( COL_FIRST ) +
                knownColumnWidth( COL_SECOND ) +
                knownColumnWidth( COL_BIT ) +
                minimumColumnWidth( COL_STRING) );
        break;
    case COL_STRING:
        allowed = totalWidth - ( knownColumnWidth( COL_FIRST ) +
                knownColumnWidth( COL_SECOND ) +
                knownColumnWidth( COL_BIT ) +
                knownColumnWidth( COL_NUMBER) );
        break;
    }

    if ( allowed > d->maxContentsWidth[ col ] ){
        allowed = d->maxContentsWidth[ col ];
    }

    if ( allowed < minimumColumnWidth( col ) ){
        allowed = minimumColumnWidth( col );
    }
    return allowed;
}

int ProjectPreviewTable::knownColumnWidth( int col ) const
{
   Q_ASSERT( col >= COL_FIRST && col <= COL_LAST );

    return columnWidth( col );
}

int ProjectPreviewTable::pixelWidthOf( const QString& content )const
{
    return fontMetrics().width( content );
}

int ProjectPreviewTable::mode()
{
    Q_ASSERT( d );
    return d->previewMode;
}

void ProjectPreviewTable::keyPressEvent( QKeyEvent* e )
{
    if ( e->key() == Key_Enter || e->key() == Key_Return ){
        e->ignore();
        return;
    }
    QTable::keyPressEvent( e );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ProjectPreviewTable.cpp 1210 2006-01-06 10:17:35Z modesto $

