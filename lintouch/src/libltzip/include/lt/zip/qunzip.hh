/* $Id: qunzip.hh,v 1.2 2004/08/11 20:17:55 jbar Exp $
 *
 *   FILE: qunzip.hh --
 * AUTHOR: Hynek Petrak <hynek@swac.cz>
 *   DATE: 18 November 2003
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

#ifndef _QUNZIP_H
#define _QUNZIP_H

#include <qstring.h>
#include <qiodevice.h>
#include <qdatetime.h>
#include <qbuffer.h>

namespace lt {

/**
 * Class that exctract files from .zip archives. This class uses QIODevice
 * for file operations exclusively.
 */
class QUnZip {
    public:
        /**
         * Constructor
         */
        QUnZip();
        /**
         * Destructor
         */
        virtual ~QUnZip();
        /**
         * Opens a zip file
         * @param d QIODevice representing an archived file. QIODevice must
         * be open read access and it must be a direct access device.
         */
        bool open(QIODevice *d);
        /**
         * @returns true if zip archive was successfuly open
         */
        bool isOpen() const;
        /**
         * Closes the zip archive.
         */
        void close();
        /**
         * @returns Number of files within archive.
         */
        int getFileCount();
        /**
         * Retrives global zip comment.
         * @param comment returned zip comment.
         * @returns true on success.
         */
        bool getGlobalComment(QString &comment);
        /**
         * Sets "current file" pointer to the first file in an archive
         * @returns true on success
         */
        bool gotoFirstFile();
        /**
         * Sets "current file" pointer to the next file in an archive
         * @returns true on success
         */
        bool gotoNextFile();
        /**
         * Locates file within an archive according to givven name.
         * Sets "current file" pointer to the file located.
         * @param name name of the file (including path)
         * @param casesensitive Should the search be case sensitive?
         * @returns true on success
         */
        bool locateFile(const QString &name, bool casesensitive = false);
        /**
         * Retrieves name, extra filed and comment for the current file.
         * @param name Name retrieved.
         * @param d Time parameter of the file. Might be NULL.
         * @returns true on success.
         */
        bool getCurrentFileInfo(QString &name, QDateTime *d = NULL);
        /**
         * Read current file from archive into QIODevice d.
         * QIODevice must be open for writing otherwise the funtion fails.
         * The QIODevice remains open after return. You have to re-open it
         * for reading!
         * @param d Content of the current file from archive.
         * @returns true on success, false otherwise.
         */
        bool getCurrentFile(QIODevice &d);
        /**
         * Read current file from archive into QIODevice d without
         * decompression.
         * QIODevice must be open for writing otherwise the funtion fails.
         * The QIODevice remains open after return. You have to re-open it
         * for reading!
         * @param d Content of the current file from archive.
         * @param method Method of decompression.
         * @param crc crc
         * @param uncompressed_size uncompressed_size
         * @returns true on success, false otherwise.
         */
        bool getCurrentFileRaw(QIODevice &d, int &method,
                unsigned long &crc, unsigned long &uncompressed_size);
    protected:
    private:
        struct QUnZipPrivate;
        QUnZipPrivate *_d;
};

} // namespace lt

#endif /* _QUNZIP_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: qunzip.hh,v 1.2 2004/08/11 20:17:55 jbar Exp $
 */
