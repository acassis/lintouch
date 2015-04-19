// $Id: Lift.hh 950 2005-07-14 11:26:30Z modesto $
//
//   FILE: Lift.hh --
// AUTHOR: Jiri Barton <jbar@swac.cz>
//   DATE: 12 September 2005
//
// Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
//
// This application is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License, or(at
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

#ifndef _LT_LIFT_HH
#define _LT_LIFT_HH

#include "lt/templates/Template.hh"

namespace lt {

    class Lift : public Template
    {

    public:

        /**
         * Constructor.
         */
        Lift( LoggerPtr logger = Logger::nullPtr() );

        /**
         * Destructor.
         */
        virtual ~Lift();

    protected:

        virtual void drawShape( QPainter & );

    public:

        virtual void setRect( const QRect & rect );
        virtual void propertiesChanged();
        virtual void iopinsChanged();

        virtual void localize( LocalizatorPtr loc );

        virtual Template * clone(
                const LocalizatorPtr & loc,
                const TemplateManager & tm,
                LoggerPtr logger = Logger::nullPtr() ) const;

    protected:

        void reconstructPointArrayAndRegion();

        typedef struct LiftPrivate;
        LiftPrivate * d;

    };
}

#endif /* _LT_TEST_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Lift.hh 950 2005-07-14 11:26:30Z modesto $
