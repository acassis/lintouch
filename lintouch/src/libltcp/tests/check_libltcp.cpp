// $Id: check_libltcp.cpp,v 1.9 2004/06/22 13:33:53 mman Exp $
//
//   FILE: check_libltcp.cpp --
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 16 June 2003
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

#include <apr_strings.h>
#include <apr_time.h>

#include "lt/cp/cp-server.h"
#include "lt/cp/cp-client.h"
#include "lt/cp/http.h"
#include "cp-private.h"

apr_pool_t * pool = NULL;

char * srcdir = NULL;
char * builddir = NULL;

class TestCP : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE ( TestCP );

    CPPUNIT_TEST ( test_struct_sizes );
    CPPUNIT_TEST ( test_create );
    CPPUNIT_TEST ( test_connect_disconnect );
    CPPUNIT_TEST ( test_view_selection );
    CPPUNIT_TEST ( test_variables );

    CPPUNIT_TEST ( test_http );

    CPPUNIT_TEST_SUITE_END ();

    public:

        void setUp () {
            // fill in
        }

        void tearDown () {
            // fill in
        }

        void test_struct_sizes() {
            //these are hardcoded sizes to test struct packing works in the
            //compiler
            CPPUNIT_ASSERT( sizeof( lt_cp_message_header_t ) ==  18 );
            CPPUNIT_ASSERT( sizeof( lt_cp_message_payload_header_t ) ==  6 );
        }

        void test_create() {
            lt_cp_session_t * server = NULL, * client = NULL;

            CPPUNIT_ASSERT( lt_cp_session_create(
                        &server, LT_CP_SERVER, NULL ) == LT_CP_SUCCESS );
            CPPUNIT_ASSERT( lt_cp_session_create(
                        &client, LT_CP_CLIENT, NULL ) == LT_CP_SUCCESS );

            CPPUNIT_ASSERT( server != NULL );
            CPPUNIT_ASSERT( client != NULL );

            CPPUNIT_ASSERT( lt_cp_session_state_get( client ) ==
                    LT_CP_STATE_CONNECTION_IN_PROGRESS );
            CPPUNIT_ASSERT( lt_cp_session_state_get( server ) ==
                    LT_CP_STATE_CONNECTION_IN_PROGRESS );
            CPPUNIT_ASSERT( lt_cp_session_is_inprogress( client ) );
            CPPUNIT_ASSERT( lt_cp_session_is_inprogress( server ) );

            CPPUNIT_ASSERT( !lt_cp_session_is_alive( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_alive( server ) );

            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        server ) );

            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( server ) );
        }

        void test_connect_disconnect() {
            lt_cp_session_t * server = NULL, * client = NULL;
            void * buffer; apr_size_t bufferlen = 0;

            CPPUNIT_ASSERT( lt_cp_session_create(
                        &server, LT_CP_SERVER, NULL ) == LT_CP_SUCCESS );
            CPPUNIT_ASSERT( lt_cp_session_create(
                        &client, LT_CP_CLIENT, NULL ) == LT_CP_SUCCESS );

            CPPUNIT_ASSERT( server != NULL );
            CPPUNIT_ASSERT( client != NULL );

            /* set project url */
            lt_cp_session_project_url_set( server, "http://testurl" );
            CPPUNIT_ASSERT( strcmp( "http://testurl",
                        lt_cp_session_project_url_get( server ) ) == 0 );

            /* send connect from the client */
            CPPUNIT_ASSERT( lt_cp_session_send_msg_connect( client )
                        == LT_CP_SUCCESS );

            /* simulate process on the client,
             * while nothing passing as input */
            CPPUNIT_ASSERT( lt_cp_session_process( client,
                        NULL, 0, &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* simulate process on the server,
             * while passing in input received from client */
            CPPUNIT_ASSERT( lt_cp_session_process( server,
                        buffer, bufferlen,
                        &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* simulate process on the client,
             * while passing in input received from the server,
             * we don't want any output yet */
            CPPUNIT_ASSERT( lt_cp_session_process( client,
                        buffer, bufferlen, NULL, NULL ) == LT_CP_SUCCESS );

            /* now the client should be connected and have correct project
             * url, server should also be connected */
            CPPUNIT_ASSERT( !lt_cp_session_is_inprogress( client ) );
            CPPUNIT_ASSERT( lt_cp_session_is_alive( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( client ) );
            CPPUNIT_ASSERT( strcmp( "http://testurl",
                        lt_cp_session_project_url_get( client ) ) == 0 );

            CPPUNIT_ASSERT( !lt_cp_session_is_inprogress( server ) );
            CPPUNIT_ASSERT( lt_cp_session_is_alive( server ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        server ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( server ) );


            /* send disconnect from the client */
            CPPUNIT_ASSERT( lt_cp_session_send_msg_disconnect( client ) ==
                        LT_CP_SUCCESS );

            /* simulate process on the client,
             * while nothing passing as input */
            CPPUNIT_ASSERT( lt_cp_session_process( client,
                        NULL, 0, &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* simulate process on the server,
             * while passing in input received from client, not expecting
             * any output */
            CPPUNIT_ASSERT( lt_cp_session_process( server,
                        buffer, bufferlen,
                        NULL, NULL ) == LT_CP_SUCCESS );

            /* now both, server and client should be disconnected */
            CPPUNIT_ASSERT( lt_cp_session_state_get( client ) ==
                    LT_CP_STATE_DISCONNECTED );
            CPPUNIT_ASSERT( lt_cp_session_state_get( server ) ==
                    LT_CP_STATE_DISCONNECTED );
            CPPUNIT_ASSERT( !lt_cp_session_is_inprogress( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_inprogress( server ) );

            CPPUNIT_ASSERT( !lt_cp_session_is_alive( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_alive( server ) );

            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        server ) );

            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( server ) );

        }

        void test_view_selection() {
            lt_cp_session_t * server = NULL, * client = NULL;
            void * buffer; apr_size_t bufferlen = 0;

            CPPUNIT_ASSERT( lt_cp_session_create(
                        &server, LT_CP_SERVER, NULL ) == LT_CP_SUCCESS );
            CPPUNIT_ASSERT( lt_cp_session_create(
                        &client, LT_CP_CLIENT, NULL ) == LT_CP_SUCCESS );

            CPPUNIT_ASSERT( server != NULL );
            CPPUNIT_ASSERT( client != NULL );

            /* set project url */
            lt_cp_session_project_url_set( server, "http://testurl" );
            CPPUNIT_ASSERT( strcmp( "http://testurl",
                        lt_cp_session_project_url_get( server ) ) == 0 );

            /* send connect from the client */
            /* **************************** */
            CPPUNIT_ASSERT( lt_cp_session_send_msg_connect( client )
                        == LT_CP_SUCCESS );

            /* simulate process on the client,
             * while nothing passing as input */
            CPPUNIT_ASSERT( lt_cp_session_process( client,
                        NULL, 0, &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* simulate process on the server,
             * while passing in input received from client */
            CPPUNIT_ASSERT( lt_cp_session_process( server,
                        buffer, bufferlen,
                        &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* simulate process on the client,
             * while passing in input received from the server,
             * we don't want any output yet */
            CPPUNIT_ASSERT( lt_cp_session_process( client,
                        buffer, bufferlen, NULL, NULL ) == LT_CP_SUCCESS );

            /* now the client should be connected and have correct project
             * url, server should also be connected */
            CPPUNIT_ASSERT( !lt_cp_session_is_inprogress( client ) );
            CPPUNIT_ASSERT( lt_cp_session_is_alive( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( client ) );
            CPPUNIT_ASSERT( strcmp( "http://testurl",
                        lt_cp_session_project_url_get( client ) ) == 0 );

            CPPUNIT_ASSERT( !lt_cp_session_is_inprogress( server ) );
            CPPUNIT_ASSERT( lt_cp_session_is_alive( server ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        server ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( server ) );

            /* send select view */
            /* **************** */
            CPPUNIT_ASSERT( lt_cp_session_send_msg_select_view( client,
                        "view1" ) == LT_CP_SUCCESS );

            /* simulate process on the client,
             * while nothing passing as input */
            CPPUNIT_ASSERT( lt_cp_session_process( client,
                        NULL, 0, &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* now client should be in view_selection_inprogress */
            CPPUNIT_ASSERT( lt_cp_session_is_view_selection_inprogress(
                        client ) );

            /* simulate process on the server,
             * while passing in input received from client */
            CPPUNIT_ASSERT( lt_cp_session_process( server,
                        buffer, bufferlen,
                        &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* now server should have already selected the view */
            CPPUNIT_ASSERT( lt_cp_session_is_view_selected( server ) );

            /* simulate process on the client,
             * while passing in input received from the server,
             * we don't want any output yet */
            CPPUNIT_ASSERT( lt_cp_session_process( client,
                        buffer, bufferlen, NULL, NULL ) == LT_CP_SUCCESS );

            /* and now client should have also already selected the view */
            CPPUNIT_ASSERT( lt_cp_session_is_view_selected( client ) );

            /* send unselect view */
            /* ****************** */
            CPPUNIT_ASSERT( lt_cp_session_send_msg_unselect_view( client )
                    == LT_CP_SUCCESS );

            /* simulate process on the client,
             * while nothing passing as input */
            CPPUNIT_ASSERT( lt_cp_session_process( client,
                        NULL, 0, &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* now client should be in view_unselected */
            CPPUNIT_ASSERT( lt_cp_session_is_alive(
                        client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected(
                        client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        client ) );

            /* simulate process on the server,
             * while passing in input received from client */
            CPPUNIT_ASSERT( lt_cp_session_process( server,
                        buffer, bufferlen,
                        &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* now server should have already unselected the view */
            CPPUNIT_ASSERT( lt_cp_session_is_alive( server ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( server ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        server ) );


            /* send disconnect from the client */
            /* ******************************* */
            CPPUNIT_ASSERT( lt_cp_session_send_msg_disconnect( client ) ==
                        LT_CP_SUCCESS );

            /* simulate process on the client,
             * while nothing passing as input */
            CPPUNIT_ASSERT( lt_cp_session_process( client,
                        NULL, 0, &buffer, &bufferlen ) == LT_CP_SUCCESS );

            /* simulate process on the server,
             * while passing in input received from client, not expecting
             * any output */
            CPPUNIT_ASSERT( lt_cp_session_process( server,
                        buffer, bufferlen,
                        NULL, NULL ) == LT_CP_SUCCESS );

            /* now both, server and client should be disconnected */
            CPPUNIT_ASSERT( lt_cp_session_state_get( client ) ==
                    LT_CP_STATE_DISCONNECTED );
            CPPUNIT_ASSERT( lt_cp_session_state_get( server ) ==
                    LT_CP_STATE_DISCONNECTED );
            CPPUNIT_ASSERT( !lt_cp_session_is_inprogress( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_inprogress( server ) );

            CPPUNIT_ASSERT( !lt_cp_session_is_alive( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_alive( server ) );

            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selection_inprogress(
                        server ) );

            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( client ) );
            CPPUNIT_ASSERT( !lt_cp_session_is_view_selected( server ) );
        }

        void test_variables() {
            lt_cp_session_t * server = NULL, * client = NULL;
            void * buffer; apr_size_t bufferlen = 0;

            lt_var_set_t * srv_vs1 = NULL, * srv_vs2 = NULL;
            lt_var_set_t * cln_vs1 = NULL, * cln_vs2 = NULL;

            lt_var_t * svs1v1 = NULL, * svs1v2 = NULL;
            lt_var_t * svs2v1 = NULL, * svs2v2 = NULL;
            lt_var_t * cvs1v1 = NULL, * cvs1v2 = NULL;
            lt_var_t * cvs2v1 = NULL, * cvs2v2 = NULL;

            /* create varsets for server and client, with no frontend
             * callbacks and automatically allocated */
            srv_vs1 = lt_var_set_create( 0, NULL );
            srv_vs2 = lt_var_set_create( 0, NULL );
            cln_vs1 = lt_var_set_create( 0, NULL );
            cln_vs2 = lt_var_set_create( 0, NULL );

            /* fill varsets with identical variables on the server and on
             * the client */
            lt_var_set_variable_register( srv_vs1,
                    "variable1",
                    svs1v1 = lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
            lt_var_requested_value_set( svs1v1,
                    lt_io_value_number_create( 75, NULL ) );
            lt_var_real_value_set( svs1v1,
                    lt_io_value_number_create( 75, NULL ) );

            lt_var_set_variable_register( srv_vs1,
                    "variable2",
                    svs1v2 = lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
            lt_var_requested_value_set( svs1v2,
                lt_io_value_number_create( 76, NULL ) );
            lt_var_real_value_set( svs1v2,
                lt_io_value_number_create( 76, NULL ) );


            lt_var_set_variable_register( srv_vs2,
                    "variable1",
                    svs2v1 = lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
            lt_var_requested_value_set( svs2v1,
                lt_io_value_number_create( 77, NULL ) );
            lt_var_real_value_set( svs2v1,
                    lt_io_value_number_create( 77, NULL ) );

            lt_var_set_variable_register( srv_vs2,
                    "variable2",
                    svs2v2 = lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
            lt_var_requested_value_set( svs2v2,
                    lt_io_value_number_create( 78, NULL ) );
            lt_var_real_value_set( svs2v2,
                    lt_io_value_number_create( 78, NULL ) );


            lt_var_set_variable_register( cln_vs1,
                    "variable1",
                    cvs1v1 = lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
            lt_var_requested_value_set( cvs1v1,
                    lt_io_value_number_create( 79, NULL ) );
            lt_var_real_value_set( cvs1v1,
                    lt_io_value_number_create( 79, NULL ) );

            lt_var_set_variable_register( cln_vs1,
                    "variable2",
                    cvs1v2 = lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
            lt_var_requested_value_set( cvs1v2,
                    lt_io_value_number_create( 80, NULL ) );
            lt_var_real_value_set( cvs1v2,
                    lt_io_value_number_create( 80, NULL ) );


            lt_var_set_variable_register( cln_vs2,
                    "variable1",
                    cvs2v1 = lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
            lt_var_requested_value_set( cvs2v1,
                    lt_io_value_number_create( 81, NULL ) );
            lt_var_real_value_set( cvs2v1,
                    lt_io_value_number_create( 81, NULL ) );

            lt_var_set_variable_register( cln_vs2,
                    "variable2",
                    cvs2v2 = lt_var_create( LT_IO_TYPE_NUMBER, 1, NULL ) );
            lt_var_requested_value_set( cvs2v2,
                    lt_io_value_number_create( 82, NULL ) );
            lt_var_real_value_set( cvs2v2,
                    lt_io_value_number_create( 82, NULL ) );


            CPPUNIT_ASSERT( lt_cp_session_create(
                        &server, LT_CP_SERVER, NULL ) == LT_CP_SUCCESS );
            CPPUNIT_ASSERT( lt_cp_session_create(
                        &client, LT_CP_CLIENT, NULL ) == LT_CP_SUCCESS );

            CPPUNIT_ASSERT( server != NULL );
            CPPUNIT_ASSERT( client != NULL );

            /* register variables on the server and the client */
            CPPUNIT_ASSERT( lt_cp_session_variables_register(
                        server, "connection1", srv_vs1 ) == LT_CP_SUCCESS );
            CPPUNIT_ASSERT( lt_cp_session_variables_register(
                        server, "connection2", srv_vs2 ) == LT_CP_SUCCESS );

            CPPUNIT_ASSERT( lt_cp_session_variables_register(
                        client, "connection1", cln_vs1 ) == LT_CP_SUCCESS );
            CPPUNIT_ASSERT( lt_cp_session_variables_register(
                        client, "connection2", cln_vs2 ) == LT_CP_SUCCESS );

            /* set project url */
            lt_cp_session_project_url_set( server, "http://testurl" );
            /* send connect */
            lt_cp_session_send_msg_connect( client );

            /* process */
            lt_cp_session_process( client, NULL, 0, &buffer, &bufferlen );
            lt_cp_session_process( server, buffer, bufferlen,
                    &buffer, &bufferlen );
            lt_cp_session_process( client, buffer, bufferlen, NULL, NULL );

            /* send select view */
            lt_cp_session_send_msg_select_view( client, "view1" );

            /* process */
            lt_cp_session_process( client, NULL, 0, &buffer, &bufferlen );
            lt_cp_session_process( server, buffer, bufferlen,
                    &buffer, &bufferlen );
            lt_cp_session_process( client, buffer, bufferlen, NULL, NULL );

            /* now we should have received available_variables */
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( svs1v1 ),
                        lt_var_real_value_get( cvs1v1 ) ) == 1 );
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( svs1v2 ),
                        lt_var_real_value_get( cvs1v2 ) ) == 1 );
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( svs2v1 ),
                        lt_var_real_value_get( cvs2v1 ) ) == 1 );
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( svs2v2 ),
                        lt_var_real_value_get( cvs2v2 ) ) == 1 );

            /* now sync_real on top of the client varsets so that we
             * simulate initial values has been propagated further on the
             * client. */
            lt_var_set_real_values_sync( cln_vs1, -1 );
            lt_var_set_real_values_sync( cln_vs2, -1 );

            /* here test the vars */
            /* ****************** */

            /* real values from server to the client */
            /* ************************************* */

            /* first change real values of some variables in the
             * srv_vs1 and srv_vs2 */
            lt_var_set_real_values_sync( srv_vs1, -1 );
            lt_var_set_real_values_sync( srv_vs2, -1 );
            {
                lt_var_t * v = NULL;
                v = lt_var_set_variable_get( srv_vs1,
                        "variable1" );
                lt_var_real_value_set( v,
                        lt_io_value_number_create( 10, NULL ) );

                v = lt_var_set_variable_get( srv_vs2,
                        "variable1" );
                lt_var_real_value_set( v,
                        lt_io_value_number_create( 20, NULL ) );
            }
            /* then invoke sync_real on top of srv_vs1 and srv_vs2 */
            {
                apr_hash_t * dirty_real = NULL;

                /* before the sync, real values should be dirty */
                dirty_real = lt_var_set_dirty_real_values_get( srv_vs1 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_real ) == 1 );
                dirty_real = lt_var_set_dirty_real_values_get( srv_vs2 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_real ) == 1 );

                lt_var_set_real_values_sync( srv_vs1, -1 );
                lt_var_set_real_values_sync( srv_vs2, -1 );

                /* after the sync, real values should not be dirty till
                 * next change */
                dirty_real = lt_var_set_dirty_real_values_get( srv_vs1 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_real ) == 0 );
                dirty_real = lt_var_set_dirty_real_values_get( srv_vs2 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_real ) == 0 );

            }

            /* then call process on the server passing generated
             * output to the client */
            {
                lt_cp_session_process( server, NULL, 0,
                    &buffer, &bufferlen );
                lt_cp_session_process( client,
                        buffer, bufferlen, NULL, NULL );
            }

            /* then check that the client has got the required real
             * values into cln_vs1 and cln_vs2 */
            {
                apr_hash_t * dirty_real = NULL;
                lt_var_t * v = NULL;
                v = lt_var_set_variable_get( cln_vs1,
                        "variable1" );
                CPPUNIT_ASSERT( lt_var_real_value_is_dirty( v ) );
                CPPUNIT_ASSERT( lt_io_value_equals(
                            lt_var_real_value_get( v ),
                            lt_io_value_number_create( 10, NULL ) ) );

                v = lt_var_set_variable_get( cln_vs1,
                        "variable2" );
                CPPUNIT_ASSERT( !lt_var_real_value_is_dirty( v ) );
                CPPUNIT_ASSERT( lt_io_value_equals(
                            lt_var_real_value_get( v ),
                            lt_io_value_number_create( 76, NULL ) ) );

                v = lt_var_set_variable_get( cln_vs2,
                        "variable1" );
                CPPUNIT_ASSERT( lt_var_real_value_is_dirty( v ) );
                CPPUNIT_ASSERT( lt_io_value_equals(
                            lt_var_real_value_get( v ),
                            lt_io_value_number_create( 20, NULL ) ) );

                v = lt_var_set_variable_get( cln_vs2,
                        "variable2" );
                CPPUNIT_ASSERT( !lt_var_real_value_is_dirty( v ) );
                CPPUNIT_ASSERT( lt_io_value_equals(
                            lt_var_real_value_get( v ),
                            lt_io_value_number_create( 78, NULL ) ) );

                /* now we sync_real on top of client varset indicating that
                 * we had processed the real values */
                lt_var_set_real_values_sync( cln_vs1, -1 );
                lt_var_set_real_values_sync( cln_vs2, -1 );

                /* now real values should not be dirty till
                 * next change */
                dirty_real = lt_var_set_dirty_real_values_get( cln_vs1 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_real ) == 0 );
                dirty_real = lt_var_set_dirty_real_values_get( cln_vs2 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_real ) == 0 );
            }

            /* requested values from the client to the server */
            /* ********************************************** */

            /* first change requested values of some variables in the
             * cln_vs1 and cln_vs2 */
            {
                lt_var_set_requested_values_sync( cln_vs1, -1 );
                lt_var_set_requested_values_sync( cln_vs2, -1 );
                lt_var_t * v = NULL;
                v = lt_var_set_variable_get( cln_vs1,
                        "variable1" );
                lt_var_requested_value_set( v,
                        lt_io_value_number_create( 11, NULL ) );

                v = lt_var_set_variable_get( cln_vs2,
                        "variable1" );
                lt_var_requested_value_set( v,
                        lt_io_value_number_create( 22, NULL ) );
            }
            /* then invoke sync_requested on top of cln_vs1 and cln_vs2 */
            {
                apr_hash_t * dirty_req = NULL;

                /* before the sync, req values should be dirty */
                dirty_req = lt_var_set_dirty_requested_values_get( cln_vs1 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_req ) == 1 );
                dirty_req = lt_var_set_dirty_requested_values_get( cln_vs2 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_req ) == 1 );

                lt_var_set_requested_values_sync( cln_vs1, -1 );
                lt_var_set_requested_values_sync( cln_vs2, -1 );

                /* after the sync, req values should not be dirty till
                 * next change */
                dirty_req = lt_var_set_dirty_requested_values_get( cln_vs1 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_req ) == 0 );
                dirty_req = lt_var_set_dirty_requested_values_get( cln_vs2 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_req ) == 0 );

            }

            /* then call process on the client passing generated
             * output to the server */
            {
                lt_cp_session_process( client, NULL, 0,
                    &buffer, &bufferlen );
                lt_cp_session_process( server,
                        buffer, bufferlen, NULL, NULL );
            }

            /* then check that the server has got the required requested
             * values into srv_vs1 and srv_vs2 */
            {
                apr_hash_t * dirty_req = NULL;
                lt_var_t * v = NULL;
                v = lt_var_set_variable_get( srv_vs1,
                        "variable1" );
                CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v ) );
                CPPUNIT_ASSERT( lt_io_value_equals(
                            lt_var_requested_value_get( v ),
                            lt_io_value_number_create( 11, NULL ) ) );

                v = lt_var_set_variable_get( srv_vs1,
                        "variable2" );
                CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v ) );
                CPPUNIT_ASSERT( lt_io_value_equals(
                            lt_var_requested_value_get( v ),
                            lt_io_value_number_create( 76, NULL ) ) );

                v = lt_var_set_variable_get( srv_vs2,
                        "variable1" );
                CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v ) );
                CPPUNIT_ASSERT( lt_io_value_equals(
                            lt_var_requested_value_get( v ),
                            lt_io_value_number_create( 22, NULL ) ) );

                v = lt_var_set_variable_get( srv_vs2,
                        "variable2" );
                CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v ) );
                CPPUNIT_ASSERT( lt_io_value_equals(
                            lt_var_requested_value_get( v ),
                            lt_io_value_number_create( 78, NULL ) ) );

                /* now we sync_req on top of server varset indicating that
                 * we had processed the req values */
                lt_var_set_requested_values_sync( srv_vs1, -1 );
                lt_var_set_requested_values_sync( srv_vs2, -1 );

                /* now req values should not be dirty till
                 * next change */
                dirty_req = lt_var_set_dirty_requested_values_get( srv_vs1 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_req ) == 0 );
                dirty_req = lt_var_set_dirty_requested_values_get( srv_vs2 );
                CPPUNIT_ASSERT( apr_hash_count( dirty_req ) == 0 );
            }


            /* send unselect view */
            lt_cp_session_send_msg_unselect_view( client );

            /* process */
            lt_cp_session_process( client, NULL, 0, &buffer, &bufferlen );
            lt_cp_session_process( server, buffer, bufferlen, NULL, NULL );

            /* send disconnect */
            lt_cp_session_send_msg_disconnect( client );

            /* process */
            lt_cp_session_process( client, NULL, 0, &buffer, &bufferlen );
            lt_cp_session_process( server, buffer, bufferlen, NULL, NULL );
        }

        void test_http() {
            lt_http_server_t * server = NULL, * server2 = NULL;

            char * file = NULL, * file2 = NULL;
            file = apr_pstrcat( pool, srcdir, "/src/cp.c", NULL );
            file2 = apr_pstrcat( pool, srcdir, "/src/http.c", NULL );


            CPPUNIT_ASSERT( lt_http_server_create(
                        &server, file, 0, NULL )
                    == LT_HTTP_SUCCESS );

            CPPUNIT_ASSERT( lt_http_server_create(
                        &server2, file2, 1235, NULL )
                    == LT_HTTP_SUCCESS );

            CPPUNIT_ASSERT( server != NULL );
            CPPUNIT_ASSERT( server2 != NULL );

            // check TCP/IP port assignment
            CPPUNIT_ASSERT( lt_http_server_port_get( server ) != 0 );
            CPPUNIT_ASSERT( lt_http_server_port_get( server2 ) == 1235 );

            // start the server in separate thread
            printf( "starting http server...\n" );
            CPPUNIT_ASSERT( lt_http_server_start( server ) ==
                    LT_HTTP_SUCCESS );

            printf( "sleeping for 2 seconds...\n" );
            // sleep for two seconds
            apr_sleep( 2 * 1000 * 1000 );

            printf( "stopping http server...\n" );
            // stop the server
            CPPUNIT_ASSERT( lt_http_server_stop( server ) ==
                    LT_HTTP_SUCCESS );

            // clean up resources
            CPPUNIT_ASSERT( lt_http_server_destroy( server ) ==
                    LT_HTTP_SUCCESS );
            CPPUNIT_ASSERT( lt_http_server_destroy( server2 ) ==
                    LT_HTTP_SUCCESS );

            // free the allocated memory
            apr_pool_destroy( lt_http_server_pool_get( server ) );
            apr_pool_destroy( lt_http_server_pool_get( server2 ) );
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION ( TestCP );
// end of custom tests

// standard test suite
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main ( int argc, const char * const * argv, const char * const * env )
{
    if ( APR_SUCCESS !=
                apr_app_initialize ( &argc, &argv, &env ) ) {
        fprintf ( stderr, "Unable to initialize APR Library\n" );
        return -1;
    }
    atexit ( apr_terminate );

    if ( lt_cp_initialize () != LT_CP_SUCCESS ) {
        fprintf ( stderr, "Unable to initialize LT CP Library\n" );
        return -1;
    }
    atexit ( lt_cp_terminate );

    apr_pool_create ( &pool, NULL );

    // figure out where our sources are and where we can write
    srcdir = getenv( "srcdir" );
    if( srcdir == NULL ) srcdir = ".";
    builddir = getenv( "top_builddir" );
    if( builddir == NULL ) builddir = ".";

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry =
        CppUnit::TestFactoryRegistry::getRegistry ();
    runner.addTest ( registry.makeTest () );

    bool wasThereError = runner.run ( "", false );

    // shell expects 0 if successfull
    return !wasThereError;
}
// end of standard test suite

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_libltcp.cpp,v 1.9 2004/06/22 13:33:53 mman Exp $
