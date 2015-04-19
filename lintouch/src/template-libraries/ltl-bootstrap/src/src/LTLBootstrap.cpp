// $Id:$
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


#include "LTLBootstrap.hh"

#include "lt/templates/PluginHelper.hh"
using namespace lt;

#include "Tester.hh"

#include "config.h"

// library private data
struct LTLBootstrap::LTLBootstrap_private
{

    LTLBootstrap_private() : tester( new Tester() )
    {
        Q_CHECK_PTR( tester );
    }

    ~LTLBootstrap_private()
    {
        delete tester;
    }

    Tester* tester;
};

LTLBootstrap::LTLBootstrap() : TemplateLibrary(),
    d( new LTLBootstrap_private )
{
    Q_CHECK_PTR( d );

    //register info
    Info i;
    i.setAuthor( "Patrik Modesto <modesto@swac.cz>" );
    i.setVersion( VERSION );
    i.setDate( QDate::fromString( TODAY, Qt::ISODate ) );
    i.setShortDescription( QObject::tr( "Bootstrap Template Library",
                "ltl-bootstrap" ) );
    i.setLongDescription( QObject::tr( "Bootstrap Template Library",
                "ltl-bootstrap" ) );

    registerInfo( i );

    // register templates into this library
    registerTemplateInfo( d->tester->type(),  d->tester->info() );
}

LTLBootstrap::~LTLBootstrap()
{
    delete d;
}

Template * LTLBootstrap::instantiate( const QString & type,
                        LoggerPtr logger /*= Logger::nullPtr()*/ ) const
{
    Q_CHECK_PTR( d );
    if( type == d->tester->type() ) {
        return new Tester( logger );
    }
    return NULL;
}

// library loader macro
EXPORT_LT_PLUGIN( LTLBootstrap, "lt::TemplateLibrary" );

// vim: set et ts=4 sw=4 tw=76:
// $Id:$
