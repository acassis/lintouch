// $Id: Button.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Button.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 05 November 2003
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

#include <qapplication.h>
#include <qdrawutil.h>
#include <qpen.h>

#include "Button.xpm"
#include "Button.hh"

#include "config.h"

#define QDEBUG_RECT( r ) qDebug(QString(#r": %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct Button::ButtonPrivate {

    ButtonPrivate() : state(Released), lastState(Released), type(Push),
                      ignoreMouseReleased(false),
                      confirm_box(true),
                      localizator( LocalizatorPtr( new Localizator ) )
    {}

    QColorGroup makeColorGroup( const QBrush& b ) const;
    QImage makeIcon() const;

    QRegion collisionRegion;
    QPointArray areaPoints;

    int font_orig_size;

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

    IOPin* input;
    IOPin* output;

    struct properties_t {
        QColor color;
        QBrush brush;
        QFont font;
        QFont orig_font;
        QString label;

        QPen cpen;
        QBrush cbrush;
    } properties[2];
};

Button::Button( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    Template( "Button", logger ), d( new ButtonPrivate ) {

    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "Button", "ltl-input" ) );
    i.setLongDescription ( QObject::tr(
                "Button with one input and one output iopin",
                "ltl-input" ) );

    i.setIcon( d->makeIcon() );

    registerInfo( i );

    const QString inactive = QObject::tr( "Appearance/Inactive",
            "ltl-input" );
    const QString active = QObject::tr( "Active",
            "ltl-input" );
    const QString inactive2 = QObject::tr( "Confirmation Box/Inactive",
            "ltl-input" );
    const QString active2 = QObject::tr( "Confirmation Box/Active",
            "ltl-input" );
    Property* p = NULL;

    // Type
    registerProperty( inactive, "type",
            new Property( QObject::tr("Type", "ltl-input"),
                "choice(push,toggle)",
                "push", logger ) );

    // enable/disable CB
    registerProperty( inactive, "confirm_box",
            new Property( QObject::tr("Show Confirmation Box", "ltl-input" ),
                "choice(enabled,disabled)", "enabled", logger ) );

    // Label
    registerProperty( inactive, "label_off",
            new Property( QObject::tr("Label", "ltl-input" ), "string",
                "TEXT", logger ) );
    registerProperty( active, "label_on",
            new Property( QObject::tr("Label", "ltl-input" ), "string",
                "TEXT", logger ) );

    // Font
    registerProperty( inactive, "font_off",
            p = new Property( QObject::tr("Font", "ltl-input" ), "font",
                "", logger ) );
    registerProperty( active, "font_on",
            new Property( QObject::tr("Font", "ltl-input" ), p, logger ) );

    // Color
    registerProperty( inactive, "label_color_off",
            p = new Property( QObject::tr("Color", "ltl-input" ), "color",
                "", logger ) );
    registerProperty( active, "label_color_on",
            new Property( QObject::tr("Color", "ltl-input" ), p, logger ));

    // Fill
    registerProperty( inactive, "fill_off",
            new Property( QObject::tr("Fill", "ltl-input" ), "brush",
                "#777777;SOLIDPATTERN", logger ) );
    registerProperty( active, "fill_on",
            new Property( QObject::tr("Fill", "ltl-input" ), "brush",
                "#FFFF00;SOLIDPATTERN", logger ) );

    // CB Border
    registerProperty( inactive2, "confirm_border_off",
            p = new Property( QObject::tr("Border", "ltl-input" ), "pen",
                "", logger ) );
    registerProperty( active2, "confirm_border_on",
            new Property( QObject::tr("Border", "ltl-input" ), p, logger ) );

    registerProperty( inactive2, "confirm_fill_off",
            new Property( QObject::tr("Fill", "ltl-input" ), "brush",
                "#777777;SOLIDPATTERN", logger ) );
    registerProperty( active2, "confirm_fill_on",
            new Property( QObject::tr("Fill", "ltl-input" ), "brush",
            "#FFFF00;SOLIDPATTERN", logger ) );

    // create iopins of this template
    registerIOPin( QObject::tr("Inputs", "ltl-input") , "input",
            d->input = new IOPin( this,
                QObject::tr("Input", "ltl-input" ), "bit" ) );
    registerIOPin( QObject::tr("Outputs", "ltl-input") , "output",
            d->output = new IOPin( this,
                QObject::tr("Output", "ltl-input" ), "bit" ) );
}

Button::~Button()
{
    delete d;
}

void Button::localize( LocalizatorPtr localizator ) {

    Q_CHECK_PTR( d );
    d->localizator = localizator;
}

void Button::mousePressEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);
    if( d->collisionRegion.contains( e->pos() ) ) {
        d->lastState = d->state;

        if( d->type == ButtonPrivate::Push ) {
            d->state = ButtonPrivate::Pressed;

            d->output->setRequestedBitValue( 1 );
            syncIOPinsRequestedValues();
            iopinsChanged();

        } else {
            if( d->lastState == ButtonPrivate::Released ) {
                d->state = ButtonPrivate::Pressed;
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
void Button::mouseReleaseEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    d->lastState = d->state;

    if( d->type == ButtonPrivate::Push ) {
        d->state = ButtonPrivate::Released;

        d->output->setRequestedBitValue( 0 );
        syncIOPinsRequestedValues();
        iopinsChanged();
    } else {
        if( !d->ignoreMouseReleased &&
                d->lastState == ButtonPrivate::Pressed )
        {
            d->state = ButtonPrivate::Released;

            d->output->setRequestedBitValue( 0 );
            syncIOPinsRequestedValues();
            iopinsChanged();
        }
    }

    // update() is called in iopinsChanged() so we need not to call it
    // here
}

void Button::propertiesChanged() {

    const PropertyDict &pm =(const PropertyDict&)properties();

    QString tt = pm["type"]->asString();
    if( tt == "toggle" ) {
        d->type = ButtonPrivate::Toggle;
    } else {
        d->type = ButtonPrivate::Push;
    }

    tt = pm["confirm_box"]->asString();
    d->confirm_box =(tt == "enabled" );

    d->properties[1].color = pm [ "label_color_on" ]->asColor();
    d->properties[1].brush = pm [ "fill_on" ]->asBrush();
    d->properties[1].orig_font = pm [ "font_on" ]->asFont();
    d->properties[1].label = d->localizator->localizedMessage(
            pm [ "label_on" ]->asString() );
    d->properties[1].cpen = pm [ "confirm_border_on" ]->asPen();
    d->properties[1].cbrush = pm [ "confirm_fill_on" ]->asBrush();

    d->properties[0].color = pm [ "label_color_off" ]->asColor();
    d->properties[0].brush = pm [ "fill_off" ]->asBrush();
    d->properties[0].orig_font = pm [ "font_off" ]->asFont();
    d->properties[0].label = d->localizator->localizedMessage(
            pm [ "label_off" ]->asString() );
    d->properties[0].cpen = pm [ "confirm_border_off" ]->asPen();
    d->properties[0].cbrush = pm [ "confirm_fill_off" ]->asBrush();

    reconstructPointArrayAndRegion();

    recalculateFontSizes();

    // fetch prioperties for current value
    iopinsChanged();
}

void Button::iopinsChanged() {

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

    d->cg = d->makeColorGroup( d->brush );

    // output iopin

    if( d->state == ButtonPrivate::Pressed &&
            d->output->realBitValue() == 1 &&
            d->output->requestedBitValue() == 1 ) {

        d->cpen = d->properties[1].cpen;
        d->cbrush = d->properties[1].cbrush;

    } else {

        d->cpen = d->properties[0].cpen;
        d->cbrush = d->properties[0].cbrush;

    }

    // schedule repaint
    update();
}

void Button::drawShape( QPainter &p )
{
    p.save();

    drawRectButton( p );
    if( d->confirm_box ) {
        drawConfBox( p );
    }

    p.restore();
}

void Button::drawConfBox( QPainter& p )
{
    p.setPen( d->cpen );
    p.setBrush( d->cbrush );
    p.drawRect( d->cb );
}

void Button::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // reconstruct pointarray and region
    reconstructPointArrayAndRegion();

    recalculateFontSizes();

    // and schedule repaint
    update();
}

void Button::reconstructPointArrayAndRegion()
{
    QRect r = rect();

    if( d->confirm_box ) {

        float w16 = r.width() / 16.0;
        float h16 = r.height() / 16.0;
        d->cb = QRect( r.x() +(int)w16, r.y() +(int)(h16 * 13),
                r.width() -(int)(w16 * 2),(int)(h16 * 2) );

        d->labelRect = QRect( r.x() +(int)w16, r.y() +(int)h16,
                r.width() -(int)(w16 * 2), r.height() -(int)(h16 * 5) );
    } else {
        d->cb = QRect();
        d->labelRect = r;
    }

    d->collisionRegion = QRegion( r );
    d->areaPoints = QPointArray( d->collisionRegion.boundingRect() );
}

QPointArray Button::areaPoints() const
{
    return d->areaPoints;
}

QRegion Button::collisionRegion() const
{
    return d->collisionRegion;
}

////////////////////////////////
// [-- ButtonPrivate methods --]
QColorGroup Button::ButtonPrivate::makeColorGroup( const QBrush& b ) const {

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

QImage Button::ButtonPrivate::makeIcon() const {

    return QImage( Button_xpm );
}

/////////////////////////////
// [-- RECTANGULAR BUTTON --]
void Button::drawRectButton( QPainter & p ) {

    QRect r = rect();

    if( d->state == ButtonPrivate::Released ) {
        p.setBrush( d->cg.shadow() );
        p.setPen( d->cg.shadow() );
        p.drawLine( r.bottomLeft(), r.bottomRight() );
        p.drawLine( r.bottomRight(), r.topRight() );

        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen( d->cg.light() );
        p.setBrush( d->cg.light() );
        p.drawLine( r.bottomLeft(), r.topLeft() );
        p.drawLine( r.topLeft(), r.topRight() );

        r.moveBy( 1, 1 );
        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen( d->cg.dark() );
        p.setBrush( d->cg.dark() );
        p.drawLine( r.bottomLeft(), r.bottomRight() );
        p.drawLine( r.bottomRight(), r.topRight() );

        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen( d->cg.midlight() );
        p.setBrush( d->cg.midlight() );
        p.drawLine( r.bottomLeft(), r.topLeft() );
        p.drawLine( r.topLeft(), r.topRight() );

    } else {
        p.setPen( d->cg.light() );
        p.setBrush( d->cg.light() );
        p.drawLine( r.bottomLeft(), r.bottomRight() );
        p.drawLine( r.bottomRight(), r.topRight() );

        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen( d->cg.shadow() );
        p.setBrush( d->cg.shadow() );
        p.drawLine( r.bottomLeft(), r.topLeft() );
        p.drawLine( r.topLeft(), r.topRight() );

        r.moveBy( 1, 1 );
        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen( d->cg.midlight() );
        p.setBrush( d->cg.midlight() );
        p.drawLine( r.bottomLeft(), r.bottomRight() );
        p.drawLine( r.bottomRight(), r.topRight() );

        r.setSize( QSize( r.width()-1, r.height()-1) );
        p.setPen( d->cg.dark() );
        p.setBrush( d->cg.dark() );
        p.drawLine( r.bottomLeft(), r.topLeft() );
        p.drawLine( r.topLeft(), r.topRight() );
    }

    r.moveBy( 1, 1 );
    r.setSize( QSize( r.width()-1, r.height()-1) );
    p.setPen( d->cg.foreground() );
    p.setBrush( d->brush );
    p.drawRect( r );

    // label
    p.setPen( d->color );
    p.setBrush( QBrush( NoBrush ) );
    p.setFont( d->font );
    p.drawText( d->labelRect, Qt::AlignCenter | Qt::AlignVCenter, d->label );
}

void Button::recalculateFontSizes()
{
    d->properties[0].font = makeFont(d->properties[0].orig_font);
    d->properties[1].font = makeFont(d->properties[1].orig_font);

    iopinsChanged();
}

Template * Button::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Button( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Button.cpp 1168 2005-12-12 14:48:03Z modesto $
