// $Id: QWTScale.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTScale.cpp -- 
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

#include "QWTScale.hh"
#include "QWTScale.xpm"

#include "config.h"

#define QDEBUG_RECT(r, t) qDebug(QString("%1::"#r": x:%2 y:%3 w:%4 h:%5") \
        .arg((int)t,0,16).arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct QWTScale::QWTScalePrivate
{
    QWTScalePrivate() :
        localizator( LocalizatorPtr( new Localizator ) ) {}

    LocalizatorPtr localizator;

    QFont f;
    QColor c;

    QPointArray points;
    QRegion region;
};

Info QWTScale::makeInfo()
{
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "Scale", "ltl-qwt" ) );
    i.setLongDescription ( QObject::tr(
                "Qwt Scale Widget", "ltl-qwt" ) );
    i.setIcon(QImage(QWTScale_xpm));
    return i;
}

const QString QWTScale::TYPE = "Scale";

QWTScale::QWTScale( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    QwtScaleDraw(),
    Template( TYPE, logger ),
    d( new QWTScalePrivate )
{
    Q_CHECK_PTR( d );

    //register info
    registerInfo( makeInfo() );

    QString g = QObject::tr("Appearance","ltl-qwt");

    registerProperty( g, "font",
            new Property( QObject::tr("Font", "ltl-qwt"),
                "font", "", logger ) );

    registerProperty( g, "color",
            new Property( QObject::tr("Color", "ltl-qwt"),
                "color", "", logger ) );

    registerProperty( g, "orientation",
            new Property( QObject::tr("Orientation", "ltl-qwt"),
                "choice(Left,Right,Top,Bottom)", "Bottom", logger ) );

    registerProperty( g, "logarithmic",
            new Property( QObject::tr("Logarithmic", "ltl-qwt"),
                "choice(yes,no)", "no", logger ) );


    g = QObject::tr("Scale","ltl-qwt");
    registerProperty( g, "min",
            new Property( QObject::tr("Lower Limit", "ltl-qwt"),
                "float", "0", logger ) );

    registerProperty( g, "max",
            new Property( QObject::tr("Upper Limit", "ltl-qwt"),
                "float", "100", logger ) );

    registerProperty( g, "step-size",
            new Property( QObject::tr("Step Size", "ltl-qwt"),
                "float", "10", logger ) );

#if 1
    registerProperty( g, "major-steps",
            new Property( QObject::tr("Max. Major Step Intervals", "ltl-qwt"),
                "Integer", "0", logger ) );

    registerProperty( g, "minor-steps",
            new Property( QObject::tr("Max. Minor Step Intervals", "ltl-qwt"),
                "Integer", "10", logger ) );

#endif

    g = QObject::tr("Label Format","ltl-qwt");
    registerProperty( g, "format",
            new Property( QObject::tr("Format", "ltl-qwt"),
                "choice(automatic,1.0+e02,100.0)", "automatic", logger ) );

    registerProperty( g, "precision",
            new Property( QObject::tr("Precision", "ltl-qwt"),
                "integer", "4", logger ) );

    registerProperty( g, "fieldwidth",
            new Property( QObject::tr("Field Width", "ltl-qwt"),
                "integer", "0", logger ) );

    registerProperty( g, "alignment",
            new Property( QObject::tr("Alignment", "ltl-qwt"),
                "choice(Left,Right,Top,Bottom)", "Bottom", logger ) );

    // [-- we can't find when a label goes out of the current bounding ->
    // artefacts --]
    // registerProperty( g, "rotation",
    //         new Property( QObject::tr("Rotation", "ltl-qwt"),
    //             "integer", "0", logger ) );
}

QWTScale::~QWTScale()
{
    delete d;
}

void QWTScale::localize( LocalizatorPtr localizator )
{
    Q_CHECK_PTR( d );
    d->localizator = localizator;
}

void QWTScale::drawShape( QPainter &p )
{
    p.save();

    p.setPen(d->c);
    p.setFont(d->f);

    const QPoint o = Template::rect().topLeft();
    p.translate( o.x(), o.y() );
    const QRect rr(0, 0, Template::rect().width(), Template::rect().height());

    QwtScaleDraw::draw(&p);

    // p.setPen(QPen(Qt::red));
    // p.setBrush(QBrush());
    // p.drawRect(rr);

    // p.translate( -o.x(), -o.y() );
    // // p.setBrush(QBrush());
    // p.setPen(QPen(Qt::blue));
    // p.drawRect(boundingRect());

    p.restore();
}

void QWTScale::propertiesChanged()
{
    Q_ASSERT(d);

    const PropertyDict &pm =(const PropertyDict&)properties();

    d->f = pm["font"]->asFont();
    d->c = pm["color"]->asColor();

    QString so = pm["orientation"]->asString();
    QwtScaleDraw::Orientation o;
    if(so == "Top") {
        o = QwtScaleDraw::Top;
    } else if(so == "Left") {
        o = QwtScaleDraw::Left;
    } else if(so == "Right") {
        o = QwtScaleDraw::Right;
    } else { // bottom
        o = QwtScaleDraw::Bottom;
    }
    setRect(rect(), o);

    double min = pm["min"]->asFloatingPoint();
    double max = pm["max"]->asFloatingPoint();
    int minorsteps = pm["minor-steps"]->asInteger();
    int majorsteps = pm["major-steps"]->asInteger();
    double stepsize = pm["step-size"]->asFloatingPoint();
    int log = (pm["logarithmic"]->asString() == "yes") ? 1 : 0;
    QwtScaleDraw::setScale(min, max, majorsteps, minorsteps, stepsize, log);

    int fieldwidth = pm["fieldwidth"]->asInteger();
    int precision = pm["precision"]->asInteger();
    QString fmt = pm["format"]->asString();
    if(fmt == "1.0+e02") {
        fmt = 'e';
    } else if(fmt == "100.0") {
        fmt = 'f';
    } else {
        fmt = 'g';
    }
    QwtScaleDraw::setLabelFormat(fmt[0].latin1(), precision, fieldwidth);

    so = pm["alignment"]->asString();
    QwtScaleDraw::Orientation a;
    if(so == "Top") {
        a = QwtScaleDraw::Top;
    } else if(so == "Left") {
        a = QwtScaleDraw::Left;
    } else if(so == "Right") {
        a = QwtScaleDraw::Right;
    } else { // bottom
        a = QwtScaleDraw::Bottom;
    }
    QwtScaleDraw::setLabelAlignment(a);

    // [-- we can't find when a label goes out of the current bounding ->
    // artefacts --]
    // int rotation = pm["rotation"]->asInteger();
    // QwtScaleDraw::setLabelRotation((double)rotation);

    // update and schedule repaint
    // setRect(rect(), o);
    Template::update();
}

void QWTScale::setRect( const QRect & rect )
{
    Template::setRect( rect );

    setRect(rect, orientation());

    // and schedule repaint
    Template::update();
}

void QWTScale::setRect(const QRect& rect, QwtScaleDraw::Orientation o)
{
    invalidate();

    int boundingX = rect.x(), boundingY = rect.y();

    int scaleLength = 0;
    int geometryX = 0, geometryY = 0;

    // 1. set the new geometry
    switch(o) {
        case QwtScaleDraw::Left:
        case QwtScaleDraw::Right:
            scaleLength =  rect.height();
            break;

        case QwtScaleDraw::Top:
        case QwtScaleDraw::Bottom:
            scaleLength = rect.width();
            break;

        default:
            // we don't support Round scale here
            break;
    }
    QwtScaleDraw::setGeometry(geometryX, geometryY, scaleLength, o);

    // 2. compute new bounding rect
    bool g = false;
    int start, end, boundingW, boundingH;
    {
        QFontMetrics fm(d->f);
        QPen p(d->c);
        QwtScaleDraw::minBorderDist(fm, start, end);
        // qWarning("start: %d, end: %d", start, end);

        boundingW = QwtScaleDraw::maxWidth(p, fm);
        boundingH = QwtScaleDraw::maxHeight(p, fm);

        // qWarning("minLabelDist: %d", minLabelDist(fm));
    }
    switch(o) {
        case QwtScaleDraw::Left:
            geometryX = boundingW;
            ++boundingW;
            g = true;
        case QwtScaleDraw::Right:
            boundingY = rect.y() - start;
            break;

        case QwtScaleDraw::Top:
            geometryY = boundingH;
            ++boundingH;
            g = true;
        case QwtScaleDraw::Bottom:
            boundingX = rect.x() - start;
            break;

        default:
            // we don't support Round scale here
            break;
    }
    if(g) {
        QwtScaleDraw::setGeometry(geometryX, geometryY, scaleLength, o);
    }

    const QRect r = QRect(boundingX, boundingY,
            boundingW + start, boundingH + start);
    d->points = QPointArray(r);
    d->region = QRegion(d->points);
}

QPointArray QWTScale::areaPoints() const
{
    return d->points;
}

QRegion QWTScale::collisionRegion() const
{
    return d->region;
}


Template * QWTScale::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new QWTScale( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTScale.cpp 844 2005-06-21 12:40:50Z mman $
