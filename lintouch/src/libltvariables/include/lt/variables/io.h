/* $Id: io.h 347 2005-02-23 14:56:10Z modesto $
 *
 *   FILE: io.h --
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

#ifndef _LT_IO_H
#define _LT_IO_H

#ifndef _WIN32
#   include <inttypes.h>
#else
#   ifndef _WIN32_INTTYPES_DEFINED
#       define _WIN32_INTTYPES_DEFINED
#       define uint64_t unsigned __int64
#       define uint32_t unsigned __int32
#       define uint16_t unsigned __int16
#       define uint8_t  unsigned __int8
#       define int64_t  __int64
#       define int32_t  __int32
#       define int16_t  __int16
#       define int8_t   __int8
#   endif
#endif

#include <apr_general.h>

/**
 * @file io.h
 * @brief Lintouch IO Library
 */

/**
 * @defgroup LTIO Lintouch IO Library
 * @{
 */

/**
 * @defgroup LTIOOverview Overview
 * @{
 *
 * Lintouch IO Library.
 *
 * Definitions and functions for basic io value handling.
 * Library contains functions for io value creation, destruction.
 *
 * Also provided are routines to encode/decode io values into/from octet
 * streams using Simplified BER encoding.
 *
 * Simplified BER Encoding
 * -----------------------
 *
 * encoding scheme: tag(1 octet) length(optional 4 octets) data
 *
 * supported types(tags):
 *
 * 0x0 = NULL, no length, no data octets
 *
 * 0x1 = BIT, no length, next octet is 0x00/false or 0xff/true
 *
 * 0x2 = INTEGER, no length, next four octets are big endian int32_t
 *
 * 0x24 = STRING, big endian uint32_t length, `length` octets
 *
 * 0x30 = SEQUENCE, big endian uint32_t number of items in a sequence.
 *
 * @author Martin Man <mman@swac.cz>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Definition of a type. These constants essentialy corespond to those
     * used by BER.
     */
    typedef enum {

        /**
         * null
         */
        LT_IO_TYPE_NULL       =   0x00,

        /**
         * true/false or 1/0 value.
         */
        LT_IO_TYPE_BIT        =   0x01,

        /**
         * -2^31 <= x < 2^31
         */
        LT_IO_TYPE_NUMBER     =   0x02,

        /**
         * opaque array of 8bit octets of given length
         */
        LT_IO_TYPE_STRING     =   0x24,

        /**
         * sequence of given number of items
         */
        LT_IO_TYPE_SEQUENCE   =   0x30

    } lt_io_value_type_e;

    /**
     * IO Value type.
     */
    typedef struct lt_io_value_t lt_io_value_t;

    /**
     * Opaque structure holding data to be encoded/decoded.
     *
     * When being used as a lt_io_decode_buffer_t, initialize `length`,
     * `offset` and `data` with appropriate values matching the buffer
     * (read from file or network) containing values to be decoded.
     *
     * When being used as a lt_io_encode_buffer_t, initialize `length`,
     * `offset` and `data` with allocated memory that will be used to
     * store encoded values.
     */
    typedef struct {
        uint32_t length;
        uint32_t offset;
        uint8_t *data;
    } lt_io_buffer_t;

#define lt_io_encode_buffer_t    lt_io_buffer_t
#define lt_io_decode_buffer_t    lt_io_buffer_t


#define LT_IO_VALUE_IS_TYPE_NULL(v) \
    ((v) != NULL && lt_io_value_type_get(v) == LT_IO_TYPE_NULL)

#define LT_IO_VALUE_IS_TYPE_BIT(v) \
    ((v) != NULL && lt_io_value_type_get(v) == LT_IO_TYPE_BIT)

#define LT_IO_VALUE_IS_TYPE_NUMBER(v) \
    ((v) != NULL && lt_io_value_type_get(v) == LT_IO_TYPE_NUMBER)

#define LT_IO_VALUE_IS_TYPE_STRING(v) \
    ((v) != NULL && lt_io_value_type_get(v) == LT_IO_TYPE_STRING)

#define LT_IO_VALUE_IS_TYPE_SEQUENCE(v) \
    ((v) != NULL && lt_io_value_type_get(v) == LT_IO_TYPE_SEQUENCE)


    /**
     * Routines to create lt_io_value_t
     * ==================================
     */

    /**
     * return pool out of which the given io value has been allocated.
     */
    apr_pool_t * lt_io_value_pool_get( const lt_io_value_t * src );

    /**
     * create new null io value without any content.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_create( apr_pool_t * pool );

    /**
     * make a deep copy of the given value.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_deepcopy( const lt_io_value_t * src,
            apr_pool_t * pool );

    /**
     * create new bit value.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_bit_create( uint8_t val,
            apr_pool_t * pool );

    /**
     * create new number value.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_number_create( int32_t val,
            apr_pool_t * pool );

    /**
     * create new string value. make a copy of a given val.
     * return null in case of problem. `val` need not be zero-terminated,
     * one trailing zero will be appended anyway.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_string_create( const uint8_t * val,
            uint32_t len, apr_pool_t * pool );

    /**
     * create new string value. make a copy of a given val.
     * return null in case of problem. `val` need to be zero-terminated.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_string_create2( const uint8_t * val,
            apr_pool_t * pool );

    /**
     * create new sequence value. make a sequence of null pointers of given
     * len.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_sequence_create( uint32_t len,
            apr_pool_t * pool );

    /**
     * test if the two values are the same (by matching type/contents)
     * return 0/1.
     */
    int lt_io_value_equals( const lt_io_value_t * a,
            const lt_io_value_t * b );

    /**
     * compare two values in the spirit of strcmp.
     * if the two passed values are not of the same type, comparison is done
     * in a way null < bit < number < string < sequence.
     *
     * note that string contents is compared using strcmp, e.g., it won't
     * work correctly for utf-8 encoded strings.
     *
     * sequences are compared item per item, first non-matching items give
     * the result, if all items of one sequence match the other one, the
     * shorter sequence is less than the other one.
     */
    int lt_io_value_compare( const lt_io_value_t * a,
            const lt_io_value_t * b );

    /**
     * Routines to access members of a lt_io_value_t
     * ===============================================
     */

    /**
     * Extract type of a lt_io_value_t
     */
    lt_io_value_type_e lt_io_value_type_get(
            const lt_io_value_t * val );

    /**
     * Extract BIT value from a lt_io_value_t
     */
    uint8_t lt_io_value_bit_get( const lt_io_value_t * val );

    /**
     * Extract NUMBER value from a lt_io_value_t
     */
    int32_t lt_io_value_number_get( const lt_io_value_t * val );

    /**
     * Extract length of a STRING value from a lt_io_value_t.
     */
    uint32_t lt_io_value_string_length_get( const lt_io_value_t * val );

    /**
     * Extract STRING value from a lt_io_value_t. It actually returns a
     * buffer containing length + 1 characters. The trailing one is always
     * zero.
     */
    uint8_t * lt_io_value_string_get( const lt_io_value_t * val );

    /**
     * Extract length of a SEQUENCE value from a lt_io_value_t.
     */
    uint32_t lt_io_value_sequence_length_get( const lt_io_value_t * val );

    /**
     * Extract n-th item from a sequence lt_io_value_t.
     */
    lt_io_value_t * lt_io_value_sequence_item_get(
            const lt_io_value_t * val,
            uint32_t i );

    /**
     * Routines to change members of a lt_io_value_t
     * ===============================================
     */

    /**
     * Set type of a lt_io_value_t.
     * return null in case of problem or given arg.
     */
    lt_io_value_t * lt_io_value_type_set( lt_io_value_t * val,
            lt_io_value_type_e type );

    /**
     * Set BIT value to a lt_io_value_t, return self or null.
     */
    lt_io_value_t * lt_io_value_bit_set( lt_io_value_t * val,
            uint8_t v );

    /**
     * Set NUMBER value to a lt_io_value_t, return self or null.
     */
    lt_io_value_t * lt_io_value_number_set( lt_io_value_t * val,
            int32_t v );

    /**
     * Set STRING value to a lt_io_value_t, return self or null.
     * previous contents of the string (if any) will be freed.
     */
    lt_io_value_t * lt_io_value_string_set( lt_io_value_t * val,
            const uint8_t * v, uint32_t len );

    /**
     * Set SEQUENCE length, return self or null.
     * previous contents of a sequence will be freed.
     */
    lt_io_value_t * lt_io_value_sequence_length_set(
            lt_io_value_t * val,
            uint32_t len );

    /**
     * Set i-th item of the SEQUENCE val to the item. previous item (if any)
     * will be freed.
     */
    lt_io_value_t * lt_io_value_sequence_item_set(
            lt_io_value_t * val,
            uint32_t i, lt_io_value_t * item );

    /**
     * Routines to decode lt_io_value_t from lt_io_decode_buffer_t
     * ===============================================================
     */

    /**
     * decode next value from the stream.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_decode( lt_io_decode_buffer_t * db,
            apr_pool_t * pool );

    /**
     * internal function.
     * determine type of the next value encoded in a stream.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_type_e lt_io_value_next_type_get(
            const lt_io_decode_buffer_t * db );

    /**
     * internal function.
     * decode null from the stream and shift offset appropriately.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_null_decode( lt_io_decode_buffer_t * db,
            apr_pool_t * pool );

    /**
     * internal function.
     * decode bit from the stream and shift offset appropriately.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_bit_decode( lt_io_decode_buffer_t * db,
            apr_pool_t * pool );

    /**
     * internal function.
     * decode number from the stream and shift offset appropriately.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_number_decode(
            lt_io_decode_buffer_t * db,
            apr_pool_t * pool );

    /**
     * internal function.
     * decode string from the stream and shift offset appropriately.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_string_decode(
            lt_io_decode_buffer_t * db,
            apr_pool_t * pool );

    /**
     * internal function.
     * decode sequence from the stream and shift offset appropriately.
     * return null in case of problem.
     *
     * when NULL pool is provided new one will be created.
     */
    lt_io_value_t * lt_io_value_sequence_decode(
            lt_io_decode_buffer_t * db,
            apr_pool_t * pool );

    /**
     * Routines to encode lt_io_values to lt_io_encode_buffer_t
     * ============================================================
     */

    /**
     * encode io_value to the stream.
     * return -1 in case of problem.
     */
    int8_t lt_io_value_encode( lt_io_encode_buffer_t * b,
            const lt_io_value_t * val );

    /**
     * internal function.
     * encode null to the stream and shift offset appropriately.
     * note that NULL val is encoded correctly to the stream as the io value
     * of type LT_IO_TYPE_NULL.
     * return -1 in case of problem.
     */
    int8_t lt_io_value_null_encode( lt_io_encode_buffer_t * b,
            const lt_io_value_t * val );

    /**
     * internal function.
     * encode bit to the stream and shift offset appropriately.
     * return -1 in case of problem.
     */
    int8_t lt_io_value_bit_encode( lt_io_encode_buffer_t * b,
            const lt_io_value_t * val );

    /**
     * internal function.
     * encode number to the stream and shift offset appropriately.
     * return -1 in case of problem.
     */
    int8_t lt_io_value_number_encode( lt_io_encode_buffer_t * b,
            const lt_io_value_t * val );

    /**
     * internal function.
     * encode string to the stream and shift offset appropriately.
     * return -1 in case of problem.
     */
    int8_t lt_io_value_string_encode( lt_io_encode_buffer_t * b,
            const lt_io_value_t * val );

    /**
     * internal function.
     * encode sequence to the stream and shift offset appropriately.
     * return -1 in case of problem.
     */
    int8_t lt_io_value_sequence_encode( lt_io_encode_buffer_t * b,
            const lt_io_value_t * val );

    /**
     * Routines to help store lt_io_values within APR Hash Tables
     * ============================================================
     */
    /**
     * Convert an IO Value into the string KEY that can be used to place
     * such IO Value into APR Hash table. The length of the key can be
     * determined by using strlen, e.g., use APR_HASH_KEY_STRING to indicate
     * the required key length.
     *
     * Note: null io values return the key "null"
     * Note: bit io values return the key "0" or "1"
     * Note: numeric io values return the output of itoa as a key
     * Note: string io values return its contents as a key
     * Note: sequence io values return its address as a key, this is bug and
     * will be fixed in feature versions, don't rely on this behavior.
     *
     * Note: that at this moment two identical sequence io values, that are
     * equal (e.g., lt_io_value_equals returns true for them) wont get the
     * same strkey, since for sequences the key is created from the sequence
     * address, not the contents.
     */
    const char * lt_io_value_2_strkey( const lt_io_value_t * val );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_IO_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: io.h 347 2005-02-23 14:56:10Z modesto $
 */
