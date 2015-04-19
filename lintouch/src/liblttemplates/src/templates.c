// $Id: templates.c 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: templates.c -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 15 January 2004
//
// Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
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

#include <stdio.h>
#include <apr_general.h>

#include "lt/templates/templates.h"

int lt_templates_initialize(void) {

    apr_status_t rv;
    char buf[ 2048 ];

    // initialize APR library
    if( APR_SUCCESS != ( rv = apr_initialize() ) ) {
        fprintf( stderr, "Unable to initialize APR Library\n" );
        fprintf( stderr, "APR Error: %s\n",
                apr_strerror( rv, buf, 2048 ) );
        return -1;
    }

    // initialize our own structures

    return 0;
}

void lt_templates_terminate(void) {

    // terminate our own structures

    // terminate APR library
    apr_terminate();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: templates.c 1774 2006-05-31 13:17:16Z hynek $
