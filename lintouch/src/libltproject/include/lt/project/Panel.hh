// $Id: Panel.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Panel.hh --
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

#ifndef _PANEL_HH
#define _PANEL_HH

#include <qmap.h>
#include <qstring.h>

#include <lt/SharedPtr.hh>

#include <lt/LTMap.hh>

#include <lt/templates/Template.hh>

namespace lt {

    class Panel;

    typedef LTMap <Panel> PanelMap;

    /**
     * Panel.
     *
     * Panel holds one or more Templates. You can put a Template on a
     * panel by calling addTemplate and remove it by calling
     * removeTemplate.
     *
     * All templates that were added to a Panel are automatically
     * deleted when the panel is deleted.
     *
     * When used as a value based class the contents is implicitly
     * shared among all instances created by assigning or via copy
     * constructor:
     *
     * You can create independent instance of the panel with the same
     * contents by calling the clone() method.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class Panel
    {

        public:

            /**
             * Constructor.
             */
            Panel();

            Panel( const Panel & p );

            Panel & operator =( const Panel & p );

            /**
             * Destructor.
             */
            virtual ~Panel();

            /**
             * Create new panel with the same templates.
             */
            Panel clone( const LocalizatorPtr loc,
                    const TemplateManager & tm,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        public:

            /**
             * Put the item to this panel. It does not overwrite the key
             * with the new value if the key exists, and returns false.
             *
             * Takes ownership of the template. The template is deleted when
             * the panel is deleted.
             */
            bool addTemplate( const QString & key,
                    Template * it );

            /**
             * Remove the item from this panel. Returns false if the
             * key does not exist.
             *
             * Does not delete the template.
             */
            bool removeTemplate( const QString & key );

            /**
             * Return Templates contained on this panel.
             */
            const TemplateDict & templates() const;

            /**
             * Return Templates contained on this panel.
             */
            TemplateDict & templates();

        private:

            typedef struct PanelPrivate;
            typedef SharedPtr<PanelPrivate>
                PanelPrivatePtr;

            PanelPrivatePtr d;

    };
} // namespace lt

#endif /* _PANEL_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Panel.hh 1168 2005-12-12 14:48:03Z modesto $
