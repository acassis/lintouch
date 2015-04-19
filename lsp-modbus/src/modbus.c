/* $Id: modbus.c 1520 2006-03-08 12:22:15Z hynek $
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
 */

#include "modbus.h"
#include <lt/variables/variables.h>
#define APR_WANT_BYTEFUNC /* htons, ntohs */
#include "apr_want.h"
#include <string.h>

enum ModbusExceptionCodes
{
    ILLEGAL_FUNCTION = 1,
    ILLEGAL_DATA_ADDRESS,
    ILLEGAL_DATA_VALUE
};

typedef struct
{
    /* incoming frame */
    apr_byte_t incoming[FRAMELEN];

    /* outgoing frame */
    apr_byte_t outgoing[FRAMELEN];

    /* the same as in the plugin data */
    apr_array_header_t **addressspace;

    /* the same as in the plugin data */
    sa_t *addressspacestart;

    /* pool to use when needed */
    apr_pool_t *pool;
} _modbus_pdu;

/* Initialize and return all the necessary data associated with this
   connection. */
void *modbus_create(apr_pool_t *p, apr_array_header_t *addressspace[],
    sa_t addressspacestart[])
{
    /* allocate the room for a modbus frame (telegram) */
    _modbus_pdu *result = (_modbus_pdu *)
        apr_pcalloc(p, sizeof(_modbus_pdu));
    apr_byte_t *tel = result->incoming;

    /* initialize the outgoing frame */
    tel[2] = 0;  /* bytes 2,3 Protocol number. Must be zero. */
    tel[3] = 0;
    tel[4] = 0;  /* byte 4 Length (upper byte). Since all requests will be
                 less than 256 bytes in length (!), this will always be
                 zero. */

    result->pool = p;

    result->addressspace = addressspace;
    result->addressspacestart = addressspacestart;

    return result;
}

/* Free the associated data. */
void modbus_destroy(void *client_data)
{
}

/* Read the count bytes into the buffer and check if the actual number of
   the read bytes is as expected (=count) */
apr_status_t _modbus_read_chunk(apr_socket_t *s, apr_byte_t *buffer,
    apr_size_t count)
{
  apr_size_t actualread = count;
  apr_status_t status = apr_socket_recv(s, (char *) buffer, &actualread);
  return status == APR_SUCCESS ?
      (count == actualread ? APR_SUCCESS : APR_INCOMPLETE) : status;
}

/* Indicate an exception. Write into the half-prepared telegram and send it
   out */
apr_status_t _modbus_error(apr_socket_t *s, apr_byte_t *tel,
    apr_byte_t exception)
{
  apr_size_t len = DATAOFFSET+1;
  tel[FUNCTIONOFFSET] |= 128; /* set the exception flag */
  tel[DATAOFFSET] = exception;
  tel[LENOFFSET] = 3; /* slave, function, and the exception number */
  return apr_socket_send(s, (char *) tel, &len);
}

/* Process the incoming frame. */
apr_status_t modbus_indication(const apr_pollfd_t *desc)
{
  _modbus_pdu *pdu = desc->client_data;
  apr_byte_t *tel = (apr_byte_t *) pdu->incoming;
  apr_byte_t *out = (apr_byte_t *) pdu->outgoing;
  apr_socket_t *s = desc->desc.s;
  apr_status_t status;
  apr_uint16_t i, j, starting_address, quantity;
  apr_size_t len;
  apr_byte_t function;
  apr_array_header_t *addressspace;
  sa_t addressspacestart;


  /* Modbus header (transaction ID, proto, length) plus slave and fcn */
  if ((status = _modbus_read_chunk(s, tel, DATAOFFSET)) != APR_SUCCESS)
  {
      return status;
  }

  /* prepare the outgoing telegram */
  out[TRANSACTIONIDOFFSET] = tel[TRANSACTIONIDOFFSET];
  out[TRANSACTIONIDOFFSET+1] = tel[TRANSACTIONIDOFFSET+1];
  out[LENOFFSET] = tel[LENOFFSET]; /* some functions only */
  out[SLAVEOFFSET] = tel[SLAVEOFFSET];

#if 0
  //DEBUG: print out the content of all the hash tables
  {
      apr_hash_t *tables[4];
      tables[0] = pdu->discreteinputs;
      tables[1] = pdu->coils;
      tables[2] = pdu->inputregisters;
      tables[3] = pdu->holdingregisters;
      int i = 0;
      for (; i < 4; i++)
      {
          apr_hash_t *table = tables[i];
          apr_hash_index_t * hi =
              apr_hash_first( pdu->pool, table );

          for( ; hi != NULL; hi = apr_hash_next( hi ) ) {
              apr_uint16_t * varname = NULL;
              lt_var_t * var = NULL;

              apr_hash_this( hi, (const void**)&varname, &len,
                  (void**)&var );

              const lt_io_value_t *v = lt_var_real_value_get(var);
              printf("hash %d - %hd ", i, *varname);
              switch(lt_io_value_type_get(v))
              {
                  case LT_IO_TYPE_NULL:
                      printf("NULL");
                      break;
                  case LT_IO_TYPE_BIT:
                      printf("%d", (int) lt_io_value_bit_get(v));
                      break;
                  case LT_IO_TYPE_NUMBER:
                      printf("%d", lt_io_value_number_get(v));
                      break;
                  default:
                      printf("UNKNOWN TYPE");
              }
              printf("\n");
            }
      }
  }
#endif

  switch(function = out[FUNCTIONOFFSET] = tel[FUNCTIONOFFSET])
  {
    case READ_COILS:
    case READ_DISCRETE_INPUTS:
        addressspace = pdu->addressspace[function-1];
        addressspacestart = pdu->addressspacestart[function-1];

        if ((status = _modbus_read_chunk(s, tel, 4)) != APR_SUCCESS) return status;

        j = ntohs(*(apr_uint16_t *)&tel[0]); /* starting address */
        quantity = ntohs(*(apr_uint16_t *)&tel[2]);

        /* check the address range */
        if (quantity > 2000)
        {
            return _modbus_error(s, out, ILLEGAL_DATA_VALUE);
        }
        if (j < addressspacestart ||
            j + quantity > addressspacestart + (addressspace->nelts<<3))
        {
            return _modbus_error(s, out, ILLEGAL_DATA_ADDRESS);
        }

        /* the number of bytes to copy */
        len = out[DATAOFFSET] = (apr_byte_t) ((quantity+7) >> 3);

        /* copy all the requested bits */
        if ((j-addressspacestart) & 7)
        {
            apr_byte_t shift1 = (apr_byte_t) ((j-addressspacestart) & 7);
            apr_byte_t shift2 = 8 - shift1;
            apr_byte_t *outi = out + DATAOFFSET + 1;
            apr_byte_t *asi = addressspace->elts+((j-addressspacestart)>>3);
            for (i = 0; i < len; i++, outi++, asi++)
            {
                *outi = *asi >> shift1 | asi[1] << shift2;
            }
        }
        else
        {   /* good, we can use memcpy */
            memcpy(out + DATAOFFSET + 1,
                addressspace->elts + ((j-addressspacestart) >> 3), len);
        }

        /* fill in the rest of the frame */
        out[LENOFFSET] = (apr_byte_t) len + 3;
        len += DATAOFFSET + 1;

        /* zero pad the last byte */
        if (quantity&7)
        {
            out[len] &= (apr_byte_t) ((1 << (quantity&7)) - 1);
        }

        return apr_socket_send(s, (char *) out, &len);
    case READ_HOLDING_REGISTERS:
    case READ_INPUT_REGISTERS:
        addressspace = pdu->addressspace[function-1];
        addressspacestart = pdu->addressspacestart[function-1];
        if ((status = _modbus_read_chunk(s, tel, 4)) != APR_SUCCESS) return status;

        starting_address = ntohs(*(apr_uint16_t *)&tel[0]);
        quantity = ntohs(*(apr_uint16_t *)&tel[2]);

        /* the number of bytes to copy */
        if (quantity > 125)
        {
            return _modbus_error(s, out, ILLEGAL_DATA_VALUE);
        }
        if (starting_address < addressspacestart ||
            starting_address + quantity > addressspacestart +
            (addressspace->nelts>>1))
        {
            return _modbus_error(s, out, ILLEGAL_DATA_ADDRESS);
        }

        len = out[DATAOFFSET] = quantity<<1;
        memcpy(out + DATAOFFSET + 1,
            addressspace->elts + ((starting_address-addressspacestart)<< 1),
            len);

        out[LENOFFSET] = (apr_byte_t) len + 3;
        len += DATAOFFSET + 1;
        return apr_socket_send(s, (char *) out, &len);
    case WRITE_SINGLE_COIL:
    case WRITE_SINGLE_REGISTER:
        addressspace =
            pdu->addressspace[function == WRITE_SINGLE_COIL ? 0 : 2];
        addressspacestart = 
            pdu->addressspacestart[function == WRITE_SINGLE_COIL ? 0 : 2];
        if ((status = _modbus_read_chunk(s, tel, 4)) != APR_SUCCESS) return status;

        starting_address = ntohs(*(apr_uint16_t *)&tel[0]);

        /* check the range */
        if (function == WRITE_SINGLE_COIL &&
            ((tel[2] != 0xFF && tel[2] != 0) || tel[3] != 0))
        {
            return _modbus_error(s, out, ILLEGAL_DATA_VALUE);
        }
        if (starting_address < addressspacestart ||
            starting_address > addressspacestart +
            (function == WRITE_SINGLE_COIL ? addressspace->nelts<<3 :
            addressspace->nelts>>1))
        {
            return _modbus_error(s, out, ILLEGAL_DATA_ADDRESS);
        }

        /* update the address space */
        if (function == WRITE_SINGLE_COIL)
        {
            starting_address -= addressspacestart;
            addressspace->elts[starting_address>>3] =
                (~(1 << (starting_address&7))
                & addressspace->elts[starting_address>>3])
                | (tel[2] ? 1 << (starting_address&7) : 0);
        }
        else
        {
            ((sa_t *) addressspace->elts)
            [starting_address - addressspacestart] =
                *(sa_t *) (tel + 2);
        }

        out[DATAOFFSET] = tel[0];
        out[DATAOFFSET + 1] = tel[1];
        out[DATAOFFSET + 2] = tel[2];
        out[DATAOFFSET + 3] = tel[3];
        len = DATAOFFSET + 4;
        return apr_socket_send(s, (char *) out, &len);
    case WRITE_MULTIPLE_COILS:
    case WRITE_MULTIPLE_REGISTERS:
        addressspace =
            pdu->addressspace[function == WRITE_MULTIPLE_COILS ? 0 : 2];
        addressspacestart = 
            pdu->addressspacestart[function == WRITE_MULTIPLE_COILS ? 0 : 2];
        if ((status = _modbus_read_chunk(s, tel, 5)) != APR_SUCCESS) return status;

        starting_address = ntohs(*(apr_uint16_t *)&tel[0]);
        quantity = ntohs(*(apr_uint16_t *)&tel[2]);

        out[DATAOFFSET] = tel[0];
        out[DATAOFFSET + 1] = tel[1];
        out[DATAOFFSET + 2] = tel[2];
        out[DATAOFFSET + 3] = tel[3];
        len = DATAOFFSET + 4;
        out[LENOFFSET] = len - LENOFFSET - 1;

        /* check the range */
        if (starting_address < addressspacestart ||
            starting_address + quantity > addressspacestart +
            (function == WRITE_MULTIPLE_COILS ? addressspace->nelts<<3 :
            addressspace->nelts>>1))
        {
            return _modbus_error(s, out, ILLEGAL_DATA_ADDRESS);
        }

        /* read the coil values */
        if ((status = _modbus_read_chunk(s, tel, tel[4])) != APR_SUCCESS) return status;

        if (function == WRITE_MULTIPLE_COILS)
        {
            /* copy all the received bits */
            if ((starting_address - addressspacestart) & 7)
            {
                apr_byte_t shift1 = (starting_address-addressspacestart)&7;
                apr_byte_t shift2 = 8 - shift1;
                apr_byte_t *asi = addressspace->elts
                    + ((starting_address-addressspacestart) >> 3);
                apr_byte_t *teli = tel;

                int incompletefirst = starting_address - addressspacestart +
                    quantity;
                *asi =
                    (~((apr_byte_t) ((1 << shift1) - 1) | (apr_byte_t)
                        (incompletefirst < 8 ? 0xFF << incompletefirst : 0))
                    & addressspace->elts[*asi])
                    | (*teli << shift1);
                asi++;

                for (i = 0; i < (quantity>>3) - 1; i++, teli++, asi++)
                {
                    *asi = *teli >> shift2 | teli[1] << shift1;
                }
            }
            else
            {   /* good, we can use memcpy */
                memcpy(addressspace->elts +
                    ((starting_address-addressspacestart) >> 3), tel,
                    quantity>>3);
            }

            /* copy the last byte */
            starting_address += -addressspacestart + quantity;
            if (quantity&7 && (starting_address > 8
                || !((starting_address - addressspacestart) & 7)))
            {
                starting_address >>= 3;
                addressspace->elts[starting_address] =
                    (~((apr_byte_t) ((1 << (quantity&7)) - 1))
                    & addressspace->elts[starting_address])
                    | tel[quantity>>3];
            }
        }
        else /* function == WRITE_MULTIPLE_REGISTERS */
        {
            memcpy(addressspace->elts +
                ((starting_address-addressspacestart) << 1), tel,quantity<<1);
        }

        return apr_socket_send(s, (char *) out, &len);
    case MASK_WRITE_REGISTER:
        if ((status = _modbus_read_chunk(s, tel, 6)) != APR_SUCCESS) return status;

        addressspace = pdu->addressspace[2];
        addressspacestart = pdu->addressspacestart[2];

        starting_address = ntohs(*(apr_uint16_t *)&tel[0]);
        i = *(sa_t *) (tel + 2); /* And_Mask */
        j = *(sa_t *) (tel + 4); /* Or_Mask */

        /* check the range */
        if (starting_address < addressspacestart ||
            starting_address > addressspacestart +(addressspace->nelts>>1))
        {
            return _modbus_error(s, out, ILLEGAL_DATA_ADDRESS);
        }

        /* update the value */
        starting_address -= addressspacestart;
        ((sa_t *) addressspace->elts)[starting_address] =
           (((sa_t *) addressspace->elts)[starting_address] & i) | (j & ~i);

        for (i = 0; i < 6; i++) out[DATAOFFSET + i] = tel[i];
        len = DATAOFFSET + 6;
        return apr_socket_send(s, (char *) out, &len);
    case READWRITE_MULTIPLE_REGISTERS:
        if ((status = _modbus_read_chunk(s, tel, 9)) != APR_SUCCESS) return status;

        addressspace = pdu->addressspace[2];
        addressspacestart = pdu->addressspacestart[2];

        starting_address = ntohs(*(apr_uint16_t *)&tel[0]); /* to read */
        quantity = ntohs(*(apr_uint16_t *)&tel[2]); /* to read */
        j = ntohs(*(apr_uint16_t *)&tel[4]); /* to write */
        len = ntohs(*(apr_uint16_t *)&tel[6]); /* to write */

        /* check the range */
        if (starting_address < addressspacestart ||
            starting_address + quantity > addressspacestart
                + (addressspace->nelts>>1) ||
            j < addressspacestart ||
            j + len > addressspacestart + (addressspace->nelts>>1))
        {
            return _modbus_error(s, out, ILLEGAL_DATA_ADDRESS);
        }

        if ((status = _modbus_read_chunk(s, tel, tel[8])) != APR_SUCCESS) return status;

        /* the write phase */
        memcpy(addressspace->elts +((j-addressspacestart)<<1), tel, len<<1);

        /* the read phase */
        len = out[DATAOFFSET] = quantity<<1;
        memcpy(out + DATAOFFSET + 1,
            addressspace->elts +((starting_address-addressspacestart) << 1),
            len);

        out[LENOFFSET] = (apr_byte_t) len + 3;
        len += DATAOFFSET + 1;
        return apr_socket_send(s, (char *) out, &len);
    default:
      return _modbus_error(s, out, ILLEGAL_FUNCTION);
  }
}

apr_status_t modbus_response(apr_socket_t *s, apr_uint16_t *transaction,
    apr_byte_t *function, apr_byte_t *data, apr_byte_t *datalen)
{
  apr_status_t status;
  apr_byte_t tel[DATAOFFSET];

  /* Modbus header (transaction ID, proto, length) plus slave and fcn */
  if ((status = _modbus_read_chunk(s, tel, DATAOFFSET)) != APR_SUCCESS)
  {
      return status;
  }

  *transaction = ntohs(*(apr_uint16_t *) (tel + TRANSACTIONIDOFFSET));
  switch(*function = tel[FUNCTIONOFFSET])
  {
    case READ_COILS:
    case READ_DISCRETE_INPUTS:
    case READ_HOLDING_REGISTERS:
    case READ_INPUT_REGISTERS:
        if ((status = _modbus_read_chunk(s, datalen, 1)) != APR_SUCCESS) return status;
        return _modbus_read_chunk(s, data, *datalen);
    case WRITE_SINGLE_COIL:
    case WRITE_SINGLE_REGISTER:
        *datalen = 4;
        return _modbus_read_chunk(s, data, *datalen);
    default:
        if (*function & 128)
        {
            *datalen = 1;
            return _modbus_read_chunk(s, data, *datalen);
        }
        else
        {
            /* read the rest of the frame */
            *datalen = tel[LENOFFSET] - 2;
            return _modbus_read_chunk(s, data, *datalen);
        }
  }
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: modbus.c 1520 2006-03-08 12:22:15Z hynek $
 */
