// $Id: Button.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Button.hh -- 
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


#ifndef _BUTTON_HH
#define _BUTTON_HH

#include <qpixmap.h>
#include <qpalette.h>

#include "lt/templates/Template.hh"
using namespace lt;

namespace lt {

    /**
     * Button.
     *
     * \author Patrik Modesto <modesto@swac.cz>
     */
    class Button : public Template
    {
        public:

            /**
             * Constructor.
             */
            Button( LoggerPtr logger = Logger::nullPtr() );

            /**
             * Destructor.
             */
            virtual ~Button();

        protected:

            virtual void drawShape( QPainter & );

            void recalculateFontSizes();

        public:

            virtual void mousePressEvent( QMouseEvent * e );
            virtual void mouseReleaseEvent( QMouseEvent * e );

            virtual void localize( LocalizatorPtr localizator );
            virtual void setRect( const QRect & rect );
            virtual void propertiesChanged();
            virtual void iopinsChanged();

            virtual QPointArray areaPoints() const;
            virtual QRegion collisionRegion() const;

            virtual Template * clone(
                    const LocalizatorPtr & loc,
                    const TemplateManager & tm,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        private:

            struct ButtonPrivate;
            ButtonPrivate* d;

            void reconstructPointArrayAndRegion();

            virtual void drawRectButton( QPainter& p );
            virtual void drawConfBox( QPainter& p );
    };

} // namespace lt

#endif /* _BUTTON_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Button.hh 1168 2005-12-12 14:48:03Z modesto $
