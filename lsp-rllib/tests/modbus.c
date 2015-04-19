/* $Id: $
 *
 *   FILE: modbus.c --
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
                          rlmodbus.cpp  -  description
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

#include "modbus.h"
#include <string.h>

#define FRAMELEN 256

enum ModbusFunctionCodes
{
  ReadCoilStatus         = 1,
  ReadInputStatus        = 2,
  ReadHoldingRegisters   = 3,
  ReadInputRegisters     = 4,
  ForceSingleCoil        = 5,
  PresetSingleRegister   = 6,
  ReadExceptionStatus    = 7,
  FetchCommEventCtr      = 11,
  FetchCommEventLog      = 12,
  ForceMultipleCoils     = 15,
  PresetMultipleRegs     = 16,
  ReportSlaveID          = 17,
  ReadGeneralReference   = 20,
  WriteGeneralReference  = 21,
  MaskWrite4XRegisters   = 22,
  ReadWrite4XRegisters   = 23,
  ReadFifoQueue          = 24
};

/* Initialize and return all the necessary data associated with this
   connection. */
void *modbus_create(apr_pool_t *p)
{
    //allocate the room for a modbus frame (telegram)
    return apr_pcalloc(p, FRAMELEN);
}

/* Free the associated data. */
void modbus_destroy(void *client_data)
{
}

/* Form a response from the data and send out the resulting frame. */
apr_status_t modbus_response(const apr_pollfd_t *desc, int slave, int function,
    const apr_byte_t *data, apr_size_t datalen)
{
  apr_size_t len = 0;
  apr_byte_t *tel = (apr_byte_t *) desc->client_data;

  tel[len++] = 0;          // bytes 0,1 Transaction ID. Not important.
                           // Usually zero when making a request, the server
                           // will copy them faithfully into the response.
  tel[len++] = 0;
  tel[len++] = 0;         // bytes 2,3 Protocol number. Must be zero.
  tel[len++] = 0;
  tel[len++] = 0;         // byte 4 Length (upper byte). Since all requests
                          // will be less than 256 bytes in length (!),
                          // this will always be zero.
  tel[len++] = (apr_byte_t) 2+datalen; // byte 5 Length (lower byte).
                          // Equal to the number of bytes which follow
  tel[len++] = (apr_byte_t) slave;

  if (slave < 0 || slave > 255 || datalen > FRAMELEN)
  {
      tel[len++] = (apr_byte_t) function + 128;
      tel[len++] = 4; //implementation error
  }
  else
  {
      tel[len++] = (apr_byte_t) function;
      memcpy(tel + len, data, datalen);
      len += datalen;
  }

  return apr_socket_send(desc->desc.s, (char *) tel, &len);
}

/* Read the count bytes into the buffer and check if the actual number of
   the read bytes is as expected (=count) */
apr_status_t _modbus_read_chunk(apr_socket_t *s, apr_byte_t *buffer,
    apr_size_t count)
{
  apr_size_t actualread = count;
  int i; //debug only
  char errstr[1024];
  apr_status_t status = apr_socket_recv(s, (char *) buffer, &actualread);
  for (i = 0; i < actualread; i++) printf("%02x", (int) buffer[i]);
  printf(" %s\n", apr_strerror(status, errstr, 1024));
  return status == APR_SUCCESS ?
      (count == actualread ? APR_SUCCESS : APR_INCOMPLETE) : status;
}

/* Process the incoming frame. */
apr_status_t modbus_indication(const apr_pollfd_t *desc, int *slave,
    int *function, apr_byte_t *data)
{
  apr_byte_t *tel = (apr_byte_t *) desc->client_data;
  apr_socket_t *s = desc->desc.s;
  apr_status_t status;

  if ((status = _modbus_read_chunk(s, tel, 6)) != APR_SUCCESS) return status;
  // bytes 0,1 Transaction ID faithfully copied from the request message
  // bytes 2,3 Protocol number always zero
  // byte 4 Response length (upper byte) Always zero
  // byte 5 Response length (lower byte). Equal to the number of bytes which follow
  // Here comes the normal Modus telegram
  if ((status = _modbus_read_chunk(s, tel, 2)) != APR_SUCCESS) return status;
  *slave     = tel[0];
  *function  = tel[1];
  printf("slave: %d, fcn: %d\n", *slave, *function);
  switch(*function)
  {
    case ReadCoilStatus:
    case ReadInputStatus:
    case ReadHoldingRegisters:
    case ReadInputRegisters:
    case FetchCommEventLog:
    case ReportSlaveID:
    case ReadGeneralReference:
    case WriteGeneralReference:
    case ReadWrite4XRegisters:
      if ((status = _modbus_read_chunk(s, tel, 1)) != APR_SUCCESS) return status;
      return _modbus_read_chunk(s, data, tel[0]);
    case ForceSingleCoil:
    case PresetSingleRegister:
    case FetchCommEventCtr:
    case ForceMultipleCoils:
    case PresetMultipleRegs:
      return _modbus_read_chunk(s, data, 4);
    case ReadExceptionStatus:
      return _modbus_read_chunk(s, data, 1);
    case MaskWrite4XRegisters:
      return _modbus_read_chunk(s, data, 6);
    case ReadFifoQueue:
      if ((status = _modbus_read_chunk(s, tel, 2)) != APR_SUCCESS) return status;
      return _modbus_read_chunk(s, data, (int) tel[0] << 8 + tel[1]);
    default:
      //debug only
      *function = 2; //this implies, not implemented, for the further processing
      return APR_SUCCESS;
  }
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: $
 */
