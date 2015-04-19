// $Id: TemplateManager.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: TemplateManager.hh -- 
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

#ifndef _TEMPLATEMANAGER_HH
#define _TEMPLATEMANAGER_HH

#include <lt/SharedPtr.hh>

#include "lt/templates/TemplateLibrary.hh"

namespace lt {

    /**
     * TemplateManager.
     *
     * A Container(or manager) of template libraries.
     *
     * When used as a value based class the contents is implicitly
     * shared among all instances created by assigning or via copy
     * constructor:
     *
     * There is no support for copy-on-write ATM. It'll be added as it
     * will be needed(if ever).
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class TemplateManager
    {

        public:

            /**
             * Constructor.
             */
            TemplateManager();

            TemplateManager( const TemplateManager & p );

            TemplateManager & operator =( const TemplateManager & p );

            /**
             * Destructor.
             */
            virtual ~TemplateManager();

        public:

            /**
             * Return the list of directories that should be searched when
             * loading template libraries. The returned list is OS
             * dependent.
             */
            static QStringList templateLibraryDirs();

        public:

            /**
             * Register given template library.
             *
             * Takes ownership of the library. When
             * TemplateManager will be destroyed, it will destroy all
             * registered template libraries.
             */
            bool registerTemplateLibrary( const QString & hint, 
                    const TemplateLibrary * tmpl );

            /**
             * Unregister given template library.
             *
             * unregister and delete template library under the given
             * hint.
             */
            bool unregisterTemplateLibrary( const QString & hint );

            /**
             * Will clear all libraries that are registered within this
             * TemplateManager. Note that libraries will be deleted as
             * well.
             */
            void unregisterAllTemplateLibraries();

            /**
             * Return the registered template libraries.
             */
            const TemplateLibraryDict & templateLibraries() const;

            /**
             * Instantiate a template of given type.
             *
             * When a hint is null, lookup in all registered template
             * libraries.
             */
            Template * instantiate( const QString & type,
                    LoggerPtr logger = Logger::nullPtr(),
                    const QString & hint = QString::null ) const;

        private:

            typedef struct TemplateManagerPrivate;
            typedef SharedPtr<TemplateManagerPrivate>
                TemplateManagerPrivatePtr;

            TemplateManagerPrivatePtr d;

    };

} // namespace lt

#endif /* _TEMPLATEMANAGER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateManager.hh 1168 2005-12-12 14:48:03Z modesto $
