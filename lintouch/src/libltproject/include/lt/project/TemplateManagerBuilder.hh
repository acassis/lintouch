// $Id: TemplateManagerBuilder.hh,v 1.3 2004/08/12 22:31:13 jbar Exp $
//
//   FILE: TemplateManagerBuilder.hh --
//   AUTHOR: Jiri Barton <jbar@swac.cz>
//   DATE: 14 July 2004
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

#ifndef _TEMPLATEMANAGERBUILDER_HH
#define _TEMPLATEMANAGERBUILDER_HH

#include <lt/logger/Logger.hh>
#include <lt/configuration/Configuration.hh>

#include <lt/vfs/VFS.hh>
#include <lt/templates/TemplateManager.hh>

namespace lt {

    /**
     * Load libraries into a TemplateManager. Several ways are provided.
     */

    class TemplateManagerBuilder:
        public LogEnabled
    {
        public:
            /**
             * Constructor.
             */
            TemplateManagerBuilder(
                    LoggerPtr logger = Logger::nullPtr());

            /**
             * Destructor.
             */
            virtual ~TemplateManagerBuilder();

            /**
             * Create a TemplateManager and populate it with libraries
             * matching the names. The libraries are searched in the
             * default locations.
             *
             */
            TemplateManager buildFromNames(ConfigurationPtr names);

            /**
             * Create a TemplateManager and populate it with all the
             * libraries found in the default locations.
             */
            TemplateManager buildFromDefaultDirs();

        protected:
            /**
             * Return list of all directories that may be searched for a
             * TemplateLibrary.
             *
             * \return Return list of all directories that may be searched
             * for a TemplateLibrary.
             */
            const QStringList& defaultDirs() const;

        private:
            /**
             * Prevent copying.
             */
            TemplateManagerBuilder &operator =(
                    const TemplateManagerBuilder &);

            /**
             * Prevent copying
             */
            TemplateManagerBuilder(const TemplateManagerBuilder &);

            /**
             * Private data.
             */
            struct TemplateManagerBuilderPrivate;
            TemplateManagerBuilderPrivate *d;
    };
}

#endif /*_TEMPLATEMANAGERBUILDER_HH*/


// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateManagerBuilder.hh,v 1.3 2004/08/12 22:31:13 jbar Exp $
