// $Id: Lift.cpp 950 2005-07-14 11:26:30Z modesto $
//
//   FILE: Lift.cpp --
// AUTHOR: Jiri Barton <jbar@swac.cz>
//   DATE: 12 September 2005
//
// Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
//
// This application is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License, or(at
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

#include "Lift.hh"
#include "Lift.xpm"

using namespace lt;

#include "config.h"

struct Lift::LiftPrivate
{
    LiftPrivate() : m_localizator( Localizator::nullPtr() ),
        m_imlift(NULL), m_imcar(NULL) {}
    ~LiftPrivate()
    {
        delete m_imcar;
        delete m_imlift;
    }

    LocalizatorPtr  m_localizator;
    QImage *m_imlift, *m_imcar;
    bool m_cargo;
    int m_pos;
    int m_top_limit, m_bottom_limit, m_platform_top, m_shaft_width,
        m_shaft_height, m_car_offset;
    QRect m_rectlift, m_rectcar;
};

Lift::Lift( LoggerPtr logger ) :
    Template( "Lift", logger ),
    d( new LiftPrivate() )
{
    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Jiri Barton <jbar@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Lift", "ltl-elevator" ) );
    i.setLongDescription( QObject::tr(
        "Lift", "ltl-elevator" ) );
    i.setIcon( QImage( (const char **) Lift_xpm ) );

    registerInfo( i );

    // create properties of this template
    d->m_pos = 0;
    d->m_cargo = false;

    // define groups
    QString general = QObject::tr("Graphics", "ltl-elevator");

    registerProperty( general, "image_platform",
            new Property( QObject::tr( "image_platform", "ltl-elevator" ),
                "resource", "", logger ) );
    registerProperty( general, "image_car",
            new Property( QObject::tr( "image_car", "ltl-elevator" ),
                "resource", "", logger ) );
    registerProperty( general, "top_limit",
            new Property( QObject::tr( "top_limit", "ltl-elevator" ),
                "integer", "10", logger ) );
    registerProperty( general, "bottom_limit",
            new Property( QObject::tr( "bottom_limit", "ltl-elevator" ),
                "integer", "100", logger ) );
    registerProperty( general, "platform_top",
            new Property( QObject::tr( "platform_top", "ltl-elevator" ),
                "integer", "5", logger ) );
    registerProperty( general, "top_limit",
            new Property( QObject::tr( "top_limit", "ltl-elevator" ),
                "integer", "10", logger ) );
    registerProperty( general, "shaft_width",
            new Property( QObject::tr( "shaft_width", "ltl-elevator" ),
                "integer", "10", logger ) );
    registerProperty( general, "shaft_height",
            new Property( QObject::tr( "shaft_height", "ltl-elevator" ),
                "integer", "110", logger ) );
    registerProperty( general, "car_offset",
            new Property( QObject::tr( "car_offset", "ltl-elevator" ),
                "integer", "10", logger ) );

    // create iopins of this template
    // define groups
    QString inp = QObject::tr("Inputs", "ltl-elevator");

    registerIOPin( inp, "pos",
            new IOPin( this, QObject::tr( "Position", "ltl-elevator" ),
                "number" ) );
    registerIOPin( inp, "cargo",
            new IOPin( this, QObject::tr( "Cargo", "ltl-elevator" ),
                "bit" ) );
}

Lift::~Lift()
{
    delete d;
}

void Lift::drawShape( QPainter & p )
{
    Q_ASSERT( d );
    p.save();
    p.drawImage(d->m_rectlift, *d->m_imlift);
    if (d->m_cargo)
        p.drawImage(d->m_rectcar, *d->m_imcar);
    p.restore();
}

void Lift::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    // shedule repaint
    update();
}

void Lift::propertiesChanged()
{
    Q_ASSERT( d );

    d->m_top_limit = properties()["top_limit"]->asInteger();
    d->m_bottom_limit = properties()["bottom_limit"]->asInteger();
    d->m_platform_top = properties()["platform_top"]->asInteger();
    d->m_shaft_width = properties()["shaft_width"]->asInteger();
    d->m_shaft_height = properties()["shaft_height"]->asInteger();
    d->m_car_offset = properties()["car_offset"]->asInteger();

    localize(d->m_localizator);

    reconstructPointArrayAndRegion();

    // fire off iopinsChanged to set up painting brush and pen correctly
    iopinsChanged();

    // shedule repaint
    update();
}

void Lift::iopinsChanged()
{
    Q_ASSERT( d );
    Q_ASSERT( iopins()[ "pos" ] );
    Q_ASSERT( iopins()[ "cargo" ] );

    d->m_pos = iopins()["pos"]->realNumberValue();
    d->m_cargo = iopins()["cargo"]->realBitValue();

    reconstructPointArrayAndRegion();

    // shedule repaint
    update();
}


void Lift::localize( LocalizatorPtr loc )
{
    Q_ASSERT( d );

    if( ! loc.isNull() ) {
        d->m_localizator = loc;
        QIODevice *dev = d->m_localizator->resourceAsIODevice(properties()
            ["image_platform"]->asString());
        if (dev)
        {
            delete d->m_imlift;
            d->m_imlift = new QImage(dev->readAll());
            delete dev;
        }
        else
            d->m_imlift = new QImage;
        dev = d->m_localizator->resourceAsIODevice(properties()
            ["image_car"]->asString());
        if (dev)
        {
            delete d->m_imcar;
            d->m_imcar = new QImage(dev->readAll());
            delete dev;
        }
        else
            d->m_imcar = new QImage;
    }
}

void Lift::reconstructPointArrayAndRegion()
{
    Q_ASSERT( d );
    if (d->m_imlift == NULL || d->m_imcar == NULL || d->m_bottom_limit == 0
        || d->m_shaft_width == 0 || d->m_shaft_height == 0)
    {
        d->m_rectlift.setRect(0, 0, 0, 0);
        d->m_rectcar.setRect(0, 0, 0, 0);
        return;
    }

    int platform_top_org = (d->m_bottom_limit - d->m_top_limit) *
        d->m_pos / 100;
    int platform_pos = rect().height() * platform_top_org /
        d->m_shaft_height + rect().y();
    d->m_rectlift.setRect(
        rect().x(),
        platform_pos + rect().height() * d->m_platform_top /
            d->m_shaft_height,
        rect().width(),
        rect().height() * d->m_imlift->height() / d->m_shaft_height);
    d->m_rectcar.setRect(
        rect().x() + rect().width() * d->m_car_offset / d->m_shaft_width,
        platform_pos,
        rect().width() * d->m_imcar->width() / d->m_shaft_width,
        rect().height() * d->m_imcar->height() / d->m_shaft_height);
}

Template * Lift::clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Lift( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Lift.cpp 950 2005-07-14 11:26:30Z modesto $
