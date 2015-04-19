// $Id: Slider.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Slider.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 04 November 2004
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

#include <qrect.h>
#include <qpen.h>
#include <qbrush.h>

#include "Slider.hh"
#include "Slider.xpm"

#include "config.h"

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#ifndef QDEBUG_RECT
#define QDEBUG_RECT( r ) qDebug(QString(#r": %1;%2 %3;%4 [w:%5, h:%6]") \
        .arg(r.x()).arg(r.y()).arg(r.right()).arg(r.bottom()) \
        .arg(r.width()).arg(r.height()))
#endif

#ifndef ROUND_OF_DIGIT
#define ROUND_OF_DIGIT( v ) (int)(((v)<((int)(v)+0.5))?(v):((v)+1))
#endif

#ifndef THUMB_CENTER_LINE
#define THUMB_CENTER_LINE 0.1
#endif

struct Slider::SliderPrivate
{
    /*
     * ctor
     */
    SliderPrivate() : m_minimum( 0 ), m_maximum( 99 ), m_page_size( 10 ),
                      m_line_size( 1 ),  m_drag_size( 33 ), m_thumb_size( 10 ),
                      iopinSegment( 1.0 ), sliderPos( m_minimum ),
                      allowDragAndDrop( false ) ,
                      m_localizator( Localizator::nullPtr() ), focused(false)
    {}

    /*
     * Slider properties 
     */
    Orientation     m_orientation;
    int             m_minimum;
    int             m_maximum;
    int             m_page_size;
    int             m_line_size;

    /*
     * Drag related properties
     */
    double          m_drag_size;
    QPen            m_drag_border;
    QBrush          m_drag_background;
    QBrush          m_drag_fill;

    /*
     * Thumb related properties
     */
    double          m_thumb_size;
    QPen            m_thumb_border;
    QBrush          m_thumb_fill;

    /*
     * Ref to output IOPin
     */
    IOPin* m_output;

    /*
     * IOPin segment
     */
    double iopinSegment;

    /*
     * TODO
     */
    double sliderPos;

    /*
     *  drag rect
     */
    QRect dragRect;

    /*
     * drag fill rect
     */
    QRect dragFillRect;

    /*
     * thumb's rect
     */
    QRect thumbRect;

    bool allowDragAndDrop; //added Shivaji

    /*
     * Holds the clicked pos
     */
    QPoint clickPos;

    /*
     *TODO
     */
    LocalizatorPtr  m_localizator;

    bool focused;

    /*
     * Recompute drag rect,thumb rect, iopinSegment from template rect
     * and max and min value set by the user
     */
    void recompute(QRect r)
    {
        dragRect = r;
        thumbRect = r;
        int totalSegments = abs(m_maximum - m_minimum);
        if(totalSegments < 1) {
            totalSegments = 1;
        }

        if( m_orientation == Qt::Vertical ) {
            thumbRect.setHeight( 
                    ROUND_OF_DIGIT( r.height()*m_thumb_size*0.01 ) );
            dragRect.setWidth( 
                    ROUND_OF_DIGIT( r.width()*m_drag_size*0.01 ) );
            int heightOfDragRect = r.height() - thumbRect.height();
            dragRect.setHeight( heightOfDragRect );
            dragRect.moveCenter(r.center());
            QPoint p;
            p.setX( r.center().x() );
            p.setY(dragRect.bottom() );
            thumbRect.moveCenter( p );
            iopinSegment = ( heightOfDragRect*1.0 ) / totalSegments;

        } else {
            thumbRect.setWidth( 
                    ROUND_OF_DIGIT( r.width()*m_thumb_size*0.01 ) );
            int widthOfDragFill = r.width() - thumbRect.width();
            dragRect.setWidth( widthOfDragFill );
            dragRect.setHeight( 
                    ROUND_OF_DIGIT( r.height()*m_drag_size*0.01 ) );
            dragRect.moveCenter(r.center());
            thumbRect.moveCenter(QPoint(
                        dragRect.left(), r.center().y() ) );
            iopinSegment = ( widthOfDragFill*1.0 ) / totalSegments;
        }
        if( sliderPos != MIN(m_minimum,m_maximum) ) {
            moveThumpBySliderPos();
        }

        computeDragFillRect();
    }

    void moveThumpByPageSize( bool isValueUp ) {

        int min = MIN(m_minimum, m_maximum);
        int max = MAX(m_minimum, m_maximum);

        // the min/max way
        bool normal = (m_minimum <= m_maximum);

        if( isValueUp ) {
            if(normal) {
                sliderPos +=  m_page_size;
            } else {
                sliderPos -=  m_page_size;
            }
        } else {
            if(normal) {
                sliderPos -=  m_page_size;
            } else {
                sliderPos +=  m_page_size;
            }
        }
        sliderPos = sliderPos > max ? max : sliderPos;
        sliderPos = sliderPos < min ? min : sliderPos;

        sliderPos = ROUND_OF_DIGIT( sliderPos );

        moveThumpBySliderPos();
    }

    void moveThumpByLineSize( bool isValueUp ) {

        int min = MIN(m_minimum, m_maximum);
        int max = MAX(m_minimum, m_maximum);

        if( isValueUp ) {
            sliderPos+=  m_line_size;
            sliderPos = sliderPos > max ? max : sliderPos;
        } else {
            sliderPos-=  m_line_size;
            sliderPos = sliderPos < min ? min : sliderPos;
        }
        sliderPos = ROUND_OF_DIGIT( sliderPos );
        moveThumpBySliderPos();
    }

    void moveThumpBySliderPos() {

        double moveBySegment = 1.0*sliderPos - MIN(m_minimum, m_maximum);
        int moveByPixel =  ROUND_OF_DIGIT( moveBySegment*iopinSegment );

        // the min/max way
        bool normal = (m_minimum <= m_maximum);

        if( moveByPixel < 0 ) {
            return;
        }
        QPoint p;
        if( m_orientation == Qt::Horizontal ) { 
            if(normal) {
                p.setX(dragRect.left() + moveByPixel);
            } else {
                p.setX(dragRect.right() - moveByPixel);
            }
            p.setY(thumbRect.center().y());
        } else {
            p.setX(thumbRect.center().x());
            if(normal) {
                p.setY(dragRect.bottom() - moveByPixel);
            } else {
                p.setY(dragRect.top() + moveByPixel);
            }
        }
        thumbRect.moveCenter(p);
        validateThumpPos();

        m_output->setRequestedNumberValue( ROUND_OF_DIGIT( sliderPos ) );
        m_output->parent()->syncIOPinsRequestedValues();
        m_output->parent()->iopinsChanged();
    }

    void moveThumpBySteps( int moveStep )
    {
        if( !moveStep ) {
            return;
        }
        if( m_orientation == Qt::Horizontal ) {
            thumbRect.moveBy( moveStep, 0);
        } else {
            thumbRect.moveBy( 0, moveStep );
        }
        validateThumpPos();
        setOutputPin();
    }

    void validateThumpPos() {
        if( !dragRect.contains( thumbRect.center() ) ) {
            QPoint p;
            if( m_orientation == Qt::Horizontal ) {
                if( thumbRect.center().x() > dragRect.right() ) {
                    p.setX( dragRect.right() );
                } else {
                    p.setX( dragRect.left() );
                }
                p.setY( thumbRect.center().y() );
            } else {
                if( thumbRect.center().y() < dragRect.top() ) {
                    p.setY( dragRect.top() );
                } else {
                    p.setY( dragRect.bottom() );
                }
                p.setX( thumbRect.center().x() );
            }
            thumbRect.moveCenter( p );
        }
    }

    void computeDragFillRect() {
        // the input value
        int intInput = m_output->realNumberValue();

        // by default we take the whole area
        dragFillRect = dragRect;

        // the min/max way
        bool normal = (m_minimum <= m_maximum);

        // limit situations
        if(normal) { // from min to max
            if(intInput <= m_minimum) {
                // value too low; draw nothing
                dragFillRect = QRect();
                return;
            }
            if(intInput >= m_maximum) {
                // we have the full meter
                return;
            }
        } else { // from max to min
            if(intInput <= m_maximum) {
                // value too low; draw nothing
                dragFillRect = QRect();
                return;
            }
            if(intInput >= m_minimum) {
                // we have the full meter
                return;
            }
        }

        int window = abs(m_maximum - m_minimum);
        float f;
        int val = intInput - MIN(m_minimum,m_maximum);

        if( m_orientation == Qt::Horizontal ) {
            f = 1.0*dragFillRect.width() / window;
            val = (int)(f * val);
            if(normal) {
                dragFillRect.setRight(dragFillRect.left() + val);
            } else {
                dragFillRect.setLeft(dragFillRect.right() - val);
            }
        } else {
            f = 1.0*dragFillRect.height() / window;
            val = (int)(f * val);
            if(normal) {
                dragFillRect.setTop(dragFillRect.bottom() - val);
            } else {
                dragFillRect.setBottom(dragFillRect.top() + val);
            }
        }
        Q_ASSERT(dragFillRect.isValid());
    }

    void setOutputPin() {
        Q_ASSERT( m_output );

        // the min/max way
        bool normal = (m_minimum <= m_maximum);

        int pixelMoved = 0;

        if( m_orientation == Qt::Horizontal ) {
            pixelMoved = thumbRect.center().x() - dragRect.left();
        } else {
            pixelMoved = dragRect.bottom() - thumbRect.center().y();
        }
        if( pixelMoved >= 0 ) {
            double ioPinNo = pixelMoved*1.0/iopinSegment;

            if(normal) {
                sliderPos = MIN(m_minimum,m_maximum) + ioPinNo;
            } else {
                sliderPos = MAX(m_minimum,m_maximum) - ioPinNo;
            }

            m_output->setRequestedNumberValue( ROUND_OF_DIGIT( sliderPos) );
            m_output->parent()->syncIOPinsRequestedValues();
            m_output->parent()->iopinsChanged();
        }
    }
};

Slider::Slider(LoggerPtr logger) : Template("Slider", logger),
                                   d(new SliderPrivate)
{
    Q_CHECK_PTR(d);

    //register info
    Info i;
    i.setAuthor("Patrik Modesto <modesto@swac.cz>");
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription(QObject::tr("Slider", "ltl-input"));
    i.setLongDescription(QObject::tr(
                "Slider with one input/output iopin", "ltl-input"));
    i.setIcon(QImage((const char **)Slider_xpm));
    registerInfo(i);
    // create properties of this template and place them within three
    // groups, the "on" properties are created as shadows for "off"
    // properties.
    QString label = QObject::tr("Slider", "ltl-input");

    registerProperty(label, "direction",
            new Property(QObject::tr("Direction", "ltl-input"),
                "choice(horizontal,vertical)", "horizontal", logger));
    registerProperty(label, "minimum",
            new Property(QObject::tr("Minimum", "ltl-input"),
                "integer", "0", logger));
    registerProperty(label, "maximum",
            new Property(QObject::tr("Maximum", "ltl-input"),
                "integer", "99", logger));
    registerProperty(label, "page_size",
            new Property(QObject::tr("Page Size", "ltl-input"),
                "integer", "10", logger));
    registerProperty(label, "line_size",
            new Property(QObject::tr("Line Size", "ltl-input"),
                "integer", "1", logger));

    label = QObject::tr("Drag Area", "ltl-input");
    registerProperty(label, "drag_size",
            new Property(QObject::tr("Size (%)", "ltl-input"),
                "float", "33", logger));
    registerProperty(label, "drag_border",
            new Property(QObject::tr("Border", "ltl-input"),
                "pen", "#000000;1;SOLIDLINE", logger));
    registerProperty(label, "drag_background",
            new Property(QObject::tr("Background", "ltl-input"),
                "brush", "#FFFFFF;NOBRUSH", logger));
    registerProperty(label, "drag_fill",
            new Property(QObject::tr("Fill", "ltl-input"),
                "brush", "#000000;SOLIDPATTERN", logger));
    label = QObject::tr("Thumb", "ltl-input");

    registerProperty(label, "thumb_size",
            new Property(QObject::tr("Size (%)", "ltl-input"),
                "float", "10", logger));
    registerProperty(label, "thumb_border",
            new Property(QObject::tr("Border", "ltl-input"),
                "pen", "#000000;1;SOLIDLINE", logger));
    registerProperty(label, "thumb_fill",
            new Property(QObject::tr("Fill", "ltl-input"),
                "brush", "#FFFFFF;SOLIDPATTERN", logger));

    // create iopins of this template
    QString inp = QObject::tr("Input", "ltl-input");
    inp = QObject::tr("Output", "ltl-input");
    registerIOPin(inp, "Output",
            d->m_output = new IOPin(this, QObject::tr("Output", "ltl-input"),
                "number"));
}

Slider::~Slider()
{
    delete d;
}

void Slider::drawShape(QPainter & p)
{
    Q_ASSERT(d);

    p.save();
    p.setBrush(d->m_drag_background);
    p.setPen(d->m_drag_border);
    p.drawRect(d->dragRect);

    if( d->dragFillRect.isValid() 
            && d->dragRect.contains( d->dragFillRect ) ) {
        p.setBrush( d->m_drag_fill );
        p.drawRect( d->dragFillRect );
    }

    p.setBrush(d->m_thumb_fill);
    p.setPen( d->m_thumb_border);
    p.drawRect( d->thumbRect );      
    QPoint p0 = d->thumbRect.center();
    if( d->m_orientation == Qt::Horizontal ) {
        int rectHeight = d->thumbRect.height();
        int centerLineHeight = 
            ROUND_OF_DIGIT( rectHeight*THUMB_CENTER_LINE );
        QPoint p1( p0.x(),p0.y() - centerLineHeight/2);
        QPoint p2( p0.x(), p0.y() + centerLineHeight/2 );
        p.drawLine(p1,p2);
    } else {
        int rectWidth = d->thumbRect.width();
        int centerLineWidth =
            ROUND_OF_DIGIT( rectWidth*THUMB_CENTER_LINE );
        QPoint p1( p0.x() - centerLineWidth/2,p0.y() );
        QPoint p2( p0.x() + centerLineWidth/2,p0.y() );
        p.drawLine(p1,p2);
    }

    p.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
    p.setBrush(QBrush());
    p.drawRect(rect());
    p.restore();

    if(d->focused)
        drawFocusRect(p);
}

void Slider::mousePressEvent(QMouseEvent *  e )
{
    if( e->button() != Qt::LeftButton ) {
        return;
    }

    d->allowDragAndDrop = d->thumbRect.contains( e->pos() ); //Shivaji
    if( d->allowDragAndDrop ) {
        d->clickPos = e->pos();
        return;
    }

    if( d->m_orientation == Qt::Horizontal ) {
        if( e->pos().x() > d->thumbRect.right() ) {
            d->moveThumpByPageSize( true );    
        } else if ( e->pos().x() < d->thumbRect.left() ){
            d->moveThumpByPageSize( false );
        }
    } else {
        if( e->pos().y() < d->thumbRect.top() ) {
            d->moveThumpByPageSize( true );
        } else if ( e->pos().y() > d->thumbRect.bottom() ){
            d->moveThumpByPageSize( false );
        }
    }

}

void Slider::mouseReleaseEvent(QMouseEvent *  e )
{
    if( e->button() == Qt::LeftButton && d->allowDragAndDrop ) {
        d->allowDragAndDrop = false;
    }
}

void Slider::mouseMoveEvent(QMouseEvent *  e )
{
    Q_ASSERT( d );

    if ( d->allowDragAndDrop && e->state() == Qt::LeftButton ) {

        int nSteps;
        if ( d->m_orientation == Qt::Horizontal ){
            nSteps = e->pos().x() - d->clickPos.x(); 
            d->clickPos = e->pos();
        } else {
            nSteps = e->pos().y() - d->clickPos.y();
            d->clickPos = e->pos();
        }

        d->moveThumpBySteps( nSteps );
    } else {
        d->allowDragAndDrop = false;
    }
}

void Slider::mouseDoubleClickEvent(QMouseEvent *  /*e*/ )
{
    //qWarning("mouseDoubleClickEvent at %d, %d", e->pos().x(), e->pos().y());
}

void Slider::setRect(const QRect & rect)
{
    Q_ASSERT(d);

    Template::setRect(rect);

    d->recompute(rect);

    // and shedule repaint
    update();
}

void Slider::propertiesChanged()
{
    Q_ASSERT(d);
    const PropertyDict& pd = properties();

    if( pd["direction"]->asString() == "horizontal" ) {
        d->m_orientation = Qt::Horizontal;
    } else {
        d->m_orientation = Qt::Vertical;
    }
    d->m_minimum = pd["minimum"]->asInteger();
    d->m_maximum = pd["maximum"]->asInteger();

    // update slider position to be inside limits
    int min = MIN(d->m_minimum, d->m_maximum);
    int max = MAX(d->m_minimum, d->m_maximum);
    if(d->sliderPos < min) {
        d->sliderPos = min;
    } else if(d->sliderPos > max) {
        d->sliderPos = max;
    }

    int page_size = pd["page_size"]->asInteger();
    if( page_size > 0 ) {
        d->m_page_size = page_size;
    } else {
        getLogger(). warn( QString("Invalid Slider Page size : %1").
                arg( page_size ) );            
    }
    int line_size = pd["line_size"]->asInteger();
    if( line_size > 0 ) {
        d->m_line_size = line_size;
    } else {
        getLogger(). warn( QString("Invalid Slider Line size : %1").
                arg( line_size ) );
    }

    //d->m_drag_size = pd["drag_size"]->asFloatingPoint();
    double drag_size = pd["drag_size"]->asFloatingPoint();
    if( drag_size > 0 && drag_size <= 100 ) {
        d->m_drag_size = drag_size;
    } else {
        //pd["drag_size"]->setIntegerValue( d->m_drag_size );
        getLogger(). warn( QString("Invalid Drag Size[%] : %1. \
                    Select value between 1 and 100").arg( drag_size ) );
    }
    d->m_drag_border = pd["drag_border"]->asPen();
    d->m_drag_background = pd["drag_background"]->asBrush();
    d->m_drag_fill = pd["drag_fill"]->asBrush();

    //d->m_thumb_size = pd["thumb_size"]->asFloatingPoint();
    double thumb_size = pd["thumb_size"]->asFloatingPoint();
    if (  thumb_size > 0  && thumb_size <= 100 ) { 
        d->m_thumb_size = thumb_size;
    } else {        
        getLogger().warn( QString("Invalid Thumb Size[%] : %1 . \
                    Select value between 1 and 100").arg( thumb_size ) );
        //  pd["thumb_size"]->setIntegerValue( d->m_thumb_size );
    }
    d->m_thumb_border = pd["thumb_border"]->asPen();
    d->m_thumb_fill = pd["thumb_fill"]->asBrush();

    d->recompute( rect() );
    // shedule repaint    
    update();
}

void Slider::iopinsChanged()
{
    d->computeDragFillRect();
    // shedule repaint
    update();
}

Template * Slider::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */) const
{
    Template * result = new Slider(logger);
    setupAfterClone(result, loc, tm);
    return result;
}

void Slider::keyPressEvent( QKeyEvent * e )
{
    Q_ASSERT( d );
    //qWarning( "Button::keyPressEvent: %d", e->key() );
    switch( e->key() ){    
        case Qt::Key_Prior:
            d->moveThumpByPageSize( true );
            e->accept();
            update();
            break;
        case Qt::Key_Next:
            d->moveThumpByPageSize( false );
            e->accept();
            update();
            break;
        case Qt::Key_Left:
            d->moveThumpByLineSize( false );
            e->accept();
            update();
            break;
        case Qt::Key_Up:
            d->moveThumpByLineSize( true );
            e->accept();
            update();
            break;   
        case Qt::Key_Right:
            d->moveThumpByLineSize( true );
            e->accept();
            update();
            break;
        case Qt::Key_Down:    
            d->moveThumpByLineSize( false );
            e->accept();
            update();
            break;
        default:
            e->ignore();
            break;
    }
}

QPointArray Slider::areaPoints() const {

    return Template::areaPoints();

}

QRegion Slider::collisionRegion() const {

    return Template::collisionRegion();

}

void Slider::setVisible( bool visible ) {

    Template::setVisible( visible );

}

void Slider::setCanvas( QCanvas * c ) {

    Template::setCanvas( c );

}

void Slider::advance( int /* phase */ ) {
}


QWidget::FocusPolicy Slider::focusPolicy() const
{
    return QWidget::StrongFocus;
}

void Slider::localize( LocalizatorPtr loc ) {
    Q_ASSERT( d );

    if( ! loc.isNull() ) {
        d->m_localizator = loc;
    }
}

void Slider::focusInEvent ( QFocusEvent* e )
{
    Template::focusInEvent(e);

    Q_ASSERT(d);
    d->focused = true;
    update();
}

void Slider::focusOutEvent ( QFocusEvent* e )
{
    Template::focusOutEvent(e);

    Q_ASSERT(d);
    d->focused = false;
    update();
}

#undef MIN
#undef MAX
#undef QDEBUG_RECT
#undef ROUND_OF_DIGIT
#undef THUMB_CENTER_LINE

// vim: set et ts=4 sw=4 tw=76:
// $Id: Slider.cpp 1168 2005-12-12 14:48:03Z modesto $
