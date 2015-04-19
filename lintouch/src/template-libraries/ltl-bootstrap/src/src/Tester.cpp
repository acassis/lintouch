// $Id: Tester.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Tester.cpp -- 
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
//   DATE: 20 August 2004
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

#include "Tester.hh"
#include "Tester.xpm"

using namespace lt;

#include "config.h"

struct Tester::TesterPrivate
{
    TesterPrivate() : m_localizator( Localizator::nullPtr() ) {}
    ~TesterPrivate() {}

    QPen            m_pen;
    QBrush          m_brush;
    QFont           m_font;
    QString         m_text;
    QRect           m_text_rect;
    QRect           m_points;
    QRegion         m_collisionRegion;
    LocalizatorPtr  m_localizator;
    int font_orig_size;
};

Tester::Tester( LoggerPtr logger ) :
    Template( "Tester", logger ),
    d( new TesterPrivate() )
{
    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Shivaji Basu <shivaji@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Tester", "ltl-bootstrap" ) );
    i.setLongDescription( QObject::tr(
        "Tester", "ltl-bootstrap" ) );
    i.setIcon( QImage( (const char **) Tester_xpm ) );

    registerInfo( i );

    // create properties of this template
    // define groups
    QString inactive = QObject::tr("Inactive", "ltl-bootstrap");
    QString active = QObject::tr("Active", "ltl-bootstrap");

    registerProperty( active, "border", 
            new Property( QObject::tr( "Border", "ltl-bootstrap" ), "pen",
                "#000000;1;NOPEN", logger ) );
    registerProperty( active, "fill", 
            new Property( QObject::tr( "Fill", "ltl-bootstrap" ), "brush",
                "", logger ) );
    registerProperty( active, "value_font", 
            new Property( QObject::tr( "Font", "ltl-bootstrap" ),
                "font", "", logger ) );

    // create iopins of this template
    // define groups
    QString inp = QObject::tr("Inputs", "ltl-bootstrap");

    registerIOPin( inp, "input", 
            new IOPin( this, QObject::tr( "Input", "ltl-bootstrap" ),
                "bit,number,string" ) );
}

Tester::~Tester()
{
    delete d;
}

void Tester::drawShape( QPainter & p )
{
    Q_ASSERT( d );
    p.save();

    p.setPen( d->m_pen );
    p.setBrush( d->m_brush );
    p.drawRect( d->m_points );

    if( ! d->m_text.isEmpty() ) {
        p.setFont( d->m_font );
        p.drawText( d->m_text_rect, Qt::AlignCenter, d->m_text );
    }

    p.restore();
}

void Tester::mousePressed( const QPoint & /* e */ )
{
    //qWarning( "mousePressed at %d, %d", e.x(), e.y() );
}

void Tester::mouseReleased( const QPoint & /* e */ )
{
    //qWarning( "mouseReleased at %d, %d", e.x(), e.y() );
}

void Tester::mouseMoved( const QPoint & /* e */ )
{
    //qWarning( "mouseMoved at %d, %d", e.x(), e.y() );
}

void Tester::mouseDoubleClicked( const QPoint & /* e */ )
{
    //qWarning( "mouseDoubleClicked at %d, %d", e.x(), e.y() );
}

void Tester::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    recalculateFontSizes();

    // shedule repaint
    update();
}

void Tester::propertiesChanged()
{
    Q_ASSERT( d );

    d->m_pen = properties() [ "border" ]->asPen();
    d->m_brush = properties() [ "fill" ]->asBrush();
    d->m_font = properties() [ "value_font" ]->asFont();

    reconstructPointArrayAndRegion();

    // fire off iopinsChanged to set up painting brush and pen correctly
    iopinsChanged();

    if( d->m_font.pixelSize() != -1 ) {
        d->font_orig_size = d->m_font.pixelSize();
    } else {
        d->font_orig_size = d->m_font.pointSize();
    }
    recalculateFontSizes();

    // shedule repaint
    update();
}

void Tester::iopinsChanged()
{
    Q_ASSERT( d );
    Q_ASSERT( iopins()[ "input" ] );

    IOPin::Type typ = iopins()[ "input" ]->realValueType();
    if ( typ == IOPin::BitType ){
        if ( iopins() [ "input" ]->realBitValue() ) {
            d->m_text = QString( "Bit: %1" ).arg( "1" );
        }else{
            d->m_text = QString( "Bit: %1" ).arg( "0" );
        }    
    } else if ( typ == IOPin::StringType ) {
        d->m_text = QString( "String: %1" ).
                        arg ( d->m_localizator->localizedMessage( 
                        iopins() [ "input" ]->realStringValue() ) );
    } else if ( typ == IOPin::NumberType ) {
        d->m_text = QString( "Number: %1" )
                        .arg( iopins() [ "input" ]->realNumberValue() );    
    } else {
        d->m_text = "";
    }

    // shedule repaint
    update();
}


void Tester::localize( LocalizatorPtr loc )
{
    Q_ASSERT( d );

    if( ! loc.isNull() ) {
        d->m_localizator = loc;
    }
}

void Tester::reconstructPointArrayAndRegion()
{
    Q_ASSERT( d );

    d->m_points = rect();

    d->m_collisionRegion = QRegion( QPointArray( d->m_points, TRUE ) );

    // make text bounding rectangle equal to our rectangle
    d->m_text_rect = rect();
    int w = d->m_pen.width();
    // when we are big enough, clip text inside
    if( d->m_text_rect.width() >( w + 4 ) * 2 &&
            d->m_text_rect.height() >( w + 4 ) * 2 ) {
        QPoint p( w + 4, w + 4 );
        d->m_text_rect.setTopLeft( d->m_text_rect.topLeft() + p );
        d->m_text_rect.setBottomRight( d->m_text_rect.bottomRight() - p );
    }
}

void Tester::recalculateFontSizes()
{
    Q_ASSERT(d);

    double scale =
        (this->rect().height() / (float)virtualRect().height());

    int newsize = (int)(d->font_orig_size * scale);

    // manage real font size either thru pixel size or point size
    if( d->m_font.pixelSize() != -1 ) {
        if( newsize > 0 ) d->m_font.setPixelSize( newsize );
    } else {
        if( newsize > 0 ) d->m_font.setPointSize( newsize );
    }
}

Template * Tester::clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Tester( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Tester.cpp 1168 2005-12-12 14:48:03Z modesto $
