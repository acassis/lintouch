// $Id: SVGImage.cpp,v 1.3 2004/12/07 09:52:37 modesto Exp $
//
//   FILE: SVGImage.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 24 February 2004
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

#include <qpicture.h>
#include <qwmatrix.h>

#include "lt/templates/Localizator.hh"

#include "SVGImage.hh"
#include "SVGImage.xpm"
#include "Image_broken.xpm"

#include "config.h"

#define QDEBUG_RECT( r ) qDebug(QString(#r": %1;%2 %3;%4") \
        .arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()))

struct SVGImage::SVGImagePrivate
{
    LocalizatorPtr localizator;

    QPicture pic;
    QString link;

    QImage broken_image;
    QPixmap broken_px;
};


SVGImage::SVGImage( LoggerPtr logger /*= Logger::nullPtr()*/ ) :
    Template( "SVGImage", logger ), d( new SVGImagePrivate ) {

    Q_CHECK_PTR( d );

    Info i;
    i.setAuthor ( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion ( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( QObject::tr( "SVG Image", "ltl-display" ) );
    i.setLongDescription ( QObject::tr( "SVG Image", "ltl-display" ) );
    i.setIcon( QImage( (const char**)SVGImage_xpm ) );

    registerInfo ( i );

    d->broken_image = QImage((const char**)Image_broken_xpm);

    const QString inactive = QObject::tr( "Appearance", "ltl-display" );

    registerProperty( inactive, "image_id",
            new Property( QObject::tr("SVG Image", "ltl-display"), "resource",
                "", logger ) );
    registerProperty( inactive, "link_to_panel",
            new Property( QObject::tr("Link", "ltl-display"), "link",
                "", logger) );
}

SVGImage::~SVGImage()
{
    delete d;
}

void SVGImage::localize( LocalizatorPtr localizator )
{
    Q_CHECK_PTR( d );
    d->localizator = localizator;

    propertiesChanged();
}

void SVGImage::drawShape( QPainter& p )
{

    Q_ASSERT( d );

    p.save();

    if( d->pic.isNull() ) {
        p.drawPixmap(rect(), d->broken_px);
        p.setPen(QPen(Qt::black, 1, Qt::SolidLine));
        p.setBrush(QBrush(Qt::NoBrush));
        p.drawRect(rect());
    } else {
        p.translate( rect().x(), rect().y() );
        const QRect picr = d->pic.boundingRect();
        p.scale( rect().width() / (float)picr.width(),
                rect().height() / (float)picr.height() );

        d->pic.play( &p );
    }

    p.restore();
}

void SVGImage::setRect( const QRect & rect )
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

    }

    update();
}

void SVGImage::mouseReleaseEvent( QMouseEvent * e )
{
    Q_ASSERT(d);
    Q_ASSERT(e);

    // test if the release is inside the region, otherwise the user releases
    // mouse button semewhere else and we should not handle this click
    if( collisionRegion().contains( e->pos() ) ) {
        if( !d->link.isEmpty() ) {
            viewAdaptor()->displayPanel( d->link );
        }
    }
}

void SVGImage::propertiesChanged()
{
    const PropertyDict &pm = properties();

    QIODevice *io = d->localizator->resourceAsIODevice(
                pm["image_id"]->asResource() );

    if( io != NULL && io->isOpen() ) {
        d->pic.load( io, "svg" );
    } else {
        d->pic = QPicture();
    }

    d->link = pm[ "link_to_panel" ]->asString();

    update();
}

Template * SVGImage::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Template * result = new SVGImage( logger );
    setupAfterClone( result, loc, tm );
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: SVGImage.cpp,v 1.3 2004/12/07 09:52:37 modesto Exp $
