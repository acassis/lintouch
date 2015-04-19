// $Id: View.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: View.cpp --
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

#include "lt/project/Panel.hh"
#include "lt/project/View.hh"
using namespace lt;

struct View::ViewPrivate
{

    unsigned width;
    unsigned height;

    PanelMap panels;

    QString firstPanelId;
    QString homePanelId;

};

View::View () : d ( new ViewPrivate () )
{
    d->width = d->height = (unsigned int) 0;
}

View::View ( const View & p ) : d ( p.d )
{
}

View & View::operator = ( const View & p )
{
    d = p.d;
    return *this;
}

View::~View ()
{
}

void View::setVirtualWidth ( unsigned w )
{
    d->width = w;
}

void View::setVirtualHeight ( unsigned h )
{
    d->height = h;
}

unsigned View::virtualWidth () const
{
    return d->width;
}

unsigned View::virtualHeight () const
{
    return d->height;
}

bool View::addPanel ( const QString & key, const Panel & panel )
{
    if ( d->panels.contains ( key ) ) {
        return false;
    }
    d->panels.insert ( key, panel );
    d->firstPanelId = d->panels.currentKey();
    return true;
}

bool View::removePanel ( const QString & key )
{
    if ( d->panels.contains ( key ) ) {
        d->panels.remove ( key );
        d->firstPanelId = d->panels.currentKey();
        return true;
    }
    return false;
}

const PanelMap & View::panels () const
{
    return d->panels;
}

PanelMap & View::panels ()
{
    return d->panels;
}

void View::setHomePanelId ( const QString & pid )
{
    d->homePanelId = pid;
}

QString View::homePanelId () const
{
    //if the home is valid, return it,
    if ( d->panels.contains ( d->homePanelId ) ) {
        return d->homePanelId;
    }
    //otherwise return the first one to be on the safe side.
    return d->firstPanelId;
}

View View::clone(
        const LocalizatorPtr & loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    View result;

    result.d->firstPanelId = d->firstPanelId;
    result.d->homePanelId = d->homePanelId;
    result.d->width = d->width;
    result.d->height = d->height;

    for( unsigned i = 0; i < d->panels.count(); i++ ) {
        result.d->panels.insert(
                d->panels.keyAt( i ),
                d->panels[ i ].clone( loc, tm, logger ) );
    }

    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: View.cpp 1168 2005-12-12 14:48:03Z modesto $
