// $Id: Lamp.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Lamp.cpp -- 
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


#include <qimage.h>
#include <qpen.h>
#include <qbrush.h>

#include "Lamp_Arrow.xpm"
#include "Lamp_Circle.xpm"
#include "Lamp_Line.xpm"
#include "Lamp_Paralello.xpm"
#include "Lamp_Rect.xpm"
#include "Lamp_RR.xpm"
#include "Lamp_Semic.xpm"
#include "Lamp_Triangle.xpm"
#include "Lamp.hh"
using namespace lt;

#include "config.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

#define QDEBUG_RECT( r ) qDebug(QString(#r"(%5): %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()).arg((int)this))

struct Lamp::LampPrivate {

    LampPrivate() : rotation(0.0), rr_roundness(25), sc_roundness(25) {}

    void appendPointArray( QPointArray& dst, const QPointArray& src ) const;

    QPointArray points;
    QRegion collisionRegion;

    QPen pen;
    QBrush brush;
    double rotation;

    int rr_roundness;
    int sc_roundness;

    QString link;
    QString skin;

    struct properties_t {
        QPen pen;
        QBrush brush;
        double rotation;

        int rr_roundness;
        int sc_roundness;

        QString link;
    } properties[2];
};

Lamp::Lamp( const QString& skin /*= "Rectangle"*/,
        LoggerPtr logger /*= Logger::nullPtr()*/) :
    Template( "Lamp", logger ), d( new LampPrivate )
{
    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setLongDescription ( QObject::tr( "Lamp with one input iopin",
                "ltl-display" ) );

    d->skin = skin;
    if(skin == "Ellipse") {
        i.setIcon( QImage( Lamp_Circle_xpm ));
        i.setShortDescription (
                QObject::tr( "Ellipse Lamp", "ltl-display" ) );
    } else if(skin == "Triangle") {
        i.setIcon( QImage( Lamp_Triangle_xpm ));
        i.setShortDescription (
                QObject::tr( "Triangle Lamp", "ltl-display" ) );
    } else if(skin == "Line") {
        i.setIcon( QImage( Lamp_Line_xpm ));
        i.setShortDescription (
                QObject::tr( "Line Lamp", "ltl-display" ) );
    } else if(skin == "Parallelogram") {
        i.setIcon( QImage( Lamp_Paralello_xpm ));
        i.setShortDescription (
                QObject::tr( "Parallelogram Lamp", "ltl-display" ) );
    } else if(skin == "Roundrect") {
        i.setIcon( QImage( Lamp_RR_xpm ));
        i.setShortDescription (
                QObject::tr( "Roundrect Lamp", "ltl-display" ) );
    } else if(skin == "Semicircle") {
        i.setIcon( QImage( Lamp_Semic_xpm ));
        i.setShortDescription (
                QObject::tr( "Semicircle Lamp", "ltl-display" ) );
    } else if(skin == "Arrow") {
        i.setIcon( QImage( Lamp_Arrow_xpm ));
        i.setShortDescription (
                QObject::tr( "Arrow Lamp", "ltl-display" ) );
    } else {
        d->skin = "Rectangle";
        i.setIcon( QImage( Lamp_Rect_xpm ));
        i.setShortDescription (
                QObject::tr( "Rectangle Lamp", "ltl-display" ) );
    }

    registerInfo( i );

    // create properties of this template

    const QString inactive = QObject::tr( "Appearance/Inactive", "ltl-display" );
    const QString active = QObject::tr( "Active", "ltl-display" );
    const QString inactive2 = QObject::tr( "Specific/Inactive", "ltl-display" );
    const QString active2 = QObject::tr( "Specific/Active", "ltl-display" );
    Property* p = NULL;

    // register some skins
    registerProperty( inactive, "skin",
            p = new Property( QObject::tr("Skin", "ltl-display"),
                "choice(Rectangle,Ellipse,Triangle,Line,"
                "Parallelogram,Roundrect,Semicircle,Arrow)",
                "Rectangle", logger));
    p->decodeValue(d->skin);

    // Border
    registerProperty( inactive, "border_off",
            p = new Property( QObject::tr("Border", "ltl-display"), "pen",
                "#000000;1;SolidLine", logger ) );
    registerProperty( active, "border_on",
            new Property( QObject::tr("Border", "ltl-display"), p, logger ));


    // Fill
    registerProperty( inactive, "fill_off",
            new Property( QObject::tr("Fill", "ltl-display"), "brush",
                "#777777;SOLIDPATTERN", logger ) );
    registerProperty( active, "fill_on",
            new Property( QObject::tr("Fill", "ltl-display"), "brush",
                "#FFFF00;SOLIDPATTERN", logger ) );


    // Rotation
    registerProperty( inactive, "rotation_off",
            p = new Property( QObject::tr("Rotation", "ltl-display"), "integer",
                "", logger ) );
    registerProperty( active, "rotation_on",
            new Property( QObject::tr("Rotation", "ltl-display"), p, logger ) );

    // Link
    registerProperty( inactive, "link_to_panel_off",
            p = new Property( QObject::tr("Link", "ltl-display"), "link",
                "", logger) );
    registerProperty( active, "link_to_panel_on",
            new Property( QObject::tr("Link", "ltl-display"), p, logger ) );

    // Roundness
    registerProperty( inactive2, "roundrect.roundness_off",
            p = new Property( QObject::tr("Roundrect Roundness", "ltl-display"), 
                "integer",
                "50", logger ) );
    registerProperty( active2, "roundrect.roundness_on",
            new Property( QObject::tr("Roundrect Roundness", "ltl-display"),
                p, logger ) );

    registerProperty( inactive2, "semicircle.roundness_off",
            p = new Property( QObject::tr("Semicircle Roundness", "ltl-display"),
                "integer",
                "50", logger ) );
    registerProperty( active2, "semicircle.roundness_on",
            new Property( QObject::tr("Semicircle Roundness", "ltl-display"),
                p, logger )
            );

    // create iopins of this template
    registerIOPin( QObject::tr("Inputs", "ltl-display"), "input",
            new IOPin( this, QObject::tr("Input", "ltl-display"), "bit" ) );
}

Lamp::~Lamp()
{
    delete d;
}

void Lamp::LampPrivate::appendPointArray( QPointArray& dst,
        const QPointArray& src ) const
{
    int srcSize = src.size();
    int dstSize = dst.size();

    dst.resize( dstSize + srcSize );
    for( int i = 0; i < srcSize; ++i ) {
        dst[ dstSize + i ] = src[ i ];
    }
}

void Lamp::drawShape( QPainter & p ) {

    Q_ASSERT( d );

    p.save();

    p.setPen( d->pen );
    p.setBrush( d->brush );

    p.drawPolygon( d->points );

    p.restore();
}

void Lamp::mouseReleaseEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    // test if the release is inside the region, othervise the user releases
    // mouse button semewhere else and we should not handle this click
    if( d->collisionRegion.contains( e->pos() ) ) {
        if( !d->link.isEmpty() ) {
            // qDebug( QString("new panel id: %1").arg( d->link ) );
            viewAdaptor()->displayPanel( d->link );
        }
    }
}

void Lamp::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();
    invalidate(); // because the boundingRect has changed

    // and shedule repaint
    update();
}

void Lamp::propertiesChanged() {

    const PropertyDict &pm =(const PropertyDict&) properties();

    d->skin = pm[ "skin" ]->asString();

    d->properties[1].pen = pm [ "border_on" ]->asPen();
    d->properties[1].brush = pm [ "fill_on" ]->asBrush();
    d->properties[1].rotation = (double)(pm [ "rotation_on" ]->asInteger());
    d->properties[1].link = pm [ "link_to_panel_on" ]->asString();
    d->properties[1].rr_roundness = pm["roundrect.roundness_on"]->
        asInteger();
    d->properties[1].sc_roundness = pm["semicircle.roundness_on"]->
        asInteger();

    d->properties[0].pen = pm [ "border_off" ]->asPen();
    d->properties[0].brush = pm [ "fill_off" ]->asBrush();
    d->properties[0].rotation = (double)(pm [ "rotation_off" ]->asInteger());
    d->properties[0].link = pm [ "link_to_panel_off" ]->asString();
    d->properties[0].rr_roundness = pm["roundrect.roundness_off"]->
        asInteger();
    d->properties[0].sc_roundness = pm["semicircle.roundness_off"]->
        asInteger();

    // fire off iopinsChanged to set up painting brush and pen correctly
    iopinsChanged();

    update();
}

void Lamp::iopinsChanged() {

    int index;

    if(((const IOPinDict&)iopins())[ "input" ]->realBitValue() ) {
        index = 1;
    } else {
        index = 0;
    }

    d->pen = d->properties[index].pen;
    d->brush = d->properties[index].brush;
    d->rotation = d->properties[index].rotation;
    d->link = d->properties[index].link;
    d->rr_roundness = d->properties[index].rr_roundness;
    d->sc_roundness = d->properties[index].sc_roundness;

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    // shedule repaint
    update();
}

void Lamp::reconstructPointArrayAndRegion()
{
    Q_ASSERT( d );

    // reconstruct our points and region
    QRect r( rect() );
    QRect br;

    float x_factor, y_factor;
    QWMatrix m;
    d->points = QPointArray();
    d->collisionRegion = QRegion();

    if( d->skin == "Rectangle" ) {
        ////////////////////////////////////////////////////////////////
        // [-- rectangle --]
        d->points = QPointArray( r );

        if(d->rotation != 0.0) {
            m.rotate( d->rotation );
            d->points = m.map( d->points );
            br = d->points.boundingRect();

            m.reset();
            x_factor = (r.width()-1.0) /(double)br.width();
            y_factor = (r.height()-1.0) /(double)br.height();
            m.scale( x_factor, y_factor );
            m.translate( -br.left(), -br.top() );

            d->points = m.map( d->points );

            d->points.translate( r.x(), r.y() );
        }

    } else if( d->skin == "Ellipse" ) {
        ////////////////////////////////////////////////////////////////
        // [-- ellipse --]
        d->points.makeEllipse( 0, 0, r.width(), r.height() );

        if(d->rotation != 0.0) {
            m.rotate( d->rotation );
            d->points = m.map( d->points );
            br = d->points.boundingRect();

            m.reset();
            x_factor = (r.width()-1.0) /(double)br.width();
            y_factor = (r.height()-1.0) /(double)br.height();
            m.scale( x_factor, y_factor );

            m.translate( -br.left(), -br.top() );
            d->points = m.map( d->points );
        }

        d->points.translate( r.x(), r.y() );
        d->collisionRegion = QRegion( d->points, QRegion::Ellipse );

    } else if( d->skin == "Triangle" ) {
        ////////////////////////////////////////////////////////////////
        // [-- triangle --]
        d->points.putPoints( 0, 3,
                0,r.height(),(r.width()/2),0, r.width(),r.height() );

        if(d->rotation != 0.0) {
            m.rotate( d->rotation );
            d->points = m.map( d->points );
            br = d->points.boundingRect();

            m.reset();
            x_factor = (r.width()-1.0) /(double)br.width();
            y_factor = (r.height()-1.0) /(double)br.height();
            m.scale( x_factor, y_factor );

            m.translate( -br.left(), -br.top() );
            d->points = m.map( d->points );
        }

        d->points.translate( r.x(), r.y() );

    } else if( d->skin == "Line" ) {
        ////////////////////////////////////////////////////////////////
        // [-- line --]
        // [-- prepare rotated line --]
        int yy = (r.height()-1)/2;
        int xx = (r.width()-1)/2;

        switch(((int)d->rotation)%360/45) {
            case 0:
            case 4:
                //vertical
                d->points.putPoints(0, 2, xx, 0, xx, r.height());
                break;
            case 1:
            case 5:
                // diagonal
                d->points.putPoints(0, 2, r.width(), 0, 0, r.height());
                break;
            case 2:
            case 6:
                // horizontal
                d->points.putPoints(0, 2, 0, yy, r.width(), yy);
                break;
            case 3:
            case 7:
                // diagonal
                d->points.putPoints(0, 2, 0, 0, r.width(), r.height());
                break;
        }
        d->points.translate(r.x(), r.y());

    } else if( d->skin == "Parallelogram" ) {
        ////////////////////////////////////////////////////////////////
        // [-- parallelogram --]
        // [-- prepare parallelogram --]
        d->points = QPointArray( 4 );
        d->points.putPoints( 0, 4, -30,-60, 60,-60, 30,60, -60,60 );

        if(d->rotation) {
            m.rotate( d->rotation );
            d->points = m.map( d->points );
        }
        br = d->points.boundingRect();

        m.reset();
        x_factor = (r.width()-1.0) /(double)br.width();
        y_factor = (r.height()-1.0) /(double)br.height();

        m.scale( x_factor, y_factor );
        m.translate( -br.left(), -br.top() );

        d->points = m.map( d->points );

        d->points.translate( r.x(), r.y() );

    } else if( d->skin == "Roundrect" ) {
        ////////////////////////////////////////////////////////////////
        // [-- roundrect --]
        if( d->rr_roundness > 100 ) {
            d->rr_roundness = 100;
        }
        int rfactor = 100 - d->rr_roundness;

        if( d->rr_roundness <= 0 ) {
            d->rr_roundness = 1;
        }

        if( rfactor <= 0 ) {
            rfactor = 1;
        }

        QPointArray topLeft, topRight, bottomRight, bottomLeft;
        topLeft.makeArc( 0, 0,
                d->rr_roundness, d->rr_roundness, 16*90, 16*90 );

        bottomLeft.makeArc( 0, 0,
                d->rr_roundness, d->rr_roundness, 16*180, 16*90 );
        bottomLeft.translate( 0, rfactor );

        bottomRight.makeArc( 0, 0,
                d->rr_roundness, d->rr_roundness, 16*270, 16*90 );
        bottomRight.translate( rfactor, rfactor );

        topRight.makeArc( 0, 0,
                d->rr_roundness, d->rr_roundness, 16*0, 16*90 );
        topRight.translate( rfactor, 0 );

        // [-- compose --]
        d->points = topLeft;
        d->appendPointArray( d->points, bottomLeft );
        d->appendPointArray( d->points, bottomRight );
        d->appendPointArray( d->points, topRight );

        // [-- rotate --]
        m.rotate( d->rotation );
        d->points = m.map( d->points );
        br = d->points.boundingRect();

        m.reset();
        x_factor = (r.width()-1.0) /(double)br.width();
        y_factor = (r.height()-1.0) /(double)br.height();
        m.scale( x_factor, y_factor );

        m.translate( -br.left(), -br.top() );
        d->points = m.map( d->points );

        d->points.translate( r.x(), r.y() );

    } else if( d->skin == "Semicircle" ) {
        ////////////////////////////////////////////////////////////////
        // [-- semicircle --]
        if( d->sc_roundness > 100 ) {
            d->sc_roundness = 100;
        }
        int rfactor = 100 - d->sc_roundness;

        if( d->sc_roundness <= 0 ) {
            d->sc_roundness = 1;
        }

        if( rfactor <= 0 ) {
            rfactor = 1;
        }

        QPointArray top;
        top.makeArc( 0, 0, 100, d->sc_roundness, 16*0, 16*180 );

        // [-- compose --]
        d->points = top;
        int i = d->points.size();
        d->points.resize( i + 2 );
        d->points[ i++ ] = QPoint( 0, 50 );
        d->points[ i ] = QPoint( 100, 50 );

        // [-- rotate --]
        m.rotate( d->rotation );
        d->points = m.map( d->points );
        br = d->points.boundingRect();

        m.reset();
        x_factor = (r.width()-1.0) /(double)br.width();
        y_factor = (r.height()-1.0) /(double)br.height();
        m.scale( x_factor, y_factor );

        m.translate( -br.left(), -br.top() );
        d->points = m.map( d->points );

        d->points.translate( r.x(), r.y() );

    } else if( d->skin == "Arrow" ) {
        ////////////////////////////////////////////////////////////////
        // [-- arrow --]
        // [-- prepare rotated arrow --]
        d->points.putPoints( 0, 7,
                50,0, 100,40, 70,40, 70,100, 30,100, 30,40, 0,40 );

        if( d->rotation ) {
            m.rotate( d->rotation );
            d->points = m.map( d->points );
        }
        br = d->points.boundingRect();

        if( br.width() != r.width() || br.height() != r.height() ) {
            m.reset();
            x_factor = (r.width()-1.0) /(double)br.width();
            y_factor = (r.height()-1.0) /(double)br.height();
            m.scale( x_factor, y_factor );

            m.translate( -br.left(), -br.top() );
            d->points = m.map( d->points );
        }

        d->points.translate( r.left(), r.top() );
    }
    if( d->collisionRegion.isEmpty() ) {
        d->collisionRegion = QRegion( d->points );
    }
}

Template * Lamp::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Lamp( type(), logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Lamp.cpp 1168 2005-12-12 14:48:03Z modesto $
