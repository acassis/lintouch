// $Id: SelectionHandle.cpp 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: SelectionHandle.cpp -- 
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

#include "SelectionHandle.hh"

#include <lt/templates/Template.hh>
using namespace lt;

#define SIZE    (8)
#define ACTIVE_COLOR    "#FF0000"
#define INACTIVE_COLOR  "#00FF00"

#define HIGHLIGHTED_COLOR  "#FFFF00"

struct SelectionHandle::SelectionHandlePrivate
{
    SelectionHandlePrivate() : pos(-1), t(NULL) {}

    /**
     * Position of this handle
     */
    int pos;

    /**
     * Template we belong to
     */
    Template* t;

    /**
     * Active flag
     */
    bool a;

    /**
     * Highlighted flag
     */
    bool h;
};

SelectionHandle::SelectionHandle(int pos, Template* t, QCanvas* c) :
    QCanvasRectangle(0, 0, SIZE, SIZE, c), d(new SelectionHandlePrivate)
{
    Q_CHECK_PTR(d);

    Q_ASSERT(t);
    Q_ASSERT(pos >= 0);
    Q_ASSERT(pos < 8);

    setPen(QPen(black));

    d->t = t;
    d->pos = pos;

    updatePosition();

    setActive(false);
    show();
}

SelectionHandle::~SelectionHandle()
{
    delete d;
}

int SelectionHandle::rtti() const
{
    return SelectionHandle::RTTI;
}

void SelectionHandle::setActive(bool b)
{
    Q_ASSERT(d);
    d->a = b;

    updatePosition();
}

void SelectionHandle::setHighlighted(bool h)
{
    Q_ASSERT(d);
    d->h = h;

    if(h) {
        setBrush(QBrush(HIGHLIGHTED_COLOR));
        update();
    } else {
        setActive(d->a);
    }
}

void SelectionHandle::updatePosition()
{
    Q_ASSERT(d);
    Q_ASSERT(d->t);
    Q_ASSERT(d->pos >= 0);
    Q_ASSERT(d->pos < 8);

    update();

    QRect tr = d->t->rect();

    int shiftx = (tr.width() / 2) - SIZE/2;
    int shifty = (tr.height() / 2) - SIZE/2;

    switch(d->pos) {
        case 0: // topleft
            move(tr.left() - SIZE + 1, tr.top() - SIZE + 1);
            break;
        case 2: // topright
            move(tr.right(), tr.top() - SIZE + 1);
            break;
        case 4: // bottomright
            move(tr.right(), tr.bottom());
            break;
        case 6: // bottomleft
            move(tr.left() - SIZE + 1, tr.bottom());
            break;
        case 1: // top
            move(tr.left() + shiftx , tr.top() - SIZE + 1);
            break;
        case 3: // right
            move(tr.right(), tr.top() + shifty);
            break;
        case 5: // bottom
            move(tr.left() + shiftx, tr.bottom());
            break;
        case 7: // left
            move(tr.left() - SIZE + 1, tr.top() + shifty);
            break;
    }

    switch(d->pos) {
        case 0: // topleft
        case 2: // topright
        case 4: // bottomright
        case 6: // bottomleft
            setZ(100001);
            break;
        case 1: // top
        case 3: // right
        case 5: // bottom
        case 7: // left
            setZ(100000);
            break;
    }

    if(d->a) {
        setBrush(QBrush(ACTIVE_COLOR));
        setZ(z() + 1);
    } else {
        setBrush(QBrush(INACTIVE_COLOR));
        setZ(z() - 1);
    }

    update();
}

int SelectionHandle::pos() const
{
    Q_ASSERT(d);
    return d->pos;
}

Template* SelectionHandle::templ() const
{
    Q_ASSERT(d);
    return d->t;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: SelectionHandle.cpp 1169 2005-12-12 15:22:15Z modesto $
