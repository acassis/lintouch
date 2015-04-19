// $Id: QWTSlider.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTSlider.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 27 April 2005
//
// Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
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

#include <qevent.h>

#include "QWTSlider.hh"
#include "QWTSlider.xpm"

#include "config.h"

#define QDEBUG_RECT( r ) qDebug(QString(#r": %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct QWTSlider::QWTSliderPrivate
{
    QWTSliderPrivate() :
        localizator( LocalizatorPtr( new Localizator ) ),
        focused(false), output(NULL)
    {}

    QRegion collisionRegion;
    QPointArray areaPoints;

    LocalizatorPtr localizator;

    bool focused;

    IOPin* output;

    QPointArray points;
    QPen c;
    QFont f;
};

Info QWTSlider::makeInfo()
{
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "Slider", "ltl-qwt" ) );
    i.setLongDescription ( QObject::tr(
                "Qwt Slider Widget", "ltl-qwt" ) );
    i.setIcon(QImage(Slider_xpm));
    return i;
}

const QString QWTSlider::TYPE = "Slider";

QWTSlider::QWTSlider( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    QwtSlider( 0, TYPE ),
    Template( TYPE, logger ),
    d( new QWTSliderPrivate )
{
    Q_CHECK_PTR( d );

    //register info
    registerInfo( makeInfo() );

    QString g = QObject::tr("Appearance", "ltl-qwt");
    registerProperty( g, "orientation",
            new Property( QObject::tr("Orientation", "ltl-qwt"),
                "choice(Horizontal,Vertical)",
                "Horizontal", logger ) );

    registerProperty( g, "border-width",
            new Property( QObject::tr("Border Width", "ltl-qwt"),
                "integer", "2", logger ) );

    registerProperty( g, "background-style",
            new Property( QObject::tr("Background Style", "ltl-qwt"),
                "choice(Trough,Slot,Both)", "Both", logger ) );

    registerProperty( g, "thumb-length",
            new Property( QObject::tr("Thumb Length", "ltl-qwt"),
                "integer", "16", logger ) );

    g = QObject::tr("Limits", "ltl-qwt");
    registerProperty( g, "min",
            new Property( QObject::tr("Lower Limit", "ltl-qwt"),
                "Integer", "0", logger ) );

    registerProperty( g, "max",
            new Property( QObject::tr("Upper Limit", "ltl-qwt"),
                "Integer", "100", logger ) );

    registerProperty( g, "step",
            new Property( QObject::tr("Step Size", "ltl-qwt"),
                "Integer", "5", logger ) );

#if 0 /* not doing anything */
    registerProperty( g, "page-size",
            new Property( QObject::tr("Page Size", "ltl-qwt"),
                "Integer", "25", logger ) );
#endif

    g = QObject::tr("Scale", "ltl-qwt");
    registerProperty( g, "scale-pos",
            new Property( QObject::tr("Position", "ltl-qwt"),
                "choice(None,Left,Right,Top,Bottom)", "None", logger ) );

    registerProperty( g, "scale-font",
            new Property( QObject::tr("Font", "ltl-qwt"),
                "font", "", logger ) );

    registerProperty( g, "scale-color",
            new Property( QObject::tr("Color", "ltl-qwt"),
                "color", "", logger ) );

    // [-- IOPins --]
    registerIOPin(QObject::tr("Outputs", "ltl-qwt"), "output",
            d->output = new IOPin(this,
                QObject::tr("Output", "ltl-qwt"), "number"));
    Q_CHECK_PTR(d->output);
}

QWTSlider::~QWTSlider()
{
    delete d;
}

void QWTSlider::localize( LocalizatorPtr localizator )
{
    Q_CHECK_PTR( d );
    d->localizator = localizator;
}

void QWTSlider::mousePressEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtSlider::mousePressEvent(&e2);
    QwtSlider::update();
    Template::update();
}

void QWTSlider::mouseReleaseEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtSlider::mouseReleaseEvent(&e2);
    QwtSlider::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedNumberValue((int)QwtSlider::value());
    syncIOPinsRequestedValues();
}

void QWTSlider::mouseMoveEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtSlider::mouseMoveEvent(&e2);
    QwtSlider::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedNumberValue((int)QwtSlider::value());
    syncIOPinsRequestedValues();
}

void QWTSlider::keyPressEvent( QKeyEvent * e )
{
    QwtSlider::keyPressEvent(e);
    QwtSlider::update();
    Template::update();
}

void QWTSlider::keyReleaseEvent( QKeyEvent * e )
{
    QwtSlider::keyReleaseEvent(e);
    QwtSlider::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedNumberValue((int)QwtSlider::value());
    syncIOPinsRequestedValues();
}

QWidget::FocusPolicy QWTSlider::focusPolicy() const
{
    return QWidget::StrongFocus;
}

void QWTSlider::focusInEvent ( QFocusEvent* e )
{
    Q_ASSERT(d);
    d->focused = true;

    Template::focusInEvent(e);
    QwtSlider::focusInEvent(e);
    QwtSlider::update();
    Template::update();
}

void QWTSlider::focusOutEvent ( QFocusEvent* e )
{
    Q_ASSERT(d);
    d->focused = false;

    Template::focusOutEvent(e);
    QwtSlider::focusOutEvent(e);
    QwtSlider::update();
    Template::update();
}

void QWTSlider::drawShape( QPainter &p )
{
    // don't draw if the thumb would not fit in given space
    if( (orientation()==Qt::Horizontal?Template::rect().width():
                Template::rect().height()) < thumbLength()) {
        Template::drawShape(p);
        return;
    }
    p.save();

    const QPoint o = Template::rect().topLeft();
    p.translate( o.x(), o.y() );
    const QRect rr(0, 0, Template::rect().width(), Template::rect().height());

    p.setPen(d->c);
    p.setFont(d->f);

    QwtSlider::draw(&p, rr);

    // p.setPen(QPen(Qt::red));
    // p.setBrush(QBrush());
    // p.drawRect(rr);

    // p.translate( -o.x(), -o.y() );
    // // p.setBrush(QBrush());
    // p.setPen(QPen(Qt::blue));
    // p.drawRect(Template::boundingRect());

    p.restore();
}

void QWTSlider::propertiesChanged()
{
    const PropertyDict &pm =(const PropertyDict&)properties();

    // scale
    QString o = pm["scale-pos"]->asString();
    if(o == "Left") {
        QwtSlider::setScalePosition(QwtSlider::Left);
    } else if(o == "Right") {
        QwtSlider::setScalePosition(QwtSlider::Right);
    } else if(o == "Top") {
        QwtSlider::setScalePosition(QwtSlider::Top);
    } else if(o == "Bottom") {
        QwtSlider::setScalePosition(QwtSlider::Bottom);
    } else {    // None
        QwtSlider::setScalePosition(QwtSlider::None);
    }
    d->f = pm["scale-font"]->asFont();
    d->c = pm["scale-color"]->asColor();

    o = pm["orientation"]->asString();
    if(o == "Horizontal") {
        setOrientation(Qt::Horizontal);
    } else {
        setOrientation(Qt::Vertical);
    }

    int b = pm["border-width"]->asInteger();
    if(b >= 0) {
        QwtSlider::setBorderWidth(b);
    }

    o = pm["background-style"]->asString();
    if(o == "Trough") {
        QwtSlider::setBgStyle(QwtSlider::BgTrough);
    } else if(o == "Slot") {
        QwtSlider::setBgStyle(QwtSlider::BgSlot);
    } else {    // Both
        QwtSlider::setBgStyle(QwtSlider::BgBoth);
    }

    int min = pm["min"]->asInteger();
    int max = pm["max"]->asInteger();
    int step = pm["step"]->asInteger();
#if 0 /* not doing anything */
    int page = pm["page-size"]->asInteger();
#endif
    int page = 0;
    QwtSlider::setRange(min, max, step, page);

    QwtSlider::setThumbLength(pm["thumb-length"]->asInteger());

    QWTSlider::setRect(Template::rect());
    Template::update();
}

void QWTSlider::setRect( const QRect & rect )
{
    const QSize oldSize = Template::rect().size();

    Q_ASSERT(d);

    setThumbWidth(0);       // set the thumb to the minimum size

    Template::setRect( rect );

    const QRect rr(0, 0, rect.width(), rect.height());
    QwtSlider::setGeometry(rr);
    {
        QResizeEvent rse( oldSize, rect.size());
        QwtSlider::resizeEvent(&rse);
    }

    // change bounding-rect if we have the scale
    QRect scr(rect.topLeft(), sizeHint());
    int diff = 0;
    switch(scalePosition()) {
        case QwtSlider::Top:
            diff = rect.height() - scr.height();
            scr.moveBy(0, diff);
            break;
        case QwtSlider::Bottom:
            diff = rect.height() - scr.height();
            break;
        case QwtSlider::Left:
            diff = rect.width() - scr.width();
            scr.moveBy(diff, 0);
            break;
        case QwtSlider::Right:
            diff = rect.width() - scr.width();
            break;
        default:
            if(orientation() == Qt::Horizontal) {
                diff = rect.height() - 2*borderWidth();
            } else {
                diff = rect.width() - 2*borderWidth();
            }
            break;
    }
    if(diff > 0) {
        setThumbWidth(diff);
    }
    d->points = rect | scr;

    layoutSlider(true);

    // and schedule repaint
    Template::update();
}

QPointArray QWTSlider::areaPoints() const
{
    Q_ASSERT(d);
    return d->points;
}

Template * QWTSlider::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new QWTSlider( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTSlider.cpp 844 2005-06-21 12:40:50Z mman $
