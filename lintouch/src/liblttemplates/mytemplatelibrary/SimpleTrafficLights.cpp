// $Id: SimpleTrafficLights.cpp,v 1.11 2004/12/01 12:09:25 mman Exp $
//
//   FILE: SimpleTrafficLights.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 11 October 2004
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

#include "SimpleTrafficLights.hh"
using namespace lt;

#include "SimpleTrafficLights.xpm"

#include "config.h"

SimpleTrafficLights::SimpleTrafficLights( LoggerPtr logger ) :
Template( "SimpleTrafficLights", logger )
{
    //register info
    Info i;
    i.setAuthor( "Martin Man <mman@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "SimpleTrafficLights" ) );
    i.setLongDescription( QObject::tr(
            "SimpleTrafficLights with one input iopin" ) );
    i.setIcon( QImage( (const char **) SimpleTrafficLights_xpm ) );

    registerInfo( i );

    // create iopins of this template
    registerIOPin( QObject::tr( "Inputs" ), "input",
            new IOPin( this, QObject::tr( "Input" ), "bit" ) );

    // request two lamps, one for red light and one for green light
    requestNestedTemplate( "red", "Lamp", "mytemplatelibrary" );
    requestNestedTemplate( "green", "Lamp", "mytemplatelibrary" );
}

SimpleTrafficLights::~SimpleTrafficLights()
{
}

void SimpleTrafficLights::drawShape( QPainter & p )
{
    //qWarning( "SimpleTrafficLights::drawShape: %d,%d %dx%d",
    //        rect().left(), rect().top(), rect().width(), rect().height() );

    p.save();

    p.setPen( QPen( QColor( "black" ) ) );
    p.setBrush( QBrush() );

    p.drawRect( rect() );

    p.restore();
}

void SimpleTrafficLights::compose( const TemplateManager & tm )
{
    //qWarning( "SimpleTrafficLights::compose" );
    Template::compose( tm );

    m_red = nestedTemplate( "red" );
    m_green = nestedTemplate( "green" );

    // if we were unable to get Lamp templates, get at least default
    // templates which paint itself black on white
    if( m_red == NULL ) {
        qWarning( "SimpleTrafficLights::compose didn't get required template" );
        m_red = new Template( "dummy", getLoggerPtr() );
        registerNestedTemplate( "red", m_red );
    } else {
        // set fixed properties for red and green nested templates
        m_red->properties()["shape"]->setChoiceValue( "ellipse" );
        m_red->properties()["fill_off"]->setBrushValue(
                QBrush( QColor( "#500000" ) ) );
        m_red->properties()["fill_on"]->setBrushValue(
                QBrush( QColor( "#FF0000" ) ) );
        m_red->propertiesChanged();
        m_red->iopins()[ "input" ]->setRealBitValue( 1 );
        m_red->iopinsChanged();
    }

    if( m_green == NULL ) {
        qWarning( "SimpleTrafficLights::compose didn't get required template" );
        m_green = new Template( "dummy", getLoggerPtr() );
        registerNestedTemplate( "green", m_green );
    } else {
        m_green->properties()["shape"]->setChoiceValue( "ellipse" );
        m_green->properties()["fill_off"]->setBrushValue(
                QBrush( QColor( "#005000" ) ) );
        m_green->properties()["fill_on"]->setBrushValue(
                QBrush( QColor( "#00FF00" ) ) );
        m_green->propertiesChanged();
        m_green->iopins()[ "input" ]->setRealBitValue( 0 );
        m_green->iopinsChanged();
    }
}

void SimpleTrafficLights::setRect( const QRect & r )
{
    //qWarning( "SimpleTrafficLights::setRect:: %d,%d %dx%d",
    //        r.left(), r.top(), r.width(), r.height() );
    Template::setRect( r );

    if( m_red && m_green ) {
        // set correct rect on nested templates
        m_red->setRect( QRect( r.x(), r.y(), r.width(), r.height() / 2 ) );
        m_red->setZ( z() - 1 );

        m_green->setRect( QRect( r.x(), r.y() + r.height() / 2,
                    r.width(), r.height() / 2 ) );
        m_green->setZ( z() - 1 );
    }
}

void SimpleTrafficLights::setVisible( bool visible )
{
    //qWarning( "SimpleTrafficLights::setVisible( %d )", visible );
    Template::setVisible( visible );

    if( m_red && m_green ) {
        if( visible ) {
            m_red->setCanvas( canvas() );
            m_green->setCanvas( canvas() );
            m_red->show();
            m_green->show();
        } else {
            m_red->hide();
            m_green->hide();
            m_red->setCanvas( NULL );
            m_green->setCanvas( NULL );
        }
    }

    // shedule repaint
    update();
}

void SimpleTrafficLights::propertiesChanged()
{
    // shedule repaint
    update();
}

void SimpleTrafficLights::iopinsChanged()
{
    if( iopins()[ "input" ]->realBitValue() ) {
        m_red->iopins()[ "input" ]->setRealBitValue( 0 );
        m_green->iopins()[ "input" ]->setRealBitValue( 1 );
    } else {
        m_red->iopins()[ "input" ]->setRealBitValue( 1 );
        m_green->iopins()[ "input" ]->setRealBitValue( 0 );
    }

    // let nested know we changed them
    m_red->iopinsChanged();
    m_green->iopinsChanged();

    // shedule repaint
    update();
}

Template * SimpleTrafficLights::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new SimpleTrafficLights( logger );
    setupAfterClone( result, loc, tm );
    return result;
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: SimpleTrafficLights.cpp,v 1.11 2004/12/01 12:09:25 mman Exp $
