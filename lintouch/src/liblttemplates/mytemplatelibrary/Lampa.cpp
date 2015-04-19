// $Id: Lampa.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Lampa.cpp -- 
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

#include "Lampa.hh"
using namespace lt;

#include "Lampa.xpm"

#include "config.h"

Lampa::Lampa( LoggerPtr logger ) :
    Template( "Lamp", logger ),
    m_localizator( Localizator::nullPtr() )
{

    //register info
    Info i;
    i.setAuthor( "Martin Man <mman@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Lamp" ) );
    i.setLongDescription( QObject::tr(
            "Lamp with one input iopin" ) );
    i.setIcon( QImage( (const char **)Lampa_xpm ) );

    registerInfo( i );

    // create properties of this template and place them within three
    // groups, the "on" properties are created as shadows for "off"
    // properties.

    QString inactive = QObject::tr( "Inactive" );
    QString active = QObject::tr( "Active" );
    QString fancy = m_fpg = QObject::tr( "Fancy Shape" );

    Property * tmp = NULL;

    registerProperty( inactive, "shape",
            new Property( QObject::tr( "Shape" ),
                "choice(rectangle,ellipse,triangle,fancy)",
                "rectangle", logger ) );

    registerProperty( inactive, "link",
            new Property( QObject::tr( "Link to panel" ),
                "link", "", logger ) );

    registerProperty( inactive, "border_off",
            tmp = new Property( QObject::tr( "Border" ),
                "pen", "", logger ) );
    registerProperty( active, "border_on",
            new Property( QObject::tr( "Border" ), tmp, logger ) );

    registerProperty( inactive, "fill_off",
            tmp = new Property( QObject::tr( "Fill" ),
                "brush", "", logger ) );
    registerProperty( active, "fill_on",
            new Property( QObject::tr( "Fill" ), tmp, logger ) );

    registerProperty( inactive, "text_off",
            tmp = new Property( QObject::tr( "Text" ),
                "string", "", logger ) );
    registerProperty( active, "text_on",
            new Property( QObject::tr( "Text" ), tmp, logger ) );

    registerProperty( inactive, "text_color_off",
            tmp = new Property( QObject::tr( "Text color" ),
                "color", "", logger ) );
    registerProperty( active, "text_color_on",
            new Property( QObject::tr( "Text color" ), tmp, logger ) );

    registerProperty( inactive, "text_font_off",
            tmp = new Property( QObject::tr( "Font" ),
                "font", "Arial;10;NORMAL", logger) );
    registerProperty( active, "text_font_on",
            new Property( QObject::tr( "Font" ), tmp, logger ) );

    registerProperty( inactive, "text_valign",
            new Property( QObject::tr( "Align vertically" ),
                "choice(top,center,bottom)", "center", logger ) );

    registerProperty( inactive, "text_halign",
            new Property( QObject::tr( "Align horizontally" ),
                "choice(left,center,right,justify)", "center", logger ) );

    registerProperty( fancy, "fancy.animated",
            new Property( QObject::tr( "Animated" ),
                "choice(no,yes)", "no", logger ) );

    registerProperty( fancy, "fancy.steps",
            new Property( QObject::tr( "Steps" ),
                "integer", "10", logger ) );

    // disable fancy group by default
    setPropertyGroupEnabled( m_fpg, false );

    QString inp = QObject::tr( "Inputs" );

    // create iopins of this template
    registerIOPin( inp, "input",
            new IOPin( this, QObject::tr( "Input" ), "bit" ) );
}

Lampa::~Lampa() {
}

void Lampa::drawShape( QPainter & p ) {

    p.save();

    p.setPen( m_pen );
    p.setBrush( m_brush );

    p.drawPolygon( m_points );

    if( ! m_text.isEmpty() ) {
        p.setFont( m_font );
        p.setPen( m_font_pen );
        p.drawText( m_text_rect, m_textf, m_text );
    }

    p.restore();
}

void Lampa::mousePressEvent( QMouseEvent * /* e */ ) {
    //qWarning( "mousePressEvent at %d, %d", e->pos().x(), e->pos().y() );
    QString id = properties()[ "link" ]->asString();
    if( !id.isEmpty() ) {
        //qWarning( "trying to instruct ViewAdaptor to display panel " + id );
        viewAdaptor()->displayPanel( id );
    }
}

void Lampa::mouseReleaseEvent( QMouseEvent * /* e */ ) {
    //qWarning( "mouseReleaseEvent at %d, %d", e->pos().x(), e->pos().y() );
}

void Lampa::mouseMoveEvent( QMouseEvent * /* e */ ) {
    //qWarning( "mouseMoveEvent at %d, %d", e->pos().x(), e->pos().y() );
}

void Lampa::mouseDoubleClickEvent( QMouseEvent * /* e */ ) {
    //qWarning( "mouseDoubleClickEvent at %d, %d", e->pos().x(), e->pos().y() );
}

void Lampa::setRect( const QRect & rect ) {
    //qWarning( "Lampa::setRect" );
    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    // compute new font sizes
    recalculateFontSizes();

    // update animation stuff
    step = 0;
    xdiff = ydiff = 0;
    xpos = ypos = 0;
    if( steps > 0 ) {
        xdiff = rect.width() / (double)steps;
        ydiff = rect.height() / (double)steps;
    }

    // and shedule repaint
    update();
}

void Lampa::propertiesChanged() {
    //qWarning( "Lampa::propertiesChanged" );

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

    // update our pointarray and region, since shape possibly changed.
    reconstructPointArrayAndRegion();

    // fire off iopinsChanged to set up painting brush and pen correctly
    iopinsChanged();

    // update animation stuff
    anim = properties()["fancy.animated"]->asString() == "yes";
    steps = properties()["fancy.steps"]->asInteger();

    if( properties()["shape"]->asString() == "fancy" ) {
        setAnimated( anim );
        setPropertyGroupEnabled( m_fpg, true );
    } else {
        setPropertyGroupEnabled( m_fpg, false );
    }

    // shedule repaint
    update();
}

void Lampa::iopinsChanged() {
    //qWarning("Lampa::iopinsChanged()");

    if( iopins()[ "input" ]->realBitValue() ) {

        //qWarning( "our iopin is on" );

        m_pen = properties() [ "border_on" ]->asPen();
        m_brush = properties() [ "fill_on" ]->asBrush();
        m_original_font = properties() [ "text_font_on" ]->asFont();
        recalculateFontSizes();
        m_font_pen = QPen( properties() [ "text_color_on" ]->asColor() );
        //simplify all whitespaces in a text and replace <br> with \n, also
        //remove all leading and trailing spaces occupying \n
        m_text = m_localizator->localizedMessage(
                properties() [ "text_on" ]->asString() ).simplifyWhiteSpace();
        m_text.replace( "<br>", "\n" );
        m_text.replace( " \n", "\n" );
        m_text.replace( "\n ", "\n" );

    } else {

        //qWarning( "our iopin is off" );

        m_pen = properties() [ "border_off" ]->asPen();
        m_brush = properties() [ "fill_off" ]->asBrush();
        m_original_font = properties() [ "text_font_off" ]->asFont();
        recalculateFontSizes();
        m_font_pen = QPen( properties() [ "text_color_off" ]->asColor() );
        //simplify all whitespaces in a text and replace <br> with \n, also
        //remove all leading and trailing spaces occupying \n
        m_text = m_localizator->localizedMessage(
                properties() [ "text_off" ]->asString() ).simplifyWhiteSpace();
        m_text.replace( "<br>", "\n" );
        m_text.replace( " \n", "\n" );
        m_text.replace( "\n ", "\n" );
    }

    // shedule repaint
    update();
}


QPointArray Lampa::areaPoints() const {
    return m_points;
}

QRegion Lampa::collisionRegion() const {
    return m_collisionRegion;
}

void Lampa::setVisible( bool visible ) {
    Template::setVisible( visible );
    if( properties()[ "shape" ]->asString() == "fancy" ) {
        step = 0;
        m_points = QPointArray( rect(), TRUE );
    }
}

void Lampa::advance( int phase ) {
    //qWarning( "Lampa::advance( %d )", phase );
    if( animated() && phase ) {

        QPoint p;
        m_points = QPointArray( rect(), TRUE );

        step ++;

        // make the movement
        if( step < steps ) {

            xpos += xdiff;
            ypos += ydiff;

            p = m_points.point( 0 );
            p.setX( (int)( p.x() + xpos + .5 ) );
            m_points.setPoint( 0, p );

            p = m_points.point( 1 );
            p.setY( (int)( p.y() + ypos + .5 ) );
            m_points.setPoint( 1, p );

            p = m_points.point( 2 );
            p.setX( (int)( p.x() - xpos + .5 ) );
            m_points.setPoint( 2, p );

            p = m_points.point( 3 );
            p.setY( (int)( p.y() - ypos + .5 ) );
            m_points.setPoint( 3, p );

            p = m_points.point( 4 );
            p.setX( (int)( p.x() + xpos + .5 ) );
            m_points.setPoint( 4, p );

        } else {
            step = 0;
            xpos = 0; ypos = 0;
        }

        m_collisionRegion = QRegion( m_points );
        canvas()->setChanged( rect() );
    }
}

void Lampa::reconstructPointArrayAndRegion() {

    // reconstruct our points and region
    m_points = QPointArray( rect(), TRUE );
    m_collisionRegion = QRegion( m_points );

    // special case is ellipse
    if( properties()[ "shape" ]->asString() == "ellipse" ) {
        m_points.makeEllipse( rect().x(), rect().y(),
                rect().width(), rect().height() );
        m_collisionRegion = QRegion( rect(), QRegion::Ellipse );
    }

    // special case is triangle
    if( properties()[ "shape" ]->asString() == "triangle" ) {
        m_points = QPointArray( 4 );
        m_points.setPoint( 0, rect().x(), rect().y() + rect().height() - 1 );
        m_points.setPoint( 1, rect().x() + rect().width() - 1,
                rect().y() + rect().height() - 1 );
        m_points.setPoint( 2, rect().x() +( rect().width() / 2 ), rect().y() );
        m_points.setPoint( 3, rect().x(), rect().y() + rect().height() - 1 );
        m_collisionRegion = QRegion( m_points );
    }

    // make text bounding rectangle equal to our rectangle
    m_text_rect = rect();
    int w = m_pen.width();
    // when we are big enough, clip text inside
    if( m_text_rect.width() > ( w + 4 ) * 2 && 
            m_text_rect.height() > ( w + 4 ) * 2 ) {
        QPoint p( w + 4, w + 4 );
        m_text_rect.setTopLeft( m_text_rect.topLeft() + p );
        m_text_rect.setBottomRight( m_text_rect.bottomRight() - p );
    }

}

void Lampa::recalculateFontSizes() {
    // rescale currently selected font
    m_font = makeFont( m_original_font, m_text );
}

void Lampa::localize( LocalizatorPtr loc ) {
    if( ! loc.isNull() ) {
        m_localizator = loc;
    }
}

Template * Lampa::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Lampa( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Lampa.cpp 1168 2005-12-12 14:48:03Z modesto $
