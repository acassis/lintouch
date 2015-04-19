// $Id: Text.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Text.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 04 November 2003
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


#include <qfontmetrics.h>
#include <qpixmap.h>
#include <qrect.h>

#include "lt/templates/Localizable.hh"
#include "lt/templates/Localizator.hh"

#include "Text.xpm"
#include "Text.hh"

using namespace lt;

#include "config.h"

#define QDEBUG_RECT( r ) qDebug(QString(#r": %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct Text::TextPrivate {

    TextPrivate() : localizator( LocalizatorPtr( new Localizator ) ) {

        flags = Qt::AlignHCenter|Qt::AlignVCenter;
    }

    QRegion collisionRegion;

    QColor color;
    int rotation;
    QFont font;
    QString link;
    QString text;
    int flags;

    struct properties_t {
        QColor color;
        int rotation;
        QFont font;
        QFont orig_font;
        QString link;
        QString text;
        int flags;
    } properties[2];

    LocalizatorPtr localizator;
};

static int _StringToFlags( const QString &ha, const QString &va ) {

    int flags;

    // update text align flags
    if( ha == "right" ) {
        flags = Qt::AlignRight;
    } else if( ha == "hcenter" ) {
        flags = Qt::AlignHCenter;
    } else if( ha == "justify" ) {
        flags = Qt::AlignJustify;
    } else {
        flags = Qt::AlignLeft;
    }

    if( va == "bottom" ) {
        flags |= Qt::AlignBottom;
    } else if( va == "vcenter" ) {
        flags |= Qt::AlignVCenter;
    } else {
        flags |= Qt::AlignTop;
    }

#ifdef Q_WS_QWS
    flags |= Qt::WordBreak | Qt::NoAccel;
#else
    flags |= Qt::BreakAnywhere | Qt::NoAccel;
#endif

    return flags;
}


Text::Text( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    Template( "Text", logger ), d( new TextPrivate ) {

    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Text", "ltl-display" ) );
    i.setLongDescription( QObject::tr( "Text with one input iopin",
                "ltl-display" ) );

    i.setIcon(QImage( Text_xpm ));

    registerInfo( i );

    const QString inactive = QObject::tr( "Appearance/Inactive", "ltl-display" );
    const QString active = QObject::tr( "Active", "ltl-display" );
    Property* p = NULL;

    // simpletext properties
    registerProperty( inactive, "text_off",
            new Property( QObject::tr("Text", "ltl-display"), "string",
                "TEXT", logger ) );
    registerProperty( active, "text_on",
            new Property( QObject::tr("Text", "ltl-display"), "string",
                "TEXT", logger ) );

    // Font
    registerProperty( inactive, "font_off",
            new Property( QObject::tr("Font", "ltl-display"), "font",
                "", logger ) );
    registerProperty( active, "font_on",
            new Property( QObject::tr("Font", "ltl-display"), "font",
                "", logger ) );

    // Color
    registerProperty( inactive, "color_off",
            p = new Property( QObject::tr("Color", "ltl-display"), "color",
                "", logger ) );
    registerProperty( active, "color_on",
            new Property( QObject::tr("Color", "ltl-display"), p, logger ));

    // text align flags
    registerProperty( inactive, "halign_off",
            p = new Property(QObject::tr("Horizontal Alignment", "ltl-display"),
                "choice(left,right,hcenter,justify)", "hcenter", logger ) );
    registerProperty( active, "halign_on",
            new Property( QObject::tr("Horizontal Alignment", "ltl-display"),
                p, logger ) );

    registerProperty( inactive, "valign_off",
            p = new Property( QObject::tr("Vertical Alignment", "ltl-display"),
                "choice(top,bottom,vcenter)", "vcenter", logger ) );
    registerProperty( active, "valign_on",
            new Property( QObject::tr("Vertical Alignment", "ltl-display"),
                p, logger ) );

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

    // create iopins of this template
    registerIOPin( QObject::tr("Inputs", "ltl-display"), "input",
            new IOPin( this, QObject::tr("Input", "ltl-display"), "bit" ) );
}

Text::~Text()
{
    delete d;
}

void Text::localize( LocalizatorPtr localizator ) {

    Q_ASSERT( d );
    d->localizator = localizator;

    propertiesChanged();
}

void Text::drawShape( QPainter & p )
{
    Q_ASSERT( d );

    p.save();
    p.setPen( d->color );

    p.setFont( d->font );

#ifdef Q_WS_QWS
    p.setClipRect( rect(), QPainter::CoordPainter );
    p.drawText( rect(), d->flags, d->text, -1 );
#else
    int ww = rect().width() / 2;
    int hh = rect().height() / 2;
    QRect rr;

    switch((d->rotation%360)/45) {
        case 1:
        case 2:
        case 5:
        case 6:
            rr = QRect( -hh, -ww, rect().height(), rect().width());
            break;
        default:
            rr = QRect( -ww, -hh, rect().width(), rect().height());
    }

    p.translate( rect().x() + ww, rect().y() + hh );
    p.rotate( d->rotation );

    p.drawText(rr, d->flags, d->text, -1 );
#endif

    d->collisionRegion = QRegion( rect() );

    p.restore();
}

void Text::setRect( const QRect & rect )
{
    Q_ASSERT(d);

    Template::setRect( rect );

    // reconstruct pointarray and region
    d->collisionRegion = QRegion( rect );

    recalculateFontSizes();

    // and shedule repaint
    update();
}

void Text::mouseReleaseEvent( QMouseEvent * e )
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

void Text::propertiesChanged() {

    Q_ASSERT( d );
    const PropertyDict& pm = (const PropertyDict&) properties();

    update();

    d->properties[1].color = pm [ "color_on" ]->asColor();
    d->properties[1].rotation = pm [ "rotation_on" ]->asInteger();
    d->properties[1].link = pm [ "link_to_panel_on" ]->asString();
    d->properties[1].orig_font = pm [ "font_on" ]->asFont();
    d->properties[1].text = d->localizator->localizedMessage(
            pm [ "text_on" ]->asString() );
    d->properties[1].flags = _StringToFlags(
            pm["halign_on"]->asString(),
            pm["valign_on"]->asString() );

    d->properties[0].color = pm [ "color_off" ]->asColor();
    d->properties[0].rotation = pm [ "rotation_off" ]->asInteger();
    d->properties[0].link = pm [ "link_to_panel_off" ]->asString();
    d->properties[0].orig_font = pm [ "font_off" ]->asFont();
    d->properties[0].text = d->localizator->localizedMessage(
            pm [ "text_off" ]->asString() );
    d->properties[0].flags = _StringToFlags(
            pm["halign_off"]->asString(),
            pm["valign_off"]->asString() );

    recalculateFontSizes();

    // fire off iopinsChanged to set up painting pen correctly
    iopinsChanged();
}

void Text::iopinsChanged() {

    int index;

    if( ((const IOPinDict&)iopins())[ "input" ]->realBitValue() ) {
        index = 1;
    } else {
        index = 0;
    }

    d->color = d->properties[index].color;
    d->rotation = d->properties[index].rotation;
    d->link = d->properties[index].link;
    d->font = d->properties[index].font;
    d->text = d->properties[index].text;
    d->flags = d->properties[index].flags;

    // and shedule repaint
    update();
}

void Text::recalculateFontSizes()
{
    d->properties[0].font = makeFont(d->properties[0].orig_font);
    d->properties[1].font = makeFont(d->properties[1].orig_font);

    iopinsChanged();
}

Template * Text::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Text( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Text.cpp 1168 2005-12-12 14:48:03Z modesto $
