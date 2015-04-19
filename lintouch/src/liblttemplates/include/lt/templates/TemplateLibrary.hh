// $Id: TemplateLibrary.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: TemplateLibrary.hh --
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

#ifndef _TEMPLATELIBRARY_HH
#define _TEMPLATELIBRARY_HH

#include <qobject.h>

#include "lt/LTDict.hh"

#include "lt/templates/Template.hh"

namespace lt {

    class TemplateLibrary;

    typedef LTDict <TemplateLibrary> TemplateLibraryDict;

    /**
     * TemplateLibrary.
     *
     * Template Library serves as a factory class for one or
     * more Templates.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class TemplateLibrary : public QObject
    {
        Q_OBJECT

        public:

            /**
             * Constructor.
             */
            TemplateLibrary();

            /**
             * Destructor.
             */
            virtual ~TemplateLibrary();

        private:

            // Disable unintentional copying
            TemplateLibrary( const TemplateLibrary & p );
            TemplateLibrary & operator =( const TemplateLibrary & p );

        public:

            /**
             * Return the info about this TemplateLibrary.
             */
            const Info & info() const;

            /**
             * Return the info about Templates contained within this
             * TemplateLibrary.
             */
            const InfoMap & templates() const;

            /**
             * Return new Template instance of the given type.
             */
            virtual Template * instantiate( const QString & type,
                    LoggerPtr logger = Logger::nullPtr() ) const;

        public:

            /*
             * The library is loaded and valid.
             */
            bool isValid() const;

            /*
             * Set the state of the library to loaded and valid.
             */
            void validate();

            /*
             * The filename from which the library is loaded.
             */
            QString filename() const;

            /*
             * Set the filename from which the library is loaded.
             */
            void setFilename(const QString &filename);

            /**
             * Should the library be available within editor?
             */
            bool isAvailable() const;

            /**
             * Set the availability flag.
             */
            void setAvailable( bool available );

        protected:

            /**
             * Specify info about this template library.
             */
            bool registerInfo( const Info & info );

            /**
             * Specify info about the template of type `type`.
             */
            bool registerTemplateInfo( const QString & type,
                    const Info & tmpl );

        private:

            typedef struct TemplateLibraryPrivate;
            TemplateLibraryPrivate * d;

    };

} // namespace lt

#endif /* _TEMPLATELIBRARY_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplateLibrary.hh 1168 2005-12-12 14:48:03Z modesto $
