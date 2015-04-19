// $Id: Buttona.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Buttona.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 27 January 2004
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

#ifndef _BUTTONA_HH
#define _BUTTONA_HH

#include "lt/templates/Template.hh"

namespace lt {

    /**
     * Buttona.
     *
     * Simple Buttona Template which paints itself either in off
     *(normal) or on(when pressed). additionally confirmation box
     * shows whether the value was sent successfully to the server.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class Buttona : public Template
    {

        public:

            /**
             * Constructor.
             */
            Buttona( LoggerPtr logger = Logger::nullPtr() );

            /**
             * Destructor.
             */
            virtual ~Buttona();

        protected:

            virtual void drawShape( QPainter & );

        public:

            virtual QWidget::FocusPolicy focusPolicy() const;
            virtual void keyPressEvent( QKeyEvent * e );
            virtual void keyReleaseEvent( QKeyEvent * e );

            virtual void mousePressEvent( QMouseEvent * e );
            virtual void mouseReleaseEvent( QMouseEvent * e );

            virtual void setRect( const QRect & rect );
            virtual void propertiesChanged();
            virtual void iopinsChanged();

            virtual QPointArray areaPoints() const;
            virtual QRegion collisionRegion() const;

            virtual void localize( LocalizatorPtr loc );

            virtual Template * clone(
                    const LocalizatorPtr & loc,
                    const TemplateManager & tm,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        protected:

            void reconstructPointArrayAndRegion();
            void recalculateFontSizes();

            QRect m_points;
            QRect m_cpoints;
            QRegion m_collisionRegion;

            QBrush m_brush;
            QPen m_pen;
            QBrush m_cbrush;
            QPen m_cpen;

            bool pressed;

            QFont m_font;
            QFont m_original_font;
            QPen m_font_pen;
            QRect m_text_rect;
            QString m_text;

            int m_textf;

            LocalizatorPtr m_localizator;

            int m_min_roundtrip;
            int m_max_roundtrip;
            double m_avg_roundtrip;
            int m_nr_pressed;

            QTime m_time_pressed;
            QTime m_time_confirmed;
    };

} // namespace lt

#endif /* _BUTTONA_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Buttona.hh 1168 2005-12-12 14:48:03Z modesto $
