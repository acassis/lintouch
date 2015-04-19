// $Id: ViewAdaptor.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: ViewAdaptor.hh -- 
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

#ifndef _VIEWADAPTOR_HH
#define _VIEWADAPTOR_HH

#include <qstring.h>

namespace lt {

    /**
     * ViewAdaptor.
     *
     * ViewAdaptor serves as a glue between Templates and their parent
     * container. For Templates the parent container is Panel. Panel is
     * in turn part of a View.
     *
     * Templates can control the View they're contained in
     * through this interface.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class ViewAdaptor
    {

        public:

            /**
             * Constructor.
             */
            ViewAdaptor() {};

            /**
             * Destructor.
             */
            virtual ~ViewAdaptor() {};

        private:

            ViewAdaptor( const ViewAdaptor & i );
            ViewAdaptor & operator =( const ViewAdaptor & i );

        public:

            virtual void displayPanel( const QString & /* id */ ) {
                qWarning( "ViewAdaptor::displayPanel: "
                        "using default ViewAdaptor" );
            };

    };

} // namespace lt

#endif /* _VIEWADAPTOR_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ViewAdaptor.hh 1168 2005-12-12 14:48:03Z modesto $
