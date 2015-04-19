// $Id: TemplateLibrary.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: TemplateLibrary.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 09 October 2003
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

#include "lt/templates/TemplateLibrary.hh"
using namespace lt;

struct TemplateLibrary::TemplateLibraryPrivate
{

    // info about the library
    Info info;

    // info about templates within this library
    InfoMap tinfo;

    // from where the template library has been loaded
    QString filename;

    // is library loaded
    bool isValid;

    // should the library be visible within editor?
    bool isAvailable;

};

TemplateLibrary::TemplateLibrary() : QObject(NULL),
    d( new TemplateLibraryPrivate() )
{
    d->isValid = false;
    d->isAvailable = false;
}

TemplateLibrary::~TemplateLibrary()
{
    delete d;
}

const Info & TemplateLibrary::info() const
{
    return d->info;
}

const InfoMap & TemplateLibrary::templates() const
{
    return d->tinfo;
}

Template * TemplateLibrary::instantiate( const QString & /* type */,
        LoggerPtr /* logger = Logger::nullPtr() */ ) const
{
    return NULL;
}

void TemplateLibrary::validate()
{
    d->isValid = true;
}

bool TemplateLibrary::isValid() const
{
    return d->isValid;
}

void TemplateLibrary::setFilename(const QString &filename)
{
    d->filename = filename;
}

QString TemplateLibrary::filename() const
{
    return d->filename;
}

bool TemplateLibrary::isAvailable() const
{
    return d->isAvailable;
}

void TemplateLibrary::setAvailable( bool available )
{
    d->isAvailable = available;
}

bool TemplateLibrary::registerInfo( const Info & info )
{
    d->info = info;
    return true;
}

bool TemplateLibrary::registerTemplateInfo( const QString & type,
        const Info & tmpl )
{
    if( type.isEmpty() || d->tinfo.contains( type ) ) {
        return false;
    }

    d->tinfo.insert( type, tmpl );
    return true;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateLibrary.cpp 1168 2005-12-12 14:48:03Z modesto $
