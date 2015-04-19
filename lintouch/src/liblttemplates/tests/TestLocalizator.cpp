// $Id: TestLocalizator.cpp,v 1.7 2004/09/14 11:20:33 jbar Exp $
//
//   FILE: TestLocalizator.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 15 January 2003
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

// custom tests
#include <cppunit/extensions/HelperMacros.h>

#include "lt/templates/Localizable.hh"
#include "lt/templates/Localizator.hh"
#include "lt/templates/VFSLocalizator.hh"

#include <lt/vfs/VFS.hh>
#include <lt/vfs/VFSLocal.hh>
using namespace lt;

#include <qapplication.h>
#include <qtextcodec.h>

extern QString srcdir;
extern QString builddir;

class DummyLocalizableClass : public Localizable
{
    virtual void localize( LocalizatorPtr /*l*/ ) {}
};

class TestLocalizator : public CppUnit::TestFixture
{

    CPPUNIT_TEST_SUITE( TestLocalizator );

    CPPUNIT_TEST( testLocalizator );

    CPPUNIT_TEST( testVFSLocNull );
    CPPUNIT_TEST( testVFSWithData );

    CPPUNIT_TEST_SUITE_END();

    public:

        void setUp()
        {
            // fill in
        }

        void tearDown()
        {
            // fill in
        }

        void testLocalizator()
        {

            Localizator ll;

            Localizator e ( "en" );
            Localizator eus ( "en_US" );
            Localizator egb ( "en_GB" );
            Localizator egbi ( "en_GB.ISO-8859-1" );
            Localizator egbu ( "en_GB.UTF-8" );

            Localizator l ( QString::null );
            CPPUNIT_ASSERT( l.locale () ==
                    Localizator::localeFull ( QTextCodec::locale () ) );

            Localizator l_empty ( "" );
            CPPUNIT_ASSERT( l_empty.locale () == "" );

            CPPUNIT_ASSERT( e.locale () == "en" );
            CPPUNIT_ASSERT( eus.locale () == "en_US" );
            CPPUNIT_ASSERT( egb.locale () == "en_GB" );
            CPPUNIT_ASSERT( egbi.locale () == "en_GB" );
            CPPUNIT_ASSERT( egbu.locale () == "en_GB" );

            QString ls = l.locale ();

            CPPUNIT_ASSERT( Localizator::localeFull ( ls ) ==
                    Localizator::localeFull ( QTextCodec::locale () ) );
            CPPUNIT_ASSERT( Localizator::localeCountry ( ls ) ==
                    Localizator::localeCountry ( QTextCodec::locale () ) );
            CPPUNIT_ASSERT( Localizator::localeLanguage ( ls ) ==
                    Localizator::localeLanguage ( QTextCodec::locale () ) );

            CPPUNIT_ASSERT( e.locale () == "en" );
            CPPUNIT_ASSERT( e.localeLanguage () == "en" );
            CPPUNIT_ASSERT( e.localeCountry () == "" );

            CPPUNIT_ASSERT( eus.locale () == "en_US" );
            CPPUNIT_ASSERT( eus.localeLanguage () == "en" );
            CPPUNIT_ASSERT( eus.localeCountry () == "US" );

            CPPUNIT_ASSERT( egb.locale () == "en_GB" );
            CPPUNIT_ASSERT( egb.localeLanguage () == "en" );
            CPPUNIT_ASSERT( egb.localeCountry () == "GB" );

            CPPUNIT_ASSERT( egbi.locale () == "en_GB" );
            CPPUNIT_ASSERT( egbi.localeLanguage () == "en" );
            CPPUNIT_ASSERT( egbi.localeCountry () == "GB" );

            CPPUNIT_ASSERT( egbu.locale () == "en_GB" );
            CPPUNIT_ASSERT( egbu.localeLanguage () == "en" );
            CPPUNIT_ASSERT( egbu.localeCountry () == "GB" );

        }

        void testVFSLocNull ()
        {
            VFSLocalizator vfsl;

            CPPUNIT_ASSERT(
                    vfsl.resourceAsByteArray( QString::null ) == QByteArray() );
            QIODevice* io = vfsl.resourceAsIODevice( QString::null );
            CPPUNIT_ASSERT( io != NULL );
            CPPUNIT_ASSERT( io->isOpen() == false );

            CPPUNIT_ASSERT( vfsl.localizedMessages() == NULL );

            CPPUNIT_ASSERT( vfsl.localizedMessage( "" ) == "" );
            CPPUNIT_ASSERT(
                    vfsl.localizedMessage( QString::null ) == QString::null );

            CPPUNIT_ASSERT(
                    vfsl.localizedMessage( "NOT_AN_ID" ) == "NOT_AN_ID" );

            CPPUNIT_ASSERT( vfsl.localizedMessage( "THE_ID" ) == "THE_ID" );

            CPPUNIT_ASSERT(
                    vfsl.localizedMessage( "THE_ID", "LT" ) == "THE_ID" );
        }

        void testVFSWithData ()
        {
            VFSPtr root = VFSPtr( new VFSLocal( QUrl( srcdir +
                            "/tests/a_project/resources/" ) ) );

            VFSLocalizator vfsl( root, "cs_CZ" );

            // [-- VFSLocalizator native message access --]
            CPPUNIT_ASSERT( vfsl.localizedMessage( "root_only", "lt" ) ==
                    "I AM ROOT MESSAGE ONLY" );
            CPPUNIT_ASSERT( vfsl.localizedMessage( "lang_only", "lt" ) ==
                    "I AM LANGUAGE MESSAGE ONLY" );
            CPPUNIT_ASSERT( vfsl.localizedMessage( "full_only", "lt" ) ==
                    "I AM FULL MESSAGE ONLY" );

            CPPUNIT_ASSERT( vfsl.localizedMessage( "root_and_full", "lt" ) ==
                    "I AM ROOT AND FULL MESSAGE from FULL" );

            CPPUNIT_ASSERT( vfsl.localizedMessage( "root_and_lang", "lt" ) ==
                    "I AM ROOT AND LANGUAGE MESSAGE from LANG" );

            CPPUNIT_ASSERT( vfsl.localizedMessage( "lang_and_full", "lt" ) ==
                    "I AM FULL AND LANGUAGE MESSAGE from FULL" );

            // [-- VFSLocalizator's QTranslator message access --]
            const QTranslator* t = vfsl.localizedMessages();
            CPPUNIT_ASSERT( t );
            CPPUNIT_ASSERT(
                    t->findMessage( "lt", "root_only","" ).translation() ==
                    "I AM ROOT MESSAGE ONLY" );
            CPPUNIT_ASSERT(
                    t->findMessage( "lt", "lang_only","" ).translation() ==
                    "I AM LANGUAGE MESSAGE ONLY" );
            CPPUNIT_ASSERT(
                    t->findMessage( "lt", "full_only", "" ).translation() ==
                    "I AM FULL MESSAGE ONLY" );

            CPPUNIT_ASSERT(
                    t->findMessage( "lt", "root_and_full", "" )
                    .translation() ==
                    "I AM ROOT AND FULL MESSAGE from FULL" );

            CPPUNIT_ASSERT(
                    t->findMessage( "lt", "root_and_lang", "" )
                    .translation() ==
                    "I AM ROOT AND LANGUAGE MESSAGE from LANG" );

            CPPUNIT_ASSERT(
                    t->findMessage( "lt", "lang_and_full", "" )
                    .translation() ==
                    "I AM FULL AND LANGUAGE MESSAGE from FULL" );

            // [-- VFSLocalizator resourceAsByteArray test --]
            QString s;
            s = QString( vfsl.resourceAsByteArray( "file_in_full.txt" ) );
            CPPUNIT_ASSERT( s.simplifyWhiteSpace() == "full" );

            s = QString( vfsl.resourceAsByteArray( "file_in_lang.txt" ) );
            CPPUNIT_ASSERT( s .simplifyWhiteSpace() == "lang" );

            s = QString( vfsl.resourceAsByteArray( "file_in_root.txt" ) );
            CPPUNIT_ASSERT( s.simplifyWhiteSpace() == "root" );

            // [-- VFSLocalizator resourceAsIODevice test --]
            QIODevice* io = NULL;
            io = vfsl.resourceAsIODevice( "file_in_full.txt" );
            CPPUNIT_ASSERT( io->isOpen() == true );
            s = QString( io->readAll() );
            CPPUNIT_ASSERT( s.simplifyWhiteSpace() == "full" );

            io = vfsl.resourceAsIODevice( "file_in_lang.txt" );
            CPPUNIT_ASSERT( io->isOpen() == true );
            s = QString( io->readAll() );
            CPPUNIT_ASSERT( s .simplifyWhiteSpace() == "lang" );

            io = vfsl.resourceAsIODevice( "file_in_root.txt" );
            CPPUNIT_ASSERT( io->isOpen() == true );
            s = QString( io->readAll() );
            CPPUNIT_ASSERT( s.simplifyWhiteSpace() == "root" );

            // [-- Language only locale --]
            VFSLocalizator vfs2( root, "cs" );

            t = vfs2.localizedMessages();
            CPPUNIT_ASSERT( t );

            CPPUNIT_ASSERT( vfs2.localizedMessage( "lang_and_full", "lt" ) ==
                    "I AM LANGUAGE AND FULL MESSAGE from LANG" );

            s = QString( vfs2.resourceAsByteArray( "file_in_full.txt" ) );
            CPPUNIT_ASSERT( s.simplifyWhiteSpace() == "lang" );

            // [-- no locale at all --]
            VFSLocalizator vfs3( root, "" );

            t = vfs3.localizedMessages();
            CPPUNIT_ASSERT( t );

            CPPUNIT_ASSERT( vfs3.localizedMessage( "root_and_full", "lt" ) ==
                    "I AM ROOT AND FULL MESSAGE from ROOT" );
            CPPUNIT_ASSERT( vfs3.localizedMessage( "root_and_lang", "lt" ) ==
                    "I AM ROOT AND LANGUAGE MESSAGE from ROOT" );

            s = QString( vfs3.resourceAsByteArray( "file_in_full.txt" ) );
            CPPUNIT_ASSERT( s.simplifyWhiteSpace() == "root" );
            s = QString( vfs3.resourceAsByteArray( "file_in_lang.txt" ) );
            CPPUNIT_ASSERT( s.simplifyWhiteSpace() == "root" );
        }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestLocalizator );
// end of custom tests

// vim: set et ts=4 sw=4 tw=76:
// $Id: TestLocalizator.cpp,v 1.7 2004/09/14 11:20:33 jbar Exp $
