// $Id: Panel.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Panel.cpp --
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
using namespace lt;

struct Panel::PanelPrivate
{
    PanelPrivate() {
    }

    virtual ~PanelPrivate()
    {
        templates.setAutoDelete( true );
        templates.clear();
    }

    // templates on this panel
    TemplateDict templates;
};

Panel::Panel() : d( new PanelPrivate() )
{
}

Panel::Panel( const Panel & p ) : d( p.d )
{
}

Panel & Panel::operator =( const Panel & p )
{
    d = p.d;
    return *this;
}

Panel::~Panel()
{
}

bool Panel::addTemplate( const QString & key, Template * t )
{
    if( d->templates.contains( key ) ) {
        return false;
    }
    if( t == NULL ) {
        return false;
    }
    d->templates.insert( key, t );
    return true;
}

bool Panel::removeTemplate( const QString & key )
{
    if( d->templates.contains( key ) ) {
        d->templates.remove( key );
        return true;
    }
    return false;
}

const TemplateDict & Panel::templates() const
{
    return d->templates;
}

TemplateDict & Panel::templates()
{
    return d->templates;
}

Panel Panel::clone( LocalizatorPtr loc,
        const TemplateManager & tm,
        LoggerPtr logger /* = Logger::nullPtr() */ ) const
{
    Panel result;

    for( unsigned i = 0; i < d->templates.count(); i++ ) {
        Template * cloned = d->templates[ i ]->clone( loc, tm, logger );
        Q_ASSERT( cloned != NULL );
        result.addTemplate( d->templates.keyAt( i ), cloned );
    }
    return result;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Panel.cpp 1168 2005-12-12 14:48:03Z modesto $
