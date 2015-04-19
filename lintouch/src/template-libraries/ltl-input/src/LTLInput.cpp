// $Id: LTLInput.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: LTLInput.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 04 November 2003
//
// Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
//
// THIS SOFTWARE IS DISTRIBUTED AS IS AND WITHOUT ANY WARRANTY.
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


#include "LTLInput.hh"

#include "lt/templates/PluginHelper.hh"
using namespace lt;

#include "Button.hh"
#include "LineInput.hh"
#include "Slider.hh"

#include "config.h"

// library private data
struct LTLInput::LTLInput_private
{

    LTLInput_private() :
        button( new Button() ), linput( new LineInput() ), slider( new Slider() )
    {
        Q_CHECK_PTR( button );
        Q_CHECK_PTR( linput );
        Q_CHECK_PTR( slider );
    }

    ~LTLInput_private()
    {
        delete button;
        delete linput;
        delete slider;
    }

    Button* button;
    LineInput* linput;
    Slider* slider;
};

LTLInput::LTLInput() : TemplateLibrary(),
    d( new LTLInput_private )
{
    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Input", "ltl-input" ) );
    i.setLongDescription( QObject::tr( "Input", "ltl-input" ) );

    registerInfo( i );

    // register templates into this library
    registerTemplateInfo( d->button->type(),   d->button->info() );
    registerTemplateInfo( d->linput->type(),  d->linput->info() );
    registerTemplateInfo( d->slider->type(),  d->slider->info() );
}

LTLInput::~LTLInput()
{

    if( d ) {
        delete d;
    }
}

Template * LTLInput::instantiate( const QString & type,
                        LoggerPtr logger /*= Logger::nullPtr()*/ ) const
{
    Q_CHECK_PTR( d );

    if( type == d->button->type() ) {
        return new Button( logger );
    } else if( type == d->linput->type() ) {
        return new LineInput( logger );
    } else if( type == d->slider->type() ) {
        return new Slider( logger );
    }
    return NULL;
}

// library loader macro
EXPORT_LT_PLUGIN( LTLInput, "lt::TemplateLibrary" );

// vim: set et ts=4 sw=4 tw=76:
// $Id: LTLInput.cpp 1168 2005-12-12 14:48:03Z modesto $
