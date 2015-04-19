/* $Id: qzip.hh,v 1.4 2004/08/11 20:17:55 jbar Exp $
 *
 *   FILE: qzip.hh --
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

#ifndef _QZIP_H
#define _QZIP_H

#include <qstring.h>
#include <qiodevice.h>
#include <qdatetime.h>

namespace lt {

/**
 * Class that stores files to .zip archives. This class uses QIODevice
 * for file operations exclusively.
 */
class QZip {
    public:
        /**
         * Constructor
         */
        QZip();
        /**
         * Destructor
         */
        virtual ~QZip();
        /**
         * Opens a zip file for writing. If append=false make sure d is an
         * empty file.
         * @param d QIODevice representing an archived file. QIODevice must
         * be open for read and write and it must be a direct access device.
         * @param append If true Qzip will append files to existing zip
         * archive. If false Qzip will ignore current contens.
         */
        bool open(QIODevice *d, bool append = false);
        /**
         * @returns true if the zip archive was successfuly open
         */
        bool isOpen() const;
        /**
         * Closes the zip archive.
         */
        void close();
        /**
         * Sets the glogal comment of the zip file.
         * Beware: when the zip archive is open for appending. The old comment
         * is silently discarded. It is a bug in zip library used. I'll
         * write a workaround in a near future.
         * @param comment Global comment.
         * @returns true on success
         */
        bool setGlobalComment(const QString &comment);
        /**
         * Stores file into the archive
         * @param name Name of the file including the path name
         * (e.g. "dir1/dir2/file.ext")
         * @param file File to be stored into the archive. It is passed
         * as QIODevice which must be open for read access.
         * @param time Time to store as file attribute.
         * @returns true on success, false otherwise.
         */
        bool storeFile(const QString &name, QIODevice &file,
                const QDateTime &time = QDateTime::currentDateTime());
        /**
         * Stores file into the archive without comressing it. Usualy it is
         * a file obtained by calling QUnZip::getCurrentFileRaw method.
         * @param name Name of the file including the path name
         * (e.g. "dir1/dir2/file.ext")
         * @param file File to be stored into the archive. It is passed
         * as QIODevice which must be open for read access.
         * @param t Time to store as file attribute.
         * @param method Method of commression.
         * @param crc Crc of uncompressed file.
         * @param uncompressed_size Size of uncompressed file.
         * @returns true on success, false otherwise.
         */
        bool storeFileRaw(const QString &name, QIODevice &file,
                const QDateTime &t, int method,
                unsigned long crc, unsigned long uncompressed_size);
    protected:
    private:
        struct QZipPrivate;
        QZipPrivate *_d;
};

} // namespace lt

#endif /* _QZIP_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: qzip.hh,v 1.4 2004/08/11 20:17:55 jbar Exp $
 */
