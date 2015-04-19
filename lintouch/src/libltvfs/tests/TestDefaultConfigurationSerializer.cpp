// $Id: TestDefaultConfigurationSerializer.cpp,v 1.1 2003/11/11 15:32:49 mman Exp $
//
//   FILE: TestDefaultConfigurationSerializer.cpp -- 
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
#include "lt/configuration/DefaultConfigurationSerializer.hh"

using namespace lt;

extern QString srcdir;
extern QString builddir;

class TestDefaultConfigurationSerializer : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE( TestDefaultConfigurationSerializer );

    CPPUNIT_TEST( testSerializeToFile );

    CPPUNIT_TEST_SUITE_END();

    public:

    void setUp() {
        // fill in
    }

    void tearDown() {
        // fill in
    }

    void testSerializeToFile () {
        DefaultConfigurationBuilder builder;
        DefaultConfigurationSerializer serializer;
        ConfigurationPtr cg;
        ConfigurationPtr cb;
        QString cge, cbe;
        int cgl, cgc, cbl, cbc;

        //construct good configuration
        cg = builder.buildFromFile( 
                srcdir + "/tests/test_config.xml", 
                cge, cgl, cgc, false );
        CPPUNIT_ASSERT_MESSAGE( 
                QString("%1 at line %2 column %3")
                .arg( cge ).arg( cgl ).arg( cgc ).latin1(), 
                cg );

        CPPUNIT_ASSERT ( 
                cg->setAttribute ( "escaped", "<>&\"'" ) != false );

        CPPUNIT_ASSERT ( serializer.serializeToFile (
                    builddir + "/serialized_config.xml", cg ) );

        //reconstruct good configuration from new file
        cb = builder.buildFromFile( 
                builddir + "/serialized_config.xml", cbe, cbl, cbc );
        CPPUNIT_ASSERT_MESSAGE( 
                QString("%1 at line %2 column %3")
                .arg( cbe ).arg( cbl ).arg( cbc ).latin1(), 
                cb );

        CPPUNIT_ASSERT ( cb->getAttribute ( "escaped" ) == "<>&\"'" );

        // compare both configs
        CPPUNIT_ASSERT ( cg->getName () == cb->getName () );
        // TODO: compare structure
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestDefaultConfigurationSerializer );

// vim: set et ts=4 sw=4 tw=76:
// $Id: TestDefaultConfigurationSerializer.cpp,v 1.1 2003/11/11 15:32:49 mman Exp $
