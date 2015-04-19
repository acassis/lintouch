// $Id: LineInput.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: LineInput.hh --
// AUTHOR: Shivaji Basu <shivaji@swac.cz>
//   DATE: 06 October 2004
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

#ifndef _LINEINPUT__HH
#define _LINEINPUT__HH

#include "lt/templates/Template.hh"
using namespace lt;

namespace lt{

    class LineInput : public Template
    {

        public:

            /**
             * Constructor.
             */
            LineInput( LoggerPtr logger = Logger::nullPtr() );

            /**
             * Destructor.
             */
            virtual ~LineInput();

        protected:

            virtual void drawShape( QPainter & );

        public:

            virtual QWidget::FocusPolicy focusPolicy() const;

            virtual void keyPressEvent( QKeyEvent * );

            virtual void setRect( const QRect & rect );
            virtual void propertiesChanged();
            virtual void iopinsChanged();

            virtual Template * clone(
                    const LocalizatorPtr & loc,
                    const TemplateManager & tm,
                    LoggerPtr logger = Logger::nullPtr() ) const;


            virtual void focusInEvent ( QFocusEvent * );
            virtual void focusOutEvent ( QFocusEvent * );
        protected:

            void reconstructPointArrayAndRegion();
            void recalculateFontSizes();

        private:

            typedef struct LineInputPrivate;
            LineInputPrivate* d;
    };
};

#endif /* _LINEINPUT__HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: LineInput.hh 1168 2005-12-12 14:48:03Z modesto $

