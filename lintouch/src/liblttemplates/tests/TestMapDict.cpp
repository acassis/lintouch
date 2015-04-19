// $Id: TestMapDict.cpp,v 1.5 2004/12/01 16:25:01 mman Exp $
//
//   FILE: TestLTMapDict.cpp --
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 19 August 2004
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

#include <qapplication.h>
#include <qfile.h>
#include <qdir.h>

#include "lt/LTDict.hh"
#include "lt/LTMap.hh"
using namespace lt;

#include <lt/SharedPtr.hh>
using namespace lt;

class DummyRefCount {
public:
    DummyRefCount() { ++ref; }
    ~DummyRefCount() { CPPUNIT_ASSERT(ref>0); --ref; }
    static int ref;
};
int DummyRefCount::ref = 0;

typedef SharedPtr<DummyRefCount> DummyRefCountPtr;


class TestLTMapNDict : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestLTMapNDict );

    CPPUNIT_TEST( testLTDictAppend );
    CPPUNIT_TEST( testLTDictRandom );
    CPPUNIT_TEST( testLTDictCurrentAndRemoveByIdx );
    CPPUNIT_TEST( testLTDictCurrentAndRemoveByKey );
    CPPUNIT_TEST( testLTDictMoveByIdx );
    CPPUNIT_TEST( testLTDictMoveByKey );
    CPPUNIT_TEST( testLTDictClear );

    CPPUNIT_TEST( testLTDictRemove );
    CPPUNIT_TEST( testLTMapRemove );

    CPPUNIT_TEST( testLTMapAppend );
    CPPUNIT_TEST( testLTMapRandom );
    CPPUNIT_TEST( testLTMapCurrentAndRemoveByIdx );
    CPPUNIT_TEST( testLTMapCurrentAndRemoveByKey );
    CPPUNIT_TEST( testLTMapMoveByIdx );
    CPPUNIT_TEST( testLTMapMoveByKey );
    CPPUNIT_TEST( testLTMapClear );

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

        void testLTDictAppend()
        {
            LTDict <QObject> c;

            QObject aa, bb, cc, dd;

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            // first append only
            c.insert( "aa", &aa );
            c.insert( "bb", &bb );
            c.insert( "cc", &cc );
            c.insert( "dd", &dd );

            CPPUNIT_ASSERT( c.contains( "aa" ) == true );
            CPPUNIT_ASSERT( c.contains( "bb" ) == true );
            CPPUNIT_ASSERT( c.contains( "cc" ) == true );
            CPPUNIT_ASSERT( c.contains( "dd" ) == true );
            CPPUNIT_ASSERT( c.contains( "ee" ) == false );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            CPPUNIT_ASSERT( c[ "aa" ] == &aa );
            CPPUNIT_ASSERT( c[ "bb" ] == &bb );
            CPPUNIT_ASSERT( c[ "cc" ] == &cc );
            CPPUNIT_ASSERT( c[ "dd" ] == &dd );
            CPPUNIT_ASSERT( c[ "ee" ] == NULL );

            CPPUNIT_ASSERT( c[ 0 ] == &aa );
            CPPUNIT_ASSERT( c[ 1 ] == &bb );
            CPPUNIT_ASSERT( c[ 2 ] == &cc );
            CPPUNIT_ASSERT( c[ 3 ] == &dd );
            CPPUNIT_ASSERT( c[ 4 ] == NULL );

        }

        void testLTDictRandom()
        {
            LTDict <QObject> c;

            QObject aa, bb, cc, dd;

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            // random insertion only
            c.insert( "aa", &aa, 0 );
            c.insert( "dd", &dd, 1 );
            c.insert( "bb", &bb, 1 );
            c.insert( "cc", &cc, 2 );

            CPPUNIT_ASSERT( c.contains( "aa" ) == true );
            CPPUNIT_ASSERT( c.contains( "bb" ) == true );
            CPPUNIT_ASSERT( c.contains( "cc" ) == true );
            CPPUNIT_ASSERT( c.contains( "dd" ) == true );
            CPPUNIT_ASSERT( c.contains( "ee" ) == false );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            CPPUNIT_ASSERT( c.keyAt( 10 ) == QString::null );
            CPPUNIT_ASSERT( c.keyAt( 0 ) == "aa" );
            CPPUNIT_ASSERT( c.keyAt( 1 ) == "bb" );
            CPPUNIT_ASSERT( c.keyAt( 2 ) == "cc" );
            CPPUNIT_ASSERT( c.keyAt( 3 ) == "dd" );

            CPPUNIT_ASSERT( c[ "aa" ] == &aa );
            CPPUNIT_ASSERT( c[ "bb" ] == &bb );
            CPPUNIT_ASSERT( c[ "cc" ] == &cc );
            CPPUNIT_ASSERT( c[ "dd" ] == &dd );
            CPPUNIT_ASSERT( c[ "ee" ] == NULL );

            CPPUNIT_ASSERT( c[ 0 ] == &aa );
            CPPUNIT_ASSERT( c[ 1 ] == &bb );
            CPPUNIT_ASSERT( c[ 2 ] == &cc );
            CPPUNIT_ASSERT( c[ 3 ] == &dd );
            CPPUNIT_ASSERT( c[ 4 ] == NULL );

        }

        void testLTDictCurrentAndRemoveByKey()
        {
            LTDict <QObject> c;

            QObject aa, bb, cc, dd;

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", &aa );
            c.insert( "bb", &bb );
            c.insert( "cc", &cc );
            c.insert( "dd", &dd );

            // now check current pointer
            // invalid key should not move the pointer
            c.setCurrentKey( "ff" );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == &aa );

            c.setCurrentKey( "dd" );

            CPPUNIT_ASSERT( c.currentIndex() == 3 );
            CPPUNIT_ASSERT( c.currentKey() == "dd" );
            CPPUNIT_ASSERT( c.current() == &dd );

            c.setCurrentKey( "bb" );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "bb" );
            CPPUNIT_ASSERT( c.current() == &bb );

            // now remove the item at the end, current should not change
            c.remove( "dd" );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "bb" );
            CPPUNIT_ASSERT( c.current() == &bb );

            // now remove the current item, current should move to cc
            c.remove( "bb" );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "cc" );
            CPPUNIT_ASSERT( c.current() == &cc );

            // now remove the current item, current should move to aa
            c.remove( "cc" );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == &aa );

            // now remove the current item, current should move to -1 and
            // the dictionary should be empty
            c.remove( "aa" );

            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );
        }

        void testLTDictCurrentAndRemoveByIdx()
        {
            LTDict <QObject> c;

            QObject aa, bb, cc, dd;

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", &aa );
            c.insert( "bb", &bb );
            c.insert( "cc", &cc );
            c.insert( "dd", &dd );

            // now check current pointer
            // invalid should not move the pointer
            c.setCurrentIndex( 6 );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == &aa );

            c.setCurrentIndex( 3 );

            CPPUNIT_ASSERT( c.currentIndex() == 3 );
            CPPUNIT_ASSERT( c.currentKey() == "dd" );
            CPPUNIT_ASSERT( c.current() == &dd );

            c.setCurrentIndex( 1 );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "bb" );
            CPPUNIT_ASSERT( c.current() == &bb );

            // now remove the item at the end, current should not change
            c.remove( 3 );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "bb" );
            CPPUNIT_ASSERT( c.current() == &bb );

            // now remove the current item, current should move to cc
            c.remove( 1 );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "cc" );
            CPPUNIT_ASSERT( c.current() == &cc );

            // now remove the current item, current should move to aa
            c.remove( 1 );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == &aa );

            // now remove the current item, current should move to -1 and
            // the dictionary should be empty
            c.remove( 0 );

            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );
        }

        void testLTDictMoveByKey()
        {
            LTDict <QObject> c;

            QObject aa, bb, cc, dd;

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", &aa );
            c.insert( "bb", &bb );
            c.insert( "cc", &cc );
            c.insert( "dd", &dd );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "cc", 2 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "aa", -1 );

            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "aa", 0 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "bb", 2 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "aa", 2 );

            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );
        }


        void testLTDictMoveByIdx()
        {
            LTDict <QObject> c;

            QObject aa, bb, cc, dd;

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", &aa );
            c.insert( "bb", &bb );
            c.insert( "cc", &cc );
            c.insert( "dd", &dd );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 2, 2 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 0, -1 );

            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 3, 0 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 1, 2 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 0, 2 );

            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );
        }

        void testLTDictClear()
        {
            LTDict <QObject> c;

            QObject aa, bb, cc, dd;

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", &aa );
            c.insert( "bb", &bb );
            c.insert( "cc", &cc );
            c.insert( "dd", &dd );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.clear();

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );
            CPPUNIT_ASSERT( c.isEmpty() );

            c.insert( "aa", &aa );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == &aa );
            CPPUNIT_ASSERT( ! c.isEmpty() );
        }

        void testLTMapAppend()
        {
            LTMap <QString> c;

            QString aa = "AA", bb = "BB", cc = "CC", dd = "DD";

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            // first append only
            c.insert( "aa", aa );
            c.insert( "bb", bb );
            c.insert( "cc", cc );
            c.insert( "dd", dd );

            CPPUNIT_ASSERT( c.contains( "aa" ) == true );
            CPPUNIT_ASSERT( c.contains( "bb" ) == true );
            CPPUNIT_ASSERT( c.contains( "cc" ) == true );
            CPPUNIT_ASSERT( c.contains( "dd" ) == true );
            CPPUNIT_ASSERT( c.contains( "ee" ) == false );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            CPPUNIT_ASSERT( c[ "aa" ] == "AA" );
            CPPUNIT_ASSERT( c[ "bb" ] == "BB" );
            CPPUNIT_ASSERT( c[ "cc" ] == "CC" );
            CPPUNIT_ASSERT( c[ "dd" ] == "DD" );
            CPPUNIT_ASSERT( c[ "ee" ].isEmpty() );

            CPPUNIT_ASSERT( c[ 0 ] == "AA" );
            CPPUNIT_ASSERT( c[ 1 ] == "BB" );
            CPPUNIT_ASSERT( c[ 2 ] == "CC" );
            CPPUNIT_ASSERT( c[ 3 ] == "DD" );
            CPPUNIT_ASSERT( c[ 4 ].isEmpty() );

        }

        void testLTMapRandom()
        {
            LTMap <QString> c;

            QString aa = "AA", bb = "BB", cc = "CC", dd = "DD";

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            // random insertion only
            c.insert( "aa", aa, 0 );
            c.insert( "dd", dd, 1 );
            c.insert( "bb", bb, 1 );
            c.insert( "cc", cc, 2 );

            CPPUNIT_ASSERT( c.contains( "aa" ) == true );
            CPPUNIT_ASSERT( c.contains( "bb" ) == true );
            CPPUNIT_ASSERT( c.contains( "cc" ) == true );
            CPPUNIT_ASSERT( c.contains( "dd" ) == true );
            CPPUNIT_ASSERT( c.contains( "ee" ) == false );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            CPPUNIT_ASSERT( c.keyAt( 10 ) == QString::null );
            CPPUNIT_ASSERT( c.keyAt( 0 ) == "aa" );
            CPPUNIT_ASSERT( c.keyAt( 1 ) == "bb" );
            CPPUNIT_ASSERT( c.keyAt( 2 ) == "cc" );
            CPPUNIT_ASSERT( c.keyAt( 3 ) == "dd" );

            CPPUNIT_ASSERT( c[ "aa" ] == "AA" );
            CPPUNIT_ASSERT( c[ "bb" ] == "BB" );
            CPPUNIT_ASSERT( c[ "cc" ] == "CC" );
            CPPUNIT_ASSERT( c[ "dd" ] == "DD" );
            CPPUNIT_ASSERT( c[ "ee" ].isEmpty() );

            CPPUNIT_ASSERT( c[ 0 ] == "AA" );
            CPPUNIT_ASSERT( c[ 1 ] == "BB" );
            CPPUNIT_ASSERT( c[ 2 ] == "CC" );
            CPPUNIT_ASSERT( c[ 3 ] == "DD" );
            CPPUNIT_ASSERT( c[ 4 ].isEmpty() );

        }

        void testLTMapCurrentAndRemoveByKey()
        {
            LTMap <QString> c;

            QString aa = "AA", bb = "BB", cc = "CC", dd = "DD";

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", aa );
            c.insert( "bb", bb );
            c.insert( "cc", cc );
            c.insert( "dd", dd );

            // now check current pointer
            // invalid key should not move the pointer
            c.setCurrentKey( "ff" );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == "AA" );

            c.setCurrentKey( "dd" );

            CPPUNIT_ASSERT( c.currentIndex() == 3 );
            CPPUNIT_ASSERT( c.currentKey() == "dd" );
            CPPUNIT_ASSERT( c.current() == "DD" );

            c.setCurrentKey( "bb" );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "bb" );
            CPPUNIT_ASSERT( c.current() == "BB" );

            // now remove the item at the end, current should not change
            c.remove( "dd" );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "bb" );
            CPPUNIT_ASSERT( c.current() == "BB" );

            // now remove the current item, current should move to cc
            c.remove( "bb" );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "cc" );
            CPPUNIT_ASSERT( c.current() == "CC" );

            // now remove the current item, current should move to aa
            c.remove( "cc" );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == "AA" );

            // now remove the current item, current should move to -1 and
            // the dictionary should be empty
            c.remove( "aa" );

            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );
        }

        void testLTMapCurrentAndRemoveByIdx()
        {
            LTMap <QString> c;

            QString aa = "AA", bb = "BB", cc = "CC", dd = "DD";

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", aa );
            c.insert( "bb", bb );
            c.insert( "cc", cc );
            c.insert( "dd", dd );

            // now check current pointer
            // invalid should not move the pointer
            c.setCurrentIndex( 6 );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == "AA" );

            c.setCurrentIndex( 3 );

            CPPUNIT_ASSERT( c.currentIndex() == 3 );
            CPPUNIT_ASSERT( c.currentKey() == "dd" );
            CPPUNIT_ASSERT( c.current() == "DD" );

            c.setCurrentIndex( 1 );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "bb" );
            CPPUNIT_ASSERT( c.current() == "BB" );

            // now remove the item at the end, current should not change
            c.remove( 3 );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "bb" );
            CPPUNIT_ASSERT( c.current() == "BB" );

            // now remove the current item, current should move to cc
            c.remove( 1 );

            CPPUNIT_ASSERT( c.currentIndex() == 1 );
            CPPUNIT_ASSERT( c.currentKey() == "cc" );
            CPPUNIT_ASSERT( c.current() == "CC" );

            // now remove the current item, current should move to aa
            c.remove( 1 );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == "AA" );

            // now remove the current item, current should move to -1 and
            // the dictionary should be empty
            c.remove( 0 );

            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );
        }

        void testLTMapMoveByKey()
        {
            LTMap <QString> c;

            QString aa = "AA", bb = "BB", cc = "CC", dd = "DD";

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", aa );
            c.insert( "bb", bb );
            c.insert( "cc", cc );
            c.insert( "dd", dd );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "cc", 2 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "aa", -1 );

            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "aa", 0 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "bb", 2 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( "aa", 2 );

            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );
        }


        void testLTMapMoveByIdx()
        {
            LTMap <QString> c;

            QString aa = "AA", bb = "BB", cc = "CC", dd = "DD";

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", aa );
            c.insert( "bb", bb );
            c.insert( "cc", cc );
            c.insert( "dd", dd );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 2, 2 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 0, -1 );

            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 3, 0 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 1, 2 );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.move( 0, 2 );

            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );
        }

        void testLTMapClear()
        {
            LTMap <QString> c;

            QString aa = "AA", bb = "BB", cc = "CC", dd = "DD";

            CPPUNIT_ASSERT( c.isEmpty() == true );
            CPPUNIT_ASSERT( c.count() == 0 );
            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );

            c.insert( "aa", aa );
            c.insert( "bb", bb );
            c.insert( "cc", cc );
            c.insert( "dd", dd );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == 1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == 2 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == 3 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            c.clear();

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            CPPUNIT_ASSERT( c.currentIndex() == -1 );
            CPPUNIT_ASSERT( c.currentKey() == QString::null );
            CPPUNIT_ASSERT( c.isEmpty() );

            c.insert( "aa", aa );

            CPPUNIT_ASSERT( c.indexOf( "aa" ) == 0 );
            CPPUNIT_ASSERT( c.indexOf( "bb" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "cc" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "dd" ) == -1 );
            CPPUNIT_ASSERT( c.indexOf( "ee" ) == -1 );

            CPPUNIT_ASSERT( c.currentIndex() == 0 );
            CPPUNIT_ASSERT( c.currentKey() == "aa" );
            CPPUNIT_ASSERT( c.current() == "AA" );
            CPPUNIT_ASSERT( ! c.isEmpty() );
        }

        void testLTDictRemove()
        {
            DummyRefCount* x = new DummyRefCount;
            CPPUNIT_ASSERT(x);
            CPPUNIT_ASSERT(x->ref == 1);

            {
                LTDict<DummyRefCount> c;

                c.insert("x", x);
                CPPUNIT_ASSERT(x->ref == 1);

                c.remove("x");
                CPPUNIT_ASSERT(x->ref == 1);

                c.setAutoDelete(true);
            }
            CPPUNIT_ASSERT_MESSAGE(
                    QString("x->ref != 1; x->ref == %1").arg(x->ref).latin1(),
                    x->ref == 1);
        }

        void testLTMapRemove()
        {
            DummyRefCountPtr x = DummyRefCountPtr(new DummyRefCount);
            CPPUNIT_ASSERT(!x.isNull());
            CPPUNIT_ASSERT(x.useCount() == 1);

            {
                LTMap<DummyRefCountPtr> c;

                c.insert("x", x);
                CPPUNIT_ASSERT(x.useCount() == 2);

                c.remove("x");
            }
            CPPUNIT_ASSERT_MESSAGE(
                    QString("x.useCount() != 1; x.useCount() == %1")
                    .arg(x.useCount()).latin1(),
                    x.useCount() == 1);
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestLTMapNDict );
// end of custom tests

// vim: set et ts=4 sw=4 tw=76:
// $Id: TestMapDict.cpp,v 1.5 2004/12/01 16:25:01 mman Exp $
