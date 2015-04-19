/* $Id: logger.c 1132 2005-11-08 08:10:44Z modesto $
 *
 *   FILE: logger.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 14 April 2005
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
#include <stdarg.h>

#include <apr_errno.h>

#include "lt/server/logger.h"

static int _logger_priority = 0;

int lt_server_logger_priority_get()
{
    return _logger_priority;
}

void lt_server_logger_priority_set( int priority )
{
    _logger_priority = priority;
}

void lt_server_logger_print( int priority, const char * format, ... )
{
    va_list ap;
    if( priority > _logger_priority ) return;
    va_start( ap, format );
    vprintf( format, ap );
    va_end( ap );
    fflush( stdout );
}

void lt_server_logger_print_apr( int priority, const char * aprcall,
        const char * format, ... )
{
    va_list ap;
    char error[1025];
    if( priority > _logger_priority ) return;
    va_start( ap, format );
    vprintf( format, ap );
    va_end( ap );
    apr_strerror( apr_get_os_error(), error, 1024 );
    printf( "APR ERROR while calling %s: (%li) %s\n",
        aprcall, (long)apr_get_os_error(), error );
    fflush( stdout );
}

void lt_server_logger_print_apr_xml( int priority,
        apr_xml_parser * parser,
        const char * format, ... )
{
    va_list ap;
    char error[1025];
    if( priority > _logger_priority ) return;
    va_start( ap, format );
    vprintf( format, ap );
    va_end( ap );
    apr_xml_parser_geterror( parser, error, 1024 );
    printf( "APR XML ERROR: %s\n", error );
    fflush( stdout );
}

void lt_server_logger_print_apr_dso( int priority,
        apr_dso_handle_t * handle,
        const char * format, ... )
{
    va_list ap;
    char error[1025];
    if( priority > _logger_priority ) return;
    va_start( ap, format );
    vprintf( format, ap );
    va_end( ap );
    apr_dso_error( handle, error, 1024 );
    printf( "APR DSO ERROR: %s\n", error );
    fflush( stdout );
}

void lt_server_logger_print_socket( int priority, const char * aprcall,
        const char * format, ... )
{
    va_list ap;
    char error[1025];
    if( priority > _logger_priority ) return;
    va_start( ap, format );
    vprintf( format, ap );
    va_end( ap );
    apr_strerror( apr_get_netos_error(), error, 1024 );
    printf( "APR SOCKET ERROR while calling %s: (%li) %s\n",
        aprcall, (long)apr_get_os_error(), error );
    fflush( stdout );
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: logger.c 1132 2005-11-08 08:10:44Z modesto $
 */
