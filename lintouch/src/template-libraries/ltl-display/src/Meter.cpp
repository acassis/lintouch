// $Id: Meter.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Meter.cpp --
// AUTHOR: abdul <henry@swac.cz>
//   DATE: 02 November 2004
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

#include "Meter.hh"
#include "Meter.xpm"

#include "config.h"

#ifndef ROUND_OF_DIGIT
#define ROUND_OF_DIGIT( v ) (int)((v<((int)(v)+0.5))?v:v+1)
#endif

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

struct Meter::LTMeterPrivate {
    LTMeterPrivate() : m_min( 0 ),m_max( 99 ), m_ignoreLimit( false ),
                       m_ticks( 0 ),m_ticksLarge( 0 ),m_intInput(m_min),
                       m_localizator( Localizator::nullPtr() ) {}

    ~LTMeterPrivate(){}

    /*
     * Appearance
     */
    int             m_min;
    int             m_max;
    /*
     * Limits
     */
    Orientation     m_orientation;
    QBrush          m_background;
    QBrush          m_fill;
    QPen            m_pen_border;

    /*
     * Not functional any more
     */
    bool            m_ignoreLimit;

    /*
     * Ticks
     */
    int             m_ticks;
    int             m_ticksLarge;
    QPen            m_tickPen;

    /*
     * Input pin value
     */
    int             m_intInput;

    /*
     * Meter Rect
     */
    QRect           m_meterRect;

    /*
     * Fill Rect
     */
    QRect           m_meterFillRect;

    /*
     * IOPin segment
     */
    double iopinSegment;

    /*
     * Offset of small tick top position
     */
    int tickSmallTop;

    /*
     * Offset of large tick top position
     */
    int tickLargeTop;

    /*
     * Tick segment
     */
    double tickSegment;

    /*
     *TODO
     */
    LocalizatorPtr  m_localizator;

    /*
     * TODO
     */
    void recompute(QRect r)
    {
        m_meterRect = r;

        int totalSegments = abs(m_max - m_min);
        if(!totalSegments) {
            totalSegments = 1;
        }

        if( m_orientation == Qt::Vertical ) {
            int heightOfMeterRect = m_meterRect.height();
            iopinSegment = ( heightOfMeterRect*1.0 ) / totalSegments;
        } else {
            int widthOfMeterRect = m_meterRect.width();
            iopinSegment = ( widthOfMeterRect*1.0 ) / totalSegments;
        }
        computeMeterFillRect();
        computeTicks();
    }

    void computeMeterFillRect() {

        // by default we take the whole area
        m_meterFillRect = m_meterRect;

        // the min/max way
        bool normal = (m_min <= m_max);

        // limit situations
        if(normal) { // from min to max
            if(m_intInput <= m_min) {
                // value too low; draw nothing
                m_meterFillRect = QRect();
                return;
            }
            if(m_intInput >= m_max) {
                // we have the full meter
                return;
            }
        } else { // from max to min
            if(m_intInput <= m_max) {
                // value too low; draw nothing
                m_meterFillRect = QRect();
                return;
            }
            if(m_intInput >= m_min) {
                // we have the full meter
                return;
            }
        }

        int window = abs(m_max - m_min);
        float f;
        int val = m_intInput - MIN(m_min,m_max);

        if( m_orientation == Qt::Horizontal ) {
            f = 1.0*m_meterFillRect.width() / window;
            val = (int)(f * val);
            if(normal) {
                m_meterFillRect.setRight(m_meterFillRect.left() + val);
            } else {
                m_meterFillRect.setLeft(m_meterFillRect.right() - val);
            }
        } else {
            f = 1.0*m_meterFillRect.height() / window;
            val = (int)(f * val);
            if(normal) {
                m_meterFillRect.setTop(m_meterFillRect.bottom() - val);
            } else {
                m_meterFillRect.setBottom(m_meterFillRect.top() + val);
            }
        }
        Q_ASSERT(m_meterFillRect.isValid());
    }

    void computeTicks() {

        if( m_ticks <= 1 ) {
            return;
        }

        int widthOfMeter = m_meterRect.width();
        int heightOfMeter = m_meterRect.height();
        int widthOfPen = m_tickPen.width();

        if( m_orientation == Qt::Horizontal ) {
            tickSegment =
                ( widthOfMeter*1.0F ) / m_ticks;
            //Need to check this code : Not sure
            if( widthOfPen >= tickSegment ) {
                m_tickPen.setWidth( ROUND_OF_DIGIT( tickSegment ) );
            }
            tickSmallTop = m_meterRect.top() + 
                ROUND_OF_DIGIT( (heightOfMeter*2*1.0F) /3 );
            tickLargeTop = m_meterRect.top() +
                ROUND_OF_DIGIT( (heightOfMeter*1.0F) /3 );
        } else {

            tickSegment = heightOfMeter*1.0F / m_ticks;
            //Need to check this code : Not sure
            if( widthOfPen >= tickSegment ) {
                m_tickPen.setWidth( ROUND_OF_DIGIT( tickSegment ) );
            }

            tickSmallTop = m_meterRect.left() +
                ROUND_OF_DIGIT( (widthOfMeter*2*1.0F) /3 );
            tickLargeTop = m_meterRect.left() +
                ROUND_OF_DIGIT( (widthOfMeter*1.0F) /3 );            
        }
    }

};

Meter::Meter( LoggerPtr logger ) :
    Template( "Meter", logger ), d( new LTMeterPrivate() ) {

        Q_CHECK_PTR( d );

        Info i;
        i.setAuthor( "Abdul Hamid <henry@swac.cz>" );
        i.setVersion( VERSION );
        i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
        i.setShortDescription( QObject::tr( "Meter", "ltl-display" ) );
        i.setLongDescription( QObject::tr( "Meter", "ltl-display" ) );
        i.setIcon( QImage( (const char **) Meter_xpm ) );

        registerInfo( i );

        QString appearance = QObject::tr("Appearance", "ltl-display");

        registerProperty( appearance, "direction",
                new Property( QObject::tr( "Direction", "ltl-display" ),
                    "choice(horizontal,vertical)",
                    "horizontal", logger ) );

        registerProperty( appearance, "border",
                new Property( QObject::tr( "Border", "ltl-display" ), "pen",
                    "#000000;1;SOLIDLINE", logger ) );

        registerProperty( appearance,"background",
                new  Property( QObject::tr( "Background", "ltl-display"),
                    "brush", "#FFFFFF;NOBRUSH", logger ) );

        registerProperty( appearance,"fill",
                new  Property( QObject::tr( "Fill", "ltl-display"),"brush",
                    "#000000;SOLIDPATTERN", logger ) );

        QString limits = QObject::tr("Limits", "ltl-display");

        registerProperty( limits, "minimum",
                new Property( QObject::tr( "Minimum", "ltl-display" ),
                    "integer", "0", logger ) );

        registerProperty(  limits, "maximum",
                new Property( QObject::tr( "Maximum", "ltl-display" ),
                    "integer", "99", logger ) );
        /**
          registerProperty(  limits, "ignore_limit",
          new Property( QObject::tr( "Ignore Limit", "ltl-display" ),
          "choice(Yes,No)","Yes", logger ) );
         **/
        QString tick = QObject::tr("Ticks", "ltl-display");

        registerProperty(  tick, "ticks",
                new Property( QObject::tr( "Ticks", "ltl-display" ), "integer",
                    "100", logger ) );

        registerProperty(  tick, "ticks_large",
                new Property( QObject::tr( "Ticks Large", "ltl-display" ),
                    "integer", "10", logger ) );

        registerProperty( tick, "ticks_color",
                new Property( QObject::tr( "Ticks Color", "ltl-display" ),
                    "pen", "#000000;1;SOLIDLINE", logger ) );

        QString inp = QObject::tr("Inputs", "ltl-display");

        registerIOPin( inp, "input",
                new IOPin( this, QObject::tr( "Input", "ltl-display" ),
                    "number" ) );
    }

Meter::~Meter() {
    delete d;
}

void Meter::drawShape( QPainter & p ) {
    Q_ASSERT( d );
    p.save();
    fillMeter( p );
    drawTicks( p );
    p.restore();
}

void Meter::fillMeter( QPainter & p ) {

    Q_ASSERT( d );

    p.setBrush( d->m_background );
    p.setPen( d->m_pen_border );
    p.drawRect( d->m_meterRect );

    if( d->m_meterFillRect.isValid() &&
            d->m_meterRect.contains( d->m_meterFillRect ) ) {
        p.setBrush( d->m_fill );
        p.drawRect( d->m_meterFillRect );
    }    
}

void Meter::drawTicks( QPainter & p ) {

    Q_ASSERT( d );

    if( d->m_ticks <= 1 ) {
        return;
    }

    int numberOfTick = d->m_ticks - 1;
    if( d->m_orientation == Qt::Horizontal ) {
        for( int i = 1; i <= numberOfTick; i++ ) {
            int x , y;
            x =  ROUND_OF_DIGIT( d->m_meterRect.left()+d->tickSegment*i );
            if( d->m_ticksLarge  && ( i%d->m_ticksLarge == 0 ) ) {
                y = d->tickLargeTop;
            } else {
                y = d->tickSmallTop;
            }
            p.setPen( d->m_tickPen );
            p.drawLine( x, y, x, d->m_meterRect.bottom() );
        }
    } else {

        for( int i = 1; i <= numberOfTick; i++ ) {
            int x , y;
            y = ROUND_OF_DIGIT( d->m_meterRect.top()+d->tickSegment*i );
            if( d->m_ticksLarge  && ( i%d->m_ticksLarge == 0 ) ) {
                x = d->tickLargeTop;
            } else {
                x = d->tickSmallTop;
            }
            p.setPen( d->m_tickPen );
            p.drawLine( x, y, d->m_meterRect.right(), y );
        }
    }

}

void Meter::setRect( const QRect & rect ) {
    Template::setRect( rect );

    d->recompute(rect);
    update();
}

void Meter::propertiesChanged() {
    // shedule repaint
    Q_ASSERT( d );

    const PropertyDict &pdict =(const PropertyDict&) properties();

    d->m_min = pdict[ "minimum" ]->asInteger();
    d->m_max = pdict[ "maximum" ]->asInteger();

    d->m_intInput = MIN(d->m_min, d->m_max);

    if( pdict[ "direction" ]->asString() == "horizontal" ) {
        d->m_orientation = Qt::Horizontal;
    } else {
        d->m_orientation = Qt::Vertical;
    }

    d->m_background = pdict[ "background" ]->asBrush();
    d->m_fill = pdict [ "fill" ]->asBrush();
    d->m_pen_border = pdict[ "border" ]->asPen();

    d->m_ticks =  pdict[ "ticks" ]->asInteger();
    d->m_ticksLarge = pdict[ "ticks_large" ]->asInteger();
    d->m_tickPen = pdict[ "ticks_color" ]->asPen();

    d->recompute( rect() );

    iopinsChanged();

}

void Meter::iopinsChanged() {

    Q_ASSERT( d );

    const IOPinDict &ioDict = iopins();
    const IOPin* inputPin = ioDict["input"];

    Q_ASSERT( inputPin );

    d->m_intInput = inputPin->realNumberValue();

    d->computeMeterFillRect();

    update();
}

QPointArray Meter::areaPoints() const {

    return Template::areaPoints();

}

QRegion Meter::collisionRegion() const {

    return Template::collisionRegion();

}

void Meter::setVisible( bool visible ) {

    Template::setVisible( visible );

}

void Meter::setCanvas( QCanvas * c ) {

    Template::setCanvas( c );

}

void Meter::advance( int /* phase */ ) {
}


Template * Meter::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Meter( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

void Meter::localize( LocalizatorPtr loc ) {
    Q_ASSERT( d );

    if( ! loc.isNull() ) {
        d->m_localizator = loc;
    }
}


#undef ROUND_OF_DIGIT
// vim: set et ts=4 sw=4 tw=76:
// $Id: Meter.cpp 1168 2005-12-12 14:48:03Z modesto $
