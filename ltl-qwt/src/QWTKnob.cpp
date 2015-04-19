// $Id: QWTKnob.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTKnob.cpp -- 
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

#include "QWTKnob.hh"
#include "QWTKnob.xpm"

#include "config.h"

#define QDEBUG_RECT(r, t) qDebug(QString("%1::"#r": x:%2 y:%3 w:%4 h:%5") \
        .arg((int)t,0,16).arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct QWTKnob::QWTKnobPrivate
{
    QWTKnobPrivate() :
        localizator(LocalizatorPtr(new Localizator)), output(NULL)
    {}

    LocalizatorPtr localizator;

    IOPin* output;

    QFont f;
    QColor c;
};

Info QWTKnob::makeInfo()
{
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "Knob", "ltl-qwt" ) );
    i.setLongDescription ( QObject::tr(
                "Qwt Knob Widget", "ltl-qwt" ) );
    i.setIcon(QImage(QWTKnob_xpm));
    return i;
}

const QString QWTKnob::TYPE = "Knob";

QWTKnob::QWTKnob(LoggerPtr logger /*= Logger::nullPtr()*/) :
    QwtKnob(0, TYPE),
    Template( TYPE, logger ),
    d(new QWTKnobPrivate)
{
    Q_CHECK_PTR(d);

    //register info
    registerInfo( makeInfo() );

    QString g = QObject::tr("Appearance","ltl-qwt");

    registerProperty( g, "font",
            new Property( QObject::tr("Font", "ltl-qwt"),
                "font", "", logger ) );

    registerProperty( g, "color",
            new Property( QObject::tr("Color", "ltl-qwt"),
                "color", "", logger ) );

    registerProperty( g, "angle",
            new Property( QObject::tr("Total Angle", "ltl-qwt"),
                "float", "270.0", logger ) );

    registerProperty( g, "border",
            new Property( QObject::tr("Border Width", "ltl-qwt"),
                "integer", "2", logger ) );

    registerProperty( g, "scale-step",
            new Property( QObject::tr("Increment", "ltl-qwt"),
                "float", "5", logger ) );

    g = QObject::tr("Scale","ltl-qwt");
    registerProperty( g, "min",
            new Property( QObject::tr("Lower Limit", "ltl-qwt"),
                "float", "0", logger ) );

    registerProperty( g, "max",
            new Property( QObject::tr("Upper Limit", "ltl-qwt"),
                "float", "100.0", logger ) );

    registerProperty( g, "step",
            new Property( QObject::tr("Step Size", "ltl-qwt"),
                "float", "10", logger ) );

    // [-- IOPins --]
    registerIOPin(QObject::tr("Outputs", "ltl-qwt"), "output",
            d->output = new IOPin(this,
                QObject::tr("Output", "ltl-qwt"), "number"));
    Q_CHECK_PTR(d->output);
}

QWTKnob::~QWTKnob()
{
    delete d;
}

void QWTKnob::localize(LocalizatorPtr localizator)
{
    Q_CHECK_PTR(d);
    d->localizator = localizator;
}

void QWTKnob::drawShape(QPainter &p)
{
    p.save();

    const QPoint o = Template::rect().topLeft();
    p.translate(o.x(), o.y());
    const QRect rr(QPoint(0, 0), Template::rect().size());

    p.setPen(d->c);
    p.setFont(d->f);

    QwtKnob::draw(&p, rr);

    p.restore();
}

void QWTKnob::propertiesChanged()
{
    Q_ASSERT(d);

    const PropertyDict &pm =(const PropertyDict&)properties();

    d->f = pm["font"]->asFont();
    d->c = pm["color"]->asColor();

    QwtKnob::setScale(pm["min"]->asFloatingPoint(),
            pm["max"]->asFloatingPoint(),
            pm["step"]->asFloatingPoint());

    QwtKnob::setRange(pm["min"]->asFloatingPoint(),
            pm["max"]->asFloatingPoint(),
            pm["scale-step"]->asFloatingPoint());

    QwtKnob::setTotalAngle(pm["angle"]->asFloatingPoint());
    QwtKnob::setBorderWidth(pm["border"]->asInteger());

    // update and schedule repaint
    QWTKnob::setRect(Template::rect());
}

void QWTKnob::setRect(const QRect & rect)
{
    const QSize oldSize = Template::rect().size();

    Q_ASSERT(d);

    setKnobWidth(0);

    Template::setRect(rect);

    const QRect rr(QPoint(0, 0), rect.size());
    QwtKnob::setGeometry(rr);
    {
        QResizeEvent rse(oldSize, rect.size());
        QwtKnob::resizeEvent(&rse);
    }

    const QSize s = sizeHint();
    setKnobWidth(QMIN( rect.width() - s.width(), rect.height() - s.height()));

    // and schedule repaint
    Template::update();
}

void QWTKnob::mousePressEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtKnob::mousePressEvent(&e2);
    QwtKnob::update();
    Template::update();
}

void QWTKnob::mouseReleaseEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtKnob::mouseReleaseEvent(&e2);
    QwtKnob::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedNumberValue((int)QwtKnob::value());
    syncIOPinsRequestedValues();
}

void QWTKnob::mouseMoveEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtKnob::mouseMoveEvent(&e2);
    QwtKnob::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedNumberValue((int)QwtKnob::value());
    syncIOPinsRequestedValues();
}

void QWTKnob::keyPressEvent( QKeyEvent * e )
{
    QwtKnob::keyPressEvent(e);
    QwtKnob::update();
    Template::update();
}

void QWTKnob::keyReleaseEvent( QKeyEvent * e )
{
    QwtKnob::keyReleaseEvent(e);
    QwtKnob::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedNumberValue((int)QwtKnob::value());
    syncIOPinsRequestedValues();
}

QWidget::FocusPolicy QWTKnob::focusPolicy() const
{
    return QWidget::StrongFocus;
}

void QWTKnob::focusInEvent ( QFocusEvent* e )
{
    Q_ASSERT(d);

    Template::focusInEvent(e);
    QwtKnob::focusInEvent(e);
    QwtKnob::update();
    Template::update();
}

void QWTKnob::focusOutEvent ( QFocusEvent* e )
{
    Q_ASSERT(d);

    Template::focusOutEvent(e);
    QwtKnob::focusOutEvent(e);
    QwtKnob::update();
    Template::update();
}


Template * QWTKnob::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */) const
{
    Template * result = new QWTKnob(logger);
    setupAfterClone(result, loc, tm);
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTKnob.cpp 844 2005-06-21 12:40:50Z mman $
