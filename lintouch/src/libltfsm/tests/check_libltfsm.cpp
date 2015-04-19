// $Id: check_libltfsm.cpp,v 1.1 2003/12/10 21:03:17 mman Exp $
//
//   FILE: check_libltfsm.cpp -- 
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 7 July 2003
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lt/fsm/fsm.h"

/* eample hook that will always allow transition to be performed */
int hook_allow( void * arg )
{
    return 1;
}

/* eample hook that will always block a transition from being performed */
int hook_deny( void * arg )
{
    return 0;
}

#define CHECK_CURRENT_STATE(state) \
    CPPUNIT_ASSERT( \
            strcmp( (state), lt_fsm_session_state_get( fsms ) ) == 0) \

class TestFSM : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE( TestFSM );

    CPPUNIT_TEST( test );

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

        void test()
        {
            apr_pool_t * pool;
            lt_fsm_t * fsm;
            lt_fsm_session_t * fsms;

            apr_pool_create( &pool, NULL );

            /* create diagram */
            CPPUNIT_ASSERT( lt_fsm_create( &fsm, pool ) ==
                    LT_FSM_SUCCESS);

            CPPUNIT_ASSERT( fsm != NULL);

            /* create some states */
            CPPUNIT_ASSERT( lt_fsm_state_create( fsm, "a", pool ) ==
                    LT_FSM_SUCCESS);
            CPPUNIT_ASSERT( lt_fsm_state_create( fsm, "b", pool ) ==
                    LT_FSM_SUCCESS);
            CPPUNIT_ASSERT( lt_fsm_state_create( fsm, "c", pool ) ==
                    LT_FSM_SUCCESS);

            /* create some transitions */
            CPPUNIT_ASSERT( lt_fsm_transition_create( 
                        fsm, "a", "b", 255, "ab", 3, pool ) ==
                    LT_FSM_SUCCESS);
            CPPUNIT_ASSERT( lt_fsm_transition_create( 
                        fsm, "b", "c", 255, "bc", 3, pool ) ==
                    LT_FSM_SUCCESS);
            CPPUNIT_ASSERT( lt_fsm_transition_create( 
                        fsm, "c", "a", 255, "ca", 3, pool ) ==
                    LT_FSM_SUCCESS);
            CPPUNIT_ASSERT( lt_fsm_transition_create( 
                        fsm, "c", "b", 255, "cb", 3, pool ) ==
                    LT_FSM_SUCCESS);

            /* install blocker for transition cb */
            CPPUNIT_ASSERT( lt_fsm_transition_prehook_register( 
                        fsm, "c", "cb", 3, &hook_deny ) == LT_FSM_SUCCESS);

            /* install allower for transition ca */
            CPPUNIT_ASSERT( lt_fsm_transition_prehook_register( 
                        fsm, "c", "ca", 3, &hook_allow ) == LT_FSM_SUCCESS);

            /* create diagram session */
            CPPUNIT_ASSERT( lt_fsm_session_create( &fsms, fsm, pool )
                    == LT_FSM_SUCCESS);

            /* set initial state */
            CPPUNIT_ASSERT( lt_fsm_session_state_set( fsms, "a" ) ==
                    LT_FSM_SUCCESS);


            /* try to perform some transitions and see what the state is */
            /* FIXME: a->c no, a->b yes, b->a no, b->c yes, c->b no, c->a yes */
            CPPUNIT_ASSERT( lt_fsm_session_process(
                        fsms, "ac", 3, NULL ) == 
                    LT_FSM_ENOSUCHTRANSITION);
            CHECK_CURRENT_STATE( "a" );

            CPPUNIT_ASSERT( lt_fsm_session_process(
                        fsms, "ab", 3, NULL ) == 
                    LT_FSM_SUCCESS);
            CHECK_CURRENT_STATE( "b" );

            CPPUNIT_ASSERT( lt_fsm_session_process(
                        fsms, "ba", 3, NULL ) == 
                    LT_FSM_ENOSUCHTRANSITION);
            CHECK_CURRENT_STATE( "b" );

            CPPUNIT_ASSERT( lt_fsm_session_process(
                        fsms, "bc", 3, NULL ) == 
                    LT_FSM_SUCCESS);
            CHECK_CURRENT_STATE( "c" );

            CPPUNIT_ASSERT( lt_fsm_session_process(
                        fsms, "cb", 3, NULL ) == 
                    LT_FSM_ETRANSITIONBLOCKED);
            CHECK_CURRENT_STATE( "c" );

            CPPUNIT_ASSERT( lt_fsm_session_process(
                        fsms, "ca", 3, NULL ) == 
                    LT_FSM_SUCCESS);
            CHECK_CURRENT_STATE( "a" );

            apr_pool_destroy( pool );

        }

};


CPPUNIT_TEST_SUITE_REGISTRATION( TestFSM );
// end of custom tests

// standard test suite
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main( int argc, char const * const argv [], char const * const env [] )
{
    if( APR_SUCCESS != 
                apr_app_initialize( &argc, &argv, &env ) ) {
        fprintf( stderr, "Unable to initialize APR Library\n" );
        return -1;
    }
    atexit( apr_terminate );

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = 
        CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );

    bool wasThereError = runner.run( "", false );

    // shell expects 0 if successfull
    return !wasThereError;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_libltfsm.cpp,v 1.1 2003/12/10 21:03:17 mman Exp $
