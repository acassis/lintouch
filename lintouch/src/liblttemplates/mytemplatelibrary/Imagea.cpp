// $Id: Imagea.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Imagea.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 03 October 2003
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

#include "Imagea.hh"
using namespace lt;

#include "Imagea.xpm"

#include "config.h"

Imagea::Imagea( LoggerPtr logger ) :
    Template( "Image", logger ),
    m_pixmap( NULL ),
    m_localizator( Localizator::nullPtr() )
{

    //register info
    Info i;
    i.setAuthor( "Martin Man <mman@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Image" ) );
    i.setLongDescription( QObject::tr(
            "Passive image without iopins" ) );
    i.setIcon( QImage( (const char **)Image_xpm ) );

    registerInfo( i );

    // create properties of this template and place them within three
    // groups, the "on" properties are created as shadows for "off"
    // properties.

    QString inactive = QObject::tr( "Appearance" );

    registerProperty( inactive, "image",
            new Property( QObject::tr( "Imagea" ),
                "resource", "", logger ) );
    registerProperty( inactive, "link",
            new Property( QObject::tr( "Link to panel" ),
                "link", "", logger ) );
}

Imagea::~Imagea() {
    if( m_pixmap ) delete m_pixmap;
}

void Imagea::drawShape( QPainter & p ) {

    p.save();

    if( m_pixmap == NULL ) {
        m_pixmap = new QPixmap( (const char**)Image_xpm );
    }

    p.drawPixmap( rect(), *m_pixmap );

    p.restore();
}

void Imagea::mousePressEvent( QMouseEvent * /* e */ ) {
    //qWarning( "mousePressEvent at %d, %d", e->pos().x(), e->pos().y() );
    QString id = properties()[ "link" ]->asString();
    if( !id.isEmpty() ) {
        //qWarning( "trying to instruct ViewAdaptor to display panel " + id );
        viewAdaptor()->displayPanel( id );
    }
}

void Imagea::mouseReleaseEvent( QMouseEvent * /* e */ ) {
    //qWarning( "mouseReleaseEvent at %d, %d", e->pos().x(), e->pos().y() );
}

void Imagea::mouseMoveEvent( QMouseEvent * /* e */ ) {
    //qWarning( "mouseMoveEvent at %d, %d", e->pos().x(), e->pos().y() );
}

void Imagea::mouseDoubleClickEvent( QMouseEvent * /* e */ ) {
    //qWarning( "mouseDoubleClickEvent at %d, %d", e->pos().x(), e->pos().y() );
}

void Imagea::propertiesChanged() {
    //qWarning( "Imagea::propertiesChanged" );

    reloadPixmap();

    // shedule repaint
    update();
}

void Imagea::reloadPixmap()
{
    // forget old pixmap
    if( m_pixmap ) delete m_pixmap;
    m_pixmap = NULL;

    // no localizator, stay with icon
    if( m_localizator.isNull() ) {
        //qWarning( "reloadPixmap: have no localizator" );
        return;
    }

    QByteArray a = m_localizator->resourceAsByteArray(
            properties()["image"]->asString() );
    // no data, stay with icon
    if( a.size() != 0 ) {
        //qWarning( "reloadPixmap: reloaded successfully from resource " +
        //        properties()["image"]->asString() );
        m_image = QImage( a );
        m_pixmap = new QPixmap( m_image );
    } else {
        //qWarning( "reloadPixmap: have no data in resource " +
        //        properties()["image"]->asString() );
    }
}

void Imagea::localize( LocalizatorPtr loc ) {
    if( ! loc.isNull() ) {
        m_localizator = loc;
        reloadPixmap();
    }
}

Template * Imagea::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Imagea( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Imagea.cpp 1168 2005-12-12 14:48:03Z modesto $
