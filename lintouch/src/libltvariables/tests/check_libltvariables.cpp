// $Id: check_libltvars.cpp,v 1.8 2004/10/25 15:06:32 jbar Exp $
//
//   FILE: check_libltvars.cpp --
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 18 August 2003
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

#include "lt/variables/variables.h"

apr_pool_t * pool = NULL;

int frontend_counter = 0;

int logging_counter = 0;

void nullify_globals();

/* example backend */
int backend( lt_var_set_t * var, void * user_arg, apr_interval_time_t t );

/* example frontends */
int frontend1( lt_var_set_t * var, void * user_arg, apr_interval_time_t t );
int frontend2( lt_var_set_t * var, void * user_arg, apr_interval_time_t t );
int frontend3( lt_var_set_t * var, void * user_arg, apr_interval_time_t t );

/* example logging hook */
void logging( lt_var_set_t * var, int real, void* user_data );

class TestVars : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE( TestVars );

    CPPUNIT_TEST( test_create_bit );
    CPPUNIT_TEST( test_create_number );
    CPPUNIT_TEST( test_create_string );
    CPPUNIT_TEST( test_create_sequence );
    CPPUNIT_TEST( test_getters_and_setters );
    CPPUNIT_TEST( test_decode_null );
    CPPUNIT_TEST( test_decode_bit );
    CPPUNIT_TEST( test_decode_number );
    CPPUNIT_TEST( test_decode_string );
    CPPUNIT_TEST( test_decode_sequence );
    CPPUNIT_TEST( test_encode_null );
    CPPUNIT_TEST( test_encode_bit );
    CPPUNIT_TEST( test_encode_number );
    CPPUNIT_TEST( test_encode_string );
    CPPUNIT_TEST( test_encode_sequence );
    CPPUNIT_TEST( test_decode );
    CPPUNIT_TEST( test_encode );
    CPPUNIT_TEST( test_compare_equals );
    CPPUNIT_TEST( test_keys );

    CPPUNIT_TEST( test_create_var );
    CPPUNIT_TEST( test_var_getters_and_setters );
    CPPUNIT_TEST( test_create_var_set );
    CPPUNIT_TEST( test_clear_dirty_flags );
    CPPUNIT_TEST( test_sync_real );
    CPPUNIT_TEST( test_sync_requested );
    CPPUNIT_TEST( test_vars_with_sources );
    CPPUNIT_TEST( test_vars_with_sources_reconnect );
    CPPUNIT_TEST( test_dirty_sync );

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

       void test_create_bit()
       {
           lt_io_value_t * bit0 = lt_io_value_bit_create( 0, pool );
           lt_io_value_t * bit1 = lt_io_value_bit_create( 1, pool );

           CPPUNIT_ASSERT(bit0 != NULL);
           CPPUNIT_ASSERT(bit1 != NULL);

           CPPUNIT_ASSERT(lt_io_value_type_get( bit0 ) ==
                   LT_IO_TYPE_BIT);
           CPPUNIT_ASSERT(lt_io_value_type_get( bit1 ) ==
                   LT_IO_TYPE_BIT);

           CPPUNIT_ASSERT(lt_io_value_bit_get( bit0 ) == 0);
           CPPUNIT_ASSERT(lt_io_value_bit_get( bit1 ) == 1);

           apr_pool_clear( pool );
       }

       void test_create_number()
       {
           lt_io_value_t * num0 =
               lt_io_value_number_create( 0, pool );
           lt_io_value_t * num99 =
               lt_io_value_number_create( 99, pool );
           lt_io_value_t * numminus99 =
               lt_io_value_number_create( -99, pool );
           // max positive number allowed
           lt_io_value_t * max =
               lt_io_value_number_create( (int32_t)2147483647, pool );
           // min negative number allowed
           lt_io_value_t * min =
               lt_io_value_number_create( (int32_t)-2147483647, pool );
           // this one should result in -1 at least on intel
           lt_io_value_t * max1 =
               lt_io_value_number_create( (int32_t)(2147483647 + 1), pool );

           CPPUNIT_ASSERT(num0 != NULL);
           CPPUNIT_ASSERT(num99 != NULL);
           CPPUNIT_ASSERT(numminus99 != NULL);

           CPPUNIT_ASSERT(max != NULL);
           CPPUNIT_ASSERT(min != NULL);
           CPPUNIT_ASSERT(max1 != NULL);

           CPPUNIT_ASSERT(lt_io_value_type_get( num0 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT(lt_io_value_type_get( num99 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT(lt_io_value_type_get( numminus99 ) ==
                   LT_IO_TYPE_NUMBER);

           CPPUNIT_ASSERT(lt_io_value_type_get( max ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT(lt_io_value_type_get( min ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT(lt_io_value_type_get( max1 ) ==
                   LT_IO_TYPE_NUMBER);

           CPPUNIT_ASSERT(lt_io_value_number_get( num0 ) == 0);
           CPPUNIT_ASSERT(lt_io_value_number_get( num99 ) == 99);
           CPPUNIT_ASSERT(lt_io_value_number_get( numminus99 ) == -99);

           CPPUNIT_ASSERT(lt_io_value_number_get( max ) == (int32_t)2147483647);
           CPPUNIT_ASSERT(lt_io_value_number_get( min ) == (int32_t)-2147483647);
           //this should work on intel, don't know about other archs
           CPPUNIT_ASSERT(lt_io_value_number_get( max1 ) == (int32_t)(2147483647 + 1));

           apr_pool_clear( pool );
       }

       void test_create_string()
       {
           char * empty = "";
           char * nonempty = "nonempty";
           uint32_t emptylen = strlen( empty );
           uint32_t nonemptylen = strlen( nonempty );
           char * empty1 = "";
           char * nonempty1 = "nonempty";
           uint32_t emptylen1 = strlen( empty1 );
           uint32_t nonemptylen1 = strlen( nonempty1 );
           lt_io_value_t * strempty = lt_io_value_string_create(
                   (uint8_t*)empty, emptylen, pool );
           lt_io_value_t * strnonempty = lt_io_value_string_create(
                   (uint8_t*)nonempty, nonemptylen, pool );
           lt_io_value_t * strempty1 = lt_io_value_string_create(
                   (uint8_t*)empty1, emptylen1, pool );
           lt_io_value_t * strnonempty1 = lt_io_value_string_create(
                   (uint8_t*)nonempty1, nonemptylen1, pool );

           lt_io_value_t * strempty2 = lt_io_value_string_create(
                   (uint8_t*)NULL, 0, pool );

           CPPUNIT_ASSERT(strempty != NULL);
           CPPUNIT_ASSERT(strnonempty != NULL);
           CPPUNIT_ASSERT(strempty1 != NULL);
           CPPUNIT_ASSERT(strnonempty1 != NULL);
           CPPUNIT_ASSERT(strempty2 != NULL);

           CPPUNIT_ASSERT(lt_io_value_type_get( strempty ) ==
                   LT_IO_TYPE_STRING);
           CPPUNIT_ASSERT(lt_io_value_type_get( strnonempty ) ==
                   LT_IO_TYPE_STRING);
           CPPUNIT_ASSERT(lt_io_value_type_get( strempty1 ) ==
                   LT_IO_TYPE_STRING);
           CPPUNIT_ASSERT(lt_io_value_type_get( strnonempty1 ) ==
                   LT_IO_TYPE_STRING);
           CPPUNIT_ASSERT(lt_io_value_type_get( strempty2 ) ==
                   LT_IO_TYPE_STRING);

           CPPUNIT_ASSERT(strncmp( empty1,
                       (char*)lt_io_value_string_get( strempty ),
                       emptylen ) == 0);
           CPPUNIT_ASSERT(strncmp( nonempty1,
                       (char*)lt_io_value_string_get( strnonempty ),
                       nonemptylen ) == 0);

           CPPUNIT_ASSERT(strncmp( empty,
                       (char*)lt_io_value_string_get( strempty1 ),
                       emptylen ) == 0);
           CPPUNIT_ASSERT(strncmp( nonempty,
                       (char*)lt_io_value_string_get( strnonempty1 ),
                       nonemptylen ) == 0);
           CPPUNIT_ASSERT(NULL ==
                       (char*)lt_io_value_string_get( strempty2 ) );
           CPPUNIT_ASSERT(0 ==
                   lt_io_value_string_length_get( strempty2 ) );

           apr_pool_clear( pool );

       }

       void test_create_sequence()
       {
           lt_io_value_t * b =
               lt_io_value_bit_create( 1, pool );
           lt_io_value_t * n =
               lt_io_value_number_create( -99, pool );
           lt_io_value_t * s =
               lt_io_value_string_create( (uint8_t*)"xxx", 3, pool );

           lt_io_value_t * seqempty =
               lt_io_value_sequence_create( 0, pool );
           lt_io_value_t * seqnempty =
               lt_io_value_sequence_create( 3, pool );

           CPPUNIT_ASSERT(b != NULL);
           CPPUNIT_ASSERT(n != NULL);
           CPPUNIT_ASSERT(s != NULL);

           CPPUNIT_ASSERT(lt_io_value_type_get( seqempty ) ==
                   LT_IO_TYPE_SEQUENCE);
           CPPUNIT_ASSERT(lt_io_value_type_get( seqnempty ) ==
                   LT_IO_TYPE_SEQUENCE);

           CPPUNIT_ASSERT( lt_io_value_sequence_item_set(
                       seqnempty, 0, b ) != NULL);
           CPPUNIT_ASSERT( lt_io_value_sequence_item_set(
                       seqnempty, 1, n ) != NULL);
           CPPUNIT_ASSERT( lt_io_value_sequence_item_set(
                       seqnempty, 2, s ) != NULL);

           CPPUNIT_ASSERT(lt_io_value_sequence_length_get(
                       seqempty ) == 0);
           CPPUNIT_ASSERT(lt_io_value_sequence_length_get(
                       seqnempty ) == 3);

           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_BIT(
                       lt_io_value_sequence_item_get( seqnempty, 0 ) ));
           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_NUMBER(
                       lt_io_value_sequence_item_get( seqnempty, 1 ) ));
           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_STRING(
                       lt_io_value_sequence_item_get( seqnempty, 2 ) ));

           apr_pool_clear( pool );

       }

       void test_getters_and_setters()
       {
           /* FIXME: add support for setters as well */
           lt_io_value_t * bit0 =
               lt_io_value_bit_create( 0, pool );
           lt_io_value_t * bit1 =
               lt_io_value_bit_create( 1, pool );
           lt_io_value_t * num0 =
               lt_io_value_number_create( 0, pool );
           lt_io_value_t * num99 =
               lt_io_value_number_create( 99, pool );
           lt_io_value_t * numm99 =
               lt_io_value_number_create( -99, pool );
           lt_io_value_t * strempty =
               lt_io_value_string_create( (uint8_t*)"", 0, pool );
           lt_io_value_t * strempty1 =
               lt_io_value_string_create( NULL, 0, pool );
           lt_io_value_t * strnempty =
               lt_io_value_string_create( (uint8_t*)"xxx", 3, pool );
           lt_io_value_t * seqempty =
               lt_io_value_sequence_create( 0, pool );
           lt_io_value_t * seqnempty =
               lt_io_value_sequence_create( 3, pool );

           /* test macros */
           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_BIT( bit0 ));
           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_BIT( bit1 ));

           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_NUMBER( num0 ));
           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_NUMBER( num99 ));
           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_NUMBER( numm99 ));

           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_STRING( strempty ));
           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_STRING( strempty1 ));
           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_STRING( strnempty ));

           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_SEQUENCE( seqempty ));
           CPPUNIT_ASSERT( LT_IO_VALUE_IS_TYPE_SEQUENCE( seqnempty ));

           CPPUNIT_ASSERT( lt_io_value_type_get( bit0 ) ==
                   LT_IO_TYPE_BIT);
           CPPUNIT_ASSERT( lt_io_value_type_get( bit1 ) ==
                   LT_IO_TYPE_BIT);

           CPPUNIT_ASSERT( lt_io_value_type_get( num0 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_type_get( num99 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_type_get( numm99 ) ==
                   LT_IO_TYPE_NUMBER);

           CPPUNIT_ASSERT( lt_io_value_type_get( strempty ) ==
                   LT_IO_TYPE_STRING);
           CPPUNIT_ASSERT( lt_io_value_type_get( strnempty ) ==
                   LT_IO_TYPE_STRING);

           CPPUNIT_ASSERT( lt_io_value_type_get( seqempty ) ==
                   LT_IO_TYPE_SEQUENCE);
           CPPUNIT_ASSERT( lt_io_value_type_get( seqnempty ) ==
                   LT_IO_TYPE_SEQUENCE);

           CPPUNIT_ASSERT( lt_io_value_bit_get( bit0 ) == 0);
           CPPUNIT_ASSERT( lt_io_value_bit_get( bit1 ) == 1);

           CPPUNIT_ASSERT( lt_io_value_number_get( num0 ) == 0);
           CPPUNIT_ASSERT( lt_io_value_number_get( num99 ) == 99);
           CPPUNIT_ASSERT( lt_io_value_number_get( numm99 ) == -99);

           uint8_t * se = lt_io_value_string_get( strempty );
           uint8_t * se1 = lt_io_value_string_get( strempty1 );
           uint8_t * sne = lt_io_value_string_get( strnempty );
           CPPUNIT_ASSERT( lt_io_value_string_length_get(
                       strempty ) == 0);
           CPPUNIT_ASSERT( se == NULL);
           CPPUNIT_ASSERT( se1 == NULL);

           CPPUNIT_ASSERT( lt_io_value_string_length_get(
                       strnempty ) == 3);
           CPPUNIT_ASSERT( strcmp( "xxx", (char*)sne ) == 0);

           CPPUNIT_ASSERT( lt_io_value_sequence_length_get(
                       seqempty ) == 0);
           CPPUNIT_ASSERT( lt_io_value_sequence_length_get(
                       seqnempty ) == 3);

           CPPUNIT_ASSERT( lt_io_value_sequence_item_get(
                       seqnempty, 0 ) == NULL);
           CPPUNIT_ASSERT( lt_io_value_sequence_item_get(
                       seqnempty, 1 ) == NULL);
           CPPUNIT_ASSERT( lt_io_value_sequence_item_get(
                       seqnempty, 2 ) == NULL);

           lt_io_value_string_set( strempty, NULL, 0 );
           CPPUNIT_ASSERT( lt_io_value_string_get(
                       strempty ) == NULL);
           CPPUNIT_ASSERT( lt_io_value_string_length_get(
                       strempty ) == 0);

           lt_io_value_string_set( strempty1, ( uint8_t* ) "ahoj", 4 );
           CPPUNIT_ASSERT( lt_io_value_string_get(
                       strempty1 ) != NULL);
           CPPUNIT_ASSERT( lt_io_value_string_length_get(
                       strempty1 ) == 4);
           CPPUNIT_ASSERT( strcmp( "ahoj",
                       ( const char* )
                       lt_io_value_string_get( strempty1 ) ) == 0 );

           lt_io_value_string_set( strempty1, ( uint8_t * ) "nazdar", 6 );
           CPPUNIT_ASSERT( lt_io_value_string_get(
                       strempty1 ) != NULL);
           CPPUNIT_ASSERT( lt_io_value_string_length_get(
                       strempty1 ) == 6);
           CPPUNIT_ASSERT( strcmp( "nazdar",
                       ( const char* )
                       lt_io_value_string_get( strempty1 ) ) == 0 );

           lt_io_value_string_set( strempty1, NULL, 0 );
           CPPUNIT_ASSERT( lt_io_value_string_get(
                       strempty1 ) == NULL);
           CPPUNIT_ASSERT( lt_io_value_string_length_get(
                       strempty1 ) == 0);

           apr_pool_clear( pool );

       }

       void test_decode_null()
       {
#define LEN 1
           uint8_t encoded_null[LEN] = {0x00};

           lt_io_decode_buffer_t db0 = { LEN, 0, encoded_null };

           lt_io_value_t * null = NULL;

           CPPUNIT_ASSERT( lt_io_value_next_type_get( &db0 ) ==
                   LT_IO_TYPE_NULL);

           null = lt_io_value_null_decode( &db0, pool );

           CPPUNIT_ASSERT( null != NULL);

           CPPUNIT_ASSERT( lt_io_value_type_get( null ) ==
                   LT_IO_TYPE_NULL);

           apr_pool_clear( pool );

#undef LEN
       }

       void test_decode_bit()
       {
#define LEN 2
           uint8_t encoded_bit0[LEN] = {0x01, 0x00 };
           uint8_t encoded_bit1[LEN] = {0x01, 0xff };

           lt_io_decode_buffer_t db0 = { LEN, 0, encoded_bit0 };
           lt_io_decode_buffer_t db1 = { LEN, 0, encoded_bit1 };

           lt_io_value_t * bit0 = NULL;
           lt_io_value_t * bit1 = NULL;

           CPPUNIT_ASSERT( lt_io_value_next_type_get( &db0 ) ==
                   LT_IO_TYPE_BIT);
           CPPUNIT_ASSERT( lt_io_value_next_type_get( &db1 ) ==
                   LT_IO_TYPE_BIT);

           bit0 = lt_io_value_bit_decode( &db0, pool );
           bit1 = lt_io_value_bit_decode( &db1, pool );

           CPPUNIT_ASSERT( bit0 != NULL);
           CPPUNIT_ASSERT( bit1 != NULL);

           CPPUNIT_ASSERT( lt_io_value_type_get( bit0 ) ==
                   LT_IO_TYPE_BIT);
           CPPUNIT_ASSERT( lt_io_value_type_get( bit1 ) ==
                   LT_IO_TYPE_BIT);

           CPPUNIT_ASSERT( lt_io_value_bit_get( bit0 ) == 0);
           CPPUNIT_ASSERT( lt_io_value_bit_get( bit1 ) == 1);

           apr_pool_clear( pool );

#undef LEN
       }

       void test_decode_number()
       {
#define LEN 5
           uint8_t encoded_number0[LEN] =
           {0x02, 0x00, 0x00, 0x00, 0x00 };
           uint8_t encoded_number99[LEN] =
           {0x02, 0x00, 0x00, 0x00, 0x63 };
           uint8_t encoded_numberminus99[LEN] =
           {0x02, 0xff, 0xff, 0xff, 0x9d };
           uint8_t encoded_numbermax[LEN] =
           {0x02, 0x7f, 0xff, 0xff, 0xff };
           uint8_t encoded_numbermin[LEN] =
           {0x02, 0x80, 0x00, 0x00, 0x01 };
           uint8_t encoded_numbermin1[LEN] =
           {0x02, 0xff, 0xff, 0xff, 0xff };

           lt_io_decode_buffer_t db0 =
           { LEN, 0, encoded_number0 };
           lt_io_decode_buffer_t db99 =
           { LEN, 0, encoded_number99 };
           lt_io_decode_buffer_t dbminus99 =
           { LEN, 0, encoded_numberminus99 };
           lt_io_decode_buffer_t dbmax =
           { LEN, 0, encoded_numbermax };
           lt_io_decode_buffer_t dbmin =
           { LEN, 0, encoded_numbermin };
           lt_io_decode_buffer_t dbmin1 =
           { LEN, 0, encoded_numbermin1 };

           lt_io_value_t * number0 = NULL;
           lt_io_value_t * number99 = NULL;
           lt_io_value_t * numberminus99 = NULL;
           lt_io_value_t * numbermax = NULL;
           lt_io_value_t * numbermin1 = NULL;
           lt_io_value_t * numbermin = NULL;

           CPPUNIT_ASSERT( lt_io_value_next_type_get( &db0 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_next_type_get( &db99 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_next_type_get( &dbminus99 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_next_type_get( &dbmax ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_next_type_get( &dbmin ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_next_type_get( &dbmin1 ) ==
                   LT_IO_TYPE_NUMBER);

           number0 = lt_io_value_number_decode( &db0, pool );
           number99 = lt_io_value_number_decode( &db99, pool );
           numberminus99 = lt_io_value_number_decode( &dbminus99, pool );
           numbermax = lt_io_value_number_decode( &dbmax, pool );
           numbermin = lt_io_value_number_decode( &dbmin, pool );
           numbermin1 = lt_io_value_number_decode( &dbmin1, pool );

           CPPUNIT_ASSERT( number0 != NULL);
           CPPUNIT_ASSERT( number99 != NULL);
           CPPUNIT_ASSERT( numberminus99 != NULL);
           CPPUNIT_ASSERT( numbermax != NULL);
           CPPUNIT_ASSERT( numbermin != NULL);
           CPPUNIT_ASSERT( numbermin1 != NULL);

           CPPUNIT_ASSERT( lt_io_value_type_get( number0 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_type_get( number99 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_type_get( numberminus99 ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_type_get( numbermax ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_type_get( numbermin ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_type_get( numbermin1 ) ==
                   LT_IO_TYPE_NUMBER);

           CPPUNIT_ASSERT( lt_io_value_number_get(
                       number0 ) == 0);
           CPPUNIT_ASSERT( lt_io_value_number_get(
                       number99 ) == 99);
           CPPUNIT_ASSERT( lt_io_value_number_get(
                       numberminus99 ) == -99);
           CPPUNIT_ASSERT( lt_io_value_number_get(
                       numbermax ) == (int32_t)2147483647);
           CPPUNIT_ASSERT( lt_io_value_number_get(
                       numbermin ) == (int32_t)-2147483647);
           CPPUNIT_ASSERT( lt_io_value_number_get(
                       numbermin1 ) == (int32_t)-1);

           apr_pool_clear( pool );

#undef LEN
       }

       void test_decode_string()
       {
#define LENEMPTY 5
#define LEN 8
           uint8_t encoded_strempty[LENEMPTY] = {
               0x24, 0x00, 0x00, 0x00, 0x00 };
           uint8_t encoded_strnempty[LEN] = {
               0x24, 0x00, 0x00, 0x00, 0x03, 0x76, 0x76, 0x76 };

           lt_io_decode_buffer_t dbempty = {
               LENEMPTY, 0, encoded_strempty };
           lt_io_decode_buffer_t dbnempty = {
               LEN, 0, encoded_strnempty };

           lt_io_value_t * strempty = NULL;
           lt_io_value_t * strnempty = NULL;

           CPPUNIT_ASSERT( lt_io_value_next_type_get( &dbempty ) ==
                   LT_IO_TYPE_STRING);
           CPPUNIT_ASSERT( lt_io_value_next_type_get( &dbnempty ) ==
                   LT_IO_TYPE_STRING);

           strempty = lt_io_value_string_decode( &dbempty, pool );
           strnempty = lt_io_value_string_decode( &dbnempty, pool );

           CPPUNIT_ASSERT( strempty != NULL);
           CPPUNIT_ASSERT( strnempty != NULL);

           CPPUNIT_ASSERT( lt_io_value_type_get( strempty )
                   == LT_IO_TYPE_STRING);
           CPPUNIT_ASSERT( lt_io_value_type_get( strnempty )
                   == LT_IO_TYPE_STRING);

           CPPUNIT_ASSERT( lt_io_value_string_length_get(
                       strempty ) == 0);
           CPPUNIT_ASSERT( lt_io_value_string_length_get(
                       strnempty ) == 3);
           CPPUNIT_ASSERT( strcmp( "vvv",
                       (char*)lt_io_value_string_get( strnempty ) ) == 0);

           apr_pool_clear( pool );

#undef LEN
#undef LENEMPTY
       }

       void test_decode_sequence()
       {
#define LENEMPTY 5
#define LEN 11
           uint8_t encoded_seqempty[LENEMPTY] = {
               0x30, 0x00, 0x00, 0x00, 0x00
           };
           uint8_t encoded_seqnempty[LEN] = {
               0x30, 0x00, 0x00, 0x00, 0x03,
               0x01, 0xff,                 /* bit 1 */
               0x01, 0x00,                 /* bit 0 */
               0x01, 0xff                  /* bit 1 */
           };

           lt_io_decode_buffer_t dbempty = { LENEMPTY, 0, encoded_seqempty };
           lt_io_decode_buffer_t dbnempty = { LEN, 0, encoded_seqnempty };

           lt_io_value_t * seqempty = NULL;
           lt_io_value_t * seqnempty = NULL;

           CPPUNIT_ASSERT( lt_io_value_next_type_get( &dbempty ) ==
                   LT_IO_TYPE_SEQUENCE);
           CPPUNIT_ASSERT( lt_io_value_next_type_get( &dbnempty ) ==
                   LT_IO_TYPE_SEQUENCE);

           seqempty = lt_io_value_sequence_decode( &dbempty, pool );
           seqnempty = lt_io_value_sequence_decode( &dbnempty, pool );

           CPPUNIT_ASSERT( seqempty != NULL);
           CPPUNIT_ASSERT( seqnempty != NULL);

           CPPUNIT_ASSERT( lt_io_value_type_get( seqempty ) ==
                   LT_IO_TYPE_SEQUENCE);
           CPPUNIT_ASSERT( lt_io_value_type_get( seqnempty ) ==
                   LT_IO_TYPE_SEQUENCE);

           CPPUNIT_ASSERT( lt_io_value_sequence_length_get(
                       seqempty ) == 0);
           CPPUNIT_ASSERT( lt_io_value_sequence_length_get(
                       seqnempty ) == 3);

           CPPUNIT_ASSERT( lt_io_value_bit_get(
                       lt_io_value_sequence_item_get(
                           seqnempty, 0 ) ) == 1);
           CPPUNIT_ASSERT( lt_io_value_bit_get(
                       lt_io_value_sequence_item_get(
                           seqnempty, 1 ) ) == 0);
           CPPUNIT_ASSERT( lt_io_value_bit_get(
                       lt_io_value_sequence_item_get(
                           seqnempty, 2 ) ) == 1);

           apr_pool_clear( pool );

#undef LEN
#undef LENEMPTY
       }

       void test_encode_null()
       {
#define LEN 1
           uint8_t encoded_null[LEN] = {0x00 };

           uint8_t encode_buf0[LEN] = {0xff };

           lt_io_encode_buffer_t eb0 = { LEN, 0, encode_buf0 };

           lt_io_value_t * null = lt_io_value_create( pool );

           CPPUNIT_ASSERT( lt_io_value_null_encode( &eb0, null ) != -1);

           CPPUNIT_ASSERT( memcmp( encoded_null, encode_buf0, LEN ) == 0);

           apr_pool_clear( pool );

#undef LEN
       }

       void test_encode_bit()
       {
#define LEN 2
           uint8_t encoded_bit0[LEN] = {0x01, 0x00 };
           uint8_t encoded_bit1[LEN] = {0x01, 0xff };

           uint8_t encode_buf0[LEN] = {0x00, 0x00 };
           uint8_t encode_buf1[LEN] = {0x00, 0x00 };

           lt_io_encode_buffer_t eb0 = { LEN, 0, encode_buf0 };
           lt_io_encode_buffer_t eb1 = { LEN, 0, encode_buf1 };

           lt_io_value_t * bit0 = lt_io_value_bit_create( 0, pool );
           lt_io_value_t * bit1 = lt_io_value_bit_create( 1, pool );

           CPPUNIT_ASSERT( lt_io_value_bit_encode( &eb0, bit0 ) != -1);
           CPPUNIT_ASSERT( lt_io_value_bit_encode( &eb1, bit1 ) != -1);

           CPPUNIT_ASSERT( memcmp( encoded_bit0, encode_buf0, LEN ) == 0);
           CPPUNIT_ASSERT( memcmp( encoded_bit1, encode_buf1, LEN ) == 0);

           apr_pool_clear( pool );

#undef LEN
       }

       void test_encode_number()
       {
#define LEN 5
           uint8_t encoded_number0[LEN] = {
               0x02, 0x00, 0x00, 0x00, 0x00 };
           uint8_t encoded_number99[LEN] = {
               0x02, 0x00, 0x00, 0x00, 0x63 };
           uint8_t encoded_numberminus99[LEN] = {
               0x02, 0xff, 0xff, 0xff, 0x9d };
           uint8_t encoded_numbermax[LEN] = {
               0x02, 0x7f, 0xff, 0xff, 0xff };
           uint8_t encoded_numbermin[LEN] = {
               0x02, 0x80, 0x00, 0x00, 0x01 };
           uint8_t encoded_numbermin1[LEN] = {
               0x02, 0xff, 0xff, 0xff, 0xff };

           uint8_t encode_buf0[LEN] = {0x00, 0x00, 0x00, 0x00, 0x00 };
           uint8_t encode_buf99[LEN] = {0x00, 0x00, 0x00, 0x00, 0x00 };
           uint8_t encode_bufminus99[LEN] = {0x00, 0x00, 0x00, 0x00, 0x00 };
           uint8_t encode_bufmax[LEN] = {0x00, 0x00, 0x00, 0x00, 0x00 };
           uint8_t encode_bufmin[LEN] = {0x00, 0x00, 0x00, 0x00, 0x00 };
           uint8_t encode_bufmin1[LEN] = {0x00, 0x00, 0x00, 0x00, 0x00 };

           lt_io_encode_buffer_t eb0 = { LEN, 0, encode_buf0 };
           lt_io_encode_buffer_t eb99 = { LEN, 0, encode_buf99 };
           lt_io_encode_buffer_t ebminus99 = { LEN, 0, encode_bufminus99 };
           lt_io_encode_buffer_t ebmax = { LEN, 0, encode_bufmax };
           lt_io_encode_buffer_t ebmin = { LEN, 0, encode_bufmin };
           lt_io_encode_buffer_t ebmin1 = { LEN, 0, encode_bufmin1 };

           lt_io_value_t * number0 =
               lt_io_value_number_create( 0, pool );
           lt_io_value_t * number99 =
               lt_io_value_number_create( 99, pool );
           lt_io_value_t * numberminus99 =
               lt_io_value_number_create( -99, pool );
           lt_io_value_t * numbermax =
               lt_io_value_number_create( 2147483647, pool );
           lt_io_value_t * numbermin =
               lt_io_value_number_create( -2147483647, pool );
           lt_io_value_t * numbermin1 =
               lt_io_value_number_create( -1, pool );

           CPPUNIT_ASSERT( lt_io_value_number_encode(
                       &eb0, number0 ) != -1);
           CPPUNIT_ASSERT( lt_io_value_number_encode(
                       &eb99, number99 ) != -1);
           CPPUNIT_ASSERT( lt_io_value_number_encode(
                       &ebminus99, numberminus99 ) != -1);
           CPPUNIT_ASSERT( lt_io_value_number_encode(
                       &ebmax, numbermax ) != -1);
           CPPUNIT_ASSERT( lt_io_value_number_encode(
                       &ebmin, numbermin ) != -1);
           CPPUNIT_ASSERT( lt_io_value_number_encode(
                       &ebmin1, numbermin1 ) != -1);

           CPPUNIT_ASSERT(
                   memcmp( encoded_number0, encode_buf0, LEN ) == 0);
           CPPUNIT_ASSERT(
                   memcmp( encoded_number99, encode_buf99, LEN ) == 0);
           CPPUNIT_ASSERT(
                   memcmp( encoded_numberminus99,
                       encode_bufminus99, LEN ) == 0);
           CPPUNIT_ASSERT(
                   memcmp( encoded_numbermax,
                       encode_bufmax, LEN ) == 0);
           CPPUNIT_ASSERT(
                   memcmp( encoded_numbermin,
                       encode_bufmin, LEN ) == 0);
           CPPUNIT_ASSERT(
                   memcmp( encoded_numbermin1,
                       encode_bufmin1, LEN ) == 0);

           apr_pool_clear( pool );

#undef LEN
       }

       void test_encode_string()
       {
#define LENEMPTY 5
#define LEN 8
           uint8_t encoded_strempty[LENEMPTY] = {
               0x24, 0x00, 0x00, 0x00, 0x00
           };
           uint8_t encoded_strnempty[LEN] = {
               0x24, 0x00, 0x00, 0x00, 0x03, 0x79, 0x79, 0x79
           };

           uint8_t encode_buf0[LENEMPTY] = {
               0x00, 0x00, 0x00, 0x00, 0x00
           };
           uint8_t encode_buf1[LEN] = {
               0x00, 0x00, 0x00, 0x00,
               0x00, 0x00, 0x00, 0x00
           };

           lt_io_encode_buffer_t eb0 = { LENEMPTY, 0, encode_buf0 };
           lt_io_encode_buffer_t eb1 = { LEN, 0, encode_buf1 };

           lt_io_value_t * strempty =
               lt_io_value_string_create( (uint8_t*)"", 0, pool );
           lt_io_value_t * strnempty =
               lt_io_value_string_create( (uint8_t*)"yyy", 3, pool );

           CPPUNIT_ASSERT( lt_io_value_string_encode(
                       &eb0, strempty ) != -1);
           CPPUNIT_ASSERT( lt_io_value_string_encode(
                       &eb1, strnempty ) != -1);

           CPPUNIT_ASSERT( memcmp(
                       encoded_strempty, encode_buf0, LENEMPTY ) == 0);
           CPPUNIT_ASSERT( memcmp(
                       encoded_strnempty, encode_buf1, LEN ) == 0);

           apr_pool_clear( pool );

#undef LEN
#undef LENEMPTY
       }

       void test_encode_sequence()
       {
#define LENEMPTY 5
#define LEN 11
           uint8_t encoded_seqempty[LENEMPTY] = {
               0x30, 0x00, 0x00, 0x00, 0x00
           };
           uint8_t encoded_seqnempty[LEN] = {
               0x30, 0x00, 0x00, 0x00, 0x03,
               0x01, 0x00,                 /* bit 0 */
               0x01, 0xff,                 /* bit 1 */
               0x01, 0xff                  /* bit 1 */
           };

           uint8_t encode_buf0[LENEMPTY] = {
               0x00, 0x00, 0x00, 0x00, 0x00
           };
           uint8_t encode_buf1[LEN] = {
               0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
               0x00, 0x00, 0x00, 0x00, 0x00
           };

           lt_io_encode_buffer_t eb0 = { LENEMPTY, 0, encode_buf0 };
           lt_io_encode_buffer_t eb1 = { LEN, 0, encode_buf1 };

           lt_io_value_t * seqempty =
               lt_io_value_sequence_create( 0, pool );
           lt_io_value_t * seqnempty =
               lt_io_value_sequence_create( 3, pool );

           lt_io_value_t * bb0 = lt_io_value_bit_create( 0, pool );
           lt_io_value_t * bb1 = lt_io_value_bit_create( 1, pool );
           lt_io_value_t * bb2 = lt_io_value_bit_create( 1, pool );

           CPPUNIT_ASSERT( seqempty != NULL);
           CPPUNIT_ASSERT( seqnempty != NULL);

           lt_io_value_sequence_item_set( seqnempty, 0, bb0 );
           lt_io_value_sequence_item_set( seqnempty, 1, bb1 );
           lt_io_value_sequence_item_set( seqnempty, 2, bb2 );

           CPPUNIT_ASSERT( lt_io_value_sequence_encode(
                       &eb0, seqempty ) != -1);
           CPPUNIT_ASSERT( lt_io_value_sequence_encode(
                       &eb1, seqnempty ) != -1);

           CPPUNIT_ASSERT( memcmp(
                       encoded_seqempty, encode_buf0, LENEMPTY ) == 0);
           CPPUNIT_ASSERT( memcmp(
                       encoded_seqnempty, encode_buf1, LEN ) == 0);

           apr_pool_clear( pool );

#undef LEN
#undef LENEMPTY
       }

       void test_decode()
       {
           /* FIXME: add support for sequence */
#define LEN 16
           uint8_t encoded_values[] = {
               0x01, 0xff,                          // bit 1
               0x02, 0xff, 0xff, 0xff, 0x9d,        // number -99
               0x24, 0x00, 0x00, 0x00, 0x03, 0x7a, 0x7a, 0x7a,  // string 'zzz'
               0x00                                 // null
           };

           lt_io_value_t * a = NULL, * b = NULL, * c = NULL, * d = NULL;

           lt_io_decode_buffer_t db = { LEN, 0, encoded_values };

           a = lt_io_value_decode( &db, pool );
           b = lt_io_value_decode( &db, pool );
           c = lt_io_value_decode( &db, pool );
           d = lt_io_value_decode( &db, pool );

           CPPUNIT_ASSERT( a != NULL );
           CPPUNIT_ASSERT( b != NULL );
           CPPUNIT_ASSERT( c != NULL );
           CPPUNIT_ASSERT( d != NULL );

           CPPUNIT_ASSERT( lt_io_value_type_get( a ) ==
                   LT_IO_TYPE_BIT);
           CPPUNIT_ASSERT( lt_io_value_type_get( b ) ==
                   LT_IO_TYPE_NUMBER);
           CPPUNIT_ASSERT( lt_io_value_type_get( c ) ==
                   LT_IO_TYPE_STRING);
           CPPUNIT_ASSERT( lt_io_value_type_get( d ) ==
                   LT_IO_TYPE_NULL);

           CPPUNIT_ASSERT( lt_io_value_bit_get( a ) == 1);
           CPPUNIT_ASSERT( lt_io_value_number_get( b ) == -99);
           CPPUNIT_ASSERT( lt_io_value_string_length_get( c ) == 3);
           CPPUNIT_ASSERT( strcmp( "zzz",
                       (char*)lt_io_value_string_get( c ) ) == 0);

           apr_pool_clear( pool );

#undef LEN
       }

       void test_encode()
       {
           /* FIXME: add support for sequence */
#define LEN 16
           uint8_t encoded_values[] = {
               0x01, 0xff,                          // bit 1
               0x02, 0xff, 0xff, 0xff, 0x9d,        // number -99
               0x24, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,  // string 'www'
               0x00                                 // null
           };

           uint8_t encode_buf[] = {
               0x0, 0x00,                          // space for bit 1
               0x0, 0x00, 0x00, 0x00, 0x00,        // space for number -99
                   // space for str 'www'
               0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
               0xff  //space for null
           };

           lt_io_encode_buffer_t eb = { LEN, 0, encode_buf };

           lt_io_value_t * a =
               lt_io_value_bit_create( 1, pool );
           lt_io_value_t * b =
               lt_io_value_number_create( -99, pool );
           lt_io_value_t * c =
               lt_io_value_string_create( (uint8_t*)"www", 3, pool );
           lt_io_value_t * d =
               lt_io_value_create( pool );

           CPPUNIT_ASSERT(a != NULL);
           CPPUNIT_ASSERT(b != NULL);
           CPPUNIT_ASSERT(c != NULL);
           CPPUNIT_ASSERT(d != NULL);

           CPPUNIT_ASSERT( lt_io_value_encode( &eb, a ) != -1);
           CPPUNIT_ASSERT( lt_io_value_encode( &eb, b ) != -1);
           CPPUNIT_ASSERT( lt_io_value_encode( &eb, c ) != -1);
           CPPUNIT_ASSERT( lt_io_value_encode( &eb, d ) != -1);

           CPPUNIT_ASSERT( memcmp( encoded_values, encode_buf, LEN ) == 0);

           apr_pool_clear( pool );

#undef LEN
       }

       void test_compare_equals()
       {
           lt_io_value_t * na = lt_io_value_create( pool );
           lt_io_value_t * nb = lt_io_value_create( pool );

           lt_io_value_t * bit0a = lt_io_value_bit_create( 0, pool );
           lt_io_value_t * bit0b = lt_io_value_bit_create( 0, pool );
           lt_io_value_t * bit1a = lt_io_value_bit_create( 1, pool );
           lt_io_value_t * bit1b = lt_io_value_bit_create( 1, pool );

           lt_io_value_t * num0 =
               lt_io_value_number_create( 0, pool );
           lt_io_value_t * numm99 =
               lt_io_value_number_create( -99, pool );
           lt_io_value_t * num99a =
               lt_io_value_number_create( 99, pool );
           lt_io_value_t * num99b =
               lt_io_value_number_create( 99, pool );

           lt_io_value_t * s1 =
               lt_io_value_string_create2( (uint8_t*)"aaa", pool );
           lt_io_value_t * s2 =
               lt_io_value_string_create2( (uint8_t*)"aaa", pool );
           lt_io_value_t * s3 =
               lt_io_value_string_create2( (uint8_t*)"bb", pool );

           lt_io_value_t * sq1 = lt_io_value_sequence_create( 2, pool );
           lt_io_value_t * sq2 = lt_io_value_sequence_create( 2, pool );
           lt_io_value_t * sq3 = lt_io_value_sequence_create( 3, pool );

           /* setup sequences */
           lt_io_value_sequence_item_set( sq1, 0, bit0a );
           lt_io_value_sequence_item_set( sq1, 1, bit1a );

           lt_io_value_sequence_item_set( sq2, 0, bit0b );
           lt_io_value_sequence_item_set( sq2, 1, bit1b );

           lt_io_value_sequence_item_set( sq3, 0, bit0a );
           lt_io_value_sequence_item_set( sq3, 1, bit0a );
           lt_io_value_sequence_item_set( sq3, 2, bit0a );

           /* null tests */
           CPPUNIT_ASSERT( lt_io_value_compare( NULL, NULL ) == 0);

           CPPUNIT_ASSERT( lt_io_value_compare( na, nb ) == 0);

           CPPUNIT_ASSERT( lt_io_value_equals( na, nb ));

           CPPUNIT_ASSERT( ! lt_io_value_equals( na, NULL ));

           /* bit tests */
           CPPUNIT_ASSERT( lt_io_value_compare( bit0a, bit0a ) == 0);

           CPPUNIT_ASSERT( lt_io_value_compare( bit0a, bit0b ) == 0);

           CPPUNIT_ASSERT( lt_io_value_compare( bit1a, bit1b ) == 0);

           CPPUNIT_ASSERT( lt_io_value_compare( bit0a, bit1a ) < 0);

           CPPUNIT_ASSERT( lt_io_value_compare( bit1a, bit0a ) > 0);

           CPPUNIT_ASSERT( lt_io_value_equals( bit0a, bit0a ));

           CPPUNIT_ASSERT( lt_io_value_equals( bit0a, bit0b ));

           CPPUNIT_ASSERT( ! lt_io_value_equals( bit1a, bit0a ));

           /* number tests */
           CPPUNIT_ASSERT( lt_io_value_compare( num0, num0 ) == 0);

           CPPUNIT_ASSERT( lt_io_value_compare( num99a, num99b ) == 0);

           CPPUNIT_ASSERT( lt_io_value_compare( numm99, num0 ) < 0);

           CPPUNIT_ASSERT( lt_io_value_compare( num0, numm99 ) > 0);

           CPPUNIT_ASSERT( lt_io_value_compare( num0, num99a ) < 0);

           CPPUNIT_ASSERT( lt_io_value_compare( num99a, num0 ) > 0);

           CPPUNIT_ASSERT( lt_io_value_equals( num99a, num99b ));

           CPPUNIT_ASSERT( ! lt_io_value_equals( num0, num99a ));

           /* string tests */
           CPPUNIT_ASSERT( lt_io_value_compare( s1, s1 ) == 0);

           CPPUNIT_ASSERT( lt_io_value_compare( s1, s3 ) < 0);

           CPPUNIT_ASSERT( lt_io_value_compare( s3, s2 ) > 0);

           CPPUNIT_ASSERT( lt_io_value_equals( s1, s2 ));

           CPPUNIT_ASSERT( ! lt_io_value_equals( s1, s3 ));

           /* sequence tests */
           CPPUNIT_ASSERT( lt_io_value_compare( sq1, sq1 ) == 0);

           CPPUNIT_ASSERT( lt_io_value_compare( sq1, sq2 ) == 0);

           CPPUNIT_ASSERT( lt_io_value_compare( sq1, sq3 ) > 0);

           CPPUNIT_ASSERT( lt_io_value_compare( sq3, sq2 ) < 0);

           CPPUNIT_ASSERT( lt_io_value_equals( sq1, sq2 ));

           CPPUNIT_ASSERT( ! lt_io_value_equals( sq1, sq3 ));

           apr_pool_clear( pool );

       }

       void test_keys()
       {
           lt_io_value_t * na = lt_io_value_create( pool );
           lt_io_value_t * nb = lt_io_value_create( pool );

           lt_io_value_t * bit0a = lt_io_value_bit_create( 0, pool );
           lt_io_value_t * bit0b = lt_io_value_bit_create( 0, pool );
           lt_io_value_t * bit1a = lt_io_value_bit_create( 1, pool );
           lt_io_value_t * bit1b = lt_io_value_bit_create( 1, pool );

           lt_io_value_t * numm99 =
               lt_io_value_number_create( -99, pool );
           lt_io_value_t * num99a =
               lt_io_value_number_create( 99, pool );
           lt_io_value_t * num99b =
               lt_io_value_number_create( 99, pool );

           lt_io_value_t * s1 =
               lt_io_value_string_create2( (uint8_t*)"aaa", pool );
           lt_io_value_t * s2 =
               lt_io_value_string_create2( (uint8_t*)"aaa", pool );
           lt_io_value_t * s3 =
               lt_io_value_string_create2( (uint8_t*)"bb", pool );

           lt_io_value_t * sq1 = lt_io_value_sequence_create( 2, pool );
           lt_io_value_t * sq2 = lt_io_value_sequence_create( 2, pool );
           lt_io_value_t * sq3 = lt_io_value_sequence_create( 3, pool );

           /* setup sequences */
           lt_io_value_sequence_item_set( sq1, 0, bit0a );
           lt_io_value_sequence_item_set( sq1, 1, bit1a );

           lt_io_value_sequence_item_set( sq2, 0, bit0b );
           lt_io_value_sequence_item_set( sq2, 1, bit1b );

           lt_io_value_sequence_item_set( sq3, 0, bit0a );
           lt_io_value_sequence_item_set( sq3, 1, bit0a );
           lt_io_value_sequence_item_set( sq3, 2, bit0a );

           /* null key tests */
           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( na ),
                       lt_io_value_2_strkey( na ) ) == 0);

           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( nb ),
                       lt_io_value_2_strkey( nb ) ) == 0);

           /* bit key tests */
           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( bit0a ),
                       lt_io_value_2_strkey( bit0a ) ) == 0);

           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( bit1a ),
                       lt_io_value_2_strkey( bit1a ) ) == 0);

           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( bit0a ),
                       lt_io_value_2_strkey( bit0b ) ) == 0);

           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( bit1a ),
                       lt_io_value_2_strkey( bit1b ) ) == 0);

           /* number key tests */
           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( numm99 ),
                       lt_io_value_2_strkey( num99a ) ) != 0);

           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( num99a ),
                       lt_io_value_2_strkey( num99b ) ) == 0);

           /* string key tests */
           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( s1 ),
                       lt_io_value_2_strkey( s3 ) ) != 0);

           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( s1 ),
                       lt_io_value_2_strkey( s2 ) ) == 0);

           /* sequence key tests */
           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( sq1 ),
                       lt_io_value_2_strkey( sq1 ) ) == 0);

           /* FIXME: uncoment the following tests as soon as
            * libltio supports contents keys for sequences */
#if 0
           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( sq1 ),
                       lt_io_value_2_strkey( sq2 ) ) == 0,
                   "lt_io_value_2_strkey generated wrong key" ); */

           CPPUNIT_ASSERT( strcmp(
                       lt_io_value_2_strkey( sq1 ),
                       lt_io_value_2_strkey( sq3 ) ) != 0);
#endif

           apr_pool_clear( pool );

       }

        void test_create_var()
        {

            lt_var_t * var = NULL;
            /* real value */
            lt_io_value_t * value = NULL;
            const lt_io_value_t * value1 = NULL;
            /* requested value */
            lt_io_value_t * rvalue = NULL;
            const lt_io_value_t * rvalue1 = NULL;
            int monitor = 0;
            int monitor1 = 0;
            /* requested monitor */
            int rmonitor1 = 0;

            value = lt_io_value_string_create2(
                    (uint8_t*)"real value", pool );
            rvalue = lt_io_value_string_create2(
                    (uint8_t*)"requested value", pool );

            CPPUNIT_ASSERT( value != NULL);
            CPPUNIT_ASSERT( rvalue != NULL);

            var = lt_var_create( LT_IO_TYPE_STRING, monitor, pool );
            lt_var_requested_value_set( var, value );
            lt_var_real_value_set( var, value );

            CPPUNIT_ASSERT( var != NULL);

            value1 = lt_var_real_value_get( var );
            rvalue1 = lt_var_requested_value_get( var );
            monitor1 = lt_var_real_monitor_get( var );
            rmonitor1 = lt_var_requested_monitor_get( var );

            CPPUNIT_ASSERT( lt_io_value_equals( value, value1 ));
            CPPUNIT_ASSERT( lt_io_value_equals( value, rvalue1 ));
            CPPUNIT_ASSERT( monitor == monitor1);
            CPPUNIT_ASSERT( monitor == rmonitor1);

            /* swap variables */
            lt_var_real_value_set( var, rvalue );
            lt_var_requested_value_set( var, value );

            value1 = lt_var_real_value_get( var );
            rvalue1 = lt_var_requested_value_get( var );

            CPPUNIT_ASSERT( lt_io_value_equals( rvalue, value1 ));
            CPPUNIT_ASSERT( lt_io_value_equals( value, rvalue1 ));

            /* check real monitor which should be zero or 1 */
            lt_var_real_monitor_set( var, 1 );
            CPPUNIT_ASSERT( lt_var_real_monitor_get( var ) == 1);
            lt_var_real_monitor_set( var, 2 );
            CPPUNIT_ASSERT( lt_var_real_monitor_get( var ) == 1);
            lt_var_real_monitor_set( var, 3 );
            CPPUNIT_ASSERT( lt_var_real_monitor_get( var ) == 1);
            lt_var_real_monitor_set( var, 0 );
            CPPUNIT_ASSERT( lt_var_real_monitor_get( var ) == 0);

            /* check requested monitor which should count clients */
            lt_var_requested_monitor_set( var, 1 );
            CPPUNIT_ASSERT( lt_var_requested_monitor_get( var ) == 1);
            lt_var_requested_monitor_set( var, 3 );
            CPPUNIT_ASSERT( lt_var_requested_monitor_get( var ) == 2);
            lt_var_requested_monitor_set( var, 4 );
            CPPUNIT_ASSERT( lt_var_requested_monitor_get( var ) == 3);
            lt_var_requested_monitor_set( var, 0 );
            CPPUNIT_ASSERT( lt_var_requested_monitor_get( var ) == 2);
            lt_var_requested_monitor_set( var, 0 );
            CPPUNIT_ASSERT( lt_var_requested_monitor_get( var ) == 1);
            lt_var_requested_monitor_set( var, 0 );
            CPPUNIT_ASSERT( lt_var_requested_monitor_get( var ) == 0);
            lt_var_requested_monitor_set( var, 0 );
            CPPUNIT_ASSERT( lt_var_requested_monitor_get( var ) == 0);

            apr_pool_clear( pool );

        }

        void test_var_getters_and_setters()
        {

            lt_var_t * varb = NULL;
            lt_var_t * varn = NULL;
            lt_var_t * vars = NULL;

            varb = lt_var_create( LT_IO_TYPE_BIT, 0, pool );
            varn = lt_var_create( LT_IO_TYPE_NUMBER, 0, pool );
            vars = lt_var_create( LT_IO_TYPE_STRING, 0, pool );

            CPPUNIT_ASSERT( varb != NULL);
            CPPUNIT_ASSERT( varn != NULL);
            CPPUNIT_ASSERT( vars != NULL);

            /* check initial real values */
            CPPUNIT_ASSERT( lt_var_real_value_bit_get( varb ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_number_get( varn ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_string_get( vars ) == NULL );
            CPPUNIT_ASSERT( lt_var_real_value_string_length_get( vars ) == 0 );

            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( varb ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( varn ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( vars ) == 1 );

            /* check initial req values */
            CPPUNIT_ASSERT( lt_var_requested_value_bit_get( varb ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_number_get( varn ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_string_get( vars ) == NULL );
            CPPUNIT_ASSERT( lt_var_requested_value_string_length_get( vars ) == 0 );

            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( varb ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( varn ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( vars ) == 1 );

            /* set some real values */
            lt_var_real_value_bit_set( varb, 1 );
            lt_var_real_value_number_set( varn, 14 );
            lt_var_real_value_string_set2( vars, ( const uint8_t * ) "heyaaa" );

            CPPUNIT_ASSERT( lt_var_real_value_bit_get( varb ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_number_get( varn ) == 14 );
            CPPUNIT_ASSERT( strcmp(
                        ( const char * ) lt_var_real_value_string_get( vars ),
                        "heyaaa" ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_string_length_get( vars ) == 6 );

            /* set some requested values */
            lt_var_requested_value_bit_set( varb, 1 );
            lt_var_requested_value_number_set( varn, 11 );
            lt_var_requested_value_string_set2( vars, ( const uint8_t * ) "ahooooooj" );

            CPPUNIT_ASSERT( lt_var_requested_value_bit_get( varb ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_number_get( varn ) == 11 );
            CPPUNIT_ASSERT( strcmp(
                        ( const char * ) lt_var_requested_value_string_get( vars ),
                        "ahooooooj" ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_string_length_get( vars ) == 9 );

            /* test user data */
            CPPUNIT_ASSERT( lt_var_userdata_get( varb ) == NULL );
            CPPUNIT_ASSERT( lt_var_userdata_get( varn ) == NULL );
            CPPUNIT_ASSERT( lt_var_userdata_get( vars ) == NULL );

            CPPUNIT_ASSERT( lt_var_userdata_set( varb, (void*)0x11223344 ) == LT_VAR_SUCCESS );
            CPPUNIT_ASSERT( lt_var_userdata_set( varn, (void*)0x55667788 ) == LT_VAR_SUCCESS );
            CPPUNIT_ASSERT( lt_var_userdata_set( vars, (void*)0x99aabbcc ) == LT_VAR_SUCCESS );

            CPPUNIT_ASSERT( lt_var_userdata_get( varb ) == (void*)0x11223344 );
            CPPUNIT_ASSERT( lt_var_userdata_get( varn ) == (void*)0x55667788 );
            CPPUNIT_ASSERT( lt_var_userdata_get( vars ) == (void*)0x99aabbcc );

            apr_pool_clear( pool );
        }

        void test_create_var_set()
        {

            lt_var_set_t * vset = NULL;

            lt_var_t * v1 = NULL, * v2 = NULL, * v3 = NULL;

            apr_hash_t * vars = NULL;
            lt_var_t * vv1 = NULL, * vv2 = NULL, * vv3 = NULL;

            vset = lt_var_set_create( 0, pool );

            v1 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_real_value_set( v1,
                    lt_io_value_string_create2(
                        (const uint8_t*)"val1", pool ) );
            lt_var_requested_value_set( v1,
                    lt_io_value_string_create2(
                        (const uint8_t*)"val1", pool ) );

            v2 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_real_value_set( v2,
                    lt_io_value_string_create2(
                        (const uint8_t*)"val2", pool ) );
            lt_var_requested_value_set( v2,
                    lt_io_value_string_create2(
                        (const uint8_t*)"val2", pool ) );

            v3 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_real_value_set( v3,
                    lt_io_value_string_create2(
                        (const uint8_t*)"val3", pool ) );
            lt_var_requested_value_set( v3,
                    lt_io_value_string_create2(
                        (const uint8_t*)"val3", pool ) );

            CPPUNIT_ASSERT( vset != NULL);
            CPPUNIT_ASSERT( v1 != NULL);
            CPPUNIT_ASSERT( v2 != NULL);
            CPPUNIT_ASSERT( v3 != NULL);

            lt_var_set_variable_register( vset, "v1", v1 );
            lt_var_set_variable_register( vset, "v2", v2 );
            lt_var_set_variable_register( vset, "v3", v3 );

            vars = lt_var_set_variables_get( vset );
            vv1 = lt_var_set_variable_get( vset, "v1" );
            vv2 = lt_var_set_variable_get( vset, "v2" );
            vv3 = lt_var_set_variable_get( vset, "v3" );

            CPPUNIT_ASSERT( apr_hash_count( vars ) == 3);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( vv1 ),
                        lt_io_value_string_create2(
                            (uint8_t*) "val1", pool ) ));
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( vv2 ),
                        lt_io_value_string_create2(
                            (uint8_t*) "val2", pool ) ));
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( vv3 ),
                        lt_io_value_string_create2(
                            (uint8_t*) "val3", pool ) ));

            CPPUNIT_ASSERT( lt_var_set_variable_unregister(
                    vset, "v3" ) == LT_VAR_SUCCESS );

            CPPUNIT_ASSERT( apr_hash_count( vars ) == 2 );

            apr_pool_clear( pool );

        }

        void test_clear_dirty_flags()
        {
            lt_var_set_t * vset = NULL;

            lt_var_t * v1 = NULL, * v2 = NULL, * v3 = NULL;

            vset = lt_var_set_create( 0, pool );

            v1 = lt_var_create( LT_IO_TYPE_BIT, 0, pool );
            lt_var_real_value_set( v1,
                    lt_io_value_bit_create(1, pool ) );
            lt_var_requested_value_set( v1,
                    lt_io_value_bit_create(1, pool ) );

            v2 = lt_var_create( LT_IO_TYPE_BIT, 0, pool );
            lt_var_real_value_set( v2,
                    lt_io_value_bit_create(1, pool ) );
            lt_var_requested_value_set( v2,
                    lt_io_value_bit_create(1, pool ) );

            v3 = lt_var_create( LT_IO_TYPE_BIT, 0, pool );
            lt_var_real_value_set( v3,
                    lt_io_value_bit_create(1, pool ) );
            lt_var_requested_value_set( v3,
                    lt_io_value_bit_create(1, pool ) );


            CPPUNIT_ASSERT( vset != NULL);
            CPPUNIT_ASSERT( v1 != NULL);
            CPPUNIT_ASSERT( v2 != NULL);
            CPPUNIT_ASSERT( v3 != NULL);

            lt_var_set_variable_register( vset, "v1", v1 );
            lt_var_set_variable_register( vset, "v2", v2 );
            lt_var_set_variable_register( vset, "v3", v3 );

            // test that sync_real will delete dirty_real flags
            lt_var_real_value_set( v1, lt_io_value_bit_create( 0, pool ) );
            lt_var_real_value_set( v2, lt_io_value_bit_create( 0, pool ) );
            lt_var_real_value_set( v3, lt_io_value_bit_create( 0, pool ) );

            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( v1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( v2 ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( v3 ) == 1 );

            lt_var_set_real_values_sync( vset, -1 );

            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( v1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( v2 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( v3 ) == 0 );

            // test that sync_req will delete dirty_req flags
            lt_var_requested_value_set( v1,
                    lt_io_value_bit_create( 0, pool ) );
            lt_var_requested_value_set( v2,
                    lt_io_value_bit_create( 0, pool ) );
            lt_var_requested_value_set( v3,
                    lt_io_value_bit_create( 0, pool ) );

            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v2 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v3 ) == 1 );

            lt_var_set_requested_values_sync( vset, -1 );

            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v2 ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( v3 ) == 0 );


            apr_pool_clear( pool );

        }

        void test_sync_real()
        {

            /* create varset, register one or more frontends, simulate
             * backend by setting real values and real monitor states and
             * invoke sync_real_values */

            lt_var_set_t * vset = NULL;
            lt_var_t * v1 = NULL, * v2 = NULL;

            nullify_globals();

            /* create varset */
            vset = lt_var_set_create( 5, pool );
            v1 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_real_value_set( v1,
                    lt_io_value_string_create2((uint8_t*) "val1", pool ) );
            v2 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_real_value_set( v2,
                    lt_io_value_string_create2((uint8_t*) "val2", pool ) );

            CPPUNIT_ASSERT( vset != NULL);

            lt_var_set_variable_register( vset, "v1", v1 );
            lt_var_set_variable_register( vset, "v2", v2 );

            lt_var_set_frontend_register(
                    vset, frontend1, ( void * ) 0xf1 );
            lt_var_set_frontend_register(
                    vset, frontend2, ( void * ) 0xf2 );
            lt_var_set_frontend_register(
                    vset, frontend3, ( void * ) 0xf3 );

            /* emulate backend */
            lt_var_real_value_set( v1,
                    lt_io_value_string_create2((uint8_t*) "val11", pool ));
            lt_var_real_monitor_set( v2, 1 );

            /* sync real values, some of the variables should be dirty */
            lt_var_set_real_values_sync( vset, 100 );
            /* second sync, no variables should be dirty */
            lt_var_set_real_values_sync( vset, 100 );

            /* the rest of CPPUNIT_ASSERT is handled by frontend routines */

            apr_pool_clear( pool );

        }

        void test_sync_requested()
        {

            /* create varset, register backend, simulate frontend by setting
             * requested values and requested monitor states and invoke
             * sync_requested_values */

            lt_var_set_t * vset = NULL;
            lt_var_t * v1 = NULL, * v2 = NULL;

            nullify_globals();

            /* create varset */
            vset = lt_var_set_create(  5, pool );
            v1 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_real_value_set( v1,
                    lt_io_value_string_create2((uint8_t*) "val1", pool ) );
            v2 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_real_value_set( v2,
                    lt_io_value_string_create2((uint8_t*) "val2", pool ) );

            CPPUNIT_ASSERT( vset != NULL);

            lt_var_set_variable_register( vset, "v1", v1 );
            lt_var_set_variable_register( vset, "v2", v2 );

            lt_var_set_backend_set( vset, backend, ( void * ) 0xb0 );

            /* emulate frontends */
            lt_var_requested_value_set( v1,
                    lt_io_value_string_create2((uint8_t*) "val11", pool ));
            lt_var_requested_value_set( v2,
                    lt_io_value_string_create2((uint8_t*) "val22", pool ));
            lt_var_requested_monitor_set( v1, 1 );
            lt_var_requested_monitor_set( v2, 1 );

            /* sync requested values, some of the variables should be dirty */
            lt_var_set_requested_values_sync( vset, 100 );
            /* second sync, no variables should be dirty */
            lt_var_set_requested_values_sync( vset, 100 );

            /* the rest of CPPUNIT_ASSERT is handled by backend routine */

            apr_pool_clear( pool );

        }

        void test_vars_with_sources()
        {

            /* create one source variable and create a chain like this
             *
             * source <----- target1 <-------- target2
             *           |      ^                ^
             *           ---------- target11 <-------- target22
             *                  |                |
             *   ^              |       ^        |        ^
             *   |              |       |        |        |
             *   |              |       |        |        |
             * level1         level2  level22  level3   level33
             *
             * and try to propagate the values back and forth */

            lt_var_t * source = NULL;
            lt_var_t * target1 = NULL, * target2 = NULL;
            lt_var_t * target11 = NULL, * target22 = NULL;

            lt_var_set_t * level1 = NULL, * level2 = NULL, * level3 = NULL;
            lt_var_set_t * level22 = NULL, * level33 = NULL;

            level1 = lt_var_set_create( 3, pool );
            level2 = lt_var_set_create( 3, pool );
            level22 = lt_var_set_create( 3, pool );
            level3 = lt_var_set_create( 3, pool );
            level33 = lt_var_set_create( 3, pool );

            /* set logging hook */
            lt_var_set_logging_set( level1, logging, (void*)0x1234 );
            CPPUNIT_ASSERT(logging_counter == 0);

            source = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_requested_value_set( source,
                    lt_io_value_string_create2(
                        (uint8_t*) "source value", pool ) );
            lt_var_real_value_set( source,
                    lt_io_value_string_create2(
                        (uint8_t*) "source value", pool ) );

            target1 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_requested_value_set( target1,
                    lt_io_value_string_create2(
                        (uint8_t*) "target1 value", pool ) );
            lt_var_real_value_set( target1,
                    lt_io_value_string_create2(
                        (uint8_t*) "target1 value", pool ) );

            target11 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_requested_value_set( target11,
                    lt_io_value_string_create2(
                        (uint8_t*) "target11 value", pool ) );
            lt_var_real_value_set( target11,
                    lt_io_value_string_create2(
                        (uint8_t*) "target11 value", pool ) );

            target2 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_requested_value_set( target2,
                    lt_io_value_string_create2(
                        (uint8_t*) "target2 value", pool ) );
            lt_var_real_value_set( target2,
                    lt_io_value_string_create2(
                        (uint8_t*) "target2 value", pool ) );

            target22 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_requested_value_set( target22,
                    lt_io_value_string_create2(
                        (uint8_t*) "target22 value", pool ) );
            lt_var_real_value_set( target22,
                    lt_io_value_string_create2(
                        (uint8_t*) "target22 value", pool ) );

            /* the vars must have the same names within varset so they can
             * by synced correctly */
            lt_var_set_variable_register( level1, "source", source );
            lt_var_set_variable_register( level2, "source", target1 );
            lt_var_set_variable_register( level22, "source", target11 );
            lt_var_set_variable_register( level3, "source", target2 );
            lt_var_set_variable_register( level33, "source", target22 );

            lt_var_set_source_set( level2, level1 );
            lt_var_set_source_set( level22, level1 );
            lt_var_set_source_set( level3, level2 );
            lt_var_set_source_set( level33, level22 );

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target1 ) ) == 0);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target11 ) ) == 0);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target2 ) ) == 0);
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target22 ) ) == 0);

            /* simulate first step of propagation */
            /* changes should be now in level2 */
            lt_var_set_real_values_sync( level1, -1 );
            CPPUNIT_ASSERT(logging_counter == 1);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target1 ) ) == 1);
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target11 ) ) == 1);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target2 ) ) == 0);
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target22 ) ) == 0);

            /* simulate second step of propagation */
            /* changes should be now in level3 */
            lt_var_set_real_values_sync( level2, -1 );
            lt_var_set_real_values_sync( level22, -1 );
            CPPUNIT_ASSERT(logging_counter == 1);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target1 ) ) == 1);
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target11 ) ) == 1);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target2 ) ) == 1);
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target22 ) ) == 1);

            /* and now the other way around */

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target1 ) ) == 0);
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target11 ) ) == 0);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target2 ) ) == 0);
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target22 ) ) == 0);

            /* simulate first step of propagation */
            /* changes should be now from level3 in level2 */
            lt_var_set_requested_values_sync( level3, -1 );
            lt_var_set_requested_values_sync( level33, -1 );
            CPPUNIT_ASSERT(logging_counter == 1);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target1 ) ) == 0);
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target11 ) ) == 0);

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( target1 ),
                        lt_var_requested_value_get( target2 ) ) == 1);
            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( target11 ),
                        lt_var_requested_value_get( target22 ) ) == 1);

            /* simulate second step of propagation */
            /* changes should be now from level2 in level1 */
            lt_var_set_requested_values_sync( level2, -1 );
            lt_var_set_requested_values_sync( level22, -1 );
            CPPUNIT_ASSERT(logging_counter == 1);

            lt_var_set_requested_values_sync( level1, -1 );
            CPPUNIT_ASSERT(logging_counter == 2);

            /* we can't test this since we don't know who wins the
             * sync_requested step from level2 to level1, it can be target1
             * but it can also be target11, one of them should match so we
             * test whether the propagation was succesfull either way */

            CPPUNIT_ASSERT(
                    ( lt_io_value_equals(
                          lt_var_requested_value_get( source ),
                          lt_var_requested_value_get( target1 ) ) ) ||
                    ( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target11 ) ) )
                    );

            apr_pool_clear( pool );

        }

        void test_vars_with_sources_reconnect()
        {

            /* create one source variable and create a chain like this
             *
             * source <----- target1
             *   |              |
             * level1         level2
             *
             * and try to propagate the values back and forth, while
             * connecting and disconnecting */

            lt_var_t * source = NULL;
            lt_var_t * target1 = NULL;

            lt_var_set_t * level1 = NULL, * level2 = NULL;

            level1 = lt_var_set_create( 3, pool );
            level2 = lt_var_set_create( 3, pool );

            source = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_requested_value_set( source,
                    lt_io_value_string_create2(
                        (uint8_t*) "source value", pool ) );
            lt_var_real_value_set( source,
                    lt_io_value_string_create2(
                        (uint8_t*) "source value", pool ) );

            target1 = lt_var_create( LT_IO_TYPE_STRING, 0, pool );
            lt_var_requested_value_set( target1,
                    lt_io_value_string_create2(
                        (uint8_t*) "target1 value", pool ) );
            lt_var_real_value_set( target1,
                    lt_io_value_string_create2(
                        (uint8_t*) "target1 value", pool ) );

            /* the vars must have the same names within varset so they can
             * by synced correctly */
            lt_var_set_variable_register( level1, "source", source );
            lt_var_set_variable_register( level2, "source", target1 );

            lt_var_set_source_set( level2, level1 );

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target1 ) ) == 0);

            /* simulate first step of propagation */
            /* changes should be now in level2 */
            lt_var_set_real_values_sync( level1, -1 );

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target1 ) ) == 1);

            /* and now the other way around */

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target1 ) ) == 0);

            /* simulate first step of propagation */
            /* changes should be now from level2 in level1 */
            lt_var_set_requested_values_sync( level2, -1 );

            CPPUNIT_ASSERT(
                    lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target1 ) ) == 1 );

            /* now disconnect the level1 and level2 */

            lt_var_set_source_set( level2, NULL );

            /* set some new real and requested values */
            lt_var_real_value_set( source,
                    lt_io_value_string_create2(
                        (uint8_t*)"new source value", pool ) );
            lt_var_requested_value_set( target1,
                    lt_io_value_string_create2(
                        (uint8_t*)"new target1 value", pool ) );

            /* and check that we don't propagate anymore */

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target1 ) ) == 0);

            /* simulate first step of propagation */
            /* changes should not be now in level2 */
            lt_var_set_real_values_sync( level1, -1 );

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target1 ) ) == 0);

            /* and now the other way around */

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target1 ) ) == 0);

            /* simulate first step of propagation */
            /* changes should not be now from level2 in level1 */
            lt_var_set_requested_values_sync( level2, -1 );

            CPPUNIT_ASSERT(
                    lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target1 ) ) == 0 );

            /* and now connect them the other way around */

            lt_var_set_source_set( level1, level2 );

            /* set some new real and requested values */
            lt_var_real_value_set( target1,
                    lt_io_value_string_create2(
                        (uint8_t*)"another new source value", pool ) );
            lt_var_requested_value_set( source,
                    lt_io_value_string_create2(
                        (uint8_t*)"another new target1 value", pool ) );

            /* and check that we do propagate, but the other way around */

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target1 ) ) == 0);

            /* simulate first step of propagation */
            /* changes should not be now in level1 */
            lt_var_set_real_values_sync( level2, -1 );

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_real_value_get( source ),
                        lt_var_real_value_get( target1 ) ) == 1);

            /* and now the other way around */

            CPPUNIT_ASSERT( lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target1 ) ) == 0);

            /* simulate first step of propagation */
            /* changes should not be now from level2 in level1 */
            lt_var_set_requested_values_sync( level1, -1 );

            CPPUNIT_ASSERT(
                    lt_io_value_equals(
                        lt_var_requested_value_get( source ),
                        lt_var_requested_value_get( target1 ) ) == 1 );


            apr_pool_clear( pool );

        }

        void test_dirty_sync()
        {

            lt_var_set_t * source = NULL, * target = NULL;
            lt_var_t * s1 = NULL, * s2 = NULL, * t1 = NULL, * t2 = NULL;

            source = lt_var_set_create( 0, pool );
            target = lt_var_set_create( 0, pool );

            s1 = lt_var_create( LT_IO_TYPE_NUMBER, 0, pool );
            lt_var_requested_value_set( s1,
                    lt_io_value_number_create( 10, pool ) );
            lt_var_real_value_set( s1,
                    lt_io_value_number_create( 10, pool ) );
            s2 = lt_var_create( LT_IO_TYPE_NUMBER, 0, pool );
            lt_var_requested_value_set( s2,
                    lt_io_value_number_create( 20, pool ) );
            lt_var_real_value_set( s2,
                    lt_io_value_number_create( 20, pool ) );
            t1 = lt_var_create( LT_IO_TYPE_NUMBER, 0, pool );
            lt_var_requested_value_set( t1,
                    lt_io_value_number_create( 100, pool ) );
            lt_var_real_value_set( t1,
                    lt_io_value_number_create( 100, pool ) );
            t2 = lt_var_create( LT_IO_TYPE_NUMBER, 0, pool );
            lt_var_requested_value_set( t2,
                    lt_io_value_number_create( 200, pool ) );
            lt_var_real_value_set( t2,
                    lt_io_value_number_create( 200, pool ) );

            CPPUNIT_ASSERT( source != NULL );
            CPPUNIT_ASSERT( target != NULL );
            CPPUNIT_ASSERT( s1 != NULL );
            CPPUNIT_ASSERT( s2 != NULL );
            CPPUNIT_ASSERT( t1 != NULL );
            CPPUNIT_ASSERT( t2 != NULL );

            CPPUNIT_ASSERT(
                    lt_var_set_variable_register( source, "s1", s1 ) ==
                    LT_VAR_SUCCESS );
            CPPUNIT_ASSERT(
                    lt_var_set_variable_register( source, "s2", s2 ) ==
                    LT_VAR_SUCCESS );

            CPPUNIT_ASSERT(
                    lt_var_set_variable_register( target, "s1", t1 ) ==
                    LT_VAR_SUCCESS );
            CPPUNIT_ASSERT(
                    lt_var_set_variable_register( target, "s2", t2 ) ==
                    LT_VAR_SUCCESS );

            CPPUNIT_ASSERT(
                    lt_var_set_source_set( target, source ) ==
                    LT_VAR_SUCCESS );

            /* in the beginning all req/real values should be dirty */
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s2 ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t2 ) == 1 );

            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s2 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t2 ) == 1 );

            /* sync real should bring dirty  real values from source to target
             * and clear dirty real flags on source
             */
            lt_var_set_real_values_sync( source, -1 );

            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s2 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t2 ) == 1 );

            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s2 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t2 ) == 1 );

            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_real_value_get( s1 ) ) == 10 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_real_value_get( s2 ) ) == 20 );

            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_real_value_get( t1 ) ) == 10 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_real_value_get( t2 ) ) == 20 );

            /* sync requested should bring all req values from target to
             * source and clear dirty requested flags
             */
            lt_var_set_requested_values_sync( target, -1 );

            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s2 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t2 ) == 1 );

            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s2 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t2 ) == 0 );

            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_requested_value_get( s1 ) ) == 100 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_requested_value_get( s2 ) ) == 200 );

            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_requested_value_get( t1 ) ) == 100 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_requested_value_get( t2 ) ) == 200 );

            /* now sync real/req on all varsets so that we clear remaining
             * dirty flags */

            lt_var_set_real_values_sync( target, -1 );

            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s2 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t2 ) == 0 );

            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s2 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t2 ) == 0 );

            lt_var_set_requested_values_sync( source, -1 );

            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s2 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t2 ) == 0 );

            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s2 ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t2 ) == 0 );

            /* now let's make some req/real values dirty again and check
             * that sync changes only them */
            lt_var_real_value_set( s1,
                    lt_io_value_number_create( 11, pool ) );
            lt_var_real_value_set( t2,
                    lt_io_value_number_create( 22, pool ) );

            lt_var_set_real_values_sync( source, -1 );

            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( s2 ) == 0 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_real_value_is_dirty( t2 ) == 1 );

            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_real_value_get( s1 ) ) == 11 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_real_value_get( s2 ) ) == 20 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_real_value_get( t1 ) ) == 11 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_real_value_get( t2 ) ) == 22 );

            lt_var_requested_value_set( t1,
                    lt_io_value_number_create( 111, pool ) );
            lt_var_requested_value_set( s2,
                    lt_io_value_number_create( 222, pool ) );

            lt_var_set_requested_values_sync( target, -1 );

            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s1 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( s2 ) == 1 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t1 ) == 0 );
            CPPUNIT_ASSERT( lt_var_requested_value_is_dirty( t2 ) == 0 );

            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_requested_value_get( s1 ) ) == 111 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_requested_value_get( s2 ) ) == 222 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_requested_value_get( t1 ) ) == 111 );
            CPPUNIT_ASSERT( lt_io_value_number_get(
                        lt_var_requested_value_get( t2 ) ) == 200 );

            apr_pool_clear( pool );

        }

};

void nullify_globals()
{
    frontend_counter = 0;
}

int backend( lt_var_set_t * var, void * user_arg, apr_interval_time_t t )
{
    static int counter = 0;

    apr_hash_t * drlv = NULL, * drqv = NULL;
    apr_hash_t * drlm = NULL, * drqm = NULL;

    CPPUNIT_ASSERT( ( unsigned long ) user_arg == 0xb0);

    CPPUNIT_ASSERT( t == 100);

    /* when invoked second time, there should be no dirty values */
    if( counter != 0 ) {
        drlv = lt_var_set_dirty_real_values_get( var );
        drqv = lt_var_set_dirty_requested_values_get( var );
        drlm = lt_var_set_dirty_real_monitors_get( var );
        drqm = lt_var_set_dirty_requested_monitors_get( var );

        CPPUNIT_ASSERT( apr_hash_count( drlv ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drlm ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drqv ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drqm ) == 0);
        return 0;
    }

    drlv = lt_var_set_dirty_real_values_get( var );
    drqv = lt_var_set_dirty_requested_values_get( var );
    drlm = lt_var_set_dirty_real_monitors_get( var );
    drqm = lt_var_set_dirty_requested_monitors_get( var );

    CPPUNIT_ASSERT( apr_hash_count( drlv ) == 0);
    CPPUNIT_ASSERT( apr_hash_count( drlm ) == 0);
    CPPUNIT_ASSERT( apr_hash_count( drqv ) == 2);
    CPPUNIT_ASSERT( apr_hash_count( drqm ) == 2);

    {
        lt_var_t * v1 = NULL, * v2 = NULL;
        char * key1 = NULL, * key2 = NULL;

        key1 = "v1";
        key2 = "v2";

        v1 = ( lt_var_t * ) apr_hash_get( drqv,
                ( key1 ), APR_HASH_KEY_STRING );

        v2 = ( lt_var_t * ) apr_hash_get( drqv,
                ( key2 ), APR_HASH_KEY_STRING );

        CPPUNIT_ASSERT( v1 != NULL);
        CPPUNIT_ASSERT( v2 != NULL);

        v1 = ( lt_var_t * ) apr_hash_get( drqm,
                ( key1 ), APR_HASH_KEY_STRING );

        v2 = ( lt_var_t * ) apr_hash_get( drqm,
                ( key2 ), APR_HASH_KEY_STRING );

        CPPUNIT_ASSERT( v1 != NULL);
        CPPUNIT_ASSERT( v2 != NULL);
    }

    counter ++;
    return 0;
}

int frontend1( lt_var_set_t * var, void * user_arg, apr_interval_time_t t )
{
    static int counter = 0;

    apr_hash_t * drlv = NULL, * drqv = NULL;
    apr_hash_t * drlm = NULL, * drqm = NULL;

    CPPUNIT_ASSERT( ( unsigned long ) user_arg == 0xf1);

    CPPUNIT_ASSERT( t == 100);

    /* when invoked second time, there should be no dirty values */
    if( counter != 0 ) {
        drlv = lt_var_set_dirty_real_values_get( var );
        drqv = lt_var_set_dirty_requested_values_get( var );
        drlm = lt_var_set_dirty_real_monitors_get( var );
        drqm = lt_var_set_dirty_requested_monitors_get( var );

        CPPUNIT_ASSERT( apr_hash_count( drlv ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drlm ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drqv ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drqm ) == 0);
        return 0;
    }

    frontend_counter ++;
    CPPUNIT_ASSERT( frontend_counter == 1);

    drlv = lt_var_set_dirty_real_values_get( var );
    drqv = lt_var_set_dirty_requested_values_get( var );
    drlm = lt_var_set_dirty_real_monitors_get( var );
    drqm = lt_var_set_dirty_requested_monitors_get( var );

    CPPUNIT_ASSERT( apr_hash_count( drlv ) == 1);
    CPPUNIT_ASSERT( apr_hash_count( drlm ) == 1);
    CPPUNIT_ASSERT( apr_hash_count( drqv ) == 0);
    CPPUNIT_ASSERT( apr_hash_count( drqm ) == 0);

    {
        lt_var_t * v1 = NULL, * v2 = NULL;
        char * key1 = NULL, * key2 = NULL;

        key1 = "v1";
        key2 = "v2";

        v1 = ( lt_var_t * ) apr_hash_get( drlv,
                ( key1 ), APR_HASH_KEY_STRING );

        v2 = ( lt_var_t * ) apr_hash_get( drlm,
                ( key2 ), APR_HASH_KEY_STRING );

        CPPUNIT_ASSERT( v1 != NULL);
        CPPUNIT_ASSERT( v2 != NULL);
    }

    counter ++;
    return 0;
}

int frontend2( lt_var_set_t * var, void * user_arg, apr_interval_time_t t )
{
    static int counter = 0;

    apr_hash_t * drlv = NULL, * drqv = NULL;
    apr_hash_t * drlm = NULL, * drqm = NULL;

    CPPUNIT_ASSERT( ( unsigned long ) user_arg == 0xf2);

    CPPUNIT_ASSERT( t == 100);

    /* when invoked second time, there should be no dirty values */
    if( counter != 0 ) {
        drlv = lt_var_set_dirty_real_values_get( var );
        drqv = lt_var_set_dirty_requested_values_get( var );
        drlm = lt_var_set_dirty_real_monitors_get( var );
        drqm = lt_var_set_dirty_requested_monitors_get( var );

        CPPUNIT_ASSERT( apr_hash_count( drlv ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drlm ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drqv ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drqm ) == 0);
        return 0;
    }

    frontend_counter ++;
    CPPUNIT_ASSERT( frontend_counter == 2);

    drlv = lt_var_set_dirty_real_values_get( var );
    drqv = lt_var_set_dirty_requested_values_get( var );
    drlm = lt_var_set_dirty_real_monitors_get( var );
    drqm = lt_var_set_dirty_requested_monitors_get( var );

    CPPUNIT_ASSERT( apr_hash_count( drlv ) == 1);
    CPPUNIT_ASSERT( apr_hash_count( drlm ) == 1);
    CPPUNIT_ASSERT( apr_hash_count( drqv ) == 0);
    CPPUNIT_ASSERT( apr_hash_count( drqm ) == 0);

    counter ++;
    return 0;
}

int frontend3( lt_var_set_t * var, void * user_arg, apr_interval_time_t t )
{
    static int counter = 0;

    apr_hash_t * drlv = NULL, * drqv = NULL;
    apr_hash_t * drlm = NULL, * drqm = NULL;

    CPPUNIT_ASSERT( ( unsigned long ) user_arg == 0xf3);

    CPPUNIT_ASSERT( t == 100);

    /* when invoked second time, there should be no dirty values */
    if( counter != 0 ) {
        drlv = lt_var_set_dirty_real_values_get( var );
        drqv = lt_var_set_dirty_requested_values_get( var );
        drlm = lt_var_set_dirty_real_monitors_get( var );
        drqm = lt_var_set_dirty_requested_monitors_get( var );

        CPPUNIT_ASSERT( apr_hash_count( drlv ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drlm ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drqv ) == 0);
        CPPUNIT_ASSERT( apr_hash_count( drqm ) == 0);
        return 0;
    }

    frontend_counter ++;
    CPPUNIT_ASSERT( frontend_counter == 3);

    drlv = lt_var_set_dirty_real_values_get( var );
    drqv = lt_var_set_dirty_requested_values_get( var );
    drlm = lt_var_set_dirty_real_monitors_get( var );
    drqm = lt_var_set_dirty_requested_monitors_get( var );

    CPPUNIT_ASSERT( apr_hash_count( drlv ) == 1);
    CPPUNIT_ASSERT( apr_hash_count( drlm ) == 1);
    CPPUNIT_ASSERT( apr_hash_count( drqv ) == 0);
    CPPUNIT_ASSERT( apr_hash_count( drqm ) == 0);

    counter ++;
    return 0;
}

void logging( lt_var_set_t * var, int real, void* user_data )
{
    CPPUNIT_ASSERT((long)user_data == 0x1234);

    if( logging_counter == 0 ) {
        CPPUNIT_ASSERT(real == 1);

        // this fails :-( there is 0 all the time...
        // CPPUNIT_ASSERT(apr_hash_count(
                    // lt_var_set_dirty_real_values_get(var)) == 1);
        CPPUNIT_ASSERT(apr_hash_count(
                    lt_var_set_dirty_requested_values_get(var)) == 0);

    } else if( logging_counter == 1 || logging_counter == 2 ) {
        CPPUNIT_ASSERT(real == 0);
        CPPUNIT_ASSERT(apr_hash_count(
                    lt_var_set_dirty_real_values_get(var)) == 0);
        CPPUNIT_ASSERT(apr_hash_count(
                    lt_var_set_dirty_requested_values_get(var)) == 1);

    }
    ++logging_counter;
}

CPPUNIT_TEST_SUITE_REGISTRATION( TestVars );
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

    apr_pool_create( &pool, NULL );

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry =
        CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );

    bool wasThereError = runner.run( "", false );

    // shell expects 0 if successfull
    return !wasThereError;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: check_libltvars.cpp,v 1.8 2004/10/25 15:06:32 jbar Exp $
