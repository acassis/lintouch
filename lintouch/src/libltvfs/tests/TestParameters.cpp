// $Id: TestParameters.cpp,v 1.1 2003/11/11 15:32:49 mman Exp $
//
//   FILE: TestParameters.cpp -- 
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
#include "lt/parameters/Parameters.hh"

using namespace lt;

extern QString srcdir;
extern QString builddir;

class TestParameters : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE( TestParameters );

    CPPUNIT_TEST( test_setParameter );
    CPPUNIT_TEST( test_removeParameter );
    CPPUNIT_TEST( test_getParameterNames );
    CPPUNIT_TEST( test_isParameter );
    CPPUNIT_TEST( test_getParameter );
    CPPUNIT_TEST( test_getParameterAsInt );
    CPPUNIT_TEST( test_getParameterAsUInt );
    CPPUNIT_TEST( test_getParameterAsLong );
    CPPUNIT_TEST( test_getParameterAsULong );
    CPPUNIT_TEST( test_getParameterAsFloat );
    CPPUNIT_TEST( test_getParameterAsBool );
    CPPUNIT_TEST( test_merge );
    CPPUNIT_TEST( test_fromConfiguration );
    CPPUNIT_TEST( test_makeReadOnly );

    CPPUNIT_TEST_SUITE_END();

    public:

    void setUp() {
        // fill in
    }

    void tearDown() {
        // fill in
    }

    void test_setParameter() {
        Parameters p;

        // set parameter to something meaningful
        p.setParameter( "parameter", "value" );
        CPPUNIT_ASSERT( p.getParameter( "parameter" ) == "value" );

        // clear out parameter
        p.setParameter( "parameter", "" );
        CPPUNIT_ASSERT( p.getParameter( "parameter" ) == "" );
    }

    void test_removeParameter() {
        Parameters p;

        // set parameter to something meaningful
        p.setParameter( "parameter", "value" );
        CPPUNIT_ASSERT( p.getParameter( "parameter" ) == "value" );

        // clear out parameter
        p.removeParameter( "parameter" );
        CPPUNIT_ASSERT( p.getParameter( "parameter" ) == "" );
    }

    void test_getParameterNames() {
        // fill in
    }

    void test_isParameter() {
        Parameters p;

        // set parameter to something meaningful
        p.setParameter( "parameter", "value" );

        CPPUNIT_ASSERT( p.isParameter( "parameter" ) == true );
        CPPUNIT_ASSERT( p.isParameter( "parameter1" ) == false );
    }

    void test_getParameter() {
        Parameters p;

        // set parameter to something meaningful
        p.setParameter( "parameter", "value" );
        CPPUNIT_ASSERT( p.getParameter( "parameter" ) == "value" );

        // clear out parameter
        p.setParameter( "parameter", "" );
        CPPUNIT_ASSERT( p.getParameter( "parameter" ) == "" );
    }

    void test_getParameterAsInt() {
        Parameters p;

        p.setParameter( "parameter", "100" );
        CPPUNIT_ASSERT( p.getParameterAsInt( "parameter" ) == 100 );

        p.setParameter( "parameter", "-100" );
        CPPUNIT_ASSERT( p.getParameterAsInt( "parameter" ) == -100 );

        p.setParameter( "parameter", "0x100" );
        CPPUNIT_ASSERT( p.getParameterAsInt( "parameter" ) == 0x100 );

        p.setParameter( "parameter", "0o100" );
        CPPUNIT_ASSERT( p.getParameterAsInt( "parameter" ) == 0100 );

        p.setParameter( "parameter", "0b1010" );
        CPPUNIT_ASSERT( p.getParameterAsInt( "parameter" ) == 10 );
    }

    void test_getParameterAsUInt() {
        Parameters p;

        p.setParameter( "parameter", "100" );
        CPPUNIT_ASSERT( p.getParameterAsUInt( "parameter" ) == 100 );

        p.setParameter( "parameter", "-100" );
        CPPUNIT_ASSERT( p.getParameterAsUInt( "parameter" ) == 0 );

        p.setParameter( "parameter", "0x100" );
        CPPUNIT_ASSERT( p.getParameterAsUInt( "parameter" ) == 0x100 );

        p.setParameter( "parameter", "0o100" );
        CPPUNIT_ASSERT( p.getParameterAsUInt( "parameter" ) == 0100 );

        p.setParameter( "parameter", "0b1010" );
        CPPUNIT_ASSERT( p.getParameterAsUInt( "parameter" ) == 10 );
    }

    void test_getParameterAsLong() {
        Parameters p;

        p.setParameter( "parameter", "100" );
        CPPUNIT_ASSERT( p.getParameterAsLong( "parameter" ) == 100 );

        p.setParameter( "parameter", "-100" );
        CPPUNIT_ASSERT( p.getParameterAsLong( "parameter" ) == -100 );

        p.setParameter( "parameter", "0x100" );
        CPPUNIT_ASSERT( p.getParameterAsLong( "parameter" ) == 0x100 );

        p.setParameter( "parameter", "0o100" );
        CPPUNIT_ASSERT( p.getParameterAsLong( "parameter" ) == 0100 );

        p.setParameter( "parameter", "0b1010" );
        CPPUNIT_ASSERT( p.getParameterAsLong( "parameter" ) == 10 );
    }

    void test_getParameterAsULong() {
        Parameters p;

        p.setParameter( "parameter", "100" );
        CPPUNIT_ASSERT( p.getParameterAsULong( "parameter" ) == 100 );

        p.setParameter( "parameter", "-100" );
        CPPUNIT_ASSERT( p.getParameterAsULong( "parameter" ) == 0 );

        p.setParameter( "parameter", "0x100" );
        CPPUNIT_ASSERT( p.getParameterAsULong( "parameter" ) == 0x100 );

        p.setParameter( "parameter", "0o100" );
        CPPUNIT_ASSERT( p.getParameterAsULong( "parameter" ) == 0100 );

        p.setParameter( "parameter", "0b1010" );
        CPPUNIT_ASSERT( p.getParameterAsULong( "parameter" ) == 10 );
    }

    void test_getParameterAsFloat() {
        Parameters p;

        p.setParameter( "parameter", "100.5" );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 
                p.getParameterAsFloat( "parameter" ), 100.5, 0.001 );

        p.setParameter( "parameter", "-100.5" );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 
                p.getParameterAsFloat( "parameter" ), -100.5, 0.001 );
    }

    void test_getParameterAsBool() {
        Parameters p;

        p.setParameter( "parameter", "" );
        CPPUNIT_ASSERT( p.getParameterAsBool( "parameter" ) == false );

        p.setParameter( "parameter", "11" );
        CPPUNIT_ASSERT( p.getParameterAsBool( "parameter" ) == false );

        p.setParameter( "parameter", "faLse" );
        CPPUNIT_ASSERT( p.getParameterAsBool( "parameter" ) == false );

        p.setParameter( "parameter", "0" );
        CPPUNIT_ASSERT( p.getParameterAsBool( "parameter" ) == false );

        p.setParameter( "parameter", "nO" );
        CPPUNIT_ASSERT( p.getParameterAsBool( "parameter" ) == false );

        p.setParameter( "parameter", "yEss" );
        CPPUNIT_ASSERT( p.getParameterAsBool( "parameter" ) == false );

        p.setParameter( "parameter", "yeS" );
        CPPUNIT_ASSERT( p.getParameterAsBool( "parameter" ) == true );

        p.setParameter( "parameter", "tRue" );
        CPPUNIT_ASSERT( p.getParameterAsBool( "parameter" ) == true );

        p.setParameter( "parameter", "1" );
        CPPUNIT_ASSERT( p.getParameterAsBool( "parameter" ) == true );
    }

    void test_merge() {
        ParametersPtr p(new Parameters());
        const ParametersPtr other(new Parameters());

        p->setParameter( "parameter1", "value55" );
        p->setParameter( "parameter2", "value55" );

        other->setParameter( "parameter1", "value1" );
        other->setParameter( "parameter2", "value2" );
        other->setParameter( "parameter3", "value3" );

        p->merge (other);

        CPPUNIT_ASSERT( p->getParameter( "parameter" ) == "" );
        CPPUNIT_ASSERT( p->getParameter( "parameter1" ) == "value1" );
        CPPUNIT_ASSERT( p->getParameter( "parameter2" ) == "value2" );
        CPPUNIT_ASSERT( p->getParameter( "parameter3" ) == "value3" );
    }

    void test_fromConfiguration() {
        DefaultConfigurationBuilder b;
        ConfigurationPtr c;
        ConfigurationPtr cp;
        ParametersPtr p;

        QString error;
        int line, column;

        c = b.buildFromFile( srcdir + "/tests/test_config.xml", 
                error, line, column );
        CPPUNIT_ASSERT_MESSAGE( 
                QString("%1 at line %2 column %3")
                .arg( error ).arg( line ).arg( column ).latin1(), 
                c );
        cp = c->getChild( "parameters" );
        p = Parameters::fromConfiguration( cp );

        CPPUNIT_ASSERT( p );
        if( !p ) return;

        CPPUNIT_ASSERT( p->getParameter( "paramS" ) == "value" );
        CPPUNIT_ASSERT( p->getParameterAsInt( "paramI" ) == 0x200 );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 
                p->getParameterAsFloat( "paramF" ), 
                10.10, 0.001 );
        CPPUNIT_ASSERT( p->getParameterAsBool( "paramB" ) == true );
    }

    void test_makeReadOnly() {
        Parameters p;

        // set parameter to something meaningful
        p.setParameter( "parameter", "value" );
        p.makeReadOnly();

        // try to clear out parameter
        p.removeParameter( "parameter" );
        CPPUNIT_ASSERT( p.getParameter( "parameter" ) == "value" );

        // try to replace out parameter
        p.setParameter( "parameter", "" );
        CPPUNIT_ASSERT( p.getParameter( "parameter" ) == "value" );

        // try to replace parameter
        p.setParameter( "parameter", "anothervalue" );
        CPPUNIT_ASSERT( p.getParameter( "parameter" ) == "value" );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestParameters );

// vim: set et ts=4 sw=4 tw=76:
// $Id: TestParameters.cpp,v 1.1 2003/11/11 15:32:49 mman Exp $
