// $Id: Localizator.cpp 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: Localizator.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 3 October 2003
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

#include "lt/templates/Localizator.hh"
using namespace lt;

#include <qiodevice.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <qtranslator.h>

Localizator * Localizator::null()
{
    static Localizator * null = new Localizator();
    return null;
}

LocalizatorPtr Localizator::nullPtr()
{
    static LocalizatorPtr nullPtr( Localizator::null() );
    return nullPtr;
}

struct Localizator::LocalizatorPrivate
{
    QString m_locale;
    QString m_localeCountry;
    QString m_localeLanguage;
};

Localizator::Localizator() :
    d( LocalizatorPrivatePtr( new LocalizatorPrivate ) )
{
    Q_CHECK_PTR( d );
}

Localizator::Localizator( const QString & locale ) :
    d( LocalizatorPrivatePtr( new LocalizatorPrivate ) )
{
    Q_CHECK_PTR( d );

    QString l = locale;
    if( l.isNull() ) {
        l = QTextCodec::locale();
    }

    d->m_locale = l;
    if( ! l.isEmpty() ) {
        d->m_locale = Localizator::localeFull( l );
        d->m_localeCountry = Localizator::localeCountry( l );
        d->m_localeLanguage = Localizator::localeLanguage( l );
    }
}

Localizator::~Localizator()
{
}

QString Localizator::localeCountry() const
{
    return d->m_localeCountry;
}

QString Localizator::localeLanguage() const
{
    return d->m_localeLanguage;
}

QString Localizator::locale() const
{
    return d->m_locale;
}

static QRegExp rx( "([a-z]{2})(_([A-Z]{2})?(\\.[a-zA-Z0-9-]+)?)?" );

QString Localizator::localeFull( const QString & locale )
{
    if( rx.exactMatch( locale ) ) {
        return rx.cap( 1 ) + 
          (( rx.cap( 3 ).isEmpty() ) ?( "" ) :( "_" + rx.cap( 3 ) ) );
    }
    return QString::null;
}

QString Localizator::localeCountry( const QString & locale )
{
    if( rx.exactMatch( locale ) ) {
        return rx.cap( 3 );
    }
    return QString::null;
}

QString Localizator::localeLanguage( const QString & locale )
{
    if( rx.exactMatch( locale ) ) {
        return rx.cap( 1 );
    }
    return QString::null;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: Localizator.cpp 1168 2005-12-12 14:48:03Z modesto $
