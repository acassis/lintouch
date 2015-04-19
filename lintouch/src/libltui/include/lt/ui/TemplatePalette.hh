// $Id: TemplatePalette.hh,v 1.3 2004/08/15 18:17:59 mman Exp $
//
//   FILE: TemplatePalette.hh -- 
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 29 October 2003
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

#ifndef _TEMPLATEPALETTE_HH
#define _TEMPLATEPALETTE_HH

#include <qtoolbox.h>

#include <lt/templates/TemplateLibrary.hh>
#include <lt/templates/TemplateManager.hh>

class QListBoxItem;

namespace lt {

    /**
     * Template selector widget ala Toolbox in the Qt Designer.
     *
     * @author Jiri Barton <barton@swac.cz>
     */
    class TemplatePalette: 
        public QToolBox
    {

        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            TemplatePalette( QWidget * parent = NULL,
                    const char * name = NULL,
                    WFlags f = 0 );

            /**
             * Add all the templates in the library to the new tab in
             * the palette.
             */
            bool addTemplateLibrary( TemplateLibrary * lib );

signals:

            /**
             * Emitted when a user selects a template in a toolbox(by
             * double-clicking on it)
             */
            void placeTemplate( lt::Template * t );

            public slots:

        private:

                /**
                 * Disable unintentional copying.
                 */
                TemplatePalette( const TemplatePalette & );
                TemplatePalette & operator =( const TemplatePalette & );

                private slots:

                    /**
                     * connected to QListBox::selected( QListBoxItem * )
                     */
                    void lbItemSelected( QListBoxItem * item );

        private:

                struct TemplatePalettePrivate;
                TemplatePalettePrivate * d;
    };
}

#endif /* _TEMPLATEPALETTE_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplatePalette.hh,v 1.3 2004/08/15 18:17:59 mman Exp $
