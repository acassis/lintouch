// $Id: TextMap.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: TextMap.hh --
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

#ifndef _TextMap_HH
#define _TextMap_HH

#include "lt/templates/Template.hh"
using namespace lt;

class TextMap : public Template
{

    public:

        /**
         * Constructor.
         */
        TextMap( LoggerPtr logger = Logger::nullPtr() );

        /**
         * Destructor.
         */
        virtual ~TextMap();

    protected:

        virtual void drawShape( QPainter & );

    public:

        virtual void setRect( const QRect & rect );
        virtual void propertiesChanged();
        virtual void iopinsChanged();

        virtual void localize( LocalizatorPtr loc );

    protected:

        LocalizatorPtr m_localizator;
        QPen m_pen;
        QColor m_color;
        int m_rotation;
        QFont m_font;
        int m_flags;

        typedef QMap<int, QString> QMessageMap;
        QMessageMap m_map;
        QString m_any;
        QString m_text;

};

#endif /* _TextMap_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TextMap.hh 1168 2005-12-12 14:48:03Z modesto $
