/* $Id: io.c 723 2005-05-24 08:46:16Z mman $
 *
 *   FILE: io.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 16 June 2003
 *
 * Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
 * Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this application; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _WIN32
    #include <netinet/in.h>
#else
    #include <Winsock2.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "lt/variables/io.h"

#include <apr_strings.h>

/**
 * Opaque structure used to hold data of a value. BIT and NUMBER
 * values are encoded within `length` field and `data` is left
 * out NULL,
 * STRING value contains `length` bytes allocated and pointed to by
 * `string`. `length` includes trailing zero-byte.
 * SEQUENCE value contains `length` items of type lt_io_value_t *
 * allocated and pointed to by `sequence`.
 */
struct lt_io_value_t {
    /** pool used to allocate this structure */
    apr_pool_t * pool;
    /** type of the value */
    lt_io_value_type_e type;
    /** string and sequence only: length in octets or elements */
    /** bit and number only: numeric value */
    uint32_t length;
    /** subpool used to allocate string/sequence contents and strkey */
    apr_pool_t * strpool;
    /** key used when storing in a hash table */
    char * strkey;
    union {
        uint8_t * string;
        lt_io_value_t ** sequence;
    } data;
};

/**
 * Allocate space necessary to hold contents
 * when given io_value is of type string or sequence.
 * Actually it allocates len + 1 octets(trailing zero) for string types
 * and len * sizeof(lt_io_value_t *) for sequence types.
 */
static void _lt_io_value_contents_allocate( lt_io_value_t * val,
            uint32_t len );

/**
 * recompute the key of given value.
 */
static void _lt_io_value_strkey_recompute( lt_io_value_t * val );

apr_pool_t * lt_io_value_pool_get( const lt_io_value_t * src )
{
    if( src == NULL ) return NULL;
    return src->pool;
}

#define CREATE_POOL_IF_NULL(p)  if((p)==NULL)apr_pool_create(&(p),NULL)

lt_io_value_t * lt_io_value_create( apr_pool_t * pool )
{
    lt_io_value_t * iov = NULL;

    CREATE_POOL_IF_NULL( pool );

    iov = ( lt_io_value_t * ) apr_pcalloc( pool,
            sizeof( lt_io_value_t ) );

    if( iov == NULL ) return NULL;
    iov->type = LT_IO_TYPE_NULL;
    iov->pool = pool;
    apr_pool_create( &iov->strpool, iov->pool );
    _lt_io_value_strkey_recompute( iov );
    return iov;
}

lt_io_value_t * lt_io_value_deepcopy( const lt_io_value_t * src,
        apr_pool_t * pool )
{
    CREATE_POOL_IF_NULL( pool );

    /* FIXME: possibly optimize for simple types such as bit/number */
    if( src == NULL ) return NULL;
    switch( lt_io_value_type_get( src ) ) {
        case LT_IO_TYPE_NULL:
            return lt_io_value_create( pool );
        case LT_IO_TYPE_BIT:
            return lt_io_value_bit_create(
                    lt_io_value_bit_get( src ), pool );
        case LT_IO_TYPE_NUMBER:
            return lt_io_value_number_create(
                    lt_io_value_number_get( src ), pool );
        case LT_IO_TYPE_STRING:
            return lt_io_value_string_create(
                    lt_io_value_string_get( src ),
                    lt_io_value_string_length_get( src ), pool );
        case LT_IO_TYPE_SEQUENCE:
            {
                uint32_t i = 0;
                uint32_t len = lt_io_value_sequence_length_get( src );
                lt_io_value_t * ret =
                    lt_io_value_sequence_create( len, pool );
                if( ret == NULL ) return NULL;
                for( i = 0; i < len; i ++ ) {
                    lt_io_value_sequence_item_set( ret, i,
                            lt_io_value_deepcopy(
                                lt_io_value_sequence_item_get( src, i ),
                                pool ) );
                }
                return ret;
            }

    }
    return NULL;
}

lt_io_value_t * lt_io_value_bit_create( uint8_t val, apr_pool_t * pool )
{
    lt_io_value_t * iov = lt_io_value_create( pool );
    return lt_io_value_bit_set( iov, val );
}

lt_io_value_t * lt_io_value_number_create( int32_t val,
        apr_pool_t * pool )
{
    lt_io_value_t * iov = lt_io_value_create( pool );
    return lt_io_value_number_set( iov, val );
}

lt_io_value_t * lt_io_value_string_create( const uint8_t * val,
        uint32_t len, apr_pool_t * pool )
{
    lt_io_value_t * iov = lt_io_value_create( pool );
    return lt_io_value_string_set( iov, val, len );
}

lt_io_value_t * lt_io_value_string_create2( const uint8_t * val,
        apr_pool_t * pool )
{
    lt_io_value_t * iov = lt_io_value_create( pool );
    return lt_io_value_string_set( iov, val, strlen( val ) );
}

lt_io_value_t * lt_io_value_sequence_create( uint32_t len,
        apr_pool_t * pool )
{
    lt_io_value_t * iov = lt_io_value_create( pool );
    return lt_io_value_sequence_length_set( iov, len );
}

int lt_io_value_equals( const lt_io_value_t * a,
        const lt_io_value_t * b )
{
    int r = lt_io_value_compare( a, b );
    return( r == 0 );
}

int lt_io_value_compare( const lt_io_value_t * a,
        const lt_io_value_t * b )
{
    lt_io_value_type_e at, bt;

    /* two nulls are the same */
    if( a == NULL && b == NULL ) {
        return 0;
    }

    /* null is less than anything */
    if( a == NULL ) {
        return -1;
    }

    /* anything is greater than null */
    if( b == NULL ) {
        return 1;
    }

    at = lt_io_value_type_get( a );
    bt = lt_io_value_type_get( b );

    if( at == bt ) {
        /* compare contents */
        switch( at ) {
            case LT_IO_TYPE_NULL:
                /* two nulls are always equal */
                return 0;
                break;
            case LT_IO_TYPE_BIT:
                /* compare bits */
                {
                    int ab = lt_io_value_bit_get( a );
                    int bb = lt_io_value_bit_get( b );
                    if( ab == bb ) return 0;
                    if( ab < bb ) return -1;
                    return 1;
                }
                break;
            case LT_IO_TYPE_NUMBER:
                /* compare numbers */
                {
                    int32_t an = lt_io_value_number_get( a );
                    int32_t bn = lt_io_value_number_get( b );
                    if( an == bn ) return 0;
                    if( an < bn ) return -1;
                    return 1;
                }
                break;
            case LT_IO_TYPE_STRING:
                /* strings lexicaly compared */
                {
                    uint8_t * as = lt_io_value_string_get( a );
                    uint8_t * bs = lt_io_value_string_get( b );
                    if( as == NULL && bs == NULL ) return 0;
                    if( as == NULL ) return -1;
                    if( bs == NULL ) return 1;
                    return strcmp( as, bs );
                }
                break;
            case LT_IO_TYPE_SEQUENCE:
                {
                    int r = 0;
                    uint32_t i;
                    uint32_t al = lt_io_value_sequence_length_get( a );
                    uint32_t bl = lt_io_value_sequence_length_get( b );
                    /* compare only the shorter len items */
                    uint32_t len = al < bl ? al : bl;

                    for( i = 0 ; i < len; i ++ ) {
                        r = lt_io_value_compare(
                                lt_io_value_sequence_item_get( a, i ),
                                lt_io_value_sequence_item_get( b, i ) );
                        /* first inconsistence gives the result */
                        if( r != 0 ) return r;
                    }
                    if( al < bl ) return -1;
                    if( al > bl ) return 1;
                    return 0;
                }
                break;
        }
    } else {
        /* type codes can be compared directly */
        if( at < bt ) return -1;
        return 1;
    }
    return 0;
}

lt_io_value_type_e lt_io_value_type_get( const lt_io_value_t * val )
{
    if( val == NULL ) return LT_IO_TYPE_NULL;
    return val->type;
}

uint8_t lt_io_value_bit_get( const lt_io_value_t * val )
{
    if( val == NULL || val->type != LT_IO_TYPE_BIT )
        return 0;
    return( val->length & 0x1 );
}

int32_t lt_io_value_number_get( const lt_io_value_t * val )
{
    if( val == NULL || val->type != LT_IO_TYPE_NUMBER )
        return 0;
    return( val->length );
}

uint32_t lt_io_value_string_length_get( const lt_io_value_t * val )
{
    if( val == NULL || val->type != LT_IO_TYPE_STRING )
        return 0;
    return( val->length );
}

uint8_t * lt_io_value_string_get( const lt_io_value_t * val )
{
    if( val == NULL || val->type != LT_IO_TYPE_STRING )
        return NULL;
    return( val->data.string );
}

uint32_t lt_io_value_sequence_length_get( const lt_io_value_t * val )
{
    if( val == NULL || val->type != LT_IO_TYPE_SEQUENCE )
        return 0;
    return( val->length );
}

lt_io_value_t * lt_io_value_sequence_item_get( const lt_io_value_t * val,
        uint32_t i )
{
    if( val == NULL || val->type != LT_IO_TYPE_SEQUENCE )
        return NULL;
    if( i >= val->length ) return NULL;
    return( val->data.sequence[ i ] );
}

lt_io_value_t * lt_io_value_type_set( lt_io_value_t * val,
        lt_io_value_type_e type )
{
    if( val == NULL ) return NULL;
    val->type = type;
    apr_pool_clear( val->strpool );
    _lt_io_value_strkey_recompute( val );
    return val;
}

lt_io_value_t * lt_io_value_bit_set( lt_io_value_t * val,
        uint8_t v )
{
    if( val == NULL ) return NULL;
    val->type = LT_IO_TYPE_BIT;
    val->length = ( v & 0x1 );
    apr_pool_clear( val->strpool );
    _lt_io_value_strkey_recompute( val );
    return val;
}

lt_io_value_t * lt_io_value_number_set( lt_io_value_t * val,
        int32_t v )
{
    if( val == NULL ) return NULL;
    val->type = LT_IO_TYPE_NUMBER;
    val->length = v;
    apr_pool_clear( val->strpool );
    _lt_io_value_strkey_recompute( val );
    return val;
}

lt_io_value_t * lt_io_value_string_set( lt_io_value_t * val,
        const uint8_t * v, uint32_t len )
{
    if( val == NULL ) return NULL;
    val->type = LT_IO_TYPE_STRING;
    apr_pool_clear( val->strpool );
    val->length = 0;
    val->data.string = NULL;
    _lt_io_value_contents_allocate( val, len );
    /* copy and add trailing zero when alloc succesfull and len > 0 */
    if( val->length == len && len > 0 ) {
        memcpy( val->data.string, v, len );
        val->data.string[ len ] = 0;
    }
    /* alloc not successfull */
    if( val->length != len ) {
        /* lt_destroy_io_value( &val ); */
    }
    _lt_io_value_strkey_recompute( val );
    return val;
}

lt_io_value_t * lt_io_value_sequence_length_set( lt_io_value_t * val,
        uint32_t len )
{
    if( val == NULL ) return NULL;
    val->type = LT_IO_TYPE_SEQUENCE;
    apr_pool_clear( val->strpool );
    _lt_io_value_contents_allocate( val, len );
    /* zero sequence pointers when alloc successfull */
    if( val->length == len && len > 0 ) {
        memset( val->data.sequence, 0, len * sizeof( lt_io_value_t * ) );
    }
    /* alloc not successfull */
    if( val->length != len ) {
        /* lt_destroy_io_value( &val ); */
    }
    _lt_io_value_strkey_recompute( val );
    return val;
}

lt_io_value_t * lt_io_value_sequence_item_set( lt_io_value_t * val,
        uint32_t i, lt_io_value_t * item )
{
    if( val == NULL ) return NULL;
    if( val->type != LT_IO_TYPE_SEQUENCE ) return NULL;
    if( i >= val->length ) return NULL;

    if( val->data.sequence[ i ] != NULL ) {
        /* lt_destroy_io_value( &val->data.sequence[ i ] ); */
    }

    val->data.sequence[ i ] = item;

    return val;
}

lt_io_value_t * lt_io_value_decode( lt_io_decode_buffer_t * db,
        apr_pool_t * pool )
{
    switch( lt_io_value_next_type_get( db ) ) {
        case LT_IO_TYPE_NULL:
            return lt_io_value_null_decode( db, pool );
        case LT_IO_TYPE_BIT:
            return lt_io_value_bit_decode( db, pool );
        case LT_IO_TYPE_NUMBER:
            return lt_io_value_number_decode( db, pool );
        case LT_IO_TYPE_STRING:
            return lt_io_value_string_decode( db, pool );
        case LT_IO_TYPE_SEQUENCE:
            return lt_io_value_sequence_decode( db, pool );
    }
    return NULL;
}

lt_io_value_type_e lt_io_value_next_type_get(
        const lt_io_decode_buffer_t * db )
{
    if( db == NULL || db->data == NULL ||
            db->offset >= db->length ) return LT_IO_TYPE_NULL;
    return db->data[ db->offset ];
}

lt_io_value_t * lt_io_value_null_decode( lt_io_decode_buffer_t * db,
        apr_pool_t * pool )
{
    if( lt_io_value_next_type_get( db ) != LT_IO_TYPE_NULL ) return NULL;
    if( db->offset + 1 > db->length ) return NULL;
    /* skip tag + one octet with boolean */
    db->offset += 1;
    return lt_io_value_create( pool );
}

lt_io_value_t * lt_io_value_bit_decode( lt_io_decode_buffer_t * db,
        apr_pool_t * pool )
{
    if( lt_io_value_next_type_get( db ) != LT_IO_TYPE_BIT ) return NULL;
    if( db->offset + 2 > db->length ) return NULL;
    /* skip tag + one octet with boolean */
    db->offset += 2;
    return lt_io_value_bit_create( db->data[ db->offset - 1 ], pool );
}

lt_io_value_t * lt_io_value_number_decode( lt_io_decode_buffer_t * db,
        apr_pool_t * pool )
{
    uint32_t num = 0;
    if( lt_io_value_next_type_get( db ) != LT_IO_TYPE_NUMBER )
        return NULL;
    if( db->offset + 5 > db->length ) return NULL;

    ( ( uint8_t * ) &num ) [ 0 ] = db->data[ db->offset + 1 ];
    ( ( uint8_t * ) &num ) [ 1 ] = db->data[ db->offset + 2 ];
    ( ( uint8_t * ) &num ) [ 2 ] = db->data[ db->offset + 3 ];
    ( ( uint8_t * ) &num ) [ 3 ] = db->data[ db->offset + 4 ];

    /* skip tag + four octets with number */
    db->offset += 5;
    return lt_io_value_number_create( ntohl( num ), pool );
}

lt_io_value_t * lt_io_value_string_decode( lt_io_decode_buffer_t * db,
        apr_pool_t * pool )
{
    uint32_t len = 0;
    if( lt_io_value_next_type_get( db ) != LT_IO_TYPE_STRING )
        return NULL;
    if( db->offset + 5 > db->length ) return NULL;

    ( ( uint8_t * ) &len ) [ 0 ] = db->data[ db->offset + 1 ];
    ( ( uint8_t * ) &len ) [ 1 ] = db->data[ db->offset + 2 ];
    ( ( uint8_t * ) &len ) [ 2 ] = db->data[ db->offset + 3 ];
    ( ( uint8_t * ) &len ) [ 3 ] = db->data[ db->offset + 4 ];

    /* skip tag + four octets with length */
    db->offset += 5;
    len = ntohl( len );
    if( db->offset + len > db->length ) return NULL;
    /* skip len octets with string */
    db->offset += len;
    return lt_io_value_string_create(
            db->data + db->offset - len , len, pool );
}

lt_io_value_t * lt_io_value_sequence_decode( lt_io_decode_buffer_t * db,
        apr_pool_t * pool )
{
    /* FIXME: check whether the recursion works well */
    uint32_t i = 0;
    uint32_t len = 0;
    lt_io_value_t * ret = NULL;
    if( lt_io_value_next_type_get( db ) != LT_IO_TYPE_SEQUENCE )
        return NULL;
    if( db->offset + 5 > db->length ) return NULL;

    ( ( uint8_t * ) &len ) [ 0 ] = db->data[ db->offset + 1 ];
    ( ( uint8_t * ) &len ) [ 1 ] = db->data[ db->offset + 2 ];
    ( ( uint8_t * ) &len ) [ 2 ] = db->data[ db->offset + 3 ];
    ( ( uint8_t * ) &len ) [ 3 ] = db->data[ db->offset + 4 ];

    /* skip tag + four octets with length */
    db->offset += 5;
    len = ntohl( len );
    ret = lt_io_value_sequence_create( len, pool );
    /* alloc unsuccessfull, reset decode pointer back */
    if( ret == NULL ) {
        db->offset -= 5;
        return NULL;
    }

    /* alloc successfull, decode items. */
    for( i = 0; i < len; i ++ ) {
        lt_io_value_sequence_item_set( ret, i,
                lt_io_value_decode( db, pool ) );
    }
    return ret;
}

int8_t lt_io_value_encode( lt_io_encode_buffer_t * b,
        const lt_io_value_t * val )
{
    if( val == NULL ) {
        return lt_io_value_null_encode( b, val );
    }

    switch( lt_io_value_type_get( val ) ) {
        case LT_IO_TYPE_NULL:
            return lt_io_value_null_encode( b, val );
        case LT_IO_TYPE_BIT:
            return lt_io_value_bit_encode( b, val );
        case LT_IO_TYPE_NUMBER:
            return lt_io_value_number_encode( b, val );
        case LT_IO_TYPE_STRING:
            return lt_io_value_string_encode( b, val );
        case LT_IO_TYPE_SEQUENCE:
            return lt_io_value_sequence_encode( b, val );
    }
    return -1;
}

int8_t lt_io_value_null_encode( lt_io_encode_buffer_t * b,
        const lt_io_value_t * val )
{
    if( b == NULL || b->data == NULL ||
            b->offset + 1 > b->length ) return -1;
    if( val != NULL && lt_io_value_type_get( val ) != LT_IO_TYPE_NULL )
        return -1;
    b->data[ b->offset ] = LT_IO_TYPE_NULL;
    /* skip tag */
    b->offset += 1;
    return 0;
}

int8_t lt_io_value_bit_encode( lt_io_encode_buffer_t * b,
        const lt_io_value_t * val )
{
    if( b == NULL || b->data == NULL ||
            b->offset + 2 > b->length ) return -1;
    if( lt_io_value_type_get( val ) != LT_IO_TYPE_BIT ) return -1;
    b->data[ b->offset ] = LT_IO_TYPE_BIT;
    b->data[ b->offset + 1 ] = lt_io_value_bit_get( val ) ? 0xff : 0x0;
    /* skip tag + one octet with boolean */
    b->offset += 2;
    return 0;
}

int8_t lt_io_value_number_encode( lt_io_encode_buffer_t * b,
        const lt_io_value_t * val )
{
    uint32_t num = 0;
    if( b == NULL || b->data == NULL ||
            b->offset + 5 > b->length ) return -1;
    if( lt_io_value_type_get( val ) != LT_IO_TYPE_NUMBER ) return -1;
    b->data[ b->offset ] = LT_IO_TYPE_NUMBER;

    num = htonl( lt_io_value_number_get( val ) );
    b->data[ b->offset + 1 ] = ( ( uint8_t * ) &num ) [ 0 ];
    b->data[ b->offset + 2 ] = ( ( uint8_t * ) &num ) [ 1 ];
    b->data[ b->offset + 3 ] = ( ( uint8_t * ) &num ) [ 2 ];
    b->data[ b->offset + 4 ] = ( ( uint8_t * ) &num ) [ 3 ];

    /* skip tag + four bytes with number */
    b->offset += 5;
    return 0;
}

int8_t lt_io_value_string_encode( lt_io_encode_buffer_t * b,
        const lt_io_value_t * val )
{
    uint32_t len = 0;
    uint32_t nlen = 0;
    uint8_t * data = 0;
    if( lt_io_value_type_get( val ) != LT_IO_TYPE_STRING ) return -1;
    len = lt_io_value_string_length_get( val );
    data = lt_io_value_string_get( val );
    if( b == NULL || b->data == NULL ||
            b->offset + 5 + len > b->length ) return -1;
    b->data[ b->offset ] = LT_IO_TYPE_STRING;

    nlen = htonl( len );

    b->data[ b->offset + 1 ] = ( ( uint8_t * ) &nlen ) [ 0 ];
    b->data[ b->offset + 2 ] = ( ( uint8_t * ) &nlen ) [ 1 ];
    b->data[ b->offset + 3 ] = ( ( uint8_t * ) &nlen ) [ 2 ];
    b->data[ b->offset + 4 ] = ( ( uint8_t * ) &nlen ) [ 3 ];

    memcpy( &b->data[ b->offset + 5 ], data, len );
    /* skip tag + four bytes len + len bytes with string */
    b->offset += 5 + len;
    return 0;
}

int8_t lt_io_value_sequence_encode( lt_io_encode_buffer_t * b,
        const lt_io_value_t * val )
{
    /* FIXME: check whether the recursion works well */
    uint32_t i = 0;
    uint32_t len = 0;
    uint32_t nlen = 0;
    int8_t ret = 0;
    if( lt_io_value_type_get( val ) != LT_IO_TYPE_SEQUENCE ) return -1;
    len = lt_io_value_sequence_length_get( val );
    if( b == NULL || b->data == NULL ||
            b->offset + 5 > b->length ) return -1;
    b->data[ b->offset ] = LT_IO_TYPE_SEQUENCE;

    nlen = htonl( len );

    b->data[ b->offset + 1 ] = ( ( uint8_t * ) &nlen ) [ 0 ];
    b->data[ b->offset + 2 ] = ( ( uint8_t * ) &nlen ) [ 1 ];
    b->data[ b->offset + 3 ] = ( ( uint8_t * ) &nlen ) [ 2 ];
    b->data[ b->offset + 4 ] = ( ( uint8_t * ) &nlen ) [ 3 ];

    /* skip tag + four bytes len */
    b->offset += 5;
    for( i = 0; i < len; i ++ ) {
        ret = lt_io_value_encode( b,
                lt_io_value_sequence_item_get( val, i ) );
        if( ret != 0 ) break;
    }
    return ret;
}

const char * lt_io_value_2_strkey( const lt_io_value_t * val )
{
    if( val == NULL ) return NULL;

    return val->strkey;
}

void _lt_io_value_contents_allocate( lt_io_value_t * val,
        uint32_t len )
{
    if( val == NULL || len == 0 ) return;

    switch( val->type ) {
        case LT_IO_TYPE_STRING:
            val->length = 0;

            val->data.string = ( uint8_t * ) apr_pcalloc( val->strpool,
                    ( len + 1 ) * sizeof( uint8_t ) );

            if( val->data.string != NULL ) {
                val->length = len;
            }
            break;
        case LT_IO_TYPE_SEQUENCE:
            val->length = 0;

            val->data.sequence = ( struct lt_io_value_t ** ) apr_pcalloc(
                    val->strpool,
                    len * sizeof( lt_io_value_t * ) );

            if( val->data.sequence != NULL ) {
                val->length = len;
            }
            break;
        default:
            break;
    }
}

static void _lt_io_value_strkey_recompute( lt_io_value_t * val )
{
    static char * shared_null = "null";
    static char * shared_zero = "0";
    static char * shared_one = "1";

    if( val == NULL ) return;

    switch( val->type ) {

        case LT_IO_TYPE_NULL:
            val->strkey = shared_null;
            break;

        case LT_IO_TYPE_BIT:
            if( val->length & 1 ) {
                val->strkey = shared_one;
            } else {
                val->strkey = shared_zero;
            }
            break;

        case LT_IO_TYPE_NUMBER:
            val->strkey = apr_itoa( val->strpool, val->length );
            break;

        case LT_IO_TYPE_STRING:
            val->strkey = val->data.string;
            break;

        case LT_IO_TYPE_SEQUENCE:
            val->strkey = shared_null;

#if 0 /* FIXME: we don't need hash keys for sequences right now */
            /* FIXME: possibly chain all values of a sequence to create
             * a sequence key */
#endif

    }
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: io.c 723 2005-05-24 08:46:16Z mman $
 */
