// $Id: SelectionHandle.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: SelectionHandle.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 05 April 2005
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

#ifndef _SELECTIONHANDLE_HH
#define _SELECTIONHANDLE_HH

#include <qcanvas.h>
#include <qptrvector.h>

namespace lt {
    class Template;
}

class SelectionHandle;

// helper dict
typedef QPtrVector<SelectionHandle> SelectionHandleVector;

/**
 * SelectionHandle represents one red/green box that lays around selected
 * template.
 */
class SelectionHandle : public QCanvasRectangle
{
    public:
        /**
         * SelectionHandle rtti
         */
        enum { RTTI = 1112 };

        /**
         * ctor
         */
        SelectionHandle(int pos, lt::Template* t, QCanvas* c);

        /**
         * dtor
         */
        virtual ~SelectionHandle();

        /**
         * rtti getter
         */
        virtual int rtti() const;

        /**
         * Set active state
         */
        void setActive(bool b);

        /**
         * Set highlighted state
         */
        void setHighlighted(bool h);

        /**
         * Update position acording the Templates rectangle
         */
        void updatePosition();

        /**
         * Return assigned handle position
         */
        int pos() const;

        /**
         * Return assigned Template
         */
        lt::Template* templ() const;

    private:
        struct SelectionHandlePrivate;
        SelectionHandlePrivate* d;
};

#endif /* _SELECTIONHANDLE_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: SelectionHandle.hh 1169 2005-12-12 15:22:15Z modesto $
