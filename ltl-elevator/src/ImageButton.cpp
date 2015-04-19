// $Id: ImageButton.cpp 950 2005-07-14 11:26:30Z modesto $
//
//   FILE: ImageButton.cpp --
// AUTHOR: Jiri Barton <jbar@swac.cz>
//   DATE: 19 September 2005
//
// Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
//
// This application is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License, or(at
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

#include "ImageButton.hh"
#include "ImageButton.xpm"

#include <qapplication.h>
#include <qdrawutil.h>
#include <qpen.h>

using namespace lt;

#include "config.h"

#define QDEBUG_RECT( r ) qDebug(QString(#r"(%5): %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()).arg((int)this))

struct ImageButton::ImageButtonPrivate
{
    ImageButtonPrivate() : state(Released), lastState(Released), type(Push),
    ignoreMouseReleased(false),
    localizator( LocalizatorPtr( new Localizator ) ), input(NULL),
    output(NULL) {}

    QColorGroup makeColorGroup( const QBrush& b ) const;
    void drawRectButton( QPainter & p, QRect r );
    void drawConfBox( QPainter& p );

    QRegion collisionRegion;

    typedef enum State { Released, Pressed };
    State state, lastState;

    typedef enum Type { Push, Toggle };
    Type type;

    bool ignoreMouseReleased;

    bool confirm_box;

    QRect labelRect;
    QRect cb;
    QColorGroup cg;

    LocalizatorPtr localizator;

    QColor color;
    QBrush brush;
    QFont font;
    QString label;

    QPen cpen;
    QBrush cbrush;

    QPixmap px;
    QPixmap src_px;

    IOPin* input;
    IOPin* output;

    struct properties_t {
        QColor color;
        QBrush brush;
        QFont font;
        QString label;
        QPen cpen;
        QBrush cbrush;
        QPixmap image;
    } properties[2];
};

ImageButton::ImageButton( LoggerPtr logger ) :
    Template( "ImageButton", logger ),
    d( new ImageButtonPrivate() )
{
    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Jiri Barton <jbar@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr("ImageButton", "ltl-elevator" ) );
    i.setLongDescription( QObject::tr( "Button with an image on it.",
        "ltl-elevator" ) );
    i.setIcon( QImage( (const char **) ImageButton_xpm ) );

    registerInfo( i );

    // define groups
    QString inactive = QObject::tr("Appearance/Inactive", "ltl-elevator");
    QString active = QObject::tr("Active", "ltl-elevator");
    QString inactive2 = QObject::tr("Confirmation Box/Inactive",
        "ltl-elevator");
    QString active2 = QObject::tr("Confirmation Box/Active",
        "ltl-elevator");

    // Type
    registerProperty( inactive, "type",
            new Property( QObject::tr( "Type", "ltl-elevator" ),
                "choice(push,toggle)",
                "push", logger ) );

     // enable/disable CB
    registerProperty( inactive, "confirm_box",
            new Property( QObject::tr( "Show Confirmation Box",
                "ltl-elevator" ), "choice(enabled,disabled)", "enabled",
                logger ) );

    // Label
    registerProperty( inactive, "label_off",
            new Property( QObject::tr( "Label",
                "ltl-elevator" ), "string", "on", logger ) );
    registerProperty( active, "label_on",
            new Property( QObject::tr( "Label",
                "ltl-elevator" ), "string", "on", logger ) );

    // Font
    registerProperty( inactive, "font_off",
            new Property( QObject::tr( "Font", "ltl-elevator" ), "font",
                "", logger ) );
    registerProperty( active, "font_on",
            new Property( QObject::tr( "Font", "ltl-elevator" ), "font",
                "", logger ) );

    // Color
    registerProperty( inactive, "label_color_off",
            new Property( QObject::tr( "Color", "ltl-elevator" ), "color",
                "", logger ) );
    registerProperty( active, "label_color_on",
            new Property( QObject::tr( "Color", "ltl-elevator" ), "color",
                "", logger ) );

    // Fill
    registerProperty( inactive, "fill_off",
            new Property( QObject::tr( "Fill", "ltl-elevator" ), "brush",
                "", logger ) );
    registerProperty( active, "fill_on",
            new Property( QObject::tr( "Fill", "ltl-elevator" ), "brush",
                "", logger ) );

    // CB Border
    registerProperty( inactive2, "confirm_border_off",
            new Property( QObject::tr( "Border", "ltl-elevator" ), "pen",
                "", logger ) );
    registerProperty( active2, "confirm_border_on",
            new Property( QObject::tr( "Border", "ltl-elevator" ), "pen",
                "", logger ) );

    registerProperty( inactive2, "confirm_fill_off",
            new Property( QObject::tr( "Fill", "ltl-elevator" ), "brush",
                "#7f0000;SOLIDPATTERN", logger ) );
    registerProperty( active2, "confirm_fill_on",
            new Property( QObject::tr( "Fill", "ltl-elevator" ), "brush",
                "#007f00;SOLIDPATTERN", logger ) );

    // Image resource id
    registerProperty( inactive, "image_id_off",
            new Property( QObject::tr( "Image", "ltl-elevator" ),
              "resource", "", logger ) );
    registerProperty( active, "image_id_on",
            new Property( QObject::tr( "Image", "ltl-elevator" ),
              "resource", "", logger ) );

    // create iopins of this template
    registerIOPin( QObject::tr("Inputs", "ltl-elevator"), "input",
            d->input = new IOPin( this, QObject::tr( "Input",
                "ltl-elevator" ), "bit" ) );
    registerIOPin( QObject::tr("Outputs", "ltl-buttons"), "output",
            d->output = new IOPin( this, QObject::tr( "Output",
                "ltl-elevator" ), "bit" ) );
}

ImageButton::~ImageButton()
{
    delete d;
}

void ImageButton::drawShape( QPainter & p )
{
    p.save();

    d->drawRectButton( p, rect() );
    d->drawConfBox( p );

    p.restore();
}

void ImageButton::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    // shedule repaint
    update();
}

void ImageButton::propertiesChanged()
{
    Q_ASSERT( d );
    const PropertyDict &pm =(const PropertyDict&)properties();

    QString tt = pm["type"]->asString();
    if( tt == "toggle" ) {
        d->type = ImageButtonPrivate::Toggle;
    } else {
        d->type = ImageButtonPrivate::Push;
    }

    tt = pm["confirm_box"]->asString();
    d->confirm_box =(tt == "enabled" );

    d->properties[1].color = pm [ "label_color_on" ]->asColor();
    d->properties[1].brush = pm [ "fill_on" ]->asBrush();
    d->properties[1].font = pm [ "font_on" ]->asFont();
    d->properties[1].label = d->localizator->localizedMessage(
            pm [ "label_on" ]->asString() );
    d->properties[1].cpen = pm [ "confirm_border_on" ]->asPen();
    d->properties[1].cbrush = pm [ "confirm_fill_on" ]->asBrush();
    d->properties[1].image =
        QPixmap ( d->localizator->resourceAsByteArray (
        pm[ "image_id_on" ]->asString () ) );

    d->properties[0].color = pm [ "label_color_off" ]->asColor();
    d->properties[0].brush = pm [ "fill_off" ]->asBrush();
    d->properties[0].font = pm [ "font_off" ]->asFont();
    d->properties[0].label = d->localizator->localizedMessage(
            pm [ "label_off" ]->asString() );
    d->properties[0].cpen = pm [ "confirm_border_off" ]->asPen();
    d->properties[0].cbrush = pm [ "confirm_fill_off" ]->asBrush();
    d->properties[0].image =
        QPixmap ( d->localizator->resourceAsByteArray (
        pm[ "image_id_off" ]->asString () ) );

    reconstructPointArrayAndRegion();

    d->px = QPixmap(); //reset the pixmap
    d->label = QString(); //reset the label

    // fire off iopinsChanged to set up painting brush and pen correctly
    iopinsChanged();
}

void ImageButton::iopinsChanged()
{
    Q_CHECK_PTR( d );

    int index;

    if( d->input->realBitValue() ) {
        index = 1;
    } else {
        index = 0;
    }

    d->color = d->properties[index].color;
    d->brush = d->properties[index].brush;
    d->font = d->properties[index].font;
    d->label  = d->properties[index].label;
    d->src_px = d->properties[index].image;

    d->cg = d->makeColorGroup( d->brush );

    // output iopin

    if( d->state == ImageButtonPrivate::Pressed &&
            d->output->realBitValue() == 1 &&
            d->output->requestedBitValue() == 1 ) {

        d->cpen = d->properties[1].cpen;
        d->cbrush = d->properties[1].cbrush;

    } else {

        d->cpen = d->properties[0].cpen;
        d->cbrush = d->properties[0].cbrush;

    }

    reconstructPointArrayAndRegion();

    // schedule repaint
    if( canvas() ) update();
}


void ImageButton::localize( LocalizatorPtr loc )
{
    Q_ASSERT( d );
    d->localizator = loc;

    propertiesChanged();
}

void ImageButton::reconstructPointArrayAndRegion()
{
    QRect r = rect();

    float w16 = r.width() / 16.0;
    float h16 = r.height() / 16.0;
    d->cb = QRect( r.x() + (int)w16, r.y() + (int)(h16 * 13),
            r.width() - (int)(w16 * 2), (int)(h16 * 2) );

    d->labelRect = QRect( r.x() + (int)w16, r.y() + (int)h16,
            r.width() - (int)(w16 * 2), r.height() - (int)(h16 * 5) );

    d->collisionRegion = QRegion( r );

    // pixmap prepare
    if( !d->src_px.isNull() ) {
        r.moveBy( 2, 2 );
        r.setSize( QSize( r.width() - 4, r.height() - 4 ) );
        d->px = QPixmap( r.width(), r.height() );
        d->px.fill();

        QPainter p( &d->px );
        p.scale( (double)r.width()/(double)d->src_px.width(),
                (double)r.height()/(double)d->src_px.height() );
        p.drawPixmap( QPoint( 0, 0 ), d->src_px );
    }
}

void ImageButton::mousePressEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);
    if( d->collisionRegion.contains( e->pos() ) ) {
        d->lastState = d->state;

        if( d->type == ImageButtonPrivate::Push ) {
            d->state = ImageButtonPrivate::Pressed;

            d->output->setRequestedBitValue( 1 );
            syncIOPinsRequestedValues();
            iopinsChanged();

        } else {
            if( d->lastState == ImageButtonPrivate::Released ) {
                d->state = ImageButtonPrivate::Pressed;
                d->ignoreMouseReleased = true;

                d->output->setRequestedBitValue( 1 );
                syncIOPinsRequestedValues();
                iopinsChanged();
            } else {
                d->ignoreMouseReleased = false;
            }
        }

        // update() is called in iopinsChanged() so we need not to call it
        // here
    }
}

void ImageButton::mouseReleaseEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    d->lastState = d->state;

    if( d->type == ImageButtonPrivate::Push ) {
        d->state = ImageButtonPrivate::Released;

        d->output->setRequestedBitValue( 0 );
        syncIOPinsRequestedValues();
        iopinsChanged();
    } else {
        if( !d->ignoreMouseReleased &&
                d->lastState == ImageButtonPrivate::Pressed )
        {
            d->state = ImageButtonPrivate::Released;

            d->output->setRequestedBitValue( 0 );
            syncIOPinsRequestedValues();
            iopinsChanged();
        }
    }

    // update() is called in iopinsChanged() so we need not to call it
    // here
}

QPointArray ImageButton::areaPoints() const
{
    return QPointArray( d->collisionRegion.boundingRect() );
}

QRegion ImageButton::collisionRegion() const
{
    return d->collisionRegion;
}

Template * ImageButton::clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new ImageButton( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

////////////////////////////////
// [-- ImageButtonPrivate methods --]
QColorGroup ImageButton::ImageButtonPrivate::makeColorGroup( const QBrush& b ) const {

    QColorGroup cg;
    cg.setBrush( QColorGroup::Foreground, b );

    QColor bc = b.color();

    cg.setColor( QColorGroup::Light, bc.light() );
    cg.setColor( QColorGroup::Midlight, bc.light() );
    cg.setColor( QColorGroup::Button, bc );
    cg.setColor( QColorGroup::Dark, bc.dark() );
    cg.setColor( QColorGroup::Shadow, bc.dark() );

    if( cg.light() == QColor(255,255,255) ) {
        cg.setColor( QColorGroup::Light, QColor( 225,225,225 ) );
        cg.setColor( QColorGroup::Midlight, QColor( 225, 225, 225 ) );
    }

    if( cg.dark() == QColor(0,0,0) ) {
        cg.setColor( QColorGroup::Light, QColor( 32,32,32 ) );
        cg.setColor( QColorGroup::Midlight, QColor( 32,32,32 ) );
    }

    return cg;
}

/////////////////////////////
// [-- RECTANGULAR ImageBUTTON --]
void ImageButton::ImageButtonPrivate::drawRectButton( QPainter & p,
    QRect r ) {

    if(  state == ImageButtonPrivate::Released ) {
        p.setBrush(  cg.shadow() );
        p.setPen(  cg.shadow() );
        p.drawRect( r );

        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen(  cg.light() );
        p.setBrush(  cg.light() );
        p.drawRect( r );

        r.moveBy( 1, 1 );
        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen(  cg.dark() );
        p.setBrush(  cg.dark() );
        p.drawRect( r );

        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen(  cg.midlight() );
        p.setBrush(  cg.midlight() );
        p.drawRect( r );
    } else {
        p.setPen(  cg.light() );
        p.setBrush(  cg.light() );
        p.drawRect( r );

        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen(  cg.shadow() );
        p.setBrush(  cg.shadow() );
        p.drawRect( r );

        r.moveBy( 1, 1 );
        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen(  cg.midlight() );
        p.setBrush(  cg.midlight() );
        p.drawRect( r );

        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen(  cg.dark() );
        p.setBrush(  cg.dark() );
        p.drawRect( r );
    }

    r.moveBy( 1, 1 );
    r.setSize( QSize( r.width()-1, r.height()-1) );
    p.setPen(  cg.foreground() );
    p.setBrush(  cg.foreground() );
    p.drawRect( r );

    if( ! px.isNull() ) {
        p.drawPixmap( r.x(), r.y(),  px, 0, 0, r.width(), r.height() );
    }

    // label
    p.setPen (  cpen );
    p.setBrush( QBrush( NoBrush ) );
    p.setFont (  font );
    p.drawText (  labelRect, Qt::AlignCenter | Qt::AlignVCenter,  label );
}

void ImageButton::ImageButtonPrivate::drawConfBox( QPainter& p )
{
    p.setPen( cpen );
    p.setBrush( cbrush );
    p.drawRect( cb );
}


// vim: set et ts=4 sw=4 tw=76:
// $Id: ImageButton.cpp 950 2005-07-14 11:26:30Z modesto $
