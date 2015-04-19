// $Id: VFSLocalizator.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: VFSLocalizator.hh -- 
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

#ifndef _VFSLOCALIZATOR_HH
#define _VFSLOCALIZATOR_HH

#include <lt/templates/Localizator.hh>

#include <lt/SharedPtr.hh>

#include <lt/vfs/VFS.hh>

class QTranslator;

namespace lt {

    /**
     * \brief A localizator with VFS location as a source of localized
     * messages
     *
     * Provide access to localized messages. There is VFS location as a
     * source for the localized mesages.
     *
     * @author <a href="mailto:modesto@swac.cz">Patrik Modesto</a>
     */
    class VFSLocalizator :
        public Localizator
    {

        public:

            /**
             * Construct a null qt localizator.
             */
            VFSLocalizator();

            /**
             * Construct a localizator for given locale.
             */
            VFSLocalizator( const QString & locale );

            /**
             * Construct a localizator for given VFS source location and
             * locale. The location points to the parent directory of
             * the 'resources' not inside the resources!
             */
            VFSLocalizator( const VFSPtr & sourceLocation,
                    const QString & locale );

            /**
             * Destructor.
             */
            virtual ~VFSLocalizator();

            /**
             * return localized resource as QByteArray.
             */
            virtual QByteArray resourceAsByteArray(
                    const QString & ) const;

            /**
             * return localized resource as generic QIODevice. Always
             * returns an object of QIODevice interface, never NULL!.
             * It's users responsibility to delete the pointer when it's
             * not used anymore.
             */
            virtual QIODevice *resourceAsIODevice(
                    const QString & ) const;

            /**
             * return localized message or the id if no localized
             * message found.
             */
            virtual QString localizedMessage(
                    const QString & id,
                    const QString & context = QString::null,
                    const QString & comment = QString::null ) const;

            /**
             * return all localized messages as a translator suitable
             * for placing inside QAppllication::installTranslator
             */
            virtual const QTranslator * localizedMessages() const;

            /**
             * set source location of localized resources. The location
             * points to the parent directory of the 'resources' not
             * inside the resources!
             */
            virtual void setSourceLocation( const VFSPtr & loc );

        private:

            VFSLocalizator( const VFSLocalizator & );
            VFSLocalizator & operator=( const VFSLocalizator & );

            struct VFSLocalizatorPrivate;
            typedef SharedPtr<VFSLocalizatorPrivate>
                VFSLocalizatorPrivatePtr;
            VFSLocalizatorPrivatePtr d;
    };

    /**
     * Predefined type of SharedPtr to QTLocalizator.
     */
    typedef SharedPtr<VFSLocalizator> VFSLocalizatorPtr;

} // namespace lt

#endif /* _VFSLOCALIZATOR_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: VFSLocalizator.hh 1168 2005-12-12 14:48:03Z modesto $
