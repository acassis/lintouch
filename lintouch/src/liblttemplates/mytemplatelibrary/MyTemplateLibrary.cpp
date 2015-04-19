// $Id: MyTemplateLibrary.cpp 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: MyTemplateLibrary.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 09 October 2003
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

#include "lt/templates/PluginHelper.hh"

#include "Lampa.hh"
#include "Buttona.hh"
#include "Imagea.hh"
#include "SimpleTrafficLights.hh"
#include "MyTemplateLibrary.hh"
using namespace lt;

#include "config.h"

struct MyTemplateLibrary::MyTemplateLibraryPrivate {

    MyTemplateLibraryPrivate() :
    lamp( new Lampa() ),
    button( new Buttona() ),
    image( new Imagea() ),
    tlights( new SimpleTrafficLights() )
    {
    }

    ~MyTemplateLibraryPrivate()
    {
        delete lamp;
        delete button;
        delete image;
        delete tlights;
    }

    Template * lamp;
    Template * button;
    Template * image;
    Template * tlights;

};

MyTemplateLibrary::MyTemplateLibrary() : TemplateLibrary(),
d( new MyTemplateLibraryPrivate() ) {

    //register info
    Info i;
    i.setAuthor( "Martin Man <mman@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "My Template Library" ) );
    i.setLongDescription( QObject::tr(
            "Template Library for demonstration purposes" ) );

    registerInfo( i );

    // register templates into this library
    registerTemplateInfo( d->lamp->type(), d->lamp->info() );
    registerTemplateInfo( d->button->type(), d->button->info() );
    registerTemplateInfo( d->image->type(), d->image->info() );
    registerTemplateInfo( d->tlights->type(), d->tlights->info() );
}

MyTemplateLibrary::~MyTemplateLibrary() {
    delete d;
}

Template * MyTemplateLibrary::instantiate(
        const QString & type,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const {

    if( type == d->lamp->type() ) {
        return new Lampa( logger );
    } else if( type == d->button->type() ) {
        return new Buttona( logger );
    } else if( type == d->image->type() ) {
        return new Imagea( logger );
    } else if( type == d->tlights->type() ) {
        return new SimpleTrafficLights( logger );
    }
    return NULL;
}

// create dll helper methods
EXPORT_LT_PLUGIN( MyTemplateLibrary, "lt::TemplateLibrary" );

// vim: set et ts=4 sw=4 tw=76:
// $Id: MyTemplateLibrary.cpp 1774 2006-05-31 13:17:16Z hynek $
