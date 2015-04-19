// $Id: QWTDial.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTDial.cpp -- 
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

#include <qfont.h>
#include <qcolor.h>
#include <qevent.h>

#include "qwt/qwt_dial_needle.h"

#include "QWTDial.hh"
#include "QWTDial.xpm"

#include "config.h"

#define QDEBUG_RECT(r, t) qDebug(QString("%1::"#r": x:%2 y:%3 w:%4 h:%5") \
        .arg((int)t,0,16).arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct QWTDial::QWTDialPrivate
{
    QWTDialPrivate() :
        localizator( LocalizatorPtr( new Localizator ) ),
        input( NULL ), oldStyle( ( QwtDialSimpleNeedle::Style ) - 1 )
    {
        o = Qt::Vertical;
    }

    LocalizatorPtr localizator;

    IOPin* input;

    Qt::Orientation o;

    QwtDialSimpleNeedle::Style oldStyle;
};

Info QWTDial::makeInfo()
{
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "Dial", "ltl-qwt" ) );
    i.setLongDescription ( QObject::tr(
                "Qwt Dial Widget", "ltl-qwt" ) );
    i.setIcon(QImage(QWTDial_xpm));
    return i;
}

const QString QWTDial::TYPE = "Dial";

QWTDial::QWTDial( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    QwtDial(0, TYPE),
    Template( TYPE, logger ),
    d( new QWTDialPrivate )
{
    Q_CHECK_PTR( d );

    //register info
    registerInfo( makeInfo() );

    QString g = QObject::tr("Appearance","ltl-qwt");

    registerProperty( g, "font",
            new Property( QObject::tr("Font", "ltl-qwt"),
                "font", "", logger ) );

    registerProperty( g, "scale-color",
            new Property( QObject::tr("Scale Color", "ltl-qwt"),
                "color", "", logger ) );

    registerProperty( g, "start-point",
            new Property( QObject::tr("Initial Angle", "ltl-qwt"),
                "integer", "90", logger ) );

    registerProperty( g, "rotate",
            new Property( QObject::tr("Rotate", "ltl-qwt"),
                "choice(Needle,Scale)", "Needle", logger ) );

    g = QObject::tr("Scale","ltl-qwt");

    registerProperty( g, "min",
            new Property( QObject::tr("Lower Limit", "ltl-qwt"),
                "Integer", "0", logger ) );

    registerProperty( g, "max",
            new Property( QObject::tr("Upper Limit", "ltl-qwt"),
                "Integer", "50", logger ) );

    registerProperty( g, "step-size",
            new Property( QObject::tr("Step Size", "ltl-qwt"),
                "float", "5", logger ) );

    registerProperty( g, "min-arc",
            new Property( QObject::tr("Start Angle", "ltl-qwt"),
                "Integer", "10", logger ) );

    registerProperty( g, "max-arc",
            new Property( QObject::tr("End Angle", "ltl-qwt"),
                "Integer", "350", logger ) );

    g = QObject::tr("Needle","ltl-qwt");

    registerProperty( g, "needle-style",
            new Property( QObject::tr("Needle Style", "ltl-qwt"),
                "choice(Ray,Arrow)", "Ray", logger ) );

    registerProperty( g, "needle-color",
            new Property( QObject::tr("Needle Color", "ltl-qwt"),
                "color", "", logger ) );

    registerProperty( g, "needle-knob-color",
            new Property( QObject::tr("Knob Color", "ltl-qwt"),
                "color", "", logger ) );

    // [-- IOPins --]
    registerIOPin(QObject::tr("Inputs"), "input",
            d->input = new IOPin(this, QObject::tr("Input"), "number"));
    Q_CHECK_PTR(d->input);
}

QWTDial::~QWTDial()
{
    delete d;
}

void QWTDial::localize( LocalizatorPtr localizator )
{
    Q_CHECK_PTR( d );
    d->localizator = localizator;
}

void QWTDial::drawShape( QPainter &p )
{
    p.save();

    const QPoint o = Template::rect().topLeft();
    p.translate( o.x(), o.y() );
    const QRect rr(QPoint(0, 0), Template::rect().size());

    QwtDial::drawContents(&p);
    QwtDial::drawFrame(&p);

    p.restore();
}

void QWTDial::propertiesChanged()
{
    Q_ASSERT(d);

    const PropertyDict &pm =(const PropertyDict&)properties();

    QFont f = pm["font"]->asFont();
    QwtDial::setFont(f);

    QColor c = pm["scale-color"]->asColor();
    QPalette pal(palette());
    pal.setColor(QPalette::Inactive, QColorGroup::Text, c);
    pal.setColor(QPalette::Active, QColorGroup::Text, c);
    pal.setColor(QPalette::Disabled, QColorGroup::Text, c);
    QwtDial::setPalette(pal);


    double min = pm["min"]->asInteger();
    double max = pm["max"]->asInteger();
    setRange(min, max);

    double min_arc = pm["min-arc"]->asInteger();
    double max_arc = pm["max-arc"]->asInteger();
    setScaleArc(min_arc, max_arc);

    setScale(36, 10, pm["step-size"]->asFloatingPoint());
    setOrigin(pm["start-point"]->asInteger());

    QString sm = pm["rotate"]->asString();
    QwtDial::Mode m;
    if(sm == "Scale") {
        m = QwtDial::RotateScale;
    } else {
        m = QwtDial::RotateNeedle;
    }
    QwtDial::setMode(m);

    //needle
    QString ss = pm["needle-style"]->asString();
    QwtDialSimpleNeedle::Style s;
    if(ss == "Ray") {
        s = QwtDialSimpleNeedle::Ray;
    } else {
        s = QwtDialSimpleNeedle::Arrow;
    }
    if(s != d->oldStyle) {
        delete QwtDial::needle();
        QwtDial::setNeedle(new QwtDialSimpleNeedle(s));
        Q_CHECK_PTR(QwtDial::needle());
        d->oldStyle = s;
    }

    QColor fg = pm["needle-color"]->asColor();
    QColor bg = pm["needle-knob-color"]->asColor();
    QColorGroup cg(fg, fg, fg, fg, fg, fg, fg, bg, bg);
    Q_ASSERT(QwtDial::needle());
    QwtDial::needle()->setPalette(QPalette(cg, cg, cg));

    // update and schedule repaint
    QWTDial::setRect(Template::rect());
}

void QWTDial::setRect( const QRect & rect )
{
    const QSize oldSize = Template::rect().size();

    Q_ASSERT(d);

    Template::setRect( rect );

    const QRect rr(QPoint(0, 0), rect.size());
    QwtDial::setGeometry(rr);
    {
        QResizeEvent rse( oldSize, rect.size());
        QwtDial::resizeEvent(&rse);
    }

    // and schedule repaint
    Template::update();
}

void QWTDial::iopinsChanged()
{
    Q_ASSERT(d);
    Q_ASSERT(d->input);

    QwtDial::setValue(d->input->realNumberValue());
    Template::update();
}

Template * QWTDial::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new QWTDial( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTDial.cpp 844 2005-06-21 12:40:50Z mman $
