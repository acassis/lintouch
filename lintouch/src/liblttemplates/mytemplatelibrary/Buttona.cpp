// $Id: Buttona.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Buttona.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 27 January 2004
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

#include "Buttona.hh"
using namespace lt;

#include "Buttona.xpm"

#include "config.h"

#undef OUTPUT_STATISTICS

Buttona::Buttona( LoggerPtr logger ) :
    Template( "Button", logger ), pressed( 0 ),
    m_localizator( Localizator::nullPtr() ),
    m_min_roundtrip( -1 ),
    m_max_roundtrip( -1 ),
    m_avg_roundtrip( 0.0 ),
    m_nr_pressed( 0 )
{
    //register info
    Info i;
    i.setAuthor( "Martin Man <mman@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Button" ) );
    i.setLongDescription( QObject::tr(
            "Button with one input/output iopin" ) );
    i.setIcon( QImage( (const char **) Buttona_xpm ) );

    registerInfo( i );

    // create properties of this template
    QString idle = QObject::tr( "Idle" );
    QString pressed = QObject::tr( "Pressed" );
    QString confirmed = QObject::tr( "Confirmed" );

    Property * tmp = NULL;

    registerProperty( idle, "border_idle",
            tmp = new Property( QObject::tr( "Border" ),
                "pen", "", logger ) );
    registerProperty( pressed, "border_pressed",
            tmp = new Property( QObject::tr( "Border" ), tmp, logger ) );
    registerProperty( confirmed, "border_confirmed",
            new Property( QObject::tr( "Border" ), tmp, logger ) );

    registerProperty( idle, "fill_idle",
            tmp = new Property( QObject::tr( "Fill" ),
                "brush", "", logger ) );
    registerProperty( pressed, "fill_pressed",
            tmp = new Property( QObject::tr( "Fill" ), tmp, logger ) );
    registerProperty( confirmed, "fill_confirmed",
            new Property( QObject::tr( "Fill" ), tmp, logger ) );

    registerProperty( idle, "text_idle",
            tmp = new Property( QObject::tr( "Text" ),
                "string", "", logger ) );
    registerProperty( pressed, "text_pressed", 
            tmp = new Property( QObject::tr( "Text" ), tmp, logger ) );
    registerProperty( confirmed, "text_confirmed",
            new Property( QObject::tr( "Text" ), tmp, logger ) );

    registerProperty( idle, "text_font_idle",
            tmp = new Property( QObject::tr( "Font" ),
                "font", "Arial;10;NORMAL", logger ) );
    registerProperty( pressed, "text_font_pressed",
            tmp = new Property( QObject::tr( "Font" ), tmp, logger ) );
    registerProperty( confirmed, "text_font_confirmed",
            new Property( QObject::tr( "Font" ), tmp, logger ) );

    registerProperty( idle, "text_color_idle",
            tmp = new Property( QObject::tr( "Text color" ),
                "color", "", logger ) );
    registerProperty( pressed, "text_color_pressed",
            tmp = new Property( QObject::tr( "Text color" ), tmp, logger ) );
    registerProperty( confirmed, "text_color_confirmed",
            new Property( QObject::tr( "Text color" ), tmp, logger ) );

    registerProperty( idle, "text_valign",
            new Property( QObject::tr( "Align vertically" ),
                "choice(top,center,bottom)", "center", logger ) );
    registerProperty( idle, "text_halign",
            new Property(QObject::tr( "Align horizontally" ),
                "choice(left,center,right,justify)", "center",
                logger ) );

    // create iopins of this template
    registerIOPin( QObject::tr( "Inputs/Outputs" ), "iopin",
            new IOPin( this, QObject::tr( "Input/Output" ), "bit" ) );
}

Buttona::~Buttona()
{
#ifdef OUTPUT_STATISTICS
    if( m_nr_pressed > 0 ) {
        qWarning( QString( "press-to-confirm:"
                    " min=%1ms,avg=%2ms,max=%3ms, %4 samples" )
                .arg( m_min_roundtrip )
                .arg( m_avg_roundtrip / m_nr_pressed )
                .arg( m_max_roundtrip )
                .arg( m_nr_pressed ) );
    }
#endif
}

void Buttona::drawShape( QPainter & p )
{
    p.save();

    p.setPen( m_pen );
    p.setBrush( m_brush );
    p.drawRect( m_points );

    p.setPen( m_cpen );
    p.setBrush( m_cbrush );
    p.drawRect( m_cpoints );

    if( ! m_text.isEmpty() ) {
        p.setFont( m_font );
        p.setPen( m_font_pen );
        p.drawText( m_text_rect, m_textf, m_text );
    }

    p.restore();
}

QWidget::FocusPolicy Buttona::focusPolicy() const
{
    return QWidget::StrongFocus;
}

void Buttona::keyPressEvent( QKeyEvent * e )
{
    //qWarning( "Buttona::keyPressEvent: %d", e->key() );
    if( ! pressed &&
            ( e->key() == Qt::Key_Space ||
              ( shortcut() != "" && keyEvent2Sequence( e ) == shortcut() ) ) )
    {
        pressed = 1;
        iopins()[ "iopin" ]->setRequestedBitValue( 1 );

        // remember the time when we were last pressed
        m_time_pressed = QTime::currentTime();

        syncIOPinsRequestedValues();
        iopinsChanged();
    }
}

void Buttona::keyReleaseEvent( QKeyEvent * e )
{
    //qWarning( "Buttona::keyReleaseEvent: %d", e->key() );
    if( pressed &&
            ( e->key() == Qt::Key_Space ||
              ( shortcut() != "" && keyEvent2Sequence( e ) == shortcut() ) ) )
    {
        pressed = 0;
        iopins()[ "iopin" ]->setRequestedBitValue( 0 );
        syncIOPinsRequestedValues();
        iopinsChanged();
    }
}

void Buttona::mousePressEvent( QMouseEvent * /* e */ )
{
    //qWarning( "mousePressEvent at %d, %d", e->pos().x(), e->pos().y() );
    pressed = 1;
    iopins()[ "iopin" ]->setRequestedBitValue( 1 );

    // remember the time when we were last pressed
    m_time_pressed = QTime::currentTime();

    syncIOPinsRequestedValues();
    iopinsChanged();
}

void Buttona::mouseReleaseEvent( QMouseEvent * /* e */ )
{
    //qWarning( "mouseReleaseEvent at %d, %d", e->pos().x(), e->pos().y() );
    pressed = 0;
    iopins()[ "iopin" ]->setRequestedBitValue( 0 );
    syncIOPinsRequestedValues();
    iopinsChanged();
}

void Buttona::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    // compute new font sizes
    recalculateFontSizes();

    // and shedule repaint
    update();
}

void Buttona::propertiesChanged()
{
    m_textf = Qt::WordBreak;

    QString valign = properties()[ "text_valign" ]->asString();
    QString halign = properties()[ "text_halign" ]->asString();

    if( valign == "top" ) {
        m_textf |= Qt::AlignTop;
    } else if( valign == "center" ) {
        m_textf |= Qt::AlignVCenter;
    } else if( valign == "bottom" ) {
        m_textf |= Qt::AlignBottom;
    }

    if( halign == "left" ) {
        m_textf |= Qt::AlignLeft;
    } else if( halign == "center" ) {
        m_textf |= Qt::AlignHCenter;
    } else if( halign == "right" ) {
        m_textf |= Qt::AlignRight;
    } else if( halign == "justify" ) {
        m_textf |= Qt::AlignJustify;
    }

    // update our pointarray and region, since skin possibly changed.
    reconstructPointArrayAndRegion();

    // fire off iopinsChanged to set up painting brush and pen correctly
    iopinsChanged();

    // shedule repaint
    update();
}

void Buttona::iopinsChanged()
{
    //qWarning("Buttona::iopinsChanged()");

    IOPin * iopin = iopins()[ "iopin" ];

    if( pressed ) {
        if( iopin->realBitValue() == iopin->requestedBitValue() ) {

            // measure the time and count press to the statistics
            m_time_confirmed = QTime::currentTime();
            m_nr_pressed += 1;

            int delay = m_time_pressed.msecsTo( m_time_confirmed );
            if( m_min_roundtrip == -1 || delay < m_min_roundtrip ) {
                m_min_roundtrip = delay;
            }
            if( m_max_roundtrip == -1 || delay > m_max_roundtrip ) {
                m_max_roundtrip = delay;
            }
            m_avg_roundtrip += delay;

#ifdef OUTPUT_STATISTICS
            qWarning( QString( "press-to-confirm:"
                        " last=%1ms, min=%2ms,avg=%3ms,max=%4ms, %5 samples" )
                    .arg( delay )
                    .arg( m_min_roundtrip )
                    .arg( m_avg_roundtrip / m_nr_pressed )
                    .arg( m_max_roundtrip )
                    .arg( m_nr_pressed ) );
#endif

            m_pen = properties() [ "border_confirmed" ]->asPen();
            m_brush = properties() [ "fill_confirmed" ]->asBrush();
            m_cpen = properties() [ "border_confirmed" ]->asPen();
            m_cbrush = properties() [ "fill_confirmed" ]->asBrush();
            m_original_font = properties() [ "text_font_confirmed" ]->asFont();
            recalculateFontSizes();
            m_font_pen = QPen(
                    properties() [ "text_color_confirmed" ]->asColor() );
            m_text = m_localizator->localizedMessage(
                    properties() [ "text_confirmed" ]->asString() );

        } else {

            m_pen = properties() [ "border_pressed" ]->asPen();
            m_brush = properties() [ "fill_pressed" ]->asBrush();
            m_cpen = properties() [ "border_pressed" ]->asPen();
            m_cbrush = properties() [ "fill_pressed" ]->asBrush();
            m_original_font = properties() [ "text_font_pressed" ]->asFont();
            recalculateFontSizes();
            m_font_pen = QPen(
                    properties() [ "text_color_pressed" ]->asColor() );
            m_text = m_localizator->localizedMessage(
                    properties() [ "text_pressed" ]->asString() );
        }
    } else {

        m_pen = properties() [ "border_idle" ]->asPen();
        m_brush = properties() [ "fill_idle" ]->asBrush();
        m_cpen = properties() [ "border_idle" ]->asPen();
        m_cbrush = properties() [ "fill_idle" ]->asBrush();
        m_original_font = properties() [ "text_font_idle" ]->asFont();
        recalculateFontSizes();
        m_font_pen = QPen( properties() [ "text_color_idle" ]->asColor() );
        m_text = m_localizator->localizedMessage(
                properties() [ "text_idle" ]->asString() );

    }

    // shedule repaint
    update();
}


QPointArray Buttona::areaPoints() const
{
    return m_points;
}

QRegion Buttona::collisionRegion() const
{
    return m_collisionRegion;
}

void Buttona::reconstructPointArrayAndRegion()
{
    // reconstruct our points and region
    m_points = rect();
    m_collisionRegion = QRegion( QPointArray( m_points, TRUE ) );

    m_cpoints = QRect( m_points.x() + 5, m_points.y() + m_points.height() - 15,
            m_points.width() - 10, 10 );

    // make text bounding rectangle equal to our rectangle
    m_text_rect = rect();
    int w = m_pen.width();
    // when we are big enough, clip text inside while avoiding confirmation
    // box
    if( m_text_rect.width() >( w + 4 ) * 2 &&
            m_text_rect.height() > w * 2 + 15 + 4 ) {
        QPoint p( w + 4, w + 4 );
        QPoint p1( w + 4, w + 15 + 4 );
        m_text_rect.setTopLeft( m_text_rect.topLeft() + p );
        m_text_rect.setBottomRight( m_text_rect.bottomRight() - p1 );
    }

}

void Buttona::recalculateFontSizes() {
    m_font = makeFont( m_original_font );
}

void Buttona::localize( LocalizatorPtr loc )
{
    if( ! loc.isNull() ) {
        m_localizator = loc;
    }
}

Template * Buttona::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Buttona( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Buttona.cpp 1168 2005-12-12 14:48:03Z modesto $
