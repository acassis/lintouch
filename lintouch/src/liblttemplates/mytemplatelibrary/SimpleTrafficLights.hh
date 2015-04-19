// $Id: SimpleTrafficLights.hh,v 1.4 2004/08/30 13:25:47 mman Exp $
//
//   FILE: SimpleTrafficLights.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 12 January 2004
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

#ifndef _SIMPLETRAFFICLIGHTS_HH
#define _SIMPLETRAFFICLIGHTS_HH

#include "lt/templates/Template.hh"

namespace lt {

    /**
     * SimpleTrafficLights.
     *
     * Very Simple Traffic Lights containing only "red" and "green"
     * light and demonstrating the use of nested templates.
     *
     * "red" and "green" lights are actually nested "Lamp" templates.
     *
     * Traffic Lights are either off(red) or on(green) according to
     * the value of the only input IOPin.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class SimpleTrafficLights : public Template
    {

        public:

            /**
             * Constructor.
             */
            SimpleTrafficLights( LoggerPtr logger = Logger::nullPtr() );

            /**
             * Destructor.
             */
            virtual ~SimpleTrafficLights();

            virtual Template * clone(
                    const LocalizatorPtr & loc,
                    const TemplateManager & tm,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        protected:

            virtual void drawShape( QPainter & );

        public:

            virtual void compose( const TemplateManager & tm );
            virtual void setRect( const QRect & rect );
            virtual void setVisible( bool visible );
            virtual void propertiesChanged();
            virtual void iopinsChanged();

        protected:

            Template * m_red, * m_green;
    };

} // namespace lt

#endif /* _SIMPLETRAFFICLIGHTS_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: SimpleTrafficLights.hh,v 1.4 2004/08/30 13:25:47 mman Exp $
