/* $Id: ziparchive.c 1774 2006-05-31 13:17:16Z hynek $
 *
 *   FILE: ziparchive.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 20 April 2005
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

#include <stdio.h>
#include <stdlib.h>

#include <apr_strings.h>

#include "lt/server/plugin.h"
#include "lt/server/logger.h"
#include "unzip.h"
#include "ziparchive.h"

/* LOCAL METHODS */

void fill_my_filefunc OF((zlib_filefunc_def* pzlib_filefunc_def));
voidpf ZCALLBACK my_open_file_func OF((
            voidpf opaque, const char* filename, int mode));

uLong ZCALLBACK my_read_file_func OF((
            voidpf opaque, voidpf stream, void* buf, uLong size));

uLong ZCALLBACK my_write_file_func OF((
            voidpf opaque, voidpf stream, const void* buf, uLong size));

long ZCALLBACK my_tell_file_func OF((
               voidpf opaque, voidpf stream));

long ZCALLBACK my_seek_file_func OF((
            voidpf opaque, voidpf stream, uLong offset, int origin));

int ZCALLBACK my_close_file_func OF((
            voidpf opaque, voidpf stream));

int ZCALLBACK my_error_file_func OF((
            voidpf opaque, voidpf stream));

/* METHOD BODIES */

apr_file_t * lt_server_zip_extract_file(
        const char * archive, const char * filename,
        apr_pool_t * pool )
{
    /* local variables */
    apr_file_t * extr_to = NULL;
    apr_off_t o = 0;
    char* tmpl = NULL;
    char* tmpd = NULL;

    /* find temporary directory */
    if(APR_SUCCESS !=apr_temp_dir_get(&tmpd, pool)) {
        lt_server_logger_print_apr( PRIO_PROJECT, "apr_temp_dir_get",
                "Unable to get temporary storage\n");
        return NULL;
    }
    tmpl = apr_pstrcat(pool, tmpd, "/ltXXXXXX", NULL);

    /* unzip related variables */
    apr_file_t * zipfile = NULL;
    unzFile myzip;
    zlib_filefunc_def myzip_callbacks;

    /* open zip archive */
    if( APR_SUCCESS !=
                apr_file_open( &zipfile, archive, APR_READ, 0, pool ) ) {
        lt_server_logger_print_apr( PRIO_PROJECT, "apr_file_open",
                "Unable to open archive %s\n", archive );
        return NULL;
    }

    /* open temporary file that will be used for extracted data. */
    if( APR_SUCCESS !=
                apr_file_mktemp( &extr_to, tmpl, 0, pool ) ) {
        lt_server_logger_print_apr( PRIO_PROJECT, "apr_file_mktemp",
                "Unable to open temporary file\n" );
        apr_file_close( zipfile );
        return NULL;
    }

    /* setup unzipping engine */
    fill_my_filefunc( &myzip_callbacks );
    myzip_callbacks.opaque = zipfile;

    /* initialize unzipping engine */
    myzip = unzOpen2( "", &myzip_callbacks );
    if( myzip == NULL ) {
        lt_server_logger_print( PRIO_PROJECT,
                "Unable to unzOpen2 archive %s\n", archive );
        apr_file_close( zipfile );
        apr_file_close( extr_to );
        return NULL;
    }

    /* locate the filename and set it as current */
    if( UNZ_OK != unzLocateFile( myzip, filename, 1 ) ) {
        lt_server_logger_print( PRIO_PROJECT,
                "Unable to unzLocateFile %s in archive %s\n",
                filename, archive );
        unzClose( myzip );
        myzip = NULL;
        apr_file_close( extr_to );
        return NULL;
    }

    /* open current file */
    if( UNZ_OK != unzOpenCurrentFile( myzip ) ) {
        lt_server_logger_print( PRIO_PROJECT,
                "Unable to unzOpenCurrentFile %s in archive %s\n",
                filename, archive );
        unzClose( myzip );
        myzip = NULL;
        apr_file_close( extr_to );
        return NULL;
    }

    /* in a loop, read all data from the current file in a zip and store it
     * to the temp file. */
    {
        char buffer[ 1024 ];
        apr_size_t len = 0;
        apr_size_t written = 0;
        while( ( len = unzReadCurrentFile( myzip, buffer, 1024 ) ) > 0 ) {
            written = len;
            if( APR_SUCCESS !=
                        apr_file_write( extr_to, buffer, &written ) ) {
                lt_server_logger_print_apr( PRIO_PROJECT, "apr_file_write",
                        "Unable to write %d extracted bytes to a temporary file"
                        " (%d written)\n", len, written );
                unzClose( myzip );
                myzip = NULL;
                apr_file_close( extr_to );
                return NULL;
            }
        }
    }

    /* close current file */
    if( UNZ_OK != unzCloseCurrentFile( myzip ) ) {
        lt_server_logger_print( PRIO_PROJECT,
                "Unable to unzCloseCurrentFile %s in archive %s\n",
                filename, archive );
        unzClose( myzip );
        myzip = NULL;
        apr_file_close( extr_to );
        return NULL;
    }

    /* finish unzipping engine ( will also apr_file_close the zipfile through
     * callback )*/
    unzClose( myzip );
    myzip = NULL;

    /* reset the extr_to seek pointer */
    o = 0;
    if( APR_SUCCESS != apr_file_seek( extr_to, APR_SET, &o ) ) {
        lt_server_logger_print_apr( PRIO_PROJECT, "apr_file_seek",
                "Unable to seek file pointer to the beginning\n" );
        apr_file_close( extr_to );
        return NULL;
    }

    return extr_to;
}

voidpf ZCALLBACK
my_open_file_func( voidpf opaque, const char* filename, int mode )
{
    return opaque;
}

uLong ZCALLBACK
my_read_file_func( voidpf opaque, voidpf stream, void* buf, uLong size )
{
    apr_file_t * d = (apr_file_t*) opaque;
    apr_size_t nbytes = size;
    apr_status_t rv = apr_file_read( d, buf, &nbytes );
    if( APR_SUCCESS == rv ) return nbytes;
    return 0;
}

uLong ZCALLBACK
my_write_file_func( voidpf opaque, voidpf stream,
        const void* buf, uLong size )
{
    apr_file_t * d = (apr_file_t*) opaque;
    apr_size_t nbytes = size;
    apr_status_t rv = apr_file_write( d, buf, &nbytes );
    if( APR_SUCCESS == rv ) return nbytes;
    return 0;
}

long ZCALLBACK
my_tell_file_func( voidpf opaque, voidpf stream )
{
    apr_file_t * d = (apr_file_t*) opaque;
    apr_off_t o = 0;
    apr_status_t rv = apr_file_seek( d, APR_CUR, &o );
    if( APR_SUCCESS == rv ) return o;
    return -1;
}

long ZCALLBACK
my_seek_file_func( voidpf opaque, voidpf stream, uLong offset, int origin )
{
    apr_file_t * d = (apr_file_t*) opaque;
    apr_off_t o = offset;
    apr_status_t rv;

    switch (origin) {
        case ZLIB_FILEFUNC_SEEK_CUR:
            rv = apr_file_seek( d, APR_CUR, &o );
            break;
        case ZLIB_FILEFUNC_SEEK_END:
            rv = apr_file_seek( d, APR_END, &o );
            break;
        case ZLIB_FILEFUNC_SEEK_SET:
            rv = apr_file_seek( d, APR_SET, &o );
            break;
        default:
            return -1;
    }
    return 0;
}

int ZCALLBACK
my_close_file_func( voidpf opaque, voidpf stream )
{
    apr_file_close( (apr_file_t*) opaque );
    return 0;
}

int ZCALLBACK
my_error_file_func( voidpf opaque, voidpf stream )
{
    return 0;
}

void
fill_my_filefunc( zlib_filefunc_def * pzlib_filefunc_def )
{
    pzlib_filefunc_def->zopen_file = my_open_file_func;
    pzlib_filefunc_def->zread_file = my_read_file_func;
    pzlib_filefunc_def->zwrite_file = my_write_file_func;
    pzlib_filefunc_def->ztell_file = my_tell_file_func;
    pzlib_filefunc_def->zseek_file = my_seek_file_func;
    pzlib_filefunc_def->zclose_file = my_close_file_func;
    pzlib_filefunc_def->zerror_file = my_error_file_func;
    pzlib_filefunc_def->opaque=NULL;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: ziparchive.c 1774 2006-05-31 13:17:16Z hynek $
 */
