// $Id: ListViewToolTip.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ListViewToolTip.cpp --
// AUTHOR: shivaji <shivaji@swac.cz>
//   DATE: 20 July 2004
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

#include "ListViewToolTip.hh"

#include <qstring.h>
#include <qrect.h>
#include <qlistview.h>


struct ListViewToolTip::ListViewToolTipPrivate{
    ListViewToolTipPrivate( QListView* lv ){
        listView = lv;
    }

    ~ListViewToolTipPrivate(){
    }
    //The list view
    QListView* listView;
};

ListViewToolTip::ListViewToolTip( QListView* lv,
        QToolTipGroup * group ):QToolTip( lv->viewport(), group ),
        d ( new ListViewToolTipPrivate( lv ) ){

}
void ListViewToolTip::maybeTip( const QPoint &p )
{
    QPoint cellPt = d->listView->viewportToContents( p );
    QListViewItem* i = d->listView->itemAt( cellPt );
   
    if ( i ){
        QRect irc = d->listView->itemRect( i ); //with respect to viewport
        //now find the column position
        int col = -1, 
        left = d->listView->viewportToContents( irc.topLeft() ).x(),
                right = 0;

        for(int c=0; c < d->listView->columns(); c++){
            right = left + d->listView->columnWidth( c );

            if ( ( cellPt.x() > left ) && ( cellPt.x() < right ) ){
                col = c;
                break;
            }
            left = right;
        }

        if ( col == -1 ){
            return;
        }

        QString contents = i->text( col );

        if ( !contents.isEmpty()
            && ( i->width( d->listView->fontMetrics(), d->listView, col ) >
                d->listView->columnWidth( col ) ) ) {

            QRect cellRect( left, irc.top(), right, irc.bottom() );

            cellRect.moveTopLeft( d->listView->contentsToViewport(
                    cellRect.topLeft() ) );

            tip( cellRect, contents  );

        }else{

            hide();

        }
    }
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: ListViewToolTip.cpp 1169 2005-12-12 15:22:15Z modesto $

