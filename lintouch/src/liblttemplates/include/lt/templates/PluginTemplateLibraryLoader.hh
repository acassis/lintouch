// $Id: PluginTemplateLibraryLoader.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: PluginTemplateLibraryLoader.hh -- 
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

#ifndef _PLUGINTEMPLATELIBRARYLOADER_HH
#define _PLUGINTEMPLATELIBRARYLOADER_HH

#include "lt/templates/TemplateLibrary.hh"

#include "lt/logger/Logger.hh"

namespace lt {

    /**
     * PluginTemplateLibraryLoader.
     *
     * Plugin Template Library Loader is able to load a TemplateLibrary
     * from an .so or .dll file.
     *
     * TemplateLibrary loaded from the file must extend the
     * TemplateLibrary class. The .so or .dll file must also define the
     * following extern "c" symbols:
     *
     * - TemplateLibrary * create()
     *
     * that are used by this class to actually create the
     * template library instance.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class PluginTemplateLibraryLoader
    {

        private:

            /**
             * Constructor.
             */
            PluginTemplateLibraryLoader();

            /**
             * Destructor.
             */
            virtual ~PluginTemplateLibraryLoader();

        public:

            /**
             * load the template library from a specified file or return
             * null in case of any error. Note that filename need not
             * include the unix specific "lib" prefix and platform
             * specific extension ".dll" or ".so". Underlying QLibrary
             * will try to determine theese for you.
             *
             * Also try to load appropriate translations by searching system
             * dependent directories. translation prefix should be something
             * like 'mytemplatelibrary'. it will be prepended with path and
             * will get a suffix _locale.
             */
            static TemplateLibrary * loadFrom( const QString & filename,
                    const QString & translationPrefix = QString::null,
                    LoggerPtr logger = Logger::nullPtr() );

    };

} // namespace lt

#endif /* _PLUGINTEMPLATELIBRARYLOADER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: PluginTemplateLibraryLoader.hh 1168 2005-12-12 14:48:03Z modesto $
