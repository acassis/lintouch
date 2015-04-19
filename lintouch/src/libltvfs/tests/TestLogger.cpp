// $Id: TestLogger.cpp,v 1.1 2003/11/11 15:32:49 mman Exp $
//
//   FILE: TestLogger.cpp -- 
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

#include "lt/logger/ConsoleLogger.hh"
#include "lt/logger/FileLogger.hh"

using namespace lt;

class TestLogger : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE( TestLogger );

    CPPUNIT_TEST( test_NullLogger );
    CPPUNIT_TEST( test_ConsoleLogger );
    CPPUNIT_TEST( test_FileLogger );

    CPPUNIT_TEST_SUITE_END();

    public:

    void setUp() {
        // fill in
    }

    void tearDown() {
        // fill in
    }

    void test_NullLogger() {
        LoggerPtr l(new Logger ());
        LoggerPtr l1;

        l->debug ( "Logger: debug" );
        l->info ( "Logger: info" );
        l->warn ( "Logger: warn" );
        l->error ( "Logger: error" );
        //l->fatalError ( "NullLogger: fatalError" );
        //
        l1 = l->getChildLogger ( "child" );
        l1->debug ( "Logger: debug" );
        l1->info ( "Logger: info" );
        l1->warn ( "Logger: warn" );
        l1->error ( "Logger: error" );

    }

    void test_ConsoleLogger() {
        LoggerPtr l(new ConsoleLogger ());
        LoggerPtr l1;

        l->debug ( "ConsoleLogger: debug" );
        l->info ( "ConsoleLogger: info" );
        l->warn ( "ConsoleLogger: warn" );
        l->error ( "ConsoleLogger: error" );
        //l->fatalError ( "ConsoleLogger: fatalError" );
        //
        l1 = l->getChildLogger ( "child" );
        l1->debug ( "ConsoleLogger: debug" );
        l1->info ( "ConsoleLogger: info" );
        l1->warn ( "ConsoleLogger: warn" );
        l1->error ( "ConsoleLogger: error" );

    }

    void test_FileLogger() {
        LoggerPtr l(new FileLogger ("build/test_filelogger.log"));
        LoggerPtr l1, l3;

        l->debug ( "FileLogger: debug" );
        l->info ( "FileLogger: info" );
        l->warn ( "FileLogger: warn" );
        l->error ( "FileLogger: error" );
        //
        l1 = l->getChildLogger ( "child" );
        l1->debug ( "FileLogger: debug" );
        l1->info ( "FileLogger: info" );
        l1->warn ( "FileLogger: warn" );
        l1->error ( "FileLogger: error" );
        //
        l3 = l1;
        l->debug ( "FileLogger: debug3" );
        l->info ( "FileLogger: info3" );
        l->warn ( "FileLogger: warn3" );
        l->error ( "FileLogger: error3" );

        // should fail
        LoggerPtr l2(new FileLogger ("/test_filelogger.log"));
        l2->debug ( "FileLogger: debug" );
        l2->info ( "FileLogger: info" );
        l2->warn ( "FileLogger: warn" );
        l2->error ( "FileLogger: error" );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestLogger );

// vim: set et ts=4 sw=4 tw=76:
// $Id: TestLogger.cpp,v 1.1 2003/11/11 15:32:49 mman Exp $
