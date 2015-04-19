// $Id: QWTWheel.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTWheel.cpp -- 
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

#include "QWTWheel.hh"
#include "QWTWheel.xpm"

#include "config.h"

#define QDEBUG_RECT( r ) qDebug(QString(#r": %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct QWTWheel::QWTWheelPrivate
{
    QWTWheelPrivate() :
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

Info QWTWheel::makeInfo()
{
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "Wheel", "ltl-qwt" ) );
    i.setLongDescription ( QObject::tr(
                "Qwt Wheel Widget", "ltl-qwt" ) );
    i.setIcon(QImage(QWTWheel_xpm));
    return i;
}

const QString QWTWheel::TYPE = "Wheel";

QWTWheel::QWTWheel( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    QwtWheel( 0, TYPE ),
    Template( TYPE, logger ),
    d( new QWTWheelPrivate )
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

    registerProperty( g, "view-angle",
            new Property( QObject::tr("View Angle", "ltl-qwt"),
                "integer", "175", logger ) );

    registerProperty( g, "total-angle",
            new Property( QObject::tr("Total Angle", "ltl-qwt"),
                "integer", "360", logger ) );

    g = QObject::tr("Limits", "ltl-qwt");
    registerProperty( g, "min",
            new Property( QObject::tr("Lower Limit", "ltl-qwt"),
                "Integer", "0", logger ) );

    registerProperty( g, "max",
            new Property( QObject::tr("Upper Limit", "ltl-qwt"),
                "Integer", "100", logger ) );

    registerProperty( g, "step",
            new Property( QObject::tr("Step Size", "ltl-qwt"),
                "Integer", "1", logger ) );

#if 0 /* not doing anything */
    registerProperty( g, "page-size",
            new Property( QObject::tr("Page Size", "ltl-qwt"),
                "Integer", "25", logger ) );
#endif


    // [-- IOPins --]
    registerIOPin(QObject::tr("Outputs", "ltl-qwt"), "output",
            d->output = new IOPin(this,
                QObject::tr("Output", "ltl-qwt"), "number"));
    Q_CHECK_PTR(d->output);
}

QWTWheel::~QWTWheel()
{
    delete d;
}

void QWTWheel::localize( LocalizatorPtr localizator )
{

    Q_CHECK_PTR( d );
    d->localizator = localizator;
}

void QWTWheel::mousePressEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtWheel::mousePressEvent(&e2);
    QwtWheel::update();
    Template::update();
}

void QWTWheel::mouseReleaseEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtWheel::mouseReleaseEvent(&e2);
    QwtWheel::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedNumberValue((int)QwtWheel::value());
    syncIOPinsRequestedValues();
}

void QWTWheel::mouseMoveEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtWheel::mouseMoveEvent(&e2);
    QwtWheel::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedNumberValue((int)QwtWheel::value());
    syncIOPinsRequestedValues();
}

void QWTWheel::keyPressEvent( QKeyEvent * e )
{
    QwtWheel::keyPressEvent(e);
    QwtWheel::update();
    Template::update();
}

void QWTWheel::keyReleaseEvent( QKeyEvent * e )
{
    QwtWheel::keyReleaseEvent(e);
    QwtWheel::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedNumberValue((int)QwtWheel::value());
    syncIOPinsRequestedValues();
}

QWidget::FocusPolicy QWTWheel::focusPolicy() const
{
    return QWidget::StrongFocus;
}

void QWTWheel::focusInEvent ( QFocusEvent* e )
{
    Q_ASSERT(d);
    d->focused = true;

    Template::focusInEvent(e);
    QwtWheel::focusInEvent(e);
    QwtWheel::update();
    Template::update();
}

void QWTWheel::focusOutEvent ( QFocusEvent* e )
{
    Q_ASSERT(d);
    d->focused = false;

    Template::focusOutEvent(e);
    QwtWheel::focusOutEvent(e);
    QwtWheel::update();
    Template::update();
}

void QWTWheel::drawShape( QPainter &p )
{
    // // don't draw if the thumb would not fit in given space
    // if( (orientation()==Qt::Horizontal?Template::rect().width():
    //             Template::rect().height()) < thumbLength()) {
    //     Template::drawShape(p);
    //     return;
    // }
    p.save();

    const QPoint o = Template::rect().topLeft();
    p.translate( o.x(), o.y() );
    const QRect rr(0, 0, Template::rect().width(), Template::rect().height());

    p.setPen(d->c);
    p.setFont(d->f);

    QwtWheel::draw(&p, rr);

    // p.setPen(QPen(Qt::red));
    // p.setBrush(QBrush());
    // p.drawRect(rr);

    // p.translate( -o.x(), -o.y() );
    // // p.setBrush(QBrush());
    // p.setPen(QPen(Qt::blue));
    // p.drawRect(Template::boundingRect());

    p.restore();
}

void QWTWheel::propertiesChanged()
{
    const PropertyDict &pm =(const PropertyDict&)properties();

    QString o = pm["orientation"]->asString();
    if(o == "Horizontal") {
        setOrientation(Qt::Horizontal);
    } else {
        setOrientation(Qt::Vertical);
    }

    int b = pm["border-width"]->asInteger();
    if(b >= 0) {
        QwtWheel::setInternalBorder(b);
    }

    QwtWheel::setViewAngle(pm["view-angle"]->asInteger());
    QwtWheel::setTotalAngle(pm["total-angle"]->asInteger());

    int min = pm["min"]->asInteger();
    int max = pm["max"]->asInteger();
    int step = pm["step"]->asInteger();
#if 0 /* not doing anything */
    int page = pm["page-size"]->asInteger();
#endif
    int page = 0;
    QwtWheel::setRange(min, max, step, page);

    QWTWheel::setRect(Template::rect());
    Template::update();
}

void QWTWheel::setRect( const QRect & rect )
{
    const QSize oldSize = Template::rect().size();

    Q_ASSERT(d);

    setWheelWidth(0);       // set the thumb to the minimum size

    Template::setRect( rect );

    const QRect rr(0, 0, rect.width(), rect.height());
    QwtWheel::setGeometry(rr);
    {
        QResizeEvent rse( oldSize, rect.size());
        QwtWheel::resizeEvent(&rse);
    }

    // change bounding-rect if we have the scale
    QRect scr(rect.topLeft(), sizeHint());
    int diff = 0;
    if(orientation() == Qt::Horizontal) {
        diff = rect.height() - 2*internalBorder();
    } else {
        diff = rect.width() - 2*internalBorder();
    }
    if(diff > 0) {
        setWheelWidth(diff);
    }
    d->points = rect | scr;

    layoutWheel(true);

    // and schedule repaint
    Template::update();
}

QPointArray QWTWheel::areaPoints() const
{
    Q_ASSERT(d);
    return d->points;
}

Template * QWTWheel::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new QWTWheel( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTWheel.cpp 844 2005-06-21 12:40:50Z mman $
