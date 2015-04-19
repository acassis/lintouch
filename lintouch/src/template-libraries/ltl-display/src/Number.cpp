// $Id: Number.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Number.cpp -- 
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
//   DATE: 11 November 2004
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

#include <qpen.h>
#include <qbrush.h>
#include <qfont.h>

#include "Number.hh"
#include "Number.xpm"

#include "config.h"

struct Number::NumberPrivate{
    NumberPrivate():m_localizator( Localizator::nullPtr() ){}
    ~NumberPrivate(){}

    LocalizatorPtr m_localizator;
    QRegion         m_collisionRegion;
    QRect           m_points;
    int             m_font_orig_size;

    QRect           m_input_pos;
    QString         m_input;

    //data structure for properties
    QPen    m_pen;
    QBrush  m_brush;
    QFont   m_font;
    QColor  m_color;
    AlignmentFlags m_alignment;
    int     m_base;
};

Number::Number( LoggerPtr logger ) :
Template( "Number", logger ), d( new NumberPrivate() ){

    Q_CHECK_PTR( d );
    Q_ASSERT( d );

    //register info
    Info i;
    i.setAuthor( "Shivaji Basu <shivaji@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Number", "ltl-display" ) );
    i.setLongDescription( QObject::tr( "Number", "ltl-display" ) );
    i.setIcon( QImage( (const char **) Number_xpm ) );

    registerInfo( i );

    // create properties of this template

    const QString group = QObject::tr( "Appearance", "ltl-display" );

    registerProperty( group, "border",
            new Property( QObject::tr( "Border", "ltl-display" ), "pen",
            "#000000;1;NOPEN", logger ) );

    registerProperty( group, "fill",
            new Property( QObject::tr( "Background", "ltl-display" ),
                "brush", "", logger ));

    registerProperty( group, "font",
            new Property( QObject::tr( "Font", "ltl-display" ),
                "font", "", logger ) );

    registerProperty( group, "font-color",
            new Property( QObject::tr( "Font Color", "ltl-display" ),
                "color", "#000000",
                logger ) );

    registerProperty( group, "halign",
            new Property( QObject::tr( "Horizontal Alignment", "ltl-display" ),
                "choice(left,right,hcenter,justify)", "hcenter", logger ) );

    registerProperty( group, "valign",
            new Property( QObject::tr( "Vertical Alignment", "ltl-display" ),
                "choice(top,bottom,vcenter)", "vcenter", logger ) );

    registerProperty( group, "base",
            new Property( QObject::tr( "Base", "ltl-display" ),
                "choice(binary,octal,decimal,hexadecimal)", "decimal",
                logger ) );

    // create iopins of this template
    registerIOPin( QObject::tr( "Inputs", "ltl-display" ), "input",
            new IOPin( this, QObject::tr( "Input", "ltl-display" ),
                "number" ) );
}

Number::~Number() {
    delete d;
}

void Number::drawShape( QPainter & p ) {
    Q_ASSERT( d );
    p.save();

    p.setBrush( d->m_brush );
    p.setPen( d->m_pen );
    p.drawRect( d->m_points );

    //Although reformatting was not required but this line is added at the
    //request of Patrick

    //used setNum to sprintf as it easier with base value to implement.
    d->m_input.setNum( iopins()["input"]->realNumberValue(), d->m_base );

    if( ! d->m_input.isEmpty() ) {
        p.setFont( d->m_font );
        p.setPen( d->m_color );
        p.drawText( d->m_input_pos, d->m_alignment, d->m_input );
    }

    p.restore();
}

void Number::mousePressed( const QPoint & /* e */ ) {
    //qWarning( "mousePressed at %d, %d", e.x(), e.y() );
}

void Number::mouseReleased( const QPoint & /* e */ ) {
    //qWarning( "mouseReleased at %d, %d", e.x(), e.y() );
}

void Number::mouseMoved( const QPoint & /* e */ ) {
    //qWarning( "mouseMoved at %d, %d", e.x(), e.y() );
}

void Number::mouseDoubleClicked( const QPoint & /* e */ ) {
    //qWarning( "mouseDoubleClicked at %d, %d", e.x(), e.y() );
}

void Number::setRect( const QRect & rect ) {

    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    // compute new font sizes
    recalculateFontSizes();

    // shedule repaint
    update();
}

void Number::propertiesChanged() {
    Q_ASSERT( d );

    d->m_pen = properties() [ "border" ]->asPen();
    d->m_brush = properties() [ "fill" ]->asBrush();
    d->m_font = properties() [ "font" ]->asFont();
    d->m_color = properties() [ "font-color" ]->asColor();
    d->m_alignment = stringToAlignment( properties()[ "halign" ]->
                            asString(), properties()[ "valign" ]->
                            asString() );
    d->m_base = stringToBase( properties() [ "base" ]->asString() );

    if( d->m_font.pixelSize() != -1 )
        d->m_font_orig_size = d->m_font.pixelSize();
    else d->m_font_orig_size = d->m_font.pointSize();

    recalculateFontSizes();

    reconstructPointArrayAndRegion();

    iopinsChanged();

    // shedule repaint
    update();
}

void Number::iopinsChanged() {

    Q_ASSERT( d );
    Q_ASSERT( iopins()["input"] );
    Q_ASSERT( iopins()["input"]->realValueType() == IOPin::NumberType );

    //used setNum to sprintf as it easier with base value to implement.
    d->m_input.setNum( iopins()["input"]->realNumberValue(), d->m_base );

    // shedule repaint
    update();
}

void Number::setVisible( bool visible ) {

    Template::setVisible( visible );

}

void Number::setCanvas( QCanvas * c ) {

    Template::setCanvas( c );

}

void Number::localize( LocalizatorPtr loc ) {
    Q_ASSERT( d );

    if( ! loc.isNull() ) {
        d->m_localizator = loc;
    }
}

void Number::reconstructPointArrayAndRegion()
{
    Q_ASSERT( d );

    d->m_points = rect();

    d->m_collisionRegion = QRegion( QPointArray( d->m_points, TRUE ) );

    // make text bounding rectangle equal to our rectangle
    d->m_input_pos = rect();
    int w = d->m_pen.width();
    // when we are big enough, clip text inside
    if( d->m_input_pos.width() >( w + 4 ) * 2 &&
            d->m_input_pos.height() >( w + 4 ) * 2 ) {
        QPoint p( w + 4, w + 4 );
        d->m_input_pos.setTopLeft( d->m_input_pos.topLeft() + p );
        d->m_input_pos.setBottomRight( d->m_input_pos.bottomRight() - p );
    }
}

void Number::recalculateFontSizes()
{
    Q_ASSERT( d );
    double scale =
        (this->rect().height() / (float)virtualRect().height());

    int newsize = (int)(d->m_font_orig_size * scale);

    // manage real font size either thru pixel size or point size
    if( d->m_font.pixelSize() != -1 ) {
        if( newsize > 0 ) d->m_font.setPixelSize( newsize );
    } else {
        if( newsize > 0 ) d->m_font.setPointSize( newsize );
    }
}

Qt::AlignmentFlags Number::stringToAlignment( const QString& ha,
        const QString& va )
{
    int align;

    //horizental alignment
    if( ha == "right" ) {
        align = Qt::AlignRight;
    } else if( ha == "justify" ) {
        align = Qt::AlignJustify;
    } else if( ha == "left" ){
        align = Qt::AlignLeft;
    } else {
        align = Qt::AlignHCenter;
    }

    //vertical alignent
    if( va == "bottom" ) {
        align |= Qt::AlignBottom;
    } else if( va == "top" ){
        align |= Qt::AlignTop;
    } else {
        align |= Qt::AlignVCenter;
    }

    return (AlignmentFlags)align;
}

int Number::stringToBase( const QString& bas )
{
    int base;

    if( bas == "binary" ){
        base = 2;
    } else if( bas == "octal" ){
        base = 8;
    } else if( bas == "hexadecimal" ){
        base = 16;
    } else {
        base = 10;
    }

    return base;
}

Template * Number::clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Number( logger );
    setupAfterClone( result, loc, tm );
    return result;
}
// vim: set et ts=4 sw=4 tw=76:
// $Id: Number.cpp 1168 2005-12-12 14:48:03Z modesto $
