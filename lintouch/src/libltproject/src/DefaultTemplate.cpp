// $Id: DefaultTemplate.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: DefaultTemplate.cpp --
// AUTHOR: Jiri Barton <jbar@swac.cz>
//   DATE: 17 September 2004
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

#include "DefaultTemplate.hh"
using namespace lt;

#include "config.h"

DefaultTemplate::DefaultTemplate(const QString &type,
    LoggerPtr logger /*= Logger::nullPtr()*/)
    : Template(type, logger)
{
    //register info
    Info i;
    i.setAuthor ( "Lintouch Team <lintouch@lintouch.org>" );
    i.setVersion ( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription ( "The default template" );
    i.setLongDescription ( "The default template is used in place of "
        "an unexisting template. It may register its properties on the fly"
        );

    registerInfo ( i );
}

void DefaultTemplate::addProperty(const QString &name, Property *prop)
{
    registerProperty("All Properties", name, prop);
}

void DefaultTemplate::drawShape( QPainter & p )
{
    QRect r = rect();

    p.save();

    p.setPen( QPen( "black" ) );
    p.setBrush( QBrush( QColor( "white" ) ) );
    p.setFont( makeFont( QFont( "Sans", 10 ) ) );

    p.drawRect( r );

    p.drawText( r, Qt::BreakAnywhere, library() + "." + type() );

    p.restore();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: DefaultTemplate.cpp 1168 2005-12-12 14:48:03Z modesto $
