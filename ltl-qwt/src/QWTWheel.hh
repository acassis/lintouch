// $Id: QWTWheel.hh 844 2005-06-21 12:40:50Z mman $
//
//   FILE: QWTWheel.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 27 April 2005
//
// Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
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

#ifndef _QWTWHEEL_HH
#define _QWTWHEEL_HH

#include <qwt/qwt_wheel.h>

#include "lt/templates/Template.hh"

namespace lt {

    /**
     * QWTWheel
     *
     * \author Patrik Modesto <modesto@swac.cz>
     */
    class QWTWheel : public QwtWheel, public Template
    {

        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            QWTWheel( LoggerPtr logger = Logger::nullPtr() );

            /**
             * Destructor.
             */
            virtual ~QWTWheel();

        protected:

            virtual void drawShape( QPainter & );

        public:

            virtual void mousePressEvent( QMouseEvent * e );
            virtual void mouseMoveEvent( QMouseEvent * e );
            virtual void mouseReleaseEvent( QMouseEvent * e );

            virtual void keyPressEvent( QKeyEvent * e );
            virtual void keyReleaseEvent( QKeyEvent * e );

            virtual QWidget::FocusPolicy focusPolicy() const;

            virtual void focusInEvent ( QFocusEvent* e );
            virtual void focusOutEvent ( QFocusEvent* e );

            virtual void localize( LocalizatorPtr localizator );
            virtual void setRect( const QRect & rect );
            virtual void propertiesChanged();

            virtual QPointArray areaPoints() const;

            virtual Template * clone(
                    const LocalizatorPtr & loc,
                    const TemplateManager & tm,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        private:
            struct QWTWheelPrivate;
            QWTWheelPrivate* d;

        public:
            static const QString TYPE;
            static Info makeInfo();
    };

} // namespace lt

#endif /* _QWTWHEEL_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: QWTWheel.hh 844 2005-06-21 12:40:50Z mman $
