// $Id: Conveyor.cpp 950 2005-07-14 11:26:30Z modesto $
//
//   FILE: Conveyor.cpp --
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

#include "Conveyor.hh"
#include "Conveyor.xpm"

using namespace lt;

#include "config.h"

struct Conveyor::ConveyorPrivate
{
    ConveyorPrivate() : m_localizator( Localizator::nullPtr() ),
        m_imcar(NULL), m_cargo(false), m_pos(0)
    {
    }

    ~ConveyorPrivate()
    {
        delete m_imcar;
    }

    LocalizatorPtr  m_localizator;
    QImage *m_imcar;
    bool m_cargo;
    int m_pos;
    int m_shaft_width;
    QRect m_rectcar;
};

Conveyor::Conveyor( LoggerPtr logger ) :
    Template( "Conveyor", logger ),
    d( new ConveyorPrivate() )
{
    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Jiri Barton <jbar@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr("Conveyor", "ltl-elevator" ) );
    i.setLongDescription( QObject::tr(
        "The left/right moving part of the elevator demo,"
       "optionally transporting a car", "ltl-elevator" ) );
    i.setIcon( QImage( (const char **) Conveyor_xpm ) );

    registerInfo( i );

    // define groups
    QString general = QObject::tr("Graphics", "ltl-elevator");

    registerProperty( general, "shaft_width",
            new Property( QObject::tr( "shaft_width", "ltl-elevator" ),
                "integer", "10", logger ) );
    registerProperty( general, "image_car",
            new Property( QObject::tr( "image_car", "ltl-elevator" ),
                "resource", "", logger ) );

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

Conveyor::~Conveyor()
{
    delete d;
}

void Conveyor::drawShape( QPainter & p )
{
    Q_ASSERT( d );
    if (d->m_cargo)
    {
        p.save();
        p.drawImage(d->m_rectcar, *d->m_imcar);
        p.restore();
    }
}

void Conveyor::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    // shedule repaint
    update();
}

void Conveyor::propertiesChanged()
{
    Q_ASSERT( d );

    d->m_shaft_width = properties()["shaft_width"]->asInteger();

    localize(d->m_localizator);

    reconstructPointArrayAndRegion();

    // fire off iopinsChanged to set up painting brush and pen correctly
    iopinsChanged();

    // shedule repaint
    update();
}

void Conveyor::iopinsChanged()
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


void Conveyor::localize( LocalizatorPtr loc )
{
    Q_ASSERT( d );
    if( ! loc.isNull() ) {
        d->m_localizator = loc;
        QIODevice *dev = d->m_localizator->resourceAsIODevice(properties()
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

void Conveyor::reconstructPointArrayAndRegion()
{
    Q_ASSERT( d );
    if (d->m_imcar == NULL || d->m_shaft_width == 0 || !d->m_cargo)
    {
        d->m_rectcar.setRect(0, 0, 0, 0);
        return;
    }

    int carwidth = rect().width() * d->m_imcar->width() / d->m_shaft_width;
    int carpos = (rect().width() - carwidth) * d->m_pos / 100;
    d->m_rectcar.setRect(
        rect().x() + carpos,
        rect().y(),
        carwidth,
        rect().height());
}

Template * Conveyor::clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Conveyor( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Conveyor.cpp 950 2005-07-14 11:26:30Z modesto $
