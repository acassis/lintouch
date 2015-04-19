// $Id: EditablePanelViewContainer.cpp 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: EditablePanelViewContainer.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 07 January 2004
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

#include <qpopupmenu.h>

#include "EditablePanelView.hh"
#include "EditablePanelViewContainer.hh"
using namespace lt;

struct EditablePanelViewContainer::EditablePanelViewContainerPrivate
{

    EditablePanelViewContainerPrivate() :
        menu( NULL ), mode( EditablePanelView::Modify ),
        gridColor( black ), gridXSpacing( 10 ), gridYSpacing( 10 ),
        gridHighlight( 5 ), gridVisible( 0 ), gridSnap( 0 )
    {
    }

    //view we are displaying
    View view;
    //right mouse click menu
    QPopupMenu * menu;
    //operation mode
   EditablePanelView::OperationMode mode;

   /**
    * Grid color for this project window
    */
   QColor gridColor;

   /**
    * Horizontal Grid spacing.
    */
   unsigned gridXSpacing;

   /**
    * Vertical Grid spacing.
    */
   unsigned gridYSpacing;

   /**
    * Hilight every Nth grid line.
    */
   unsigned gridHighlight;

   /**
    * Should the grid be painted?
    */
   bool gridVisible;

   /**
    * Should we snap to grid?
    */
   bool gridSnap;
};

EditablePanelViewContainer::EditablePanelViewContainer( QWidget * parent,
        const char * name, WFlags f ) :
    QTabWidget( parent, name, f ),
    d( new EditablePanelViewContainerPrivate )
{
    connect( this, SIGNAL( currentChanged( QWidget * ) ),
            this, SLOT( currentChangedSlot( QWidget * ) ) );
}

EditablePanelViewContainer::~EditablePanelViewContainer()
{
    delete d;
}

View EditablePanelViewContainer::projectView() const
{
    return d->view;
}

void EditablePanelViewContainer::setProjectView( const View & pView,
        double zoom /* = 1.0 */,
        const QString & /* startPanel = QString::null */ )
{
    bool signals_blocked = signalsBlocked();

    // don't emit signals while we are reconstructing the tab widget
    blockSignals( true );

    //first remove all EditablePanelViews from the tab widget
    while( currentPage() ) {
        QWidget * x = NULL;
        removePage( x = currentPage() );
        delete x;
    }

    //now set the view we will be using from now on
    d->view = pView;

    const PanelMap& panels = d->view.panels();

    for( unsigned i = 0; i < panels.count(); i ++ ) {
        //build given panel, show it and emit appropriate signals
        addPanelView( panels.keyAt( i ), zoom );
    }

    // enable signals again
    blockSignals( signals_blocked );

    // if the view has no panels, emit empty string as a current panel name
    // otherwise re-show the curr page to emit proper signals.
    if( panels.count() == 0 ) {
        emit( panelDisplayed( QString::null ) );
    } else {
        showPage( currentPage() );
        emit( panelDisplayed( tabLabel( currentPage() ) ) );
    }
}

void EditablePanelViewContainer::addPanelView( const QString & panel,
        double zoom /* = 1.0 */, int index /* = -1 */ )
{
    // qWarning(">>> EditablePanelViewContainer::addPanelView(%s)",
    //         panel.latin1());

    if( ! d->view.panels().contains( panel ) ) return;

    lt::Panel p = d->view.panels()[ panel ];

    // construct PanelView with me as parent
    EditablePanelView* pv = new EditablePanelView( this );
    Q_CHECK_PTR( pv );

    QSize s( d->view.virtualWidth(), d->view.virtualHeight() );

    pv->setAutoFit( false );
    pv->setLockAspect( true );
    pv->setVirtualSize( s );
    pv->setSize( QSize(
                (unsigned)(s.width() * zoom),
                (unsigned)(s.height() * zoom) ) );

    pv->setProjectPanel( p, this );

    // grid related
    pv->setGridVisible( false );    // !!?!!
    pv->setGridColor( d->gridColor );
    pv->setGridXYSpacing( d->gridXSpacing, d->gridYSpacing );
    pv->setGridHighlight( d->gridHighlight );
    pv->setGridVisible( d->gridVisible );
    pv->setGridSnap( d->gridSnap );

    // propagate mode
    pv->setOperationMode( d->mode );

    // receive important signals from EPV here and forward them further
    connect( pv, SIGNAL( selectionChanged() ),
            this, SIGNAL( selectionChanged() ) );
    connect( pv, SIGNAL( selectionDoubleClicked(QMouseEvent*) ),
            this, SIGNAL( selectionDoubleClicked(QMouseEvent*) ) );
    connect( pv, SIGNAL( selectionMoved( const LTMap <QRect> & ) ),
            this, SIGNAL( selectionMoved( const LTMap <QRect> & ) ) );
    connect( pv, SIGNAL( selectionResized( TemplateDict &,
                    const LTMap <QRect> & ) ),
            this, SIGNAL( selectionResized( TemplateDict &,
                    const LTMap <QRect> & ) ) );
    connect( pv, SIGNAL( templatePlaced( const QRect & ) ),
            this, SIGNAL( templatePlaced( const QRect & ) ) );
    connect( pv, SIGNAL( zoomInRequested( const QPoint & ) ),
            this, SIGNAL( zoomInRequested( const QPoint & ) ) );
    connect( pv, SIGNAL( zoomOutRequested( const QPoint & ) ),
            this, SIGNAL( zoomOutRequested( const QPoint & ) ) );
    connect(pv, SIGNAL(contentsMouseReleaseSignal(QMouseEvent*)),
            this, SIGNAL(contentsMouseReleaseSignal(QMouseEvent*)));

    // give the menu to the PanelView
    pv->setContextMenu( d->menu );

    // now place it on top and notify it's visible
    insertTab( pv, panel, index );

    // qWarning("<<< EditablePanelViewContainer::addPanelView(%s)",
    //         panel.latin1());
}

void EditablePanelViewContainer::renamePanelView( int index,
        const QString & newName, double zoom /*= 1.0*/ )
{
    Q_ASSERT( d->view.panels().contains( newName ) );
    setTabLabel(page(index), newName);
}

void EditablePanelViewContainer::removePanelView( int index )
{
    QWidget * x = page( index );
    if( x != NULL ) {
        removePage( x );
        delete x;
    }
}

void EditablePanelViewContainer::resizePanelViews( double zoom /* = 1.0 */ )
{
    if( currentPage() == NULL ) return;

    // for ordinary resize
    QSize virt( d->view.virtualWidth(), d->view.virtualHeight() );
    QSize real(
            (unsigned)(d->view.virtualWidth() * zoom),
            (unsigned)(d->view.virtualHeight() * zoom) );

    int neww = ((QScrollView*)currentPage())->visibleWidth();
    int newh = ((QScrollView*)currentPage())->visibleHeight();
    // for fit to window
    double v_w2h = (double) d->view.virtualWidth() /
        (double) d->view.virtualHeight();
    double r_w2h = (double) neww / (double) newh;

    if( v_w2h > r_w2h ) {
        // update vertical dimmension
        newh = (unsigned)( neww / v_w2h );
    } else {
        // update horizontal dimmension
        neww = (unsigned)( newh * v_w2h );
    }

    for( int i = 0; i < count(); i ++ ) {
        EditablePanelView * epv = (EditablePanelView*)page( i );

        QSize exVirt = epv->virtualSize();
        if( exVirt != virt ) {
            epv->setVirtualSize( virt );
        }

        if( zoom > 0 ) {
            QSize exReal = epv->size();
            if( exReal != real ) {
                epv->setSize( real );
            }
        } else {
            epv->setSize( QSize( neww, newh ) );
        }

        epv->setGridXYSpacing( d->gridXSpacing, d->gridYSpacing );
    }
    currentPanelView()->update();
}

void EditablePanelViewContainer::setContextMenu( QPopupMenu * menu )
{
    d->menu = menu;
}

EditablePanelView::OperationMode
EditablePanelViewContainer::operationMode() const
{
    return d->mode;
}

void EditablePanelViewContainer::setOperationMode(
        EditablePanelView::OperationMode m )
{
    if( d->mode != m ) {
        d->mode = m;

        // pass mode to all panels
        for( int i = 0; i < count(); i ++ ) {
            EditablePanelView * e = ( EditablePanelView * ) page( i );
            e->setOperationMode( d->mode );
        }
    }
}

void EditablePanelViewContainer::setMagnifyCursor( bool zoom_in )
{
    if( d->mode != EditablePanelView::Magnify ) return;

    // pass mode to all panels
    for( int i = 0; i < count(); i ++ ) {
        EditablePanelView * e = ( EditablePanelView * ) page( i );
        e->setMagnifyCursor( zoom_in );
    }
}

EditablePanelView * EditablePanelViewContainer::currentPanelView()
{
    return ( EditablePanelView* ) currentPage();
}

EditablePanelView * EditablePanelViewContainer::panelView(
        const QString & panel )
{
    int idx = d->view.panels().indexOf( panel );
    if( idx != -1 ) return (EditablePanelView*)page( idx );
    return NULL;
}

void EditablePanelViewContainer::displayPanel( const QString & id )
{
    int idx = d->view.panels().indexOf( id );
    if( idx != -1 ) showPage( page( idx ) );
}

void EditablePanelViewContainer::updatePanelOnTop()
{
    if( currentPage() ) {
        ((EditablePanelView*)currentPage())->canvas()->update();
    }
}

void EditablePanelViewContainer::fullUpdatePanelOnTop()
{
    if( currentPage() ) {
        EditablePanelView* ep =  (EditablePanelView*)currentPage();
        Q_ASSERT(ep);

        TemplateDict& tmpls = ep->templates();
        for(unsigned i = 0; i < tmpls.count(); ++i) {
            tmpls[i]->propertiesChanged();
        }

        updatePanelOnTop();
    }
}

void EditablePanelViewContainer::setGridColor( const QColor & color )
{
    d->gridColor = color;
    for( int i = 0; i < count(); i++ ) {
        EditablePanelView * epv = (EditablePanelView*)page( i );
        epv->setGridColor( color );
    }
    updatePanelOnTop();
}

void EditablePanelViewContainer::setGridXSpacing( unsigned spacing )
{
    d->gridXSpacing = spacing;
    for( int i = 0; i < count(); i++ ) {
        EditablePanelView * epv = (EditablePanelView*)page( i );
        epv->setGridXSpacing( spacing );
    }
    updatePanelOnTop();
}

void EditablePanelViewContainer::setGridYSpacing( unsigned spacing )
{
    d->gridYSpacing = spacing;
    for( int i = 0; i < count(); i++ ) {
        EditablePanelView * epv = (EditablePanelView*)page( i );
        epv->setGridYSpacing( spacing );
    }
    updatePanelOnTop();
}

void EditablePanelViewContainer::setGridHighlight( unsigned n )
{
    d->gridHighlight = n;
    for( int i = 0; i < count(); i++ ) {
        EditablePanelView * epv = (EditablePanelView*)page( i );
        epv->setGridHighlight( n );
    }
    updatePanelOnTop();
}

void EditablePanelViewContainer::setGridVisible( bool on )
{
    d->gridVisible = on;
    for( int i = 0; i < count(); i++ ) {
        EditablePanelView * epv = (EditablePanelView*)page( i );
        epv->setGridVisible( on );
    }
    updatePanelOnTop();
}

void EditablePanelViewContainer::setGridSnap( bool on )
{
    d->gridSnap = on;
    for( int i = 0; i < count(); i++ ) {
        EditablePanelView * epv = (EditablePanelView*)page( i );
        epv->setGridSnap( on );
    }
}

void EditablePanelViewContainer::currentChangedSlot( QWidget * w )
{
    emit( panelDisplayed( tabLabel( w ) ) );
    emit( selectionChanged() );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditablePanelViewContainer.cpp 1774 2006-05-31 13:17:16Z hynek $
