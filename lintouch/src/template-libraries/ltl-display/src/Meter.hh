// $Id: Meter.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Meter.hh -- 
// AUTHOR: Abdul <henry@swac.cz>
//   DATE: 02 November 2004
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

#ifndef __METER__HH
#define __METER__HH

#include "lt/templates/Template.hh"
using namespace lt;

/**
 * TODO
 */
class Meter : public Template
{

    public:

        /**
         * Constructor.
         */
        Meter( LoggerPtr logger = Logger::nullPtr() );

        /**
         * Destructor.
         */
        virtual ~Meter();

    protected:

        virtual void drawShape( QPainter & );
        void fillMeter( QPainter & );
        void drawTicks( QPainter & p );

    public:
        virtual void setRect( const QRect & rect );
        virtual void propertiesChanged();
        virtual void iopinsChanged();
        virtual QPointArray areaPoints() const;
        virtual QRegion collisionRegion() const;

        virtual void setVisible( bool visible );
        virtual void setCanvas( QCanvas * c );
        virtual void advance( int phase );
        virtual void localize( LocalizatorPtr loc );
        //virtual QPointArray areaPoints() const;
        //virtual QRegion collisionRegion() const;

        virtual Template * clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
                LoggerPtr logger = Logger::nullPtr() ) const;

    protected:
        //void reconstructPointArrayAndRegion();
        //void recalculateFontSizes();
    private:
        typedef struct LTMeterPrivate;
        LTMeterPrivate* d;

};

#endif /*__METER__HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Meter.hh 1168 2005-12-12 14:48:03Z modesto $
