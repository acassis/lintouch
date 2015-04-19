// $Id: Tester.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Tester.hh -- 
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
//   DATE: 20 August 2004
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

#ifndef _LT_TEST_HH
#define _LT_TEST_HH

#include "lt/templates/Template.hh"

namespace lt {

    class Tester : public Template
    {

    public:

        /**
         * Constructor.
         */
        Tester( LoggerPtr logger = Logger::nullPtr() );

        /**
         * Destructor.
         */
        virtual ~Tester();

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

        virtual void localize( LocalizatorPtr loc );

        virtual Template * clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
                LoggerPtr logger = Logger::nullPtr() ) const;

    protected:

        void recalculateFontSizes();
        void reconstructPointArrayAndRegion();

        typedef struct TesterPrivate;
        TesterPrivate * d;

    };
}

#endif /* _LT_TEST_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Tester.hh 1168 2005-12-12 14:48:03Z modesto $
