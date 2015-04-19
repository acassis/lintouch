// $Id: Image.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Image.cpp --
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 06 November 2003
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

#include "lt/templates/Localizable.hh"
#include "lt/templates/Localizator.hh"

#include "Image.hh"
#include "Image.xpm"
#include "Image_broken.xpm"

#define min(a,b) (((a) < (b)) ? (a) : (b))

using namespace lt;

#include "config.h"

#   define QDEBUG_RECT( r ) qDebug(QString(#r": %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

typedef struct Image::ImagePrivate {

    ImagePrivate() : rotation( 0 ),
                     localizator( LocalizatorPtr( new Localizator ) )
    {}

    QImage rotateAndScaleImage(const QImage& src, int rotation, const QRect r) {

        if(src.isNull() || src.width() == 0 || src.height() == 0) {
            return src;
        }

        QImage px;
        QWMatrix m;

        if( rotation != 0 ) {
            m.rotate( rotation );
            px = src.xForm( m );
        } else {
            px = src;
        }

        double x_factor = r.width() / (double) px.width();
        double y_factor = r.height() / (double) px.height();

        if((x_factor != 1.0 ) || (y_factor != 1.0)) {
            m.reset();
            m.scale( x_factor, y_factor );
            px = px.xForm( m );
        }

        return px;
    }

    QImage broken_image;
    QPixmap broken_px;

    int rotation;
    QString link;
    QPixmap image;

    LocalizatorPtr localizator;

    struct properties_t {
        int rotation;
        QString link_to_panel;
        QImage image_orig;
        QPixmap image;
    } properties[2];
};

Image::Image( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    Template( "Image", logger ), d( new ImagePrivate ) {

    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Image", "ltl-display" ) );
    i.setLongDescription( QObject::tr( "Image with one input iopin",
                "ltl-display" ) );
    i.setIcon( QImage( (const char**)Image_xpm ) );

    registerInfo( i );

    d->broken_image = QImage((const char**)Image_broken_xpm);

    const QString inactive = QObject::tr( "Appearance/Inactive", "ltl-display" );
    const QString active = QObject::tr( "Active", "ltl-display" );
    Property* p = NULL;

    // image resource id
    registerProperty( inactive, "image_id_off",
            p = new Property( QObject::tr("Image", "ltl-display"), "resource",
                "", logger ) );
    registerProperty( active, "image_id_on",
            new Property( QObject::tr("Image", "ltl-display"), p, logger ) );

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

Image::~Image()
{
    delete d;
}

void Image::localize( LocalizatorPtr localizator ) {

    Q_ASSERT( d );
    d->localizator = localizator;

    propertiesChanged();
}

void Image::drawShape( QPainter & p )
{
    Q_ASSERT( d );

    if( d->image.isNull() ) {
        p.drawPixmap(rect(), d->broken_px);
        p.save();
        p.setPen(QPen(Qt::black, 1, Qt::SolidLine));
        p.setBrush(QBrush(Qt::NoBrush));
        p.drawRect(rect());
        p.restore();
        return;
    }

    p.drawPixmap(rect(), d->image);
}

void Image::setRect( const QRect & rect )
{
    update();

    QRect oldRect = this->rect();

    Template::setRect( rect );

    if(oldRect.size() != rect.size()) {

        // prepare broken_image to broken_px
        QImage i(rect.size(), 32);
        i.fill(QColor("white").rgb());

        QRect br = d->broken_image.rect();
        br.moveCenter(i.rect().center());

        if(br.x() < 0) {
            br.setX(0);
        }
        if(br.y() < 0) {
            br.setY(0);
        }

        bitBlt(&i, br.x(), br.y(),
                &d->broken_image, 0, 0, -1, -1, 0);
        bool b = d->broken_px.convertFromImage(i);
        Q_ASSERT(b);

        // prepare image_orig to current size and rotation
        d->properties[1].image =
            d->rotateAndScaleImage(d->properties[1].image_orig,
                    d->properties[1].rotation, rect);
        d->properties[0].image =
            d->rotateAndScaleImage(d->properties[0].image_orig,
                    d->properties[0].rotation, rect);

        // refresh "current" image according "current" value
        iopinsChanged();
    }

    // and shedule repaint
    update();
}

void Image::mouseReleaseEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    // test if the release is inside the region, othervise the user releases
    // mouse button semewhere else and we should not handle this click
    if( collisionRegion().contains( e->pos() ) ) {
        if( !d->link.isEmpty() ) {
            viewAdaptor()->displayPanel( d->link );
        }
    }
}

void Image::propertiesChanged() {

    const PropertyDict& pm = (const PropertyDict&) properties();

    d->properties[1].rotation = pm[ "rotation_on" ]->asInteger();
    d->properties[1].link_to_panel = pm[ "link_to_panel_on" ]->asString();
    d->properties[1].image_orig = QImage(d->localizator->resourceAsByteArray(
                    pm[ "image_id_on" ]->asResource() ) );

    d->properties[0].rotation = pm[ "rotation_off" ]->asInteger();
    d->properties[0].link_to_panel = pm[ "link_to_panel_off" ]->asString();
    d->properties[0].image_orig = QImage( d->localizator->resourceAsByteArray(
                    pm[ "image_id_off" ]->asResource() ) );

    // prepare image_orig to current size and rotation
    d->properties[1].image =
        d->rotateAndScaleImage(d->properties[1].image_orig,
                d->properties[1].rotation, rect());
    d->properties[0].image =
        d->rotateAndScaleImage(d->properties[0].image_orig,
                d->properties[0].rotation, rect());

    // fire off iopinsChanged to set up painting brush and pen correctly
    iopinsChanged();
}

void Image::iopinsChanged() {

    int index;

    if( ((const IOPinDict&)iopins())[ "input" ]->realBitValue() ) {
        index = 1;
    } else {
        index = 0;
    }

    d->rotation = d->properties[index].rotation;
    d->link = d->properties[index].link_to_panel;
    d->image = d->properties[index].image;

    // shedule repaint
    update();
}

Template * Image::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new Image( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Image.cpp 1168 2005-12-12 14:48:03Z modesto $
