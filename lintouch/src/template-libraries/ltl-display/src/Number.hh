// $Id: Number.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: _T_NAME_.hh -- 
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
//   DATE: 11 Noverber 2004
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

#ifndef __NUMBER__HH
#define __NUMBER__HH

#include "lt/templates/Template.hh"
using namespace lt;

class Number : public Template
{

    public:

        /**
         * Constructor.
         */
        Number( LoggerPtr logger = Logger::nullPtr() );

        /**
         * Destructor.
         */
        virtual ~Number();

    protected:

        virtual void drawShape( QPainter & );

    public:

        virtual void mousePressed( const QPoint & );
        virtual void mouseReleased( const QPoint & );
        virtual void mouseMoved( const QPoint & );
        virtual void mouseDoubleClicked( const QPoint & );

        virtual void setRect( const QRect & rect );
        virtual void propertiesChanged();
        virtual void iopinsChanged();

        virtual void setVisible( bool visible );
        virtual void setCanvas( QCanvas * c );

        virtual void localize( LocalizatorPtr loc );

        virtual Template * clone(
            const LocalizatorPtr & loc,
            const TemplateManager & tm,
            LoggerPtr logger = Logger::nullPtr() ) const;

    protected:
        void reconstructPointArrayAndRegion();
        void recalculateFontSizes();
        Qt::AlignmentFlags stringToAlignment( const QString& ha,
                const QString& va );
        int stringToBase( const QString& bas );

    private:
        typedef struct NumberPrivate;
        NumberPrivate* d; 
};

#endif /* __NUMBER__HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Number.hh 1168 2005-12-12 14:48:03Z modesto $
