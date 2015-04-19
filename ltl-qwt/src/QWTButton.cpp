// $Id: QWTButton.cpp 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTButton.cpp -- 
// AUTHOR: patrik <patrik@modesto.cz>
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

#include "QWTButton.hh"
#include "QWTButton.xpm"

#include "config.h"

#define QDEBUG_RECT( r ) qDebug(QString(#r": %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct QWTButton::QWTButtonPrivate
{
    QWTButtonPrivate() :
        localizator( LocalizatorPtr( new Localizator ) ),
        focused( false ), output( NULL )
    {}

    QRegion collisionRegion;
    QPointArray areaPoints;

    LocalizatorPtr localizator;

    bool focused;

    IOPin* output;

    QPointArray points;
    QFont f;
};

Info QWTButton::makeInfo()
{
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "Button", "ltl-qwt" ) );
    i.setLongDescription ( QObject::tr(
                "Qwt Button Widget", "ltl-qwt" ) );
    i.setIcon(QImage(QWTButton_xpm));
    return i;
}

const QString QWTButton::TYPE = "Button";

QWTButton::QWTButton( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    QwtPushButton(0, TYPE),
    Template( TYPE, logger ),
    d( new QWTButtonPrivate )
{
    Q_CHECK_PTR( d );

    //register info
    registerInfo( makeInfo() );

    QString g = QObject::tr("Appearance", "ltl-qwt");
    registerProperty( g, "label",
            new Property( QObject::tr("Label", "ltl-qwt"),
                "string", "<label>", logger ) );

    registerProperty( g, "label-font",
            new Property( QObject::tr("Font", "ltl-qwt"),
                "font", "", logger ) );

    registerProperty( g, "label-color",
            new Property( QObject::tr("Color", "ltl-qwt"),
                "color", "", logger ) );



    // [-- IOPins --]
    registerIOPin(QObject::tr("Outputs", "ltl-qwt"), "output",
            d->output = new IOPin(this,
                QObject::tr("Output", "ltl-qwt"), "bit"));
    Q_CHECK_PTR(d->output);

    //FIXME
    setText("QwtPushButton");
}

QWTButton::~QWTButton()
{
    delete d;
}

void QWTButton::localize( LocalizatorPtr localizator )
{
    Q_CHECK_PTR( d );
    d->localizator = localizator;
}

void QWTButton::mousePressEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtPushButton::mousePressEvent(&e2);
    QwtPushButton::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedBitValue(QwtPushButton::isDown());
    syncIOPinsRequestedValues();
}

void QWTButton::mouseReleaseEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    const QPoint o = e->pos() - Template::rect().topLeft();
    QMouseEvent e2(QEvent::MouseButtonPress, o, e->button(), e->state());

    QwtPushButton::mouseReleaseEvent(&e2);
    QwtPushButton::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedBitValue(QwtPushButton::isDown());
    syncIOPinsRequestedValues();
}

void QWTButton::keyPressEvent( QKeyEvent * e )
{
    QwtPushButton::keyPressEvent(e);
    QwtPushButton::update();
    Template::update();
}

void QWTButton::keyReleaseEvent( QKeyEvent * e )
{
    QwtPushButton::keyReleaseEvent(e);
    QwtPushButton::update();
    Template::update();

    Q_ASSERT(d->output);
    d->output->setRequestedBitValue(QwtPushButton::isDown());
    syncIOPinsRequestedValues();
}

QWidget::FocusPolicy QWTButton::focusPolicy() const
{
    return QWidget::StrongFocus;
}

void QWTButton::focusInEvent ( QFocusEvent* e )
{
    Q_ASSERT(d);
    d->focused = true;

    Template::focusInEvent(e);
    QwtPushButton::focusInEvent(e);
    QwtPushButton::update();
    Template::update();
}

void QWTButton::focusOutEvent ( QFocusEvent* e )
{
    Q_ASSERT(d);
    d->focused = false;

    Template::focusOutEvent(e);
    QwtPushButton::focusOutEvent(e);
    QwtPushButton::update();
    Template::update();
}

void QWTButton::drawShape( QPainter &p )
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

    p.setFont(d->f);

    QwtPushButton::drawButton(&p);

    p.setPen(QPen(Qt::red));
    p.setBrush(QBrush());
    p.drawRect(rr);

    p.translate( -o.x(), -o.y() );
    // p.setBrush(QBrush());
    p.setPen(QPen(Qt::blue));
    p.drawRect(Template::boundingRect());

    p.restore();
}

void QWTButton::propertiesChanged()
{
    const PropertyDict &pm =(const PropertyDict&)properties();

    setText(pm["label"]->asString());
    d->f = pm["label-font"]->asFont();

    QColor c = pm["label-color"]->asColor();
    QPalette pal(palette());
    pal.setColor(QPalette::Inactive, QColorGroup::ButtonText, c);
    pal.setColor(QPalette::Active, QColorGroup::ButtonText, c);
    pal.setColor(QPalette::Disabled, QColorGroup::ButtonText, c);
    QwtPushButton::setPalette(pal);

    Template::update();
}

void QWTButton::setRect( const QRect & rect )
{
    const QSize oldSize = Template::rect().size();

    Q_ASSERT(d);

    Template::setRect( rect );

    const QRect rr(0, 0, rect.width(), rect.height());
    QwtPushButton::setGeometry(rr);
    {
        QResizeEvent rse( oldSize, rect.size());
        QwtPushButton::resizeEvent(&rse);
    }
    d->points = rect;

    // and schedule repaint
    Template::update();
}

QPointArray QWTButton::areaPoints() const
{
    Q_ASSERT(d);
    return d->points;
}

Template * QWTButton::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new QWTButton( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTButton.cpp 844 2005-06-21 12:40:50Z mman $
