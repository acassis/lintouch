// $Id: Info.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Info.hh -- 
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

#ifndef _INFO_HH
#define _INFO_HH

#include <qdatetime.h>
#include <qimage.h>
#include <qstring.h>

#include <lt/SharedPtr.hh>
#include "lt/LTMap.hh"

namespace lt {

    class Info;
    typedef LTMap <Info> InfoMap;

    /**
     * Info.
     *
     * Info is a simple class which holds general information about the
     * Lintouch Template, Lintouch Template Library, and Lintouch Project.
     *
     * Every Info instance can define the following entries:
     * <ul>
     * <li>Author: Free form string.
     * <li>Version: Free form string.
     * <li>Date: The date of creation or last modification.
     * <li>Short Description: Single line description of the entity.
     * <li>Long Description: Multi line description of the entity.
     * <li>Icon: 32x32 pixels icon visually describing the entity.
     * </ul>
     *
     * This class is value based and its contents is shared implicitly.
     *
     * To create independent instance with the same contents use the clone()
     * method.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class Info
    {

        public:

            /**
             * Constructor.
             */
            Info();

            /**
             * Copy Constructor. Will create instance whose contents is
             * implicitly shared with this instance.
             *
             * @see clone.
             */
            Info( const Info & i );

            /**
             * operator =. Will create instance whose contents is
             * implicitly shared with this instance.
             *
             * @see clone.
             */
            Info & operator=( const Info & i );

            /**
             * Create independent instance with the same contents.
             */
            Info clone() const;

            /**
             * Destructor.
             */
            virtual ~Info();

        public:

            /**
             * Specify the author.
             */
            void setAuthor( const QString & author );

            /**
             * Return the author.
             */
            QString author() const;

            /**
             * Specify the version.
             */
            void setVersion( const QString & version );

            /**
             * Return the version.
             */
            QString version() const;

            /**
             * Specify the date of creation.
             */
            void setDate( const QDate & date );

            /**
             * Return the date of creation.
             */
            QDate date() const;

            /**
             * Specify the short description.
             */
            void setShortDescription( const QString & desc );

            /**
             * Return the short description.
             */
            QString shortDescription() const;

            /**
             * Specify the long description.
             */
            void setLongDescription( const QString & desc );

            /**
             * Return the long description.
             */
            QString longDescription() const;

            /**
             * Specify the icon.
             */
            void setIcon( const QImage & icon );

            /**
             * Return the icon.
             */
            QImage icon() const;

        private:

            typedef struct InfoPrivate;
            typedef SharedPtr<InfoPrivate> 
                InfoPrivatePtr;

            InfoPrivatePtr d;

    };

} // namespace lt

#endif /* _INFO_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Info.hh 1168 2005-12-12 14:48:03Z modesto $
