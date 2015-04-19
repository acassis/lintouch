// $Id: TestDefaultConfiguration.cpp,v 1.2 2003/12/06 12:36:29 hynek Exp $
//
//   FILE: TestDefaultConfiguration.cpp -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 15 November 2003
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

#include <cppunit/extensions/HelperMacros.h>

#include "lt/configuration/DefaultConfiguration.hh"

using namespace lt;

class TestDefaultConfiguration : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE( TestDefaultConfiguration );

    CPPUNIT_TEST( test_setget );
    CPPUNIT_TEST( test_childs );

    CPPUNIT_TEST_SUITE_END();

    public:

    void setUp() {
        // fill in
    }

    void tearDown() {
        // fill in
    }

    void test_setget() {
        ConfigurationPtr c = 
            ConfigurationPtr ( new DefaultConfiguration ( "parent" ) );

        c->setName ( "config" );
        CPPUNIT_ASSERT ( c->getName () == "config" );

        c->setValue ( "value" );
        CPPUNIT_ASSERT ( c->getValue () == "value" );

        c->setLocation ( "loc" );
        CPPUNIT_ASSERT ( c->getLocation () == "loc" );

        c->setNamespace ( "nspace" );
        CPPUNIT_ASSERT ( c->getNamespace () == "nspace" );

        c->setPrefix ( "prefix" );
        CPPUNIT_ASSERT ( c->getPrefix () == "prefix" );

        c->setAttribute ( "attr1", "val1" );
        c->setAttribute ( "attr2", "val2" );
        CPPUNIT_ASSERT ( c->getAttributeNames ().count () == 2 );
        CPPUNIT_ASSERT ( c->getAttributeNames ().contains ( "attr1" ) );
        CPPUNIT_ASSERT ( c->getAttributeNames ().contains ( "attr2" ) );
    }

    void test_childs() {
        ConfigurationPtr c = 
            ConfigurationPtr ( new DefaultConfiguration ( "parent" ) );
        ConfigurationPtr ch1 = 
            ConfigurationPtr ( new DefaultConfiguration ( "child1" ) );
        ConfigurationPtr ch2 = 
            ConfigurationPtr ( new DefaultConfiguration ( "child2" ) );

        ConfigurationPtr chch1, chch2;

        CPPUNIT_ASSERT ( c->getChild ( "child1" ).isNull () );
        CPPUNIT_ASSERT ( c->getChild ( "child2" ).isNull () );

        c->addChild ( ch1 );
        c->addChild ( ch2 );

        CPPUNIT_ASSERT ( c->getChildren ().count () == 2 );
        CPPUNIT_ASSERT ( ! c->getChild ( "child1" ).isNull () );
        CPPUNIT_ASSERT ( ! c->getChild ( "child2" ).isNull () );

        chch1 = c->getChild ( "child1" );
        chch2 = c->getChild ( "child2" );

        c->removeChild ( chch1 );
        c->removeChild ( chch2 );

        CPPUNIT_ASSERT ( c->getChildren ().count () == 0 );
        CPPUNIT_ASSERT ( c->getChild ( "child1" ).isNull () );
        CPPUNIT_ASSERT ( c->getChild ( "child2" ).isNull () );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestDefaultConfiguration );

// vim: set et ts=4 sw=4 tw=76:
// $Id: TestDefaultConfiguration.cpp,v 1.2 2003/12/06 12:36:29 hynek Exp $
