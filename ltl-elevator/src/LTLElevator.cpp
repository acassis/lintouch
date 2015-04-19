// $Id:$
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


#include "LTLElevator.hh"

#include "lt/templates/PluginHelper.hh"
using namespace lt;

#include "Lift.hh"
#include "Conveyor.hh"
#include "ImageButton.hh"

#include "config.h"

// library private data
struct LTLElevator::LTLElevator_private
{

    LTLElevator_private() : lift( new Lift ), conveyor( new Conveyor ),
        imagebutton( new ImageButton )
    {
        Q_CHECK_PTR( lift );
        Q_CHECK_PTR( conveyor );
        Q_CHECK_PTR( imagebutton );
    }

    ~LTLElevator_private()
    {
        delete imagebutton;
        delete conveyor;
        delete lift;
    }

    Lift* lift;
    Conveyor* conveyor;
    ImageButton* imagebutton;
};

LTLElevator::LTLElevator() : TemplateLibrary(),
    d( new LTLElevator_private )
{
    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Jiri Barton <jbar@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Rollenbahn",
                "ltl-elevator" ) );
    i.setLongDescription( QObject::tr(
                "Templates for the rollenbahn simulation project",
                "ltl-elevator" ) );

    registerInfo( i );

    // register templates into this library
    registerTemplateInfo( d->lift->type(),  d->lift->info() );
    registerTemplateInfo( d->conveyor->type(),  d->conveyor->info() );
    registerTemplateInfo( d->imagebutton->type(),  d->imagebutton->info() );
}

LTLElevator::~LTLElevator()
{
    delete d;
}

Template * LTLElevator::instantiate( const QString & type,
                        LoggerPtr logger /*= Logger::nullPtr()*/ ) const
{
    Q_CHECK_PTR( d );
    if( type == d->lift->type() ) {
        return new Lift( logger );
    }
    if( type == d->conveyor->type() ) {
        return new Conveyor( logger );
    }
    if( type == d->imagebutton->type() ) {
        return new ImageButton( logger );
    }
    return NULL;
}

// library loader macro
EXPORT_LT_PLUGIN( LTLElevator, "lt::TemplateLibrary" );

// vim: set et ts=4 sw=4 tw=76:
// $Id:$
