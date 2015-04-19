// $Id: Info.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Info.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 03 October 2003
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

#include "lt/templates/Info.hh"
using namespace lt;

struct Info::InfoPrivate
{

    QString author;
    QString version;
    QDate date;
    QString shortDescription;
    QString longDescription;
    QImage icon;

};

Info::Info() : d( new InfoPrivate() )
{
}

Info::Info( const Info & i ) : d( i.d )
{
}

Info & Info::operator=( const Info & i )
{
    d = i.d;
    return *this;
}

Info Info::clone() const
{
    Info result;
    result.d = InfoPrivatePtr( new InfoPrivate( *d ) );
    return result;
}

Info::~Info()
{
}

void Info::setAuthor( const QString & author )
{
    d->author = author;
}

QString Info::author() const
{
    return d->author;
}

void Info::setVersion( const QString & version )
{
    d->version = version;
}

QString Info::version() const
{
    return d->version;
}

void Info::setDate( const QDate & date )
{
    d->date = date;
}

QDate Info::date() const
{
    return d->date;
}

void Info::setShortDescription( const QString & desc )
{
    d->shortDescription = desc;
}

QString Info::shortDescription() const
{
    return d->shortDescription;
}

void Info::setLongDescription( const QString & desc )
{
    d->longDescription = desc;
}

QString Info::longDescription() const
{
    return d->longDescription;
}

void Info::setIcon( const QImage & icon )
{
    d->icon = icon;
}

QImage Info::icon() const
{
    return d->icon;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Info.cpp 1168 2005-12-12 14:48:03Z modesto $
