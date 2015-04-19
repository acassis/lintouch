/* $Id: $
 *
 *   FILE: modbus.h --
 * AUTHOR: Jiri Barton <jbar@swac.cz>
 *   DATE: 29 April 2005
 *
 * Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
 * Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
 *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 * Based on the implementation of Rainer Lehrig, see below.
 */

/***************************************************************************
                          rlmodbus.h    -  description
                             -------------------
    begin                : Tue Mar 13 2003
    copyright            : (C) 2003 by Rainer Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/

#ifndef _MODBUS_H_
#define _MODBUS_H_

#include <apr_poll.h>

/* Initialize and return all the necessary data associated with this
   connection. */
void *modbus_create(apr_pool_t *p);

/* Free the associated data. */
void modbus_destroy(void *);

/* Form a response from the data and send out the resulting frame. */
apr_status_t modbus_response(const apr_pollfd_t *desc, int slave, int function,
    const apr_byte_t *data, apr_size_t datalen);

/* Process the incoming frame. */
apr_status_t modbus_indication(const apr_pollfd_t *desc, int *slave,
    int *function, apr_byte_t *data);

#endif

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: $
 */
