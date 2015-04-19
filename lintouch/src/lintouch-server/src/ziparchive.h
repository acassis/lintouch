/* $Id: ziparchive.h 593 2005-04-27 09:46:33Z mman $
 *
 *   FILE: ziparchive.h --
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

#ifndef _LT_ZIPARCHIVE_H
#define _LT_ZIPARCHIVE_H

#include <apr_file_io.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Extract a file from zip archive.
     *
     * @param archive The zip archive.
     * @param filename The filename to be extracted.
     * @param pool The pool to allocate file from.
     * @return Extracted file opened for reading/writing. The file will be
     * automatically deleted when closed. NULL is returned in case of
     * problem.
     */
    apr_file_t * lt_server_zip_extract_file(
            const char * archive, const char * filename,
            apr_pool_t * pool );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_ZIPARCHIVE_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: ziparchive.h 593 2005-04-27 09:46:33Z mman $
 */

