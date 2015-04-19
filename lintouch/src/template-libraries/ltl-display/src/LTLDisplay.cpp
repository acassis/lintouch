// $Id: LTLDisplay.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: LTLDisplay.cpp -- 
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


#include "LTLDisplay.hh"

#include "lt/templates/PluginHelper.hh"
using namespace lt;

#include "Text.hh"
#include "Image.hh"
#include "SVGImage.hh"
#include "Lamp.hh"
#include "Meter.hh"
#include "Number.hh"
#include "Chart.hh"
#include "TextMap.hh"
#include "Tester.hh"

#include "config.h"

// library private data
struct LTLDisplay::LTLDisplay_private
{

    LTLDisplay_private() :
        text( new Text() ), image( new Image() ), svgimage( new SVGImage() ),
        number( new Number() ), meter( new Meter() ), chart( new Chart() ),
        textmap( new TextMap() ), tester( new Tester() )
    {
        rectangle = new Lamp("Rectangle");
        ellipse = new Lamp("Ellipse");
        triangle = new Lamp("Triangle");
        line = new Lamp("Line");
        parallelogram = new Lamp("Parallelogram");
        roundrect = new Lamp("Roundrect");
        semicircle = new Lamp("Semicircle");
        arrow = new Lamp("Arrow");


        Q_CHECK_PTR( text );
        Q_CHECK_PTR( image );
        Q_CHECK_PTR( svgimage );

        Q_CHECK_PTR( rectangle );
        Q_CHECK_PTR( ellipse );
        Q_CHECK_PTR( triangle );
        Q_CHECK_PTR( line );
        Q_CHECK_PTR( parallelogram );
        Q_CHECK_PTR( roundrect );
        Q_CHECK_PTR( semicircle );
        Q_CHECK_PTR( arrow );

        Q_CHECK_PTR( number );
        Q_CHECK_PTR( meter );
        Q_CHECK_PTR( chart );

        Q_CHECK_PTR( textmap );
        Q_CHECK_PTR( tester );
    }

    ~LTLDisplay_private()
    {
        delete text;
        delete image;
        delete svgimage;

        delete rectangle;
        delete ellipse;
        delete triangle;
        delete line;
        delete parallelogram;
        delete roundrect;
        delete semicircle;
        delete arrow;

        delete number;
        delete meter;
        delete chart;

        delete textmap;
        delete tester;
    }

    Text* text;
    Image* image;
    SVGImage* svgimage;

    Lamp* rectangle;
    Lamp* ellipse;
    Lamp* triangle;
    Lamp* line;
    Lamp* parallelogram;
    Lamp* roundrect;
    Lamp* semicircle;
    Lamp* arrow;

    Number* number;
    Meter* meter;
    Chart* chart;

    TextMap* textmap;
    Tester* tester;
};

LTLDisplay::LTLDisplay() : TemplateLibrary(),
    d( new LTLDisplay_private )
{
    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Display", "ltl-display" ) );
    i.setLongDescription( QObject::tr( "Display", "ltl-display" ) );

    registerInfo( i );

    // register templates into this library
    registerTemplateInfo( d->text->type(),   d->text->info() );
    registerTemplateInfo( d->image->type(),  d->image->info() );
    registerTemplateInfo( d->svgimage->type(),  d->svgimage->info() );

    // splitted Lamp
    registerTemplateInfo( "Lamp", d->rectangle->info());
    registerTemplateInfo( "Ellipse", d->ellipse->info());
    registerTemplateInfo( "Triangle", d->triangle->info());
    registerTemplateInfo( "Line", d->line->info());
    registerTemplateInfo( "Parallelogram", d->parallelogram->info());
    registerTemplateInfo( "Roundrect", d->roundrect->info());
    registerTemplateInfo( "Semicircle", d->semicircle->info());
    registerTemplateInfo( "Arrow", d->arrow->info());

    registerTemplateInfo( d->number->type(),   d->number->info() );
    registerTemplateInfo( d->meter->type(),  d->meter->info() );
    registerTemplateInfo( d->chart->type(),  d->chart->info() );

    registerTemplateInfo( d->textmap->type(),  d->textmap->info() );
    registerTemplateInfo( d->tester->type(),  d->tester->info() );
}

LTLDisplay::~LTLDisplay()
{

    if( d ) {
        delete d;
    }
}

Template * LTLDisplay::instantiate( const QString & type,
                        LoggerPtr logger /*= Logger::nullPtr()*/ ) const
{
    Q_CHECK_PTR( d );
    if( type == d->text->type() ) {
        return new Text( logger );
    } else if( type == d->image->type() ) {
        return new Image( logger );
    } else if( type == d->svgimage->type() ) {
        return new SVGImage( logger );
    } else if( type == "Rectangle" || type == "Lamp" || type == "Ellipse" ||
        type == "Triangle" || type == "Line" || type == "Parallelogram" ||
        type == "Roundrect" || type == "Semicircle" || type == "Arrow" ) {
        return new Lamp(type);
    } else if( type == d->number->type() ) {
        return new Number( logger );
    } else if( type == d->meter->type() ) {
        return new Meter( logger );
    } else if( type == d->chart->type() ) {
        return new Chart( logger );
    } else if( type == d->textmap->type() ) {
        return new TextMap( logger );
    } else if( type == d->tester->type() ) {
        return new Tester( logger );
    }
    return NULL;
}

// library loader macro
EXPORT_LT_PLUGIN( LTLDisplay, "lt::TemplateLibrary" );

// vim: set et ts=4 sw=4 tw=76:
// $Id: LTLDisplay.cpp 1168 2005-12-12 14:48:03Z modesto $
