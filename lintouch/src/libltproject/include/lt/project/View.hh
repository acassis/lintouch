// $Id: View.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: View.hh --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 06 November 2003
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

#ifndef _VIEW_HH
#define _VIEW_HH

#include <qmap.h>
#include <qstring.h>

#include <lt/SharedPtr.hh>

#include <lt/LTMap.hh>

#include "lt/project/Panel.hh"

namespace lt {

    class View;

    typedef LTMap <View> ViewMap;

    /**
     * View.
     *
     * View contains one or more Panels.
     *
     * When used as a value based class the contents is implicitly
     * shared among all instances created by assigning or via copy
     * constructor:
     *
     * You can create independent instance of the view with the same
     * contents by calling the clone() method.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class View
    {

        public:

            /**
             * Constructor.
             */
            View ();

            View ( const View & p );

            View & operator = ( const View & p );

            /**
             * Destructor.
             */
            virtual ~View ();

            /**
             * Create new view with the same panels.
             */
            View clone( const LocalizatorPtr & loc,
                    const TemplateManager & tm,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        public:

            /**
             * set the virtual width of the panels in this view.
             */
            void setVirtualWidth ( unsigned w );

            /**
             * set the virtual height of the panels in this view.
             */
            void setVirtualHeight ( unsigned h );

            /**
             * return the virtual width of panels in this view.
             */
            unsigned virtualWidth () const;

            /**
             * return the virtual height of panels in this view.
             */
            unsigned virtualHeight () const;

        public:

            /**
             * Add new panel to this view. returns false if the key is
             * already taken.
             */
            bool addPanel ( const QString & key, const Panel & panel );

            /**
             * Remove panel from this view. returns false if the key is
             * not used within the view.
             */
            bool removePanel ( const QString & key );

            /**
             * Return all panels registered within this view.
             */
            const PanelMap & panels () const;

            /**
             * Return all panels registered within this view.
             */
            PanelMap & panels ();

            /**
             * Set Home Panel Id.
             */
            void setHomePanelId ( const QString & pid );

            /**
             * Returns Home Panel Id. When Id specified by calling
             * setHomePanelId can not be found the first panel id will
             * be returned.
             */
            QString homePanelId () const;

        private:

            typedef struct ViewPrivate;
            typedef SharedPtr<ViewPrivate>
                ViewPrivatePtr;

            ViewPrivatePtr d;

    };
} // namespace lt

#endif /* _VIEW_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: View.hh 1168 2005-12-12 14:48:03Z modesto $
