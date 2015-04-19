/* $Id: modbus.h 1520 2006-03-08 12:22:15Z hynek $
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
 */

#ifndef _MODBUS_H_
#define _MODBUS_H_

#include <apr_poll.h>
#include <apr_hash.h>

/* the type of the starting address (now 2 byte unsigned integer) */
typedef apr_uint16_t sa_t;

/* offsets within the modbus outgoing frame */
#define TRANSACTIONIDOFFSET 0
#define LENOFFSET 5
#define SLAVEOFFSET 6
#define FUNCTIONOFFSET 7
#define DATAOFFSET 8

#define FRAMELEN 256

/* exception codes */
enum ModbusFunctionCodes
{
  READ_COILS                   = 1,
  READ_DISCRETE_INPUTS         = 2,
  READ_HOLDING_REGISTERS       = 3,
  READ_INPUT_REGISTERS         = 4,
  WRITE_SINGLE_COIL            = 5,
  WRITE_SINGLE_REGISTER        = 6,
  WRITE_MULTIPLE_COILS         = 15,
  WRITE_MULTIPLE_REGISTERS     = 16,
  MASK_WRITE_REGISTER          = 22,
  READWRITE_MULTIPLE_REGISTERS = 23
};

enum DataTypes
{
    COILS, DISCRETE_INPUTS, HOLDINGREGISTERS, INPUT_REGISTERS
};

/* Initialize and return all the necessary data associated with this
   connection. */
void *modbus_create(apr_pool_t *p, apr_array_header_t *addressspace[],
    sa_t addressspacestart[]);

/* Free the associated data. */
void modbus_destroy(void *);

/* Process the incoming frame from the client and send out the answer
frame. */
apr_status_t modbus_indication(const apr_pollfd_t *desc);

/* Process the incoming frame from the server and store the result to
the allocated array. */
apr_status_t modbus_response(apr_socket_t *s, apr_uint16_t *transaction,
    apr_byte_t *function, apr_byte_t *data, apr_byte_t *datalen);

#endif

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: modbus.h 1520 2006-03-08 12:22:15Z hynek $
 */
