// $Id: QWTAnalogClock.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTAnalogClock.cpp -- 
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

#include "QWTAnalogClock.hh"
#include "QWTAnalogClock.xpm"

#include "config.h"

#define QDEBUG_RECT(r, t) qDebug(QString("%1::"#r": x:%2 y:%3 w:%4 h:%5") \
        .arg((int)t,0,16).arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct QWTAnalogClock::QWTAnalogClockPrivate
{
    QWTAnalogClockPrivate() :
        localizator( LocalizatorPtr( new Localizator ) ),
        ih( NULL ), im( NULL ), is( NULL )
    {}

    LocalizatorPtr localizator;

    IOPin* ih;
    IOPin* im;
    IOPin* is;
};

Info QWTAnalogClock::makeInfo()
{
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "Analog Clock", "ltl-qwt" ) );
    i.setLongDescription ( QObject::tr(
                "Qwt Analog Clock Widget", "ltl-qwt" ) );
    i.setIcon(QImage(QWTAnalogClock_xpm));
    return i;
}

const QString QWTAnalogClock::TYPE = "AnalogClock";

QWTAnalogClock::QWTAnalogClock( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    QwtAnalogClock(0, TYPE),
    Template( TYPE, logger ),
    d( new QWTAnalogClockPrivate )
{
    Q_CHECK_PTR( d );

    //register info
    registerInfo( makeInfo() );

    QString g = QObject::tr("Appearance","ltl-qwt");
    registerProperty( g, "font",
            new Property( QObject::tr("Font", "ltl-qwt"),
                "font", "", logger ) );

    registerProperty( g, "h-color",
            new Property( QObject::tr("Hour Hand", "ltl-qwt"),
                "color", "", logger ) );
    registerProperty( g, "m-color",
            new Property( QObject::tr("Minute Hand", "ltl-qwt"),
                "color", "", logger ) );
    registerProperty( g, "s-color",
            new Property( QObject::tr("Second Hand", "ltl-qwt"),
                "color", "", logger ) );


    // [-- IOPins --]
    g = QObject::tr("Inputs", "ltl-qwt");
    registerIOPin(g, "hours",
            d->ih = new IOPin(this,
                QObject::tr("Hours", "ltl-qwt"), "number"));
    Q_CHECK_PTR(d->ih);

    registerIOPin(g, "mins",
            d->im = new IOPin(this,
                QObject::tr("Minutes", "ltl-qwt"), "number"));
    Q_CHECK_PTR(d->im);

    registerIOPin(g, "secs",
            d->is = new IOPin(this,
                QObject::tr("Seconds", "ltl-qwt"), "number"));
    Q_CHECK_PTR(d->is);
}

QWTAnalogClock::~QWTAnalogClock()
{
    delete d;
}

void QWTAnalogClock::localize( LocalizatorPtr localizator )
{
    Q_CHECK_PTR( d );
    d->localizator = localizator;
}

void QWTAnalogClock::drawShape( QPainter &p )
{
    p.save();

    const QPoint o = Template::rect().topLeft();
    p.translate( o.x(), o.y() );
    const QRect rr(QPoint(0, 0), Template::rect().size());

    QwtAnalogClock::drawContents(&p);
    QwtAnalogClock::drawFrame(&p);

    // p.setPen(QPen(Qt::red));
    // p.setBrush(QBrush());
    // p.drawRect(rr);

    // p.translate( -o.x(), -o.y() );
    // // p.setBrush(QBrush());
    // p.setPen(QPen(Qt::blue));
    // p.drawRect(Template::boundingRect());

    p.restore();
}

void QWTAnalogClock::propertiesChanged()
{
    Q_ASSERT(d);

    const PropertyDict &pm =(const PropertyDict&)properties();

    QFont f = pm["font"]->asFont();
    QwtAnalogClock::setFont(f);

    QColor c = pm["h-color"]->asColor();
    QwtDialNeedle* n = hand(QwtAnalogClock::HourHand);
    QColorGroup cg(c, c, c, c, c, c, c, c, c);
    n->setPalette(QPalette(cg, cg, cg));

    c = pm["m-color"]->asColor();
    n = hand(QwtAnalogClock::MinuteHand);
    cg = QColorGroup(c, c, c, c, c, c, c, c, c);
    n->setPalette(QPalette(cg, cg, cg));

    c = pm["s-color"]->asColor();
    n = hand(QwtAnalogClock::SecondHand);
    cg = QColorGroup(c, c, c, c, c, c, c, c, c);
    n->setPalette(QPalette(cg, cg, cg));


    // update and schedule repaint
    QWTAnalogClock::setRect(Template::rect());
}

void QWTAnalogClock::setRect( const QRect & rect )
{
    const QSize oldSize = Template::rect().size();

    Q_ASSERT(d);

    Template::setRect( rect );

    const QRect rr(QPoint(0, 0), rect.size());
    QwtAnalogClock::setGeometry(rr);
    {
        QResizeEvent rse( oldSize, rect.size());
        QwtAnalogClock::resizeEvent(&rse);
    }

    // and schedule repaint
    Template::update();
}

void QWTAnalogClock::iopinsChanged()
{
    Q_ASSERT(d);
    Q_ASSERT(d->ih);
    Q_ASSERT(d->im);
    Q_ASSERT(d->is);

    QwtAnalogClock::setTime(QTime(
                d->ih->realNumberValue(),
                d->im->realNumberValue(),
                d->is->realNumberValue()));
    Template::update();
}

Template * QWTAnalogClock::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new QWTAnalogClock( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTAnalogClock.cpp 844 2005-06-21 12:40:50Z mman $
