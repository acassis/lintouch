// $Id: Project.cpp 1556 2006-04-04 12:38:17Z modesto $
//
//   FILE: Project.cpp --
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

#include "lt/project/Project.hh"

using namespace lt;

struct Project::ProjectPrivate
{
    ~ProjectPrivate() {
        //delete all connections
        for(unsigned i=0; i < connections.count(); ++i) {
            delete connections[i];
        }
    }

    Info info;

    LocalizatorPtr localizator;

    TemplateManager templateManager;

    ConnectionDict connections;

    ViewMap views;

    ResourceManagerPtr resourceManager;

    bool valid;

};

Project::Project () : d ( new ProjectPrivate () )
{
    d->localizator = LocalizatorPtr ( new Localizator () );
    d->valid = false;
    d->connections.setAutoDelete(false);
}

Project::Project ( const Project & p ) : d ( p.d )
{
}

Project & Project::operator = ( const Project & p )
{
    d = p.d;
    return *this;
}

Project::~Project ()
{
}

bool Project::isValid () const
{
    return d->valid;
}

void Project::setInfo ( const Info & i )
{
    d->info = i;
    d->valid = true;
}

const Info & Project::info () const
{
    return d->info;
}

void Project::setLocalizator ( const LocalizatorPtr & localizator )
{
    d->localizator = localizator;
}

LocalizatorPtr Project::localizator () const
{
    return d->localizator;
}

void Project::setTemplateManager ( const TemplateManager & tm )
{
    d->templateManager = tm;
}

const TemplateManager & Project::templateManager () const
{
    return d->templateManager;
}

bool Project::addConnection ( const QString & key,
        const Connection * connection)
{
    if ( d->connections.contains( key ) ) {
        return false;
    }
    d->connections.insert ( key, connection );
    return true;
}

bool Project::removeConnection ( const QString & key )
{
    if ( d->connections.contains( key ) ) {
        d->connections.remove ( key );
        return true;
    }
    return false;
}

const ConnectionDict & Project::connections () const
{
    return d->connections;
}

ConnectionDict & Project::connections ()
{
    return d->connections;
}

bool Project::addView ( const QString & key, const View & view )
{
    if ( d->views.contains ( key ) ) {
        return false;
    }
    d->views.insert ( key, view );
    return true;
}

bool Project::removeView ( const QString & key )
{
    if ( d->views.contains ( key ) ) {
        d->views.remove ( key );
        return true;
    }
    return false;
}

const ViewMap & Project::views () const
{
    return d->views;
}

ViewMap & Project::views ()
{
    return d->views;
}

ResourceManagerPtr Project::resourceManager() const
{
    return d->resourceManager;
}

void Project::setResourceManager(const ResourceManagerPtr &rm)
{
    d->resourceManager = rm;
}

void Project::syncRealValues()
{
    //qWarning(">>>Project::syncRealValues");
    for( unsigned i = 0; i < d->connections.count(); ++i ) {
        //qWarning("sync_real connection name " + i.key () );
        d->connections[i]->syncRealValues();
    }
    //qWarning("<<<Project::syncRealValues");
}

void Project::syncRequestedValues()
{
    //qWarning(">>>Project::syncRequestedValues");
    for( unsigned i = 0; i < d->connections.count(); ++i ) {
        //qWarning("sync_requested connection name " + i.key () );
        d->connections[i]->syncRequestedValues();
    }
    //qWarning("<<<Project::syncRequestedValues");
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Project.cpp 1556 2006-04-04 12:38:17Z modesto $
