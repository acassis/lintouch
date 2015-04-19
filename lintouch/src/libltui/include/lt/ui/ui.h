// $Id: ui.h,v 1.1 2004/01/15 13:11:56 mman Exp $
//
//   FILE: ui.h -- 
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

#ifndef _UI_HH
#define _UI_HH

#include <qapplication.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Initialize ui library.
     *
     * This call must be first one invoked by your application prior
     * usage of any of the classes defined by this library.
     *
     * @return -1 in case of error, you should immediately exit your
     * application is such case.
     */
    int lt_ui_initialize(void);

    /**
     * Terminate panel library.
     *
     * Call this function after you are finished with the classes from
     * this library. We suggest using atexit(2) to ensure this function
     * will get called before the application terminates.
     */
    void lt_ui_terminate(void);

#ifdef __cplusplus
}
#endif

    /**
     * Try to load localization .qm files from various locations.
     * The following locations will be tried:
     *
     * - $ac_prefix/share/lintouch/locale/$prefix*
     * - $app_path/../share/lintouch/locale/$prefix*
     * - $dir/$prefix*
     * - locale/$prefix*
     * - ./$prefix*
     *
     * where ac_prefix represents the --prefix used when configuring
     * software and app_path stands for qApp->applicationDirPath().
     */
    void loadLocalizations( QApplication & a, 
            const QString & dir, const QString & prefix );

#endif /* _UI_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: ui.h,v 1.1 2004/01/15 13:11:56 mman Exp $
