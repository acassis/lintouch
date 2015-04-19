// $Id: EditablePanelView.cpp 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: EditablePanelView.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 17 October 2003
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

#include <qcursor.h>
#include <qpoint.h>
#include <qpopupmenu.h>

#include <lt/LTMap.hh>

#include "lt/templates/TemplateManager.hh"

#include "magnify_minus.xpm"
#include "magnify_plus.xpm"

#include "EditablePanelView.hh"
#include "src/MoveNResizeSelectionCommand.hh"

#include "SelectionHandle.hh"

using namespace lt;

#define QDEBUG_RECT( r ) qDebug(QString(#r": %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

class GridLine : public QCanvasLine {
    public:
        GridLine(QCanvas* c) : QCanvasLine(c) {}
        void update() { QCanvasLine::update(); }
};

struct EditablePanelView::EditablePanelViewPrivate
{
    /**
     * What we are doing when mouse moved handler is invoked
     */
    typedef enum {
        Idle,
        RectSelecting,
        StartMoving,
        Moving,
        StartResizing,
        Resizing,
        Panning,
    } DragMode;

    EditablePanelViewPrivate() :
        mode( EditablePanelView::Modify ), menu( NULL ),
        dragMode( Idle ), va( NULL ),
        gridColor( black ), gridXSpacing( 10 ), gridYSpacing( 10 ),
        gridHighlight( 5 ), gridVisible( false ), gridSnap( 0 ),
        gridShouldBeDrawn( true ), selectionRect( NULL ), panelViewBuild(true),
        hs(NULL)
    {
    }

    virtual ~EditablePanelViewPrivate()
    {
    }

    /**
     * Because we don't have autodelete in LTMap/LTDict we have to delete
     * the items by hand.
     */
    void clearHandles()
    {
        while(handles.count()) {
            clearHandles(handles.keyAt(0));
        }
    }

    /**
     * Delete handles for one Template.
     */
    void clearHandles(const QString& key)
    {
        Q_ASSERT(handles.contains(key));

        SelectionHandleVector sh = handles[key];
        handles.remove(key);

        sh.setAutoDelete(true);
        sh.clear();

        hs = NULL;
    }

    /**
     * Creates handles for given Template and stores the handles in private
     * container.
     */
    void createHandlesFor(const QString& key, QCanvas* c)
    {
        Q_ASSERT(!handles.contains(key));

        SelectionHandle* h = NULL;

        Q_ASSERT(selection.contains(key));
        Template* t = selection[key];
        Q_ASSERT(t);

        SelectionHandleVector sh(8);
        sh.setAutoDelete(false);

        for(int i=0; i<8; ++i) {
            h = new SelectionHandle(i, t, c);
            Q_CHECK_PTR(h);
            sh.insert(i, h);
        }
        handles.insert(key, sh);
    }

    /**
     * Set handles of given Template active or not.
     */
    void setHandlesActive(const QString& key, bool b)
    {
        if(!handles.contains(key)) return;

        SelectionHandleVector& sh = handles[key];
        for(unsigned j=0; j<sh.count(); ++j) {
            sh[j]->setActive(b);
        }
    }

    /**
     * Rename key in handles map
     */
    void renameHandlesTemplate(const QString& o, const QString& n)
    {
        Q_ASSERT(handles.contains(o));

        SelectionHandleVector sh = handles[o];
        handles.remove(o);
        handles.insert(n, sh);
    }

    /**
     * Updates all resize handles to new positions
     */
    void resizeHandlesTemplate()
    {
        for(unsigned i=0; i<handles.count(); ++i) {
            resizeHandlesTemplate(handles.keyAt(i));
        }
    }

    /**
     * Updates resize handles of given template to new position
     */
    void resizeHandlesTemplate(const QString& key)
    {
        Q_ASSERT(handles.contains(key));

        SelectionHandleVector& sh = handles[key];
        for(unsigned j=0; j<sh.count(); ++j) {
            sh[j]->updatePosition();
        }
    }

    // mode of this panel view
    EditablePanelView::OperationMode mode;

    // currently selected templates
    TemplateDict selection;

    // popup menu to be displayed on right mouse click
    QPopupMenu * menu;

    // where we started to drag mouse
    QPoint dragStarted;

    // where we are dragging now
    QPoint dragCurrent;

    // what was the last compensation needed to align point to the grid
    QPoint oldCompensation;

    // original positions of templates when the move drag started
    LTMap <QRect> originalRects;

    // resize direction, see SelectionHandle::pos()
    int resizeDir;

    // Template we are resizing
    Template * resizing;

    // Template we are resizing
    QString resizingKey;

    // original position of the template when the resize drag started
    QRect resizingOrigRect;

    // the position where mouse has been "stopped" in resize mode to not
    // resize into negative sizes
    int stopx; int stopy;

    // what we are doing within our mouse moved handler
    DragMode dragMode;

    // our current view
    ViewAdaptor * va;

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
     * Should the grid be visible?
     */
    bool gridVisible;

    /**
     * Should we snap to grid?
     */
    bool gridSnap;

    /**
     * Should the grid be painted (in preview no, regardless of gridVisible)?
     */
    bool gridShouldBeDrawn;

    /**
     * Horizontal grid lines
     */
    QCanvasItemList gridHorizLines;

    /**
     * Vertical grid lines
     */
    QCanvasItemList gridVertLines;

    /**
     * Zoom In mouse cursor.
     */
    QCursor zoomInCursor;

    /**
     * Zoom Out mouse cursor.
     */
    QCursor zoomOutCursor;

    /**
     * Selection rectangle.
     */
    QCanvasRectangle * selectionRect;

    /**
     * Panel to show
     */
    lt::Panel p;

    /**
     * Panel shown at least once flag
     */
    bool panelViewBuild;

    /**
     * Selection hadles map
     */
    LTMap<SelectionHandleVector> handles;

    /**
     * Selection handle that is highlighted because the mouse is pointing at
     * it
     */
    SelectionHandle* hs;
};

EditablePanelView::EditablePanelView( QWidget * parent, const char * name,
        WFlags f ) :
    PanelView( parent, name, f, false ), d( new EditablePanelViewPrivate() )
{
    setAutoFit( false );
    setLockAspect( true );

    QPixmap in( (const char **)magnify_plus_xpm );
    QPixmap out( (const char **)magnify_minus_xpm );
    d->zoomInCursor = QCursor( in );
    d->zoomOutCursor = QCursor( out );

    d->selectionRect = new QCanvasRectangle( canvas() );
    d->selectionRect->setPen( QPen( QColor( Qt::red ), 0, Qt::DotLine ) );
    d->selectionRect->setZ( 200 );

    viewport()->setMouseTracking(true);
}

EditablePanelView::~EditablePanelView()
{
    delete d;
}

EditablePanelView::OperationMode EditablePanelView::operationMode() const
{
    return d->mode;
}

void EditablePanelView::setOperationMode( EditablePanelView::OperationMode m )
{
    d->mode = m;

    // force grid to redraw after the change
    showGrid(false);

    //disable animation by default
    setAnimated( false );

    clearFocus();

    d->gridShouldBeDrawn = true;

    switch( d->mode ) {
        case EditablePanelView::Place:
            setCursor( QCursor( QCursor::CrossCursor ) );
            unselectAllTemplates();
            break;
        case EditablePanelView::Magnify:
            setCursor( d->zoomInCursor );
            unselectAllTemplates();
            break;
        case EditablePanelView::Pan:
            setCursor( QCursor( QCursor::SizeAllCursor ) );
            unselectAllTemplates();
            break;
        case EditablePanelView::Preview:
            d->gridShouldBeDrawn = false;
            setCursor( QCursor( QCursor::ArrowCursor ) );
            unselectAllTemplates();
            //enable animation for preview
            setAnimated( true );
            //enter focus mode
            updateFocusMap();
            enterFocusMode();
            break;
        default:
            setCursor( QCursor( QCursor::ArrowCursor ) );
            break;
    }
    showGrid( d->gridVisible );
    if(d->panelViewBuild) {
        canvas()->update();
    }
}

void EditablePanelView::setMagnifyCursor( bool zoom_in )
{
    if( d->mode != EditablePanelView::Magnify ) return;

    if( zoom_in ) {
        setCursor( d->zoomInCursor );
    } else {
        setCursor( d->zoomOutCursor );
    }
}

void EditablePanelView::setContextMenu( QPopupMenu * menu )
{
    d->menu = menu;
}

void EditablePanelView::setProjectPanel( const lt::Panel & panel,
        ViewAdaptor * va )
{
    Q_ASSERT(d);

    // remember the panel that we actualy show when the QShowEvent arrives
    d->p = panel;
    d->panelViewBuild = false; // schedule PanelView build

    // remember view adaptor for newly added templates
    d->va = va;
}

void EditablePanelView::addTemplate( const QString & key )
{
    //qWarning( "EditablePanelView::addTemplate " + key );
    TemplateDict & td = templates();
    Q_ASSERT( td.contains( key ) );
    if( td.contains( key ) ) {
        //qWarning( "have it...adding and showing" );
        Template * t = td[ key ];

        Q_ASSERT( t != NULL );

        t->setViewAdaptor( d->va );
        t->setCanvas( canvas() );
        t->setEnabled( true );
        resizeTemplate( t );
        t->show();
        canvas()->update();
    }
}

void EditablePanelView::removeTemplate( const QString & key )
{
    bool selection_changed = false;
    //qWarning( "EditablePanelView::removeTemplate " + key );
    TemplateDict & td = templates();
    Q_ASSERT( td.contains( key ) );
    if( td.contains( key ) ) {
        Template * t = td[ key ];
        //qWarning( "have it...removing and hiding 0x%x", (unsigned) t );

        // remove from selection as well
        if(d->selection.contains(key)) {
            d->selection.remove(key);
            d->clearHandles(key);
            selection_changed = true;
        }

        t->setViewAdaptor( NULL );
        t->setEnabled( false );
        t->hide();
        t->setCanvas( NULL );
        canvas()->setChanged(t->rect());
        canvas()->update();

        if( selection_changed ) {
            emit( selectionChanged() );
        }
    }
}

void EditablePanelView::selectAllTemplates()
{
    //qWarning( "EditablePanelView::selectAllTemplates" );
    //qWarning( "having %d templates in selection", d->selection.count() );
    TemplateDict & tmpls = templates();

    // only select all if they are not already selected
    if( d->selection.count() == tmpls.count() ) return;

    d->selection.clear();
    d->clearHandles();

    for( unsigned i = 0; i < tmpls.count(); i++ ) {
        if(!tmpls[i]->isEnabled()) continue;
        tmpls[ i ]->setSelected( true );
        d->selection.insert( tmpls.keyAt( i ), tmpls[ i ] );
        d->createHandlesFor(tmpls.keyAt(i), canvas());
    }

    if( d->selection.current() ) {
        d->setHandlesActive(d->selection.currentKey(), true);
    }

    canvas()->update();
    emit( selectionChanged() );
}

void EditablePanelView::unselectAllTemplates()
{
    // qWarning( "EditablePanelView::unselectAllTemplates" );
    // qWarning( "having %d templates in selection", d->selection.count() );
    TemplateDict & tmpls = templates();

    for( unsigned i = 0; i < tmpls.count(); i++ ) {
        tmpls[ i ]->setSelected( false );
    }

    // only emit selection changed if they were not already unselected
    if( d->selection.count() == 0 ) return;

    d->selection.clear();
    d->clearHandles();

    canvas()->update();
    emit( selectionChanged() );
}

void EditablePanelView::invertSelection()
{
    //qWarning( "EditablePanelView::invertSelection" );
    TemplateDict & tmpls = templates();

    d->selection.clear();
    d->clearHandles();

    for( unsigned i = 0; i < tmpls.count(); i++ ) {
        Template * t = tmpls[ i ];
        if(!tmpls[i]->isEnabled()) continue;
        t->setSelected( ! t->isSelected() );
        if( t->isSelected() ) {
            d->selection.insert( tmpls.keyAt( i ), t );
            d->createHandlesFor(tmpls.keyAt(i), canvas());
        }
    }

    if( d->selection.current() ) {
        d->setHandlesActive(d->selection.currentKey(), true);
    }

    canvas()->update();
    emit( selectionChanged() );
}

void EditablePanelView::selectTemplates( const TemplateDict & t )
{
    unsigned j;
    // qWarning( "EditablePanelView::selectTemplates" );
    // qWarning( "having %d templates in selection", d->selection.count() );
    TemplateDict & tmpls = templates();

    // qWarning( "have currently %d selected, should select %d,"
    //         " currently active is %d, should be %d",
    //         d->selection.count(), t.count(), d->selection.currentIndex(),
    //         t.currentIndex() );

    // check what is selected and possibly exit immediatelly to not emit the
    // selectionChanged if nothing has changed
    if( t.count() != d->selection.count() ) goto GO_AHEAD;
    if( t.currentIndex() != d->selection.currentIndex() ) goto GO_AHEAD;

    // FIXME: this is not very intelligent, comparing contents and positions
    for( j = 0; j < t.count(); j ++ ) {
        if( d->selection[ j ] == t[ j ] ) continue;
        goto GO_AHEAD;
    }
    return;

GO_AHEAD:

#if 0
    for( unsigned i = 0; i < tmpls.count(); i++ ) {
        qWarning( "have template " + tmpls.keyAt( i ) + " at 0x%x",
                (unsigned)tmpls[ i ] );
    }
    for( unsigned i = 0; i < t.count(); i++ ) {
        qWarning( "want me to select template " + t.keyAt( i ) + " at 0x%x",
                (unsigned)t[ i ] );
    }
#endif

    unsigned i;
    for( i = 0; i < tmpls.count(); i++ ) {
        tmpls[ i ]->setSelected( false );
    }

    d->selection.clear();
    d->clearHandles();

    for( i = 0; i < t.count(); i++ ) {

        Q_ASSERT( tmpls.contains( t.keyAt( i ) ) );

        unsigned x = tmpls.indexOf( t.keyAt( i ) );

        Q_ASSERT( tmpls.keyAt( x ) == t.keyAt( i ) );

        Template * t = tmpls[ x ];
        t->setSelected( true );
        d->selection.insert( tmpls.keyAt( x ), tmpls[ x ] );
        // create selection handles for the template
        d->createHandlesFor(tmpls.keyAt(x), canvas());
    }

    d->selection.setCurrentIndex( t.currentIndex() );

    if( d->selection.current() ) {
        d->setHandlesActive(d->selection.currentKey(), true);
    }

    canvas()->update();
    emit( selectionChanged() );
}

void EditablePanelView::selectTemplate( Template * t, bool notify /* = false */ )
{
    //qWarning( "EditablePanelView::selectTemplate" );
    Q_ASSERT( t != NULL );

    //FIXME: reverse find key for the template t, very ugly operation.
    const TemplateDict & td = templates();
    QString key;
    //qWarning( "searching for a template 0x%x", (unsigned)t );
    for( unsigned i = 0; i < td.count(); i ++ ) {
        if( td[ i ] == t ) {
            key = td.keyAt( i );
            //qWarning( "found under the name " + key );
            break;
        }
    }

    Q_ASSERT( !key.isEmpty() );

    // only select and emit when the template was not already selected &
    // active
    if( t->isSelected() && t->isActive() ) return;

    if( d->selection.current() ) {
        d->setHandlesActive(d->selection.currentKey(), false);
    }

    if( ! d->selection.contains( key ) ) {
        d->selection.insert( key, t );
        d->createHandlesFor(key, canvas());
    }
    d->selection.setCurrentKey( key );
    t->setSelected( true );
    d->setHandlesActive(key, true);

    if( notify ) {
        canvas()->update();
        emit( selectionChanged() );
    }
}

void EditablePanelView::unselectTemplate( Template * t, bool notify /* = flase */ )
{
    //qWarning( "EditablePanelView::unselectTemplate" );
    Q_ASSERT( t != NULL );

    t->setSelected( false );

    //FIXME: reverse find key for the template t, very ugly operation.
    const TemplateDict & td = templates();
    QString key;
    //qWarning( "searching for a template 0x%x", (unsigned)t );
    for( unsigned i = 0; i < td.count(); i ++ ) {
        if( td[ i ] == t ) {
            key = td.keyAt( i );
            //qWarning( "found under the name " + key );
            break;
        }
    }

    Q_ASSERT( !key.isEmpty() );

    d->selection.remove( key );
    d->clearHandles(key);

    if( d->selection.current() ) {
        d->setHandlesActive(d->selection.currentKey(), true);
    }

    if( notify ) {
        canvas()->update();
        emit( selectionChanged() );
    }
}

const TemplateDict & EditablePanelView::currentSelection() const
{
    return d->selection;
}

void EditablePanelView::renameTemplate( const QString & oldName,
                    const QString & newName )
{
    Q_ASSERT( templates().contains( newName ) );
    if( d->selection.contains( oldName ) ) {
        int idx = d->selection.indexOf( oldName );
        Template * x = d->selection[ idx ];
        d->selection.remove( oldName );
        d->selection.insert( newName, x, idx );
        d->renameHandlesTemplate(oldName, newName);
        emit( selectionChanged() );
    }
}

void EditablePanelView::resizeTemplates(TemplateDict& td)
{
    Q_ASSERT(d);
    for(unsigned i=0; i<td.count(); ++i) {
        resizeTemplate(td[i]);
        if(d->selection.contains(td.keyAt(i))) {
                d->resizeHandlesTemplate(td.keyAt(i));
        }
    }
}

void EditablePanelView::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
    //qWarning( "EditablePanelView::contentsMouseDoubleClickEvent: %d, %d",
    //        e->pos().x(), e->pos().y() );

    // if we are panning, do nothing on double click
    if( d->mode == EditablePanelView::Pan ) {
        Q_ASSERT(d->dragMode == EditablePanelViewPrivate::Idle);
        d->dragMode = EditablePanelViewPrivate::Panning;
        return;
    }

    // if we are zooming, do nothing on double click
    if( d->mode == EditablePanelView::Magnify ) {
        return;
    }

    // if we are previewing, pass mouse events upwards
    if( d->mode == EditablePanelView::Preview
            && (e->button() & Qt::LeftButton ) ) {
        PanelView::contentsMouseDoubleClickEvent( e );
        return;
    }

    // otherwise we are modifying

    // we handle only left-mouse double click
    if( ! ( e->button() & Qt::LeftButton ) ) return;

    Template * i = NULL;

    // check if the doubleclick is at selection handle
    QPoint ip = inverseWorldMatrix().map( e->pos() );

    // check if the click is at resize handle
    QCanvasItemList cl = canvas()->collisions(ip);
    for(QCanvasItemList::const_iterator it = cl.begin();
            it != cl.end(); ++it) {
        if((*it)->rtti() == SelectionHandle::RTTI) {
            i = ((SelectionHandle*)(*it))->templ();
            break;
        }
    }

    if(i==NULL) {
        // handle ordinary mouse clicks
        i = findItemOnTop( e, PanelView::ItemEnabled, false );
    }

    // if we have template and it's selected we emit only, otherwise we
    // reselect and emit
    if( i ) {
        if( ! i->isSelected() ) {

            // we block signals here temporarily to make the selection
            // change atomic
            blockSignals( true );
            unselectAllTemplates();
            blockSignals( false );
            selectTemplate( i, true );
        }
        emit( selectionDoubleClicked(e) );
    }
}

void EditablePanelView::contentsMouseMoveEvent( QMouseEvent *e )
{
    // work around mystical bug where this method gets called with NULL
    // argument, which is IMHO non-sense
    if( e == NULL ) return;

    // qWarning( "EditablePanelView::contentsMouseMoveEvent: %d, %d",
    //         e->pos().x(), e->pos().y() );

    // if we are panning, we are panning
    if( d->mode == EditablePanelView::Pan &&
            d->dragMode == EditablePanelViewPrivate::Panning) {
        //qWarning( "panning..." );
        // remember current pos
        QPoint p = e->globalPos();
        scrollBy(
                - p.x() + d->dragCurrent.x(),
                - p.y() + d->dragCurrent.y() );
        // update old pos
        d->dragCurrent = p;
        return;
    }

    // if we are zooming, do nothing
    if( d->mode == EditablePanelView::Magnify ) {
        //qWarning( "magnifying..." );
        return;
    }

    // if we are previewing, pass mouse events upwards
    if( d->mode == EditablePanelView::Preview
            && (e->state() & Qt::LeftButton ) ) {
        //qWarning( "previewing..." );
        PanelView::contentsMouseMoveEvent( e );
        return;
    }

    // otherwise we are modifying

    //qWarning( "entering mouse move with drag mode %d", d->dragMode );
    //qWarning( "buttons are  %d", e->button() );

    // we handle only left-mouse drag
    // FIXME: does not work
    //if( ! ( e->button() & Qt::LeftButton ) ) return;


    // handle move mode
    if( d->dragMode == EditablePanelViewPrivate::Moving ||
            d->dragMode == EditablePanelViewPrivate::StartMoving ) {
        d->dragMode = EditablePanelViewPrivate::Moving;

        // remember current pos
        QPoint p = e->globalPos();

        //qWarning( ">>> MOVE" );
        //qWarning( "we diffed by %d, last compensation was %d",
        //        p.x() - d->dragCurrent.x(), d->oldCompensation.x() );

        //qWarning( "template being moved is at %d",
        //        d->selection.current()->virtualRect().x() );

        // compute difference
        QPoint diff = inverseMatrix().map( p - d->dragCurrent );

        //compute grid compensation for the current item
        QPoint compensation = adjustToGrid(
                d->selection.current()->virtualRect().topLeft() -
                d->oldCompensation,
                diff );

        for( unsigned i = 0; i < d->selection.count(); i ++ ) {
            //qWarning( "moving template " + d->selection.keyAt( i ) );
            QRect orig = d->selection[ i ]->virtualRect();
            canvas()->setChanged(d->selection[ i ]->rect());
            orig.moveBy(
                    diff.x() + compensation.x() -
                    d->oldCompensation.x(),
                    diff.y() + compensation.y() -
                    d->oldCompensation.y() );
            //qWarning( "template moved to %d", orig.x() );
            d->selection[ i ]->setVirtualRect( orig );
            resizeTemplate( d->selection[ i ] );
            d->resizeHandlesTemplate(d->selection.keyAt(i));
        }

        canvas()->update();

        // update old pos
        d->dragCurrent = p;
        d->oldCompensation = compensation;
        //qWarning( "<<< MOVE" );
        return;
    }

    // handle resize mode
    if( d->dragMode == EditablePanelViewPrivate::Resizing ||
            d->dragMode == EditablePanelViewPrivate::StartResizing ) {
        d->dragMode = EditablePanelViewPrivate::Resizing;

#define SMALLEST_SIZE 1

        // remember current pos
        QPoint p = e->globalPos();

        QPoint compensation;

        // compute difference
        QPoint diff = inverseMatrix().map( p - d->dragCurrent );

        QRect orig = d->resizing->virtualRect();
        canvas()->setChanged( orig );

        QRect smallestRect = inverseMatrix().mapRect(
                QRect(0, 0, SMALLEST_SIZE, SMALLEST_SIZE));

        bool reverseDir = false;

        if(diff.x() != 0) {
            // handle horizontal resize
            switch( d->resizeDir ) {
                case 0:
                case 6:
                case 7:
                    {
                        //compute grid compensation for the current item
                        compensation = adjustToGrid(
                                d->resizing->virtualRect().topLeft() -
                                d->oldCompensation,
                                diff );

                        // possibly leave stop mode
                        if( d->stopx != -1 && p.x() <= d->stopx ) {
                            d->stopx = -1;
                        }
                        // modify rect
                        int drag_diff = diff.x() +
                            compensation.x() - d->oldCompensation.x();

                        if( d->stopx == -1 ) {
                            orig.setLeft(
                                    orig.x() + drag_diff );
                        }
                        // possibly enter stop mode
                        if( d->stopx == -1 &&
                                orig.width() < smallestRect.width() ) {
                            orig.setLeft( orig.x() - drag_diff );
                            reverseDir = true;
                        }
                    }
                    break;
                case 2:
                case 3:
                case 4:
                    {
                        //compute grid compensation for the current item
                        compensation = adjustToGrid(
                                d->resizing->virtualRect().bottomRight() -
                                d->oldCompensation,
                                diff );

                        // possibly leave stop mode
                        if( d->stopx != -1 && p.x() >= d->stopx ) {
                            d->stopx = -1;
                        }
                        // modify rect
                        int drag_diff = diff.x() +
                            compensation.x() - d->oldCompensation.x();
                        if( d->stopx == -1 ) {
                            orig.setWidth(
                                    orig.width() + drag_diff );
                        }
                        // possibly enter stop mode
                        if( d->stopx == -1 &&
                                orig.width() < smallestRect.width() ) {
                            orig.setWidth( orig.width() - drag_diff );
                            reverseDir = true;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        if(diff.y() != 0) {
            // handle vertical resize
            switch( d->resizeDir ) {
                case 0:
                case 1:
                case 2:
                    {
                        //compute grid compensation for the current item
                        compensation = adjustToGrid(
                                d->resizing->virtualRect().topLeft() -
                                d->oldCompensation,
                                diff );

                        // possibly leave stop mode
                        if( d->stopy != -1 && p.y() <= d->stopy ) {
                            d->stopy = -1;
                        }
                        // modify rect
                        int drag_diff = diff.y() +
                            compensation.y() - d->oldCompensation.y();
                        if( d->stopy == -1 ) {
                            orig.setTop(
                                    orig.y() + drag_diff );
                        }
                        // possibly enter stop mode
                        if( d->stopy == -1 &&
                                orig.height() < smallestRect.height() ) {
                            orig.setTop( orig.y() - drag_diff );
                            reverseDir = true;
                        }
                    }
                    break;
                case 4:
                case 5:
                case 6:
                    {

                        //compute grid compensation for the current item
                        compensation = adjustToGrid(
                                d->resizing->virtualRect().bottomRight() -
                                d->oldCompensation,
                                diff );

                        // possibly leave stop mode
                        if( d->stopy != -1 && p.y() >= d->stopy ) {
                            d->stopy = -1;
                        }
                        // modify rect
                        int drag_diff = diff.y() +
                            compensation.y() - d->oldCompensation.y();
                        if( d->stopy == -1 ) {
                            orig.setHeight(
                                    orig.height() + drag_diff );
                        }
                        // possibly enter stop mode
                        if( d->stopy == -1 &&
                                orig.height() < smallestRect.height() ) {
                            orig.setHeight( orig.height() - drag_diff );
                            reverseDir = true;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        if(reverseDir) {
            d->resizeDir = (d->resizeDir + 4) % 8;

            if(d->hs) {
                d->hs->setHighlighted(false);
            }

            Q_ASSERT(d->handles.contains(d->resizingKey));
            d->hs = d->handles[d->resizingKey][d->resizeDir];

            Q_ASSERT(d->hs);
            d->hs->setHighlighted(true);
        }

        if(!diff.isNull()) {
            // update the old area
            canvas()->setChanged(d->resizing->rect());

            d->resizing->setVirtualRect( orig );
            resizeTemplate( d->resizing );
            d->resizeHandlesTemplate(d->resizingKey);

            canvas()->update();

            // update old pos
            d->dragCurrent = p;
            d->oldCompensation = compensation;
        }

        return;
    }

    if( d->dragMode == EditablePanelViewPrivate::RectSelecting ) {

        // remember current pos
        QPoint p = e->globalPos();
        QPoint diff = p - d->dragCurrent;

        d->selectionRect->setSize(
                d->selectionRect->width() + diff.x(),
                d->selectionRect->height() + diff.y() );

        canvas()->update();

        d->dragCurrent = p;
        return;
    }

    QPoint ip = inverseWorldMatrix().map( e->pos() );
    SelectionHandle* sh = NULL;

    // check if the click is at resize handle
    QCanvasItemList cl = canvas()->collisions(ip);
    for(QCanvasItemList::const_iterator it = cl.begin(); it != cl.end(); ++it) {
        if((*it)->rtti() == SelectionHandle::RTTI) {
            sh = (SelectionHandle*)(*it);
            break;
        }
    }
    if(d->hs != sh) {
        if(d->hs) {
            d->hs->setHighlighted(false);
        }
        d->hs = sh;

        if(d->hs) {
            d->hs->setHighlighted(true);
        }

        canvas()->update();
    }
}

void EditablePanelView::contentsMousePressEvent( QMouseEvent *e )
{
    setFocus();
    // qWarning( "EditablePanelView::contentsMousePressEvent: %d, %d",
    //        e->pos().x(), e->pos().y() );

    //debugging output
#if 0
    qWarning( "Panel has the following templates" );
    for( unsigned i = 0; i < templates().count(); i ++ ) {
        qWarning( "Name: " + templates().keyAt( i ) + " Type: " +
                templates()[ i ]->type() + " Visible: %d",
                templates()[ i ]->isVisible() );
    }
#endif

    d->dragStarted = e->globalPos();
    d->dragCurrent = e->globalPos();
    d->oldCompensation = QPoint( 0, 0 );
    QPoint ip = inverseWorldMatrix().map( e->pos() );

    // if we are panning, just let the mouse move do the work
    if( d->mode == EditablePanelView::Pan ) {
        Q_ASSERT(d->dragMode == EditablePanelViewPrivate::Idle);
        d->dragMode = EditablePanelViewPrivate::Panning;
        return;
    }

    // if we are zooming, zoom in with left mouse and zoom out with
    // shift+left mouse
    if( d->mode == EditablePanelView::Magnify ) {
        return;
    }

    // if we are previewing, just pass event upwards
    if( d->mode == EditablePanelView::Preview
            && (e->button() & Qt::LeftButton ) ) {
        //qWarning( "EditablePanelView::press we are not editable" );
        PanelView::contentsMousePressEvent( e );
        return;
    }

    // if we are placing, show selection rect and allow user specify the
    // initial template size
    if( d->mode == EditablePanelView::Place ) {
        d->dragMode = EditablePanelViewPrivate::RectSelecting;
        d->selectionRect->move( ip.x(), ip.y() );
        d->selectionRect->setSize( 0, 0 );
        d->selectionRect->show();
        canvas()->update();
        return;
    }

    // otherwise we are modifying

    // we handle only left-mouse press
    if( ! ( e->button() & Qt::LeftButton ) ) return;

    // handle ordinary mouse clicks
    Template * i = NULL;
    int dir = -1;

    // check if the click is at resize handle
    QCanvasItemList cl = canvas()->collisions(ip);
    for(QCanvasItemList::const_iterator it = cl.begin(); it != cl.end(); ++it) {
        if((*it)->rtti() == SelectionHandle::RTTI) {
            i = ((SelectionHandle*)(*it))->templ();
            dir = ((SelectionHandle*)(*it))->pos();
            break;
        }
    }

    // if the click is not at selection handle, try a Template
    if(!i) {
        i = findItemOnTop( e, PanelView::ItemEnabled, false );
    }

    // with shift we invert selection on current template
    if( e->state() & Qt::ShiftButton ) {
        if( i ) {
            if( i->isSelected() ) {
                unselectTemplate( i, true );
                // reset to select mode
                d->dragMode = EditablePanelViewPrivate::Idle;
            } else {
                selectTemplate( i, true );
                // start move or resize mode
                if( dir == -1 ) {
                    startMove();
                } else {
                    startResize( i, dir );
                }
            }
        }
    }

    // without shift, we deselect all and select the current template
    if( ! ( e->state() & Qt::ShiftButton ) ) {
        if( i ) {
            if(i != d->selection.current()) {
                if( ! i->isSelected() ) {
                    blockSignals( true );
                    unselectAllTemplates();
                    blockSignals( false );
                }
                selectTemplate( i, true );
            }
            // start move or resize mode
            if( dir == -1 ) {
                startMove();
            } else {
                startResize( i, dir );
            }
        } else {
            unselectAllTemplates();
            // reset to select mode
            d->dragMode = EditablePanelViewPrivate::Idle;
        }
    }

    // when no template has been selected we start rect selection
    if( i == NULL && d->mode == EditablePanelView::Modify ) {
        d->dragMode = EditablePanelViewPrivate::RectSelecting;
        d->selectionRect->move( ip.x(), ip.y() );
        d->selectionRect->setSize( 0, 0 );
        d->selectionRect->show();
        canvas()->update();
    }
}

void EditablePanelView::contentsMouseReleaseEvent( QMouseEvent *e )
{
    setFocus();
    // qWarning( "EditablePanelView::contentsMouseReleaseEvent: %d, %d",
    //        e->pos().x(), e->pos().y() );

    e->ignore();
    emit(contentsMouseReleaseSignal(e));
    if(e->isAccepted()) {
        return;
    }
    e->accept();

    // if in preview, pass other mouse clicks in...
    if( d->mode == EditablePanelView::Preview
            && (e->button() & Qt::LeftButton ) ) {
        PanelView::contentsMouseReleaseEvent( e );
        return;
    }

    // if in place, emit templatePlaced
    if( d->mode == EditablePanelView::Place ) {
        d->selectionRect->hide();
        QRect rrr = inverseMatrix().map( d->selectionRect->rect() );
        // check size and try to detect click-only (without drag) placement
        // thus checking selectionRect before transformation (to eliminate
        // rounding errors) but using transformed and rounded rect
        int w = 100;
        if( d->selectionRect->width() > 0.5 ) {
            w = (int)(rrr.width()+.5);
        }
        int h = 100;
        if( d->selectionRect->height() > 0.5 ) {
            h = (int)(rrr.height()+.5);
        }
        //qWarning( "w = %d, h = %d", w, h );
        emit( templatePlaced( QRect(
                        (int)(rrr.x()+.5),
                        (int)(rrr.y()+.5),
                        w, h ) ) );
        return;
    }

    // if we are panning, reset the dragMode
    if( d->mode == EditablePanelView::Pan ) {
        Q_ASSERT(d->dragMode == EditablePanelViewPrivate::Panning);
        d->dragMode = EditablePanelViewPrivate::Idle;
        return;
    }

    // if in zoom, emit zoomInRequested or zoomOutRequested
    if( d->mode == EditablePanelView::Magnify ) {
        if( e->state() & Qt::ShiftButton ) {
            emit( zoomOutRequested( inverseMatrix().map(
                            inverseWorldMatrix().map( e->pos() ) ) ) );
        } else {
            emit( zoomInRequested( inverseMatrix().map(
                            inverseWorldMatrix().map( e->pos() ) ) ) );
        }
        return;
    }

    // when we were moving, notify parent containers correctly
    if( d->dragMode == EditablePanelViewPrivate::Moving ) {
        emit( selectionMoved( d->originalRects ) );
    }

    // when we were moving, notify parent containers correctly
    if( d->dragMode == EditablePanelViewPrivate::Resizing ) {
        // prepare dict for resize command
        TemplateDict t;
        t.insert( d->resizingKey, d->resizing );
        LTMap <QRect> origRects;
        origRects.insert( d->resizingKey, d->resizingOrigRect );
        emit( selectionResized( t, origRects ) );
    }

    if( d->dragMode == EditablePanelViewPrivate::RectSelecting ) {
        d->selectionRect->hide();

        blockSignals( true );

        // unselect templates _before_ the collisions check because the
        // unselectAllTemplates deletes selection handles
        unselectAllTemplates();

        // select all conflicting items
        QCanvasItemList il = canvas()->collisions( QRect(
                    (int)(d->selectionRect->x()+.5),
                    (int)(d->selectionRect->y()+.5),
                    d->selectionRect->width(), d->selectionRect->height() ) );

        QCanvasItemList::Iterator it;
        for( it = il.begin(); it != il.end(); ++it ) {
            if( (*it)->rtti() == Template::RTTI ) {
                Template * x = (Template*)(*it);
                // work around buggy canvas behavior, canvas seems to
                // remember all canvas items even after canvas update and
                // even when they have been told QCI::setCanvas(NULL)
                Q_ASSERT( x->isVisible() );
                if( x->isVisible() && x->isEnabled()) {
                    selectTemplate( x );
                }
            }
        }
        blockSignals( false );

        canvas()->update();
        emit( selectionChanged() );
        d->dragMode = EditablePanelViewPrivate::Idle;
    }

    // when mouse has been released we always start rect selection
    d->dragMode = EditablePanelViewPrivate::Idle;
}

void EditablePanelView::keyPressEvent( QKeyEvent * e )
{
    setFocus();
    if( d->mode == EditablePanelView::Preview ) {
        PanelView::keyPressEvent( e );
        e->accept();
    } else if(d->mode == EditablePanelView::Modify) {
        if(e->key() == Qt::Key_Up || e->key() == Qt::Key_Down ||
                e->key() == Qt::Key_Left || e->key() == Qt::Key_Right) {
            e->accept();
        }
    } else {
        e->ignore();
    }
}

void EditablePanelView::keyReleaseEvent( QKeyEvent * e )
{
    if( d->mode == EditablePanelView::Preview ) {
        PanelView::keyReleaseEvent( e );
        e->accept();
    } else if(d->mode == EditablePanelView::Modify) {
        int xf = 0, yf = 0;
        switch(e->key()) {
            case Qt::Key_Up:
                yf = -1;
                break;
            case Qt::Key_Down:
                yf = 1;
                break;
            case Qt::Key_Left:
                xf = -1;
                break;
            case Qt::Key_Right:
                xf = 1;
                break;
        }
        if(xf != 0 || yf != 0) {
            if(d->gridSnap) {
                xf *= d->gridXSpacing;
                yf *= d->gridYSpacing;
            }

            // obtain actual rects of selected templates
            LTMap<QRect> newRects =
                MoveNResizeSelectionCommand::rects2Map(d->selection);

            // move the selected templates
            QRect r;
            for(unsigned i = 0; i < d->selection.count(); ++i) {
                r = d->selection[i]->virtualRect();
                r.moveBy(xf, yf);
                d->selection[i]->setVirtualRect(r);
                resizeTemplate(d->selection[i]);
                d->resizeHandlesTemplate(d->selection.keyAt(i));
            }

            // notify about the move / create undo command
            emit(selectionMoved(newRects));

            e->accept();
        } else {
            e->ignore();
        }
    } else {
        e->ignore();
    }
}

void EditablePanelView::startMove()
{
    d->originalRects.clear();
    for( unsigned i = 0; i < d->selection.count(); i ++ ) {
        d->originalRects.insert( d->selection.keyAt( i ),
                d->selection[ i ]->virtualRect() );
    }
    d->dragMode = EditablePanelViewPrivate::StartMoving;
}

void EditablePanelView::startResize( Template * t, int direction )
{
    // qWarning( "start resize with dir: %d", direction );
    d->resizeDir = direction;
    d->resizing = t;
    for( unsigned i = 0; i < templates().count(); i ++ ) {
        if( templates()[ i ] == t ) {
            d->resizingKey = templates().keyAt( i );
            break;
        }
    }
    d->resizingOrigRect = d->resizing->virtualRect();
    d->dragMode = EditablePanelViewPrivate::StartResizing;

    // reset stop coords
    d->stopx = -1;
    d->stopy = -1;
}

void EditablePanelView::setGridColor( const QColor & color )
{
    d->gridColor = color;
    changeGridProps( d->gridColor, d->gridHighlight );
    showGrid( d->gridVisible );
}

void EditablePanelView::setGridXYSpacing(
        unsigned xspacing, unsigned yspacing )
{
    d->gridXSpacing = xspacing;
    d->gridYSpacing = yspacing;
    reconstructXGrid();
    reconstructYGrid();
    changeGridProps( d->gridColor, d->gridHighlight );
    showGrid( d->gridVisible );
}

void EditablePanelView::setGridXSpacing( unsigned spacing )
{
    d->gridXSpacing = spacing;
    reconstructXGrid();
    changeGridProps( d->gridColor, d->gridHighlight );
    showGrid( d->gridVisible );
}

void EditablePanelView::setGridYSpacing( unsigned spacing )
{
    d->gridYSpacing = spacing;
    reconstructYGrid();
    changeGridProps( d->gridColor, d->gridHighlight );
    showGrid( d->gridVisible );
}

void EditablePanelView::setGridHighlight( unsigned n )
{
    d->gridHighlight = n;
    changeGridProps( d->gridColor, d->gridHighlight );
    showGrid( d->gridVisible );
}

void EditablePanelView::setGridVisible( bool on )
{
    showGrid( on );
    d->gridVisible = on;
}

void EditablePanelView::setGridSnap( bool on )
{
    d->gridSnap = on;
}

QPoint EditablePanelView::adjustToGrid( const QPoint & p, const QPoint & diff )
{
    // when in "snap to grid mode?"
    if( d->gridSnap ) {

        // first compute where p will be after being moved by diff
        QPoint moved = p + diff;

        // now align moved to the grid
        QPoint rslt;
        rslt.setX( (int)( moved.x() / (double)d->gridXSpacing + .5 ) *
                d->gridXSpacing );
        rslt.setY( (int)( moved.y() / (double)d->gridYSpacing + .5 ) *
                d->gridYSpacing );
        //qWarning( "point %d moved to %d, adjusted to %d, needed %d pixels",
        //        p.x(), moved.x(), rslt.x(), rslt.x() - moved.x() );
        return rslt - moved;
    }
    return QPoint( 0, 0 );
}

void EditablePanelView::reconstructXGrid()
{
    //hide & delete existing canvas lines
    QCanvasItemList::iterator it;
    for( it = d->gridVertLines.begin(); it != d->gridVertLines.end(); ++it ) {
        (*it)->setCanvas( NULL );
    }
    while( d->gridVertLines.begin() != d->gridVertLines.end() ) {
        QCanvasItem * item = *d->gridVertLines.begin();
        d->gridVertLines.remove( item );
        delete item;
    }

    if( d->gridXSpacing < 1 ) return;

    //create new canvas lines
    for( int i = 0; i < virtualSize().width(); i += d->gridXSpacing ) {
        GridLine * l = new GridLine( canvas() );
        QPoint p = matrix().map( QPoint( i, 0 ) );
        l->setPoints( p.x(), 0, p.x(), size().height() );
        l->setZ( - 1000 );
        d->gridVertLines.append( l );
    }
}

void EditablePanelView::reconstructYGrid()
{
    //hide & delete existing canvas lines
    QCanvasItemList::iterator it;
    for( it = d->gridHorizLines.begin(); it != d->gridHorizLines.end(); ++it ) {
        (*it)->setCanvas( NULL );
    }
    while( d->gridHorizLines.begin() != d->gridHorizLines.end() ) {
        QCanvasItem * item = *d->gridHorizLines.begin();
        d->gridHorizLines.remove( item );
        delete item;
    }

    if( d->gridYSpacing < 1 ) return;

    //create new canvas lines
    for( int i = 0; i < virtualSize().height(); i += d->gridYSpacing ) {
        GridLine * l = new GridLine( canvas() );
        QPoint p = matrix().map( QPoint( 0, i ) );
        l->setPoints( 0, p.y(), size().width(), p.y() );
        l->setZ( - 1000 );
        d->gridHorizLines.append( l );
    }
}

void EditablePanelView::showGrid( bool visible )
{
    // qWarning("EditablePanelView::showGrid(%d) ; gridShouldBeDrawn: %d",
    //         visible, d->gridShouldBeDrawn);
    bool v = visible && d->gridShouldBeDrawn;

    GridLine* gl = NULL;
    QCanvasItemList::iterator it;
    for( it = d->gridVertLines.begin();
            it != d->gridVertLines.end(); ++it ) {

        gl = (GridLine*)(*it);
        gl->setVisible(v);
        gl->update();
    }
    for( it = d->gridHorizLines.begin();
            it != d->gridHorizLines.end(); ++it ) {

        gl = (GridLine*)(*it);
        gl->setVisible(v);
        gl->update();
    }
}

void EditablePanelView::changeGridProps( const QColor & color,
        unsigned highlight )
{
    GridLine* gl = NULL;
    QCanvasItemList::iterator it;
    int h = 0;
    for( it = d->gridVertLines.begin(); it != d->gridVertLines.end(); ++it ) {
        bool xx = ( h % highlight ) == 0;
        gl = (GridLine*)(*it);
        gl->setPen( QPen( color, 0, xx ? SolidLine : DotLine ) );
        gl->update();
        h++;
    }
    h = 0;
    for( it = d->gridHorizLines.begin(); it != d->gridHorizLines.end(); ++it ) {
        bool xx = ( h % highlight ) == 0;
        gl = (GridLine*)(*it);
        gl->setPen( QPen( color, 0, xx ? SolidLine : DotLine ) );
        gl->update();
        h++;
    }
}

void EditablePanelView::setSize( const QSize & size )
{
    // force grid to redraw after resize if was visible
    showGrid(false);
    PanelView::setSize(size);

    Q_ASSERT(d);
    d->resizeHandlesTemplate();
}

void EditablePanelView::showEvent(QShowEvent* e)
{
    Q_ASSERT(d);

    if(!d->panelViewBuild) {
        // create the panel
        PanelView::setProjectPanel( d->p, d->va );

        // reset the mode to correctly set enabled/selected/active flags for all
        // templates
        setOperationMode( d->mode );

        d->panelViewBuild = true;
    }

    PanelView::showEvent(e);
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditablePanelView.cpp 1774 2006-05-31 13:17:16Z hynek $
