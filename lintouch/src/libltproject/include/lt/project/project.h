// $Id: project.h 347 2005-02-23 14:56:10Z modesto $
//
//   FILE: project.h -- 
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

#ifndef _LTPROJECT_HH
#define _LTPROJECT_HH

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Initialize project library.
     *
     * This call must be first one invoked by your application prior
     * usage of any of the classes defined by this library.
     *
     * @return -1 in case of error, you should immediately exit your
     * application is such case.
     */
    int lt_project_initialize(void);

    /**
     * Terminate project library.
     *
     * Call this function after you are finished with the classes from
     * this library. We suggest using atexit(2) to ensure this function
     * will get called before the application terminates.
     */
    void lt_project_terminate(void);

#ifdef __cplusplus
}
#endif

#endif /* _LTPROJECT_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: project.h 347 2005-02-23 14:56:10Z modesto $
