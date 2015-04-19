// $Id: QWTThermo.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTThermo.cpp -- 
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

#include <qcolor.h>
#include <qevent.h>
#include <qfont.h>

#include "QWTThermo.hh"
#include "QWTThermo.xpm"

#include "config.h"

#define QDEBUG_RECT(r) qDebug(QString(#r": x:%1 y:%2 w:%3 h:%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct QWTThermo::QWTThermoPrivate
{
    QWTThermoPrivate() :
        localizator( LocalizatorPtr( new Localizator ) ),
        input(NULL)
    {
        o = Qt::Vertical;
    }

    LocalizatorPtr localizator;

    IOPin* input;

    QFont f;
    QColor c;

    QPointArray points;

    Qt::Orientation o;
};

Info QWTThermo::makeInfo()
{
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "Thermometer", "ltl-qwt" ) );
    i.setLongDescription ( QObject::tr(
                "Qwt Thermometer Widget", "ltl-qwt" ) );
    i.setIcon(QImage(QWTThermo_xpm));
    return i;
}

const QString QWTThermo::TYPE = "Thermo";

QWTThermo::QWTThermo( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    QwtThermo( 0, TYPE ),
    Template( TYPE, logger ),
    d( new QWTThermoPrivate )
{
    Q_CHECK_PTR( d );

    //register info
    registerInfo( makeInfo() );

    QString g = QObject::tr("Appearance","ltl-qwt");

    registerProperty( g, "orientation",
            new Property( QObject::tr("Orientation", "ltl-qwt"),
                "choice(Horizontal,Vertical)", "Vertical", logger ) );

    registerProperty( g, "border-width",
            new Property( QObject::tr("Border Width", "ltl-qwt"),
                "integer", "2", logger ) );

#if 0
    registerProperty( g, "pipe-color",
            new Property( QObject::tr("Pipe Color", "ltl-qwt"),
                "color", "#000000", logger ) );
#endif

    registerProperty( g, "pipe-fill",
            new Property( QObject::tr("Pipe Fill", "ltl-qwt"),
                "brush", "#000000;SOLIDPATTERN", logger ) );


    g = QObject::tr("Limits","ltl-qwt");

    registerProperty( g, "min-value",
            new Property( QObject::tr("Lower Limit", "ltl-qwt"),
                "float", "0", logger ) );

    registerProperty( g, "max-value",
            new Property( QObject::tr("Upper Limit", "ltl-qwt"),
                "float", "100.0", logger ) );

    registerProperty( g, "step",
            new Property( QObject::tr("Step Size", "ltl-qwt"),
                "float", "15", logger ) );

    g = QObject::tr("Scale","ltl-qwt");
    registerProperty( g, "scale-pos",
            new Property( QObject::tr("Position", "ltl-qwt"),
                "choice(None,Left,Right,Top,Bottom)", "None", logger ) );

    registerProperty( g, "font",
            new Property( QObject::tr("Font", "ltl-qwt"),
                "font", "", logger ) );

    registerProperty( g, "color",
            new Property( QObject::tr("Color", "ltl-qwt"),
                "color", "", logger ) );

    // [-- IOPins --]
    registerIOPin(QObject::tr("Inputs", "ltl-qwt"), "input",
            d->input = new IOPin(this,
                QObject::tr("Input", "ltl-qwt"), "number"));
    Q_CHECK_PTR(d->input);

}

QWTThermo::~QWTThermo()
{
    delete d;
}

void QWTThermo::localize( LocalizatorPtr localizator )
{

    Q_CHECK_PTR( d );
    d->localizator = localizator;
}

void QWTThermo::drawShape( QPainter &p )
{
    p.save();

    p.setPen(d->c);
    p.setFont(d->f);

    const QPoint o = Template::rect().topLeft();
    p.translate(o.x(), o.y());
    const QRect rr(QPoint(0, 0), Template::rect().size());

    QwtThermo::draw(&p, rr);

    // p.setPen(QPen(Qt::red));
    // p.setBrush(QBrush());
    // p.drawRect(rr);

    // p.translate( -o.x(), -o.y() );
    // // p.setBrush(QBrush());
    // p.setPen(QPen(Qt::blue));
    // p.drawRect(Template::boundingRect());

    p.restore();
}

void QWTThermo::propertiesChanged()
{
    Q_ASSERT(d);

    const PropertyDict &pm =(const PropertyDict&)properties();

    d->f = pm["font"]->asFont();
    d->c = pm["color"]->asColor();

    QwtThermo::setBorderWidth(pm["border-width"]->asInteger());

    //QwtThermo::setFillColor(pm["pipe-color"]->asColor());
    QwtThermo::setFillBrush(pm["pipe-fill"]->asBrush());

    double max = pm["max-value"]->asFloatingPoint();
    double min = pm["min-value"]->asFloatingPoint();
    double step = pm["step"]->asFloatingPoint();
    QwtThermo::setRange(min, max);
    QwtThermo::setScale(min, max, step);

    QwtThermo::ScalePos so;
    QString s = pm["scale-pos"]->asString();
    if(s == "Top") {
        so = QwtThermo::Top;
    } else if(s == "Bottom") {
        so = QwtThermo::Bottom;
    } else if(s == "Left") {
        so = QwtThermo::Left;
    } else if(s == "Right") {
        so = QwtThermo::Right;
    } else {
        so = QwtThermo::None;
    }

    if(pm["orientation"]->asString() == "Horizontal") {
        d->o = Qt::Horizontal;
    } else {
        d->o = Qt::Vertical;
    }
    setOrientation(d->o, so);

    // update and schedule repaint
    QWTThermo::setRect(Template::rect());
}

void QWTThermo::setRect( const QRect & rect )
{
    const QSize oldSize = Template::rect().size();

    Q_ASSERT(d);

    // set the pipe to the minimum
    if(scalePosition() != QwtThermo::None) {
        setPipeWidth(1);
    }

    Template::setRect( rect );

    const QRect rr(QPoint(0, 0), rect.size());
    QwtThermo::setGeometry(rr);
    {
        QResizeEvent rse( oldSize, rect.size());
        QwtThermo::resizeEvent(&rse);
    }

    layoutThermo(true);

    // change bounding-rect if we have the scale
    QRect scr(rect.topLeft(), sizeHint());
    int w = 0;
    switch(scalePosition()) {
        case QwtThermo::Bottom:
        case QwtThermo::Top:
            w = rect.height() - scr.height();
            break;
        case QwtThermo::Right:
        case QwtThermo::Left:
            w = rect.width() - scr.width();
            break;
        default:
            if(d->o == Qt::Horizontal) {
                w = rect.height() - 3*borderWidth();
            } else {
                w = rect.width() - 3*borderWidth();
            }
            break;
    }

    if(w > 0) {
        setPipeWidth(w);
        layoutThermo(true);
    }

    scr = QRect(rect.topLeft(), sizeHint());
    switch(scalePosition()) {
        case QwtThermo::Bottom:
            break;
        case QwtThermo::Top:
            scr.moveBottom(rect.bottom());
            break;
        case QwtThermo::Right:
            break;
        case QwtThermo::Left:
        default:
            scr.moveRight(rect.right());
            break;
    }
    d->points = rect | scr;

    // and schedule repaint
    Template::update();
}

void QWTThermo::iopinsChanged()
{
    Q_ASSERT(d);
    Q_ASSERT(d->input);

    QwtThermo::setValue(d->input->realNumberValue());
    Template::update();
}

QPointArray QWTThermo::areaPoints() const
{
    return d->points;
}


Template * QWTThermo::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new QWTThermo( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTThermo.cpp 844 2005-06-21 12:40:50Z mman $
