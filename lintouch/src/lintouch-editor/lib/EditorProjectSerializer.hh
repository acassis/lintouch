// $Id: EditorProjectSerializer.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: ProjectSerializer.hh --
// AUTHOR: Jiri Barton <barton@swac.cz>
//   DATE: 13 September 2004
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

#ifndef _PROJECTSERIALIZER_HH
#define _PROJECTSERIALIZER_HH

#include <EditorProject.hh>
#include <lt/logger/Logger.hh>
#include <lt/configuration/Configuration.hh>
#include <qurl.h>

class ProjectWindow;

namespace lt {

    /**
     * Save the EditorProject.
     */
    class EditorProjectSerializer:
        public LogEnabled
    {
        public:

            /**
              * Initialize the class.
              */
            EditorProjectSerializer(LoggerPtr logger = Logger::nullPtr());

            /**
             * Destructor.
             */
            virtual ~EditorProjectSerializer();

            /**
             * Serialize the EditorProject to the given location and return
             * the success of the operation.
             */
            bool saveEditorProject(EditorProject &prj,
                const QUrl& location, ConfigurationPtr es);

        private:
            struct EditorProjectSerializerPrivate;
            EditorProjectSerializerPrivate *d;
    };
}

#endif /* _PROJECTSERIALIZER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: EditorProjectSerializer.hh 1169 2005-12-12 15:22:15Z modesto $
