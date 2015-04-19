/* $Id: logger.h 1132 2005-11-08 08:10:44Z modesto $
 *
 *   FILE: logger.h --
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

#include <apr_dso.h>
#include <apr_xml.h>

#ifndef _LT_LOGGER_H
#define _LT_LOGGER_H

/**
 * @file logger.h
 * @brief Lintouch Server Logging Tools.
 */

/**
 * @defgroup LTLG Lintouch Server Logging Tools
 * @{
 *
 * Lintouch Server Logging Tools.
 *
 * Methods in this module allow you to print messages with given priority.
 * They are always printed to stdout and filtered according to the currently
 * set priority.
 *
 * @author Martin Man <mman@swac.cz>
 */

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        PRIO_DEFAULT = 0,
        PRIO_PROJECT = 1,
        PRIO_SOCKET = 2,
        PRIO_THREAD = 3,
        PRIO_PLUGIN = 4,
    } lt_server_logger_priority_e;

    /**
     * Return the currently set logging priority.
     *
     * @return The logger priority.
     */
    int lt_server_logger_priority_get();

    /**
     * Set current logging priority.
     *
     * @param priority Priority to be set.
     */
    void lt_server_logger_priority_set(
            int priority );

    /**
     * Log a message with given priority.
     *
     * The message will be actually printed to stdout if the priority set by
     * lt_server_logger_priority_set will be higher or equal with the
     * indicated message priority
     *
     * @param priority The message priority.
     * @param format The message format as in printf(3).
     * @param ... The message arguments.
     */
    void lt_server_logger_print( int priority, const char * format,
            ... );

    /**
     * Log a message with given priority and append apr related error
     * message.
     *
     * The message will be actually printed to stdout if the priority set by
     * lt_server_logger_priority_set will be higher or equal with the
     * indicated message priority
     *
     * @param priority The message priority.
     * @param aprcall The apr call that failed.
     * @param format The message format as in printf(3).
     * @param ... The message arguments.
     */
    void lt_server_logger_print_apr( int priority, const char * aprcall,
            const char * format,
            ... );

    /**
     * Log a message with given priority and append apr xml related error
     * message.
     *
     * The message will be actually printed to stdout if the priority set by
     * lt_server_logger_priority_set will be higher or equal with the
     * indicated message priority
     *
     * @param priority The message priority.
     * @param parser The apr xml parser that failed.
     * @param format The message format as in printf(3).
     * @param ... The message arguments.
     */
    void lt_server_logger_print_apr_xml( int priority,
            apr_xml_parser * parser,
            const char * format,
            ... );

    /**
     * Log a message with given priority and append apr dso related error
     * message.
     *
     * The message will be actually printed to stdout if the priority set by
     * lt_server_logger_priority_set will be higher or equal with the
     * indicated message priority
     *
     * @param priority The message priority.
     * @param handle The apr dso handle that failed.
     * @param format The message format as in printf(3).
     * @param ... The message arguments.
     */
    void lt_server_logger_print_apr_dso( int priority,
            apr_dso_handle_t * handle,
            const char * format,
            ... );

    /**
     * Log a message with given priority and append apr related error
     * message.
     *
     * The message will be actually printed to stdout if the priority set by
     * lt_server_logger_priority_set will be higher or equal with the
     * indicated message priority
     *
     * @param priority The message priority.
     * @param aprcall The apr call that failed.
     * @param format The message format as in printf(3).
     * @param ... The message arguments.
     */
    void lt_server_logger_print_socket( int priority, const char * aprcall,
            const char * format,
            ... );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _LT_LOGGER_H */

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: logger.h 1132 2005-11-08 08:10:44Z modesto $
 */

