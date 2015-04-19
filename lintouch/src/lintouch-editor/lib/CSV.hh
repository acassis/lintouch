// $Id: CSV.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: CSV.hh --
// AUTHOR: Martin Man <mman@swac.cz>
//   DATE: 05 October 2004
//
// Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
//
// THIS SOFTWARE IS DISTRIBUTED AS IS AND WITHOUT ANY WARRANTY.
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

#ifndef _CSV_HH
#define _CSV_HH

#include <qiodevice.h>

#include <lt/templates/Connection.hh>

/**
 * Parse given lines of CSV file and try to create Lintouch Connection
 * out of them.
 *
 * @param lines The CSV file to parse.
 * @return Newly created connection.
 */
Connection * csv2connection( const QStringList & lines );

/**
 * Export a Lintouch Connection to a CSV file.
 *
 * @param conn The connection to be exported.
 * @param delim The CSV delimiter.
 * @return The CSV lines to be written to the file (including line
 * endings).
 */
QStringList connection2csv( const Connection * conn );

/**
 * Encode given string into a CSV representation. It will quote the
 * string if required and it will double all quotes inside the string.
 *
 * Example:
 *
 * That is, this will be "quoted" -> "That is, this will be ""quoted"""
 *
 * @param src The string to be CSV encoded.
 * @return Encoded string.
 */
QString CSVEncodeField( const QString & src );

/**
 * Encode given string list into a CSV representation.
 *
 * @param src The string list to be CSV encoded.
 * @return Encoded string.
 */
QString CSVEncodeRecord( const QStringList & src );

/**
 * Decode given CSV record into a list of fields.
 *
 * @param line The line to be parsed (can contain \r\n at the end).
 * @return The list of fields
 */
QStringList CSVDecodeRecord( const QString & line );

#endif /* _CSV_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: CSV.hh 1169 2005-12-12 15:22:15Z modesto $
