// $Id: Localizator.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Localizator.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 3 October 2002
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

#ifndef _LOCALIZATOR_HH
#define _LOCALIZATOR_HH

#include <qobject.h>
#include <qstring.h>
#include <qmemarray.h>

#include <lt/SharedPtr.hh>

class QIOdevice;
class QTranslator;

namespace lt {

    class Localizator;

    /**
     * Predefined type of SharedPtr to Localizator.
     */
    typedef SharedPtr<Localizator> LocalizatorPtr;


    /**
     * Provide access to localized resources and/or messages.
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    class Localizator
    {
        public:

            /**
             * Shared null.
             */
            static Localizator * null();

            /**
             * Shared shared pointer to null.
             */
            static LocalizatorPtr nullPtr();

            /**
             * Construct a null localizator.
             */
            Localizator();

            /**
             * Construct a localizator for given locale. If
             * QString::null as a locale is specified the system
             * predefined one will be used. If and empty string( "" )
             * is specified, the Localizator will work without a locale.
             */
            Localizator( const QString & locale );

            /**
             * Destructor.
             */
            virtual ~Localizator();

            /**
             * return current country in a two character ISO format.
             */
            QString localeCountry() const;

            /**
             * return current language in a two character ISO format.
             */
            QString localeLanguage() const;

            /**
             * return current locale in a five character ISO format
             *(without charset), e.g., country_LANGUAGE
             */
            QString locale() const;

            /**
             * return localized resource as QByteArray.
             */
            virtual QByteArray resourceAsByteArray(
                    const QString & ) const
            { return QByteArray(); };

            /**
             * return localized resource as generic QIODevice.
             */
            virtual QIODevice *resourceAsIODevice(
                    const QString & ) const
            { return NULL; };

            /**
             * return localized message.
             */
            virtual QString localizedMessage( 
                    const QString & id,
                    const QString & /* context */ = QString::null,
                    const QString & /* comment */ = QString::null ) const
            { return id; };

            /**
             * return all localized messages as a translator suitable
             * for placing inside QAppllication::installTranslator
             */
            virtual const QTranslator *
                localizedMessages() const
                { return NULL; };

        public:

            /**
             * return the locale in validated form(stripping the
             * unnecessaty $charset part, e.g.
             * $language_$COUNTRY
             */
            static QString localeFull( const QString & locale );

            /**
             * return the country part from given locale.
             */
            static QString localeCountry( const QString & locale );

            /**
             * return the language part from given locale.
             */
            static QString localeLanguage( const QString & locale );

        private:

            Localizator( const Localizator& );
            Localizator& operator=( const Localizator& );

            struct LocalizatorPrivate;
            typedef SharedPtr<LocalizatorPrivate>
                LocalizatorPrivatePtr;
            LocalizatorPrivatePtr d;
    };

} // namespace lt

#endif /* _LOCALIZATOR_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: Localizator.hh 1168 2005-12-12 14:48:03Z modesto $
