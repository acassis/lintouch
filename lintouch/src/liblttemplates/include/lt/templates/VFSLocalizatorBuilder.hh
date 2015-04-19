// $Id: VFSLocalizatorBuilder.hh,v 1.3 2004/08/12 07:14:59 mman Exp $
//
//   FILE: VFSLocalizatorBuilder.hh -- 
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

#ifndef _VFSLOCALIZATORBUILDER_HH
#define _VFSLOCALIZATORBUILDER_HH

#include <lt/templates/VFSLocalizator.hh>

#include <qurl.h>
#include <qstring.h>

namespace lt {

    /**
     * \brief A helper class
     *
     * Helps with create of a VFSLocalizator.
     *
     * \author Patrik Modesto <modesto@swac.cz>
     */
    class VFSLocalizatorBuilder
    {
        public:
            /**
             * Build VFSLocalizator from the given VFS.
             *
             * \param location The top level project VFS.
             * \param locale The locale of translated messages.
             * \return A pointer object with the freshly created
             * VFSLocalizator or null SharedPtr<> on error.
             */
            VFSLocalizatorPtr buildFromUrl( const VFSPtr & location,
                    const QString& locale ) const;
    };

} // namespace lt

#endif /* _VFSLOCALIZATORBUILDER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFSLocalizatorBuilder.hh,v 1.3 2004/08/12 07:14:59 mman Exp $
