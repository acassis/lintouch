// $Id: TextMap.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: TextMap.cpp --
// AUTHOR: Jiri Barton <jbar@lintouch.org>
//   DATE: 04 June 2004
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

#include "TextMap.xpm"
#include "TextMap.hh"

#include "config.h"

static int _StringToFlags( const QString &ha, const QString &va )
{
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

TextMap::TextMap( LoggerPtr logger ) :
Template( "TextMap", logger ), m_localizator( Localizator::nullPtr() )
{
    //register info
    Info i;
    i.setAuthor( "Jiri Barton <jbar@lintouch.org>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription(
        QObject::tr("TextMap", "ltl-display"));
    i.setLongDescription(
        QObject::tr("Look up a number in the string table and"
        " display the corresponding a string/message", "ltl-display"));
    i.setIcon( QImage( (const char **) TextMap_xpm ) );

    registerInfo( i );

    QString group;

    // visual properties
    group = QObject::tr("Appearance", "ltl-display");

    registerProperty( group, "border",
            new Property( QObject::tr( "Border", "ltl-display" ), "pen",
                    "#000000;1;NOPEN", logger ) );

    registerProperty( group, "font",
            new Property( QObject::tr( "Font", "ltl-display"),"font",
                    "", logger ) );

    registerProperty( group, "font-color",
            new Property( QObject::tr( "Font Color", "ltl-display" ),
                "color", "#000000",
                logger ) );

    registerProperty( group, "rotation",
            new Property( QObject::tr( "Rotation", "ltl-display"),"integer",
                    "0", logger ) );

    registerProperty( group, "halign",
            new Property( QObject::tr( "Horizontal Alignment", "ltl-display"),
                    "choice(left,right,hcenter,justify)", "left", logger ));

    registerProperty( group, "valign",
            new Property( QObject::tr( "Vertical Alignment", "ltl-display"),
                    "choice(top,vcenter,bottom)", "vcenter", logger ));

    group = QObject::tr("Data Source", "ltl-display");

    // data properties
    registerProperty( group, "strings",
            new Property( QObject::tr( "Strings", "ltl-display"),
                    "resource", "", logger ));

    // create iopins of this template
    registerIOPin( QObject::tr("Inputs", "ltl-display"), "input",
            new IOPin( this, QObject::tr("Input", "ltl-display"), "number" ));
}

TextMap::~TextMap()
{
}

void TextMap::drawShape( QPainter & p )
{
    int ww = rect().width() / 2;
    int hh = rect().height() / 2;
    QRect br;

    p.save();
    p.setPen ( m_pen );

    p.setClipRect( rect(), QPainter::CoordPainter );

    p.drawRect(rect());

    p.setPen(m_color);
    p.setFont ( m_font );

#ifdef Q_WS_QWS
    p.drawText( rect(), m_flags, m_text, -1, &br );
#else
    p.translate( rect().x() + ww, rect().y() + hh );
    p.rotate( m_rotation );
    p.translate( -ww, -hh );
    p.drawText( 0, 0, rect().width(), rect().height(), m_flags, m_text, -1,
            &br );

    br.moveBy( rect().x(), rect().y() );
#endif

    p.restore();
}

void TextMap::setRect( const QRect & rect )
{
    Template::setRect( rect );

    // shedule repaint
    if( canvas() ) update();
}

void TextMap::propertiesChanged()
{
    const PropertyDict& pm = (const PropertyDict&) properties();

    m_pen = pm["border"]->asPen();
    m_rotation = pm["rotation"]->asInteger();
    m_font = pm["font"]->asFont();
    m_color = pm["font-color"]->asColor();
    m_flags = _StringToFlags( pm["halign"]->asString(),
            pm["valign"]->asString() );

    localize(m_localizator);

    // shedule repaint
    if( canvas() ) update();
}

void TextMap::iopinsChanged()
{
    int index = ((const IOPinDict&)iopins())[ "input" ]->realNumberValue();
    m_text = m_map.contains(index) ? m_map[index] :
        (m_any.contains('%') ? m_any.arg(index) : m_any);

    // shedule repaint
    if( canvas() ) update();
}

void TextMap::localize( LocalizatorPtr loc )
{
    if( ! loc.isNull() ) {
        m_localizator = loc;
        QIODevice *dev = m_localizator->resourceAsIODevice(properties()
            ["strings"]->asString());
        if (dev && dev->isOpen())
        {
            m_map.clear();
            #define BUFLEN 4096
            char buf[BUFLEN];
            while (!dev->atEnd())
            {
              dev->readLine(buf, BUFLEN);
              //...and the C++ string parsing hell follows
              QString line = QString(buf).stripWhiteSpace();
              int found = line.find('=');
              if (found == -1) continue;
              QString key = line.left(found).stripWhiteSpace();
              QString val = line.mid(found+1).stripWhiteSpace();
              if (key == "*")
                  m_any = val;
              else
                  m_map.insert(key.toInt(), val);
            }
            delete dev;
        }
    }
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TextMap.cpp 1168 2005-12-12 14:48:03Z modesto $
