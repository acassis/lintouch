// $Id: Lamp.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Lamp.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 03 October 2003
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


#ifndef _LAMP_HH
#define _LAMP_HH

#include <qpointarray.h>
#include <qregion.h>

#include "lt/templates/Template.hh"
using namespace lt;

namespace lt {

    /**
     * Lamp.
     *
     * A Simple Lamp Template which paints itself either in off or on
     * state according to the value of its only one IOPin.
     *
     * When Lamp is clicked it can open another panel in a View in which
     * it is contained.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     * \author Patrik Modesto <modesto@swac.cz>
     */
    class Lamp : public Template
    {

        public:

            /**
             * Constructor.
             */
            Lamp( const QString& skin, LoggerPtr logger = Logger::nullPtr() );

            /**
             * Destructor.
             */
            virtual ~Lamp();

        protected:

            virtual void drawShape( QPainter & );

        public:

            virtual void mouseReleaseEvent( QMouseEvent * e );

            virtual void setRect( const QRect & rect );
            virtual void propertiesChanged();
            virtual void iopinsChanged();

            virtual Template * clone(
                    const LocalizatorPtr & loc,
                    const TemplateManager & tm,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        protected:

            void reconstructPointArrayAndRegion();

        private:
            struct LampPrivate;
            LampPrivate* d;
    };

} // namespace lt

#endif /* _LAMP_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Lamp.hh 1168 2005-12-12 14:48:03Z modesto $
