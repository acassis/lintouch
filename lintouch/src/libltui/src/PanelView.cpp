// $Id: PanelView.cpp,v 1.38 2004/12/01 17:41:16 mman Exp $
//
//   FILE: PanelView.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 09 October 2003
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

#include "lt/ui/PanelView.hh"
using namespace lt;

struct PanelView::PanelViewPrivate
{

    PanelViewPrivate() :
        canvas( new QCanvas() ),
        frame( new QCanvasRectangle( NULL ) ),
        onTop( NULL ),
        lockAspect( false ), autoFit( true ),
        width( 100 ), height( 100 ),
        virtualWidth( 100 ), virtualHeight( 100 )
    {
    }

    ~PanelViewPrivate()
    {
        delete frame;
        delete canvas;
    }

    QCanvas * canvas;

    QCanvasRectangle * frame;

    lt::Panel panel;

    Template * onTop;

    bool lockAspect;
    bool autoFit;

    // real coordinate system
    int width;
    int height;

    // virtual coordinate system
    int virtualWidth;
    int virtualHeight;

    // transformation from virtual to real coordinate system
    QWMatrix matrix;
    QWMatrix imatrix;

    // should we animate
    bool animated;

    // shortcut map
    LTMap <Template*> shortcutMap;

    // focus map
    LTMap <Template*> focusMap;
};

PanelView::PanelView( QWidget * parent, const char * name,
        WFlags f, bool animated /*= true */ ) :
    QCanvasView( parent, name, f ),
    d( new PanelViewPrivate )
{
    // use our canvas
    setCanvas( d->canvas );

    // update scrollbar state
    setAutoFit( d->autoFit );

    d->animated = animated;

    setFocusPolicy( StrongFocus );
}

PanelView::~PanelView()
{
    // remove all templates
    clear();

    //disable animation
    d->canvas->setAdvancePeriod( -1 );

    delete d;
}

void PanelView::setSize( const QSize & size )
{
    if(d->width == size.width() && d->height == size.height()) {
        return;
    }

    d->width = size.width();
    d->height = size.height();

    // allow canvas to be adopted
    updateTransformationMatrix();
    resizeItems();
    resizeCanvas();
    resizeFrame();
}

QSize PanelView::size() const
{
    return QSize( d->width, d->height );
}

const QWMatrix & PanelView::matrix() const
{
    return d->matrix;
}

const QWMatrix & PanelView::inverseMatrix() const
{
    return d->imatrix;
}

void PanelView::setVirtualSize( const QSize & size )
{
    if(d->virtualWidth == size.width() && d->virtualHeight == size.height()) {
        return;
    }

    d->virtualWidth = size.width();
    d->virtualHeight = size.height();

    // allow canvas to be adopted
    updateTransformationMatrix();
    resizeItems();
}

QSize PanelView::virtualSize() const
{
    return QSize( d->virtualWidth, d->virtualHeight );
}

void PanelView::setAnimated( bool animated )
{
    if( d->animated != animated ) {
        d->animated = animated;
        if( animated ) {
            canvas()->setAdvancePeriod( 200 );
        } else {
            canvas()->setAdvancePeriod( -1 );
        }
    }
}

bool PanelView::animated() const
{
    return d->animated;
}

void PanelView::setProjectPanel( const lt::Panel & panel,
                    ViewAdaptor * va )
{
    clear();

    d->panel = panel;

    TemplateDict td = panel.templates();
    for( unsigned i = 0; i < td.count(); i ++ ) {
        td[ i ]->setViewAdaptor( va );
        td[ i ]->setCanvas( canvas() );
        // place the template correctly within the current coordinate system
        resizeTemplate( td[ i ] );
        td[ i ]->show();
        td[ i ]->propertiesChanged();
        td[ i ]->iopinsChanged();
    }

    updateFocusMap();
    enterFocusMode();
}

const lt::Panel & PanelView::projectPanel() const
{
    return d->panel;
}

const TemplateDict & PanelView::templates() const
{
    return d->panel.templates();
}

TemplateDict & PanelView::templates()
{
    return d->panel.templates();
}

bool PanelView::lockAspect() const
{
    return d->lockAspect;
}

void PanelView::setLockAspect( bool lock )
{
    d->lockAspect = lock;

    if( d->autoFit ) {
        fitVisible();
    }
}

bool PanelView::autoFit() const
{
    return d->autoFit;
}

void PanelView::setAutoFit( bool autoFit )
{
    d->autoFit = autoFit;
    if( autoFit ) {
        setHScrollBarMode( AlwaysOff );
        setVScrollBarMode( AlwaysOff );

    } else {
        setHScrollBarMode( AlwaysOn );
        setVScrollBarMode( AlwaysOn );
    }
}

void PanelView::fitVisible()
{
    // we set our new size to the available area
    d->width = visibleWidth();
    d->height = visibleHeight();

    // if aspect locking is enabled we update the new size accordingly
    if( d->lockAspect ) {
        double v_w2h = (double) d->virtualWidth / (double) d->virtualHeight;
        double r_w2h = (double) d->width / (double) d->height;

        //qWarning( "ratio virtual = %f, ration real = %f",
        //        v_w2h, r_w2h );

        if( v_w2h > r_w2h ) {
            // update vertical dimmension
            d->height = (unsigned)( d->width / v_w2h );
        } else {
            // update horizontal dimmension
            d->width = (unsigned)( d->height * v_w2h );
        }
    }

    //qWarning( "after lockaspect: new width is %d, new height %d",
    //        d->width, d->height );

    updateTransformationMatrix();
    resizeItems();
    resizeCanvas();
    resizeFrame();
}

void PanelView::resizeEvent( QResizeEvent * e )
{
    QCanvasView::resizeEvent( e );

    //qWarning( "virtual size is %d x %d, real size is %d x %d, "
    //        "canvas size is %d x %d, "
    //        "contents rect is %d, %d, %d x %d",
    //        d->virtualWidth, d->virtualHeight,
    //        d->width, d->height,
    //        canvas()->width(), canvas()->height(),
    //        contentsRect().x(), contentsRect().y(),
    //        contentsRect().width(), contentsRect().height() );

    if( d->autoFit ) {
        fitVisible();
    }

    if(isVisible()) {
        resizeFrame();
    }
}

void PanelView::showEvent( QShowEvent * e )
{
    // center panel in the available area of canvas view
    resizeFrame();

    if( d->animated ) {
        //auto animate every 200 miliseconds
        //qWarning( "setting advance period" );
        canvas()->setAdvancePeriod( 200 );

    }

    QCanvasView::showEvent( e );
}

void PanelView::hideEvent( QHideEvent *e )
{
    if( d->animated ) {
        //disable animation
        //qWarning( "resetting advance period" );
        canvas()->setAdvancePeriod( -1 );
    }

    QCanvasView::hideEvent( e );
}

void PanelView::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
    //qWarning( "PanelView::contentsMouseDoubleClickEvent: %d, %d",
    //        e->pos().x(), e->pos().y() );
    Template * i = NULL;
    if(( i = findItemOnTop( e ) ) != NULL ) {
        // forward mouse event to the template
        i->mouseDoubleClickEvent( e );
        // shedule canvas for repaint
        canvas()->update();
    }
}

void PanelView::contentsMouseMoveEvent( QMouseEvent *e )
{
    //qWarning( "PanelView::contentsMouseMoveEvent: %d, %d",
    //        e->pos().x(), e->pos().y() );
    if( d->onTop != NULL ) {
        // create a QMouseEvent with inversed pos; we need thins because the
        // canvas is now centered inside the PVC...
        QMouseEvent pe(e->type(), inverseWorldMatrix().map( e->pos() ),
                e->globalPos(), e->button(), e->state());

        // forward mouse event to the template
        d->onTop->mouseMoveEvent( &pe );
        // shedule canvas for repaint
        canvas()->update();

        if(pe.isAccepted()) {
            e->accept();
        }
    }
}

void PanelView::contentsMousePressEvent( QMouseEvent *e )
{
    // qWarning( "PanelView::contentsMousePressEvent: %d, %d",
    //         e->pos().x(), e->pos().y());

    Template * i = NULL;
    if(( i = findItemOnTop( e ) ) != NULL ) {
        // create a QMouseEvent with inversed pos; we need thins because the
        // canvas is now centered inside the PVC...
        QMouseEvent pe(e->type(), inverseWorldMatrix().map( e->pos() ),
                e->globalPos(), e->button(), e->state());

        // forward mouse event to the template
        i->mousePressEvent( &pe );
        d->onTop = i;
        // shedule canvas for repaint
        canvas()->update();

        if(pe.isAccepted()) {
            e->accept();
        }
    }
}

void PanelView::contentsMouseReleaseEvent( QMouseEvent *e )
{
    //qWarning( "PanelView::contentsMouseReleaseEvent: %d, %d",
    //        e->pos().x(), e->pos().y() );
    if( d->onTop != NULL ) {
        // create a QMouseEvent with inversed pos; we need thins because the
        // canvas is now centered inside the PVC...
        QMouseEvent pe(e->type(), inverseWorldMatrix().map( e->pos() ),
                e->globalPos(), e->button(), e->state());

        // forward mouse event to the template
        d->onTop->mouseReleaseEvent( &pe );
        d->onTop = NULL;
        // shedule canvas for repaint
        canvas()->update();

        if(pe.isAccepted()) {
            e->accept();
        }
    }
}

void PanelView::keyPressEvent( QKeyEvent * e )
{
    //qWarning( "PanelView::keyPressEvent( e->key = %d )",
    //        e->key() );

    // focus switching
    if( e->key() == Qt::Key_Tab && d->focusMap.count() > 1 ) {

        int curr = d->focusMap.currentIndex();
        {
            QFocusEvent fe( QEvent::FocusOut );
            fe.setReason( QFocusEvent::Tab );
            d->focusMap.current()->focusOutEvent( &fe );
        }
        curr ++;
        if( curr >= (int)d->focusMap.count() ) {
            d->focusMap.setCurrentIndex( 0 );
        } else {
            d->focusMap.setCurrentIndex( curr );
        }
        {
            QFocusEvent fe( QEvent::FocusIn );
            fe.setReason( QFocusEvent::Tab );
            d->focusMap.current()->focusInEvent( &fe );
        }
        e->accept();
        canvas()->update();
        return;
    }

    // shortcut handling
    QString seq = Template::keyEvent2Sequence( e );
    if( d->shortcutMap.contains( seq ) ) {
        d->shortcutMap[ seq ]->keyPressEvent( e );
        canvas()->update();
        return;
    }

    // ordinary key events
    if( d->focusMap.currentIndex() != -1 ) {
        d->focusMap.current()->keyPressEvent( e );
        canvas()->update();
        return;
    }

}

void PanelView::keyReleaseEvent( QKeyEvent * e )
{
    //qWarning( "PanelView::keyReleaseEvent( e->key = %d )",
    //        e->key() );

    QString seq = Template::keyEvent2Sequence( e );
    if( d->shortcutMap.contains( seq ) ) {
        d->shortcutMap[ seq ]->keyReleaseEvent( e );
        canvas()->update();
        return;
    }

    // ordinary key events
    if( d->focusMap.currentIndex() != -1 ) {
        d->focusMap.current()->keyReleaseEvent( e );
        canvas()->update();
    }
}

void PanelView::clear()
{
    TemplateDict& td = d->panel.templates();
    for( unsigned i = 0; i < td.count(); i ++ ) {
        td[ i ]->setViewAdaptor( NULL );
        td[ i ]->setCanvas( NULL );
    }
    canvas()->update();
}

void PanelView::updateFocusMap()
{
    // find the templates that can accept focus
    d->focusMap.clear();
    d->shortcutMap.clear();
    TemplateDict& td = d->panel.templates();
    for( unsigned i = 0; i < td.count(); i ++ ) {
        if( td[ i ]->focusPolicy() == QWidget::StrongFocus ) {
            d->focusMap.insert( td.keyAt( i ), td[ i ] );
        }
        // remember in a shortcut map
        if( ! QString( td[ i ]->shortcut() ).isEmpty() ) {
            d->shortcutMap.insert( td[ i ]->shortcut(), td[ i ] );
        }
    }
}

void PanelView::enterFocusMode()
{
    // focus first template
    if( d->focusMap.count() > 0 ) {
        d->focusMap.setCurrentIndex( 0 );
        {
            QFocusEvent fe( QEvent::FocusIn );
            fe.setReason( QFocusEvent::Tab );
            d->focusMap.current()->focusInEvent( &fe );
        }
        // canvas()->update();
    }
}

void PanelView::clearFocus()
{
    QFocusEvent fe( QEvent::FocusOut );
    fe.setReason( QFocusEvent::Tab );

    for(unsigned i=0; i<d->focusMap.count(); ++i) {
        d->focusMap[i]->focusOutEvent( &fe );
    }
}

static bool _CheckItemState( Template * item, 
        int state )
{
#undef unless
#define unless(a) if(!(a))

    // do enabled match
    unless((( state & PanelView::ItemEnabled ) && item->isEnabled() ) ||
       (( !( state & PanelView::ItemEnabled ) 
            /* && !item->isEnabled() */ ) ) )
        return false;

    // do selected match
    unless((( state & PanelView::ItemSelected ) && item->isSelected() ) ||
       (( !( state & PanelView::ItemSelected ) 
            /* && !item->isSelected() */ ) ) )
        return false;

    return true;
}

void PanelView::updateTransformationMatrix()
{
    // compute scaling matrix
    double sx =( d->width ) / ( double ) d->virtualWidth;
    double sy =( d->height ) / ( double ) d->virtualHeight;
    d->matrix = QWMatrix();
    d->matrix.scale( sx, sy );

    d->imatrix = d->matrix.invert();
}

void PanelView::resizeTemplate( Template * t )
{
    Q_ASSERT( t != NULL );

    QRect virt = t->virtualRect();
    QRect newrect = d->matrix.mapRect( virt );

    //qWarning( "moving template with virtual rect %d, %d, %d x %d"
    //        " to the new position %d, %d, %d x %d",
    //        virt.x(), virt.y(), virt.width(), virt.height(),
    //        newrect.x(), newrect.y(), newrect.width(), newrect.height() );

    t->setRect( newrect );
}

void PanelView::resizeItems()
{
    // resize all templates
    TemplateDict& td = d->panel.templates();
    for( unsigned i = 0; i < td.count(); i ++ ) {

        resizeTemplate( td[ i ] );
    }

    // canvas()->update();
}

void PanelView::resizeCanvas()
{
    // resize canvas
    canvas()->resize( d->width, d->height );
    // need not to call update here, it's updated automatically
    // canvas()->update();
}

void PanelView::resizeFrame()
{
    //qWarning( "visible size is %d x %d, our size is %d x %d",
    //        visibleWidth(),
    //        visibleHeight(),
    //        size().width(),
    //        size().height() );

    int movex = 0;
    int movey = 0;

    if( visibleWidth() > contentsWidth() ) {
        movex = ( visibleWidth() - contentsWidth() ) / 2;
    }

    if( visibleHeight() > contentsHeight() ) {
        movey = ( visibleHeight() - contentsHeight() ) / 2;
    }

    QWMatrix m;
    m.translate( movex, movey );
    setWorldMatrix( m );

    // update frame size
    d->frame->move( 0, 0 );
#ifdef Q_WS_X11
    d->frame->setSize( contentsWidth() + 1, contentsHeight() + 1 );
#else
    d->frame->setSize( contentsWidth(), contentsHeight() );
#endif
    d->frame->setZ( 100000 );
    if( ! d->frame->isVisible() ) {
        d->frame->setPen( QPen( black ) );
        d->frame->setCanvas( canvas() );
        d->frame->show();
    }
    // canvas()->update();
}

#undef SIZE_IS_SMALLER
#define SIZE_IS_SMALLER(a,b) \
((a).width() <(b).width() && (a).height() <(b).height())

Template * PanelView::findItemOnTop( QMouseEvent * e,
        int flags, bool exact )
{

    bool found = false;
    Template * result = NULL;
    QSize min;
    double zmax = 0;
    bool zfound = false;

    // find items under the mouse pointer
    QPoint p = inverseWorldMatrix().map( e->pos() );

    QCanvasItemList l;
    if( ! exact ) {
        // do non-exact collision search with small rectangle around mouse
        // cursor
        QRect r( p.x() - 2, p.y() - 2, 4, 4 );
        l = canvas()->collisions( r );
    } else {
        l = canvas()->collisions( p );
    }

    //qWarning( "going to find items at %d, %d", p.x(), p.y() );
    for( QCanvasItemList::const_iterator i =
            const_cast<QCanvasItemList&>(l).begin();
            i != const_cast<QCanvasItemList&>(l).end(); ++i ) {
        //qWarning( "found QCanvasItem with rtti: %d",(*i)->rtti() );
        // find our items
        if(((*i)->rtti() == Template::RTTI ) &&
                // do the flags match
                _CheckItemState(( Template * )(*i), flags ) ) {

            // if the first item or in the same or higher level
            if( ! found || ( !zfound ) || ( ( zmax <= (*i)->z() ) ) ) {
                // if in the same layer and not smaller, just skip to the
                // next one
                if( zfound && zmax == (*i)->z() &&
                    ! SIZE_IS_SMALLER((*i)->boundingRect().size(), min ) ) {
                    continue;
                }
                //qWarning( "found item 0x%x at layer %d",
                //        (unsigned)(*i), (int)(*i)->z() );

                found = true;
                zfound = true;
                result = ( Template * )(*i);
                min = (*i)->boundingRect().size();
                zmax = (*i)->z();

            }
        }
    }
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: PanelView.cpp,v 1.38 2004/12/01 17:41:16 mman Exp $
