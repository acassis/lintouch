// $Id: ltproject.c 347 2005-02-23 14:56:10Z modesto $
//
//   FILE: project.c -- 
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

#include <lt/templates/templates.h>

#include "lt/project/project.h"

int lt_project_initialize(void) {

    // initialize Templates library
    if( lt_templates_initialize() == -1 ) {
        fprintf( stderr, "Unable to initialize Lintouch Templates Library\n" );
        return -1;
    }

    // initialize our own structures

    return 0;
}

void lt_project_terminate(void) {

    // terminate our own structures

    // terminate Templates library
    lt_templates_terminate();
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: ltproject.c 347 2005-02-23 14:56:10Z modesto $
