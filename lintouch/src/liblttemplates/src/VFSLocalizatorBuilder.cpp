// $Id: VFSLocalizatorBuilder.cpp,v 1.3 2004/08/12 07:15:00 mman Exp $
//
//   FILE: VFSLocalizatorBuilder.cpp -- 
// AUTHOR: Patrik Modesto <modesto@swac.cz>
//   DATE: 14 October 2003
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

#include "lt/templates/VFSLocalizatorBuilder.hh"
#include "lt/templates/VFSLocalizator.hh"

#include <lt/vfs/VFS.hh>
using namespace lt;

VFSLocalizatorPtr VFSLocalizatorBuilder::buildFromUrl(
        const VFSPtr & location, const QString& locale ) const
{
    return VFSLocalizatorPtr( new VFSLocalizator( location, locale ) );
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFSLocalizatorBuilder.cpp,v 1.3 2004/08/12 07:15:00 mman Exp $
