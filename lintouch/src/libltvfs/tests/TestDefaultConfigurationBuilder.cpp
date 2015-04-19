// $Id: TestDefaultConfigurationBuilder.cpp,v 1.2 2003/11/15 18:34:29 hynek Exp $
//
//   FILE: TestDefaultConfigurationBuilder.cpp -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 19 June 2002
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

#include "lt/configuration/DefaultConfigurationBuilder.hh"

using namespace lt;

extern QString srcdir;
extern QString builddir;

class TestDefaultConfigurationBuilder : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE( TestDefaultConfigurationBuilder );

    CPPUNIT_TEST( testBuildFromFile );
    CPPUNIT_TEST( testGetChild );
    CPPUNIT_TEST( testGetChildren );
    CPPUNIT_TEST( testNamespaces );
    CPPUNIT_TEST( testReadOnly );
    CPPUNIT_TEST( testReadWrite );

    CPPUNIT_TEST_SUITE_END();

    public:

    void setUp() {
        // fill in
    }

    void tearDown() {
        // fill in
    }

    void testBuildFromFile() {
        DefaultConfigurationBuilder builder;
        ConfigurationPtr cg;
        ConfigurationPtr cb;
        QString cge, cbe;
        int cgl, cgc, cbl, cbc;

        //construct good configuration
        cg = builder.buildFromFile( 
                srcdir + "/tests/test_config.xml", cge, cgl, cgc );
        //construct bad configuration
        cb = builder.buildFromFile( 
                srcdir + "/tests/test_config_buggy.xml", cbe, cbl, cbc );

        // firstcheck that they're pared (not parsed)
        CPPUNIT_ASSERT_MESSAGE( 
                QString("%1 at line %2 column %3")
                .arg( cge ).arg( cgl ).arg( cgc ).latin1(), 
                cg );
        CPPUNIT_ASSERT( !cb );
        CPPUNIT_ASSERT( cb.isNull() );

        // check the names
        CPPUNIT_ASSERT( cg->getName() == "test_config" );

    }

    void testGetChild() {
        DefaultConfigurationBuilder builder;
        ConfigurationPtr c;
        QString ce;
        int cl, cc;

        //construct good configuration
        c = builder.buildFromFile( 
                srcdir + "/tests/test_config.xml", ce, cl, cc );
        CPPUNIT_ASSERT_MESSAGE( 
                QString("%1 at line %2 column %3")
                .arg( ce ).arg( cl ).arg( cc ).latin1(), 
                c );

        const ConfigurationPtr c1 = c->getChild( "parameters" );
        CPPUNIT_ASSERT( c1 );
        CPPUNIT_ASSERT( c1->getName() == "parameters" );

        CPPUNIT_ASSERT( 
                c->getChild( "paramS" )->getValue() == "value" );
        CPPUNIT_ASSERT( 
                c->getChild( "paramI" )->getValueAsInt() == 0x200 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 
                c->getChild( "paramF" )->getValueAsFloat(), 
                10.1, 0.001 );
        CPPUNIT_ASSERT( 
                c->getChild( "paramB" )->getValueAsBool() == true );
    }

    void testGetChildren() {
        DefaultConfigurationBuilder builder;
        ConfigurationPtr c;
        QString ce;
        int cl, cc;

        //construct good configuration
        c = builder.buildFromFile( 
                srcdir + "/tests/test_config.xml", ce, cl, cc );
        CPPUNIT_ASSERT_MESSAGE( 
                QString("%1 at line %2 column %3")
                .arg( ce ).arg( cl ).arg( cc ).latin1(), 
                c );

        const ConfigurationPtr c1 = c->getChild( "parameters" );
        CPPUNIT_ASSERT( c1 );

        ConfigurationList vect = 
            c1->getChildren( "parameter" );

        CPPUNIT_ASSERT( vect.count() == 4 );
        CPPUNIT_ASSERT( vect [ 0 ] );
        CPPUNIT_ASSERT( vect [ 1 ] );
        CPPUNIT_ASSERT( vect [ 2 ] );
        CPPUNIT_ASSERT( vect [ 3 ] );
        CPPUNIT_ASSERT( 
                vect[ 0 ]->getAttribute( "value" ) == "value" );
        CPPUNIT_ASSERT( 
                vect[ 1 ]->getAttributeAsInt( "value" ) == 0x200 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 
                vect[ 2 ]->getAttributeAsFloat( "value" ), 
                10.1, 0.001 );
        CPPUNIT_ASSERT( 
                vect[ 3 ]->getAttributeAsBool( "value" ) == true );
    }

    void testNamespaces() {
        DefaultConfigurationBuilder builder;
        ConfigurationPtr c;
        QString ce;
        int cl, cc;

        //construct good configuration
        c = builder.buildFromFile( 
                srcdir + "/tests/test_config.xml", ce, cl, cc );
        CPPUNIT_ASSERT_MESSAGE( 
                QString("%1 at line %2 column %3")
                .arg( ce ).arg( cl ).arg( cc ).latin1(), 
                c );

        //test namespace handling
        const ConfigurationPtr sw = c->getChild( "xxx" );
        CPPUNIT_ASSERT( sw );

        CPPUNIT_ASSERT( sw->getName() == "xxx" );
        CPPUNIT_ASSERT( sw->getNamespace() == "http://swac.cz/Test" );
    }

    void testReadOnly() {
        DefaultConfigurationBuilder builder;
        ConfigurationPtr c_ro;
        QString ce;
        int cl, cc;

        //construct good readonly configuration
        c_ro = builder.buildFromFile( 
                srcdir + "/tests/test_config.xml", ce, cl, cc, true );
        CPPUNIT_ASSERT_MESSAGE( 
                QString("%1 at line %2 column %3")
                .arg( ce ).arg( cl ).arg( cc ).latin1(), 
                c_ro );

        //test readonlyness
        CPPUNIT_ASSERT( c_ro->setName ( "name" ) != true );
        CPPUNIT_ASSERT( c_ro->setValue ( "value" ) != true );
        CPPUNIT_ASSERT( c_ro->setLocation ( "location" ) != true );
        CPPUNIT_ASSERT( c_ro->setPrefix ( "prefix" ) != true );
        CPPUNIT_ASSERT( c_ro->setNamespace ( "namespace" ) != true );
    }

    void testReadWrite() {
        DefaultConfigurationBuilder builder;
        ConfigurationPtr c_rw;
        QString ce;
        int cl, cc;

        //construct good writable configuration
        c_rw = builder.buildFromFile( 
                srcdir + "/tests/test_config.xml", ce, cl, cc, false );
        CPPUNIT_ASSERT_MESSAGE( 
                QString("%1 at line %2 column %3")
                .arg( ce ).arg( cl ).arg( cc ).latin1(), 
                c_rw );

        //test writability
        CPPUNIT_ASSERT( c_rw->setName ( "name" ) != false );
        CPPUNIT_ASSERT( c_rw->setValue ( "value" ) != false );
        CPPUNIT_ASSERT( c_rw->setLocation ( "location" ) != false );
        CPPUNIT_ASSERT( c_rw->setPrefix ( "prefix" ) != false );
        CPPUNIT_ASSERT( c_rw->setNamespace ( "namespace" ) != false );

        //test whether modifications completed
        CPPUNIT_ASSERT( c_rw->getName () == "name" );
        CPPUNIT_ASSERT( c_rw->getValue () == "value" );
        CPPUNIT_ASSERT( c_rw->getLocation () == "location" );
        CPPUNIT_ASSERT( c_rw->getPrefix () == "prefix" );
        CPPUNIT_ASSERT( c_rw->getNamespace () == "namespace" );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestDefaultConfigurationBuilder );

// vim: set et ts=4 sw=4 tw=76:
// $Id: TestDefaultConfigurationBuilder.cpp,v 1.2 2003/11/15 18:34:29 hynek Exp $
