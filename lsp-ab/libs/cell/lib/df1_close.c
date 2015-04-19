
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int df1_close(_comm_header * comm, _path * path, _df1_comm *df1_comm,
		      int debug)
{
  int x,ret=0;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *cpf_buffer;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;
  _ioi_data *ioi1;

  dprint(DEBUG_TRACE, "df1_close.c entered.\n");

  if (path == NULL)
  {
    CELLERROR(1,"path structure not allocated");
    return -1;
  }
  if (df1_comm == NULL)
  {
    CELLERROR(2,"df1_comm structure not allocated");
    return -1;
  }

  buff = malloc(sizeof(_data_buffer));
  if (buff == NULL)
  {
    CELLERROR(3,"Could not allocate memory for data buffer");
    return -1;
  }

  msgbuff = malloc(sizeof(_data_buffer));
  if (msgbuff == NULL)
  {
    CELLERROR(4,"Could not allocate memory for msgdata buffer");
    free(buff);
    return -1;
  }

  head = malloc(sizeof(_encaps_header));
  if (head == NULL)
  {
    CELLERROR(5,"Could not allocate memory for Encaps Header");
    free(buff);
    free(msgbuff);
    return -1;
  }

  cpf_buffer = malloc(sizeof(_data_buffer));
  if (cpf_buffer == NULL)
  {
    CELLERROR(6,"Could not allocate memory for cpf_buffer");
    free(buff);
    free(msgbuff);
    free(head);
    return -1;
  }

  sendbuff = malloc(sizeof(_data_buffer));
  if (sendbuff == NULL)
  {
    CELLERROR(7,"Could not allocate memory for send buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    return -1;
  }

  ioi1 = malloc(sizeof(_ioi_data));
  if (ioi1 == NULL)
  {
    CELLERROR(8,"Could not allocate memory for ioi buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    return -1;
  }
  memset(buff, 0, sizeof(_data_buffer));
  memset(msgbuff, 0, sizeof(_data_buffer));
  memset(head, 0, sizeof(_encaps_header));
  memset(cpf_buffer, 0, sizeof(_data_buffer));
  memset(sendbuff, 0, sizeof(_data_buffer));
  memset(ioi1, 0, sizeof(_ioi_data));

  msgbuff->len = 0;
  msgbuff->data[msgbuff->len++] = (byte)(df1_comm->conn_serial & 255);
  msgbuff->data[msgbuff->len++] = (byte)(df1_comm->conn_serial >> 8);
  msgbuff->data[msgbuff->len++] = (byte)(df1_comm->conn_serial >> 16);
  msgbuff->data[msgbuff->len++] = (byte)(df1_comm->conn_serial >> 24);
  msgbuff->data[msgbuff->len++] = (byte)(VENDOR_ID & 255);
  msgbuff->data[msgbuff->len++] = (byte)(VENDOR_ID / 256);
  msgbuff->data[msgbuff->len++] = (byte)(VENDOR_SN & 255);
  msgbuff->data[msgbuff->len++] = (byte)(VENDOR_SN >> 8);
  msgbuff->data[msgbuff->len++] = (byte)(VENDOR_SN >> 16);
  msgbuff->data[msgbuff->len++] = (byte)(VENDOR_SN >> 24);
  msgbuff->data[msgbuff->len++] = 0;
  msgbuff->data[msgbuff->len++] = 0;
  
  


  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Msgbuff = ");
    for (x = 0; x < msgbuff->len; x++)
      dprint(DEBUG_BUILD, "%02X ", msgbuff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }

  /*pathsize = msgbuff->len++;
  count = 0;
  if (path->device1 != -1)
    {
    count++;
    msgbuff->data[msgbuff->len++] = path->device1;
    }
  if (path->device2 != -1)
    {
    count++;
    msgbuff->data[msgbuff->len++] = path->device2;
    }
  if (path->device3 != -1)
    {
    count++;
    msgbuff->data[msgbuff->len++] = path->device3;
    }
  if (path->device4 != -1)
    {
    count++;
    msgbuff->data[msgbuff->len++] = path->device4;
    }
  if (path->device5 != -1)
    {
    count++;
    msgbuff->data[msgbuff->len++] = path->device5;
    }
  if (path->device6 != -1)
    {
    count++;
    msgbuff->data[msgbuff->len++] = path->device6;
    }
  if (path->device7 != -1)
    {
    count++;
    msgbuff->data[msgbuff->len++] = path->device7;
    }
  if (path->device8 != -1)
    {
    count++;
    msgbuff->data[msgbuff->len++] = path->device8;
    }

  msgbuff->data[msgbuff->len++] = 0x20;
  msgbuff->data[msgbuff->len++] = DF1CMD;
  msgbuff->data[msgbuff->len++] = 0x24;
  msgbuff->data[msgbuff->len++] = FIRST_INSTANCE;
  msgbuff->data[msgbuff->len++] = 0x2c;
  msgbuff->data[msgbuff->len++] = dh_channel;

  if (count & 1)
    {
    msgbuff->data[msgbuff->len++] = 0;
    count++;
    }
  msgbuff->data[pathsize] = (count + 6)/2;  
*/  
    
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Msgbuff = ");
    for (x = 0; x < msgbuff->len; x++)
      dprint(DEBUG_BUILD, "%02X ", msgbuff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }


  if (path->device1 != -1)
  {
    cpf_buffer->data[cpf_buffer->len++] = PDU_Forward_Close;
    ioi1->ioiclass = CONNECTION_MANAGER;
    ioi1->instance = FIRST_INSTANCE;
    ioi1->member = -1;
    ioi1->point = -1;
    ioi1->attribute = -1;
    ioi1->tagname = NULL;
    ioi1->elem[0] = -1;
    ioi1->elem[1] = -1;
    ioi1->elem[2] = -1;

    ioi(cpf_buffer, ioi1, NULL, debug);
    settimeout(6, 0x9a, cpf_buffer, debug);
//    cpf_buffer->data[cpf_buffer->len++] = msgbuff->len & 255;
//    cpf_buffer->data[cpf_buffer->len++] = msgbuff->len / 0x100;
  }
  memcpy(cpf_buffer->data + cpf_buffer->len, msgbuff->data, msgbuff->len);
  cpf_buffer->len += msgbuff->len;

  sendRRdata(10, comm, head, buff, debug);
  cpf(CPH_Null, NULL, CPH_Unconnected_message, cpf_buffer, buff, debug);

  head->len = buff->len;
  memcpy(sendbuff->data, head, ENCAPS_Header_Length);
  sendbuff->overall_len = ENCAPS_Header_Length;
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Loading sendbuffer with command data.\n");
    for (x = 0; x < buff->len; x++)
      dprint(DEBUG_BUILD, "%02X ", buff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }

  memcpy(sendbuff->data + sendbuff->overall_len, buff->data, buff->len);
  sendbuff->overall_len += buff->len;
  ret = senddata(sendbuff, comm, debug);
  if (!ret) ret = readdata(buff, comm, debug);
  comm->df1_tns += 4;
  memset(head, 0, ENCAPS_Header_Length);
  memcpy(head, buff->data, ENCAPS_Header_Length);
  if (head->status != 0)
  {
    ret=-1;
    dprint(DEBUG_VALUES, "df1_close command returned an error.\n");
    CELLERROR(9,"DF1 Close command returned an error");
  }
  if (ret)
  {
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    free(ioi1);
    return -1;
  }

//buff = buff + ENCAPS_Header_Length;
  memset (df1_comm, 0, sizeof(_df1_comm));
    
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "Got good reply to DF1 Open Command - %d\n",
	   buff->overall_len);
    for (x = 44; x < buff->overall_len; x++)
      dprint(DEBUG_VALUES, "%02X ", buff->data[x]);
    dprint(DEBUG_VALUES, "\n");
  }

  free(buff);
  free(msgbuff);
  free(head);
  free(cpf_buffer);
  free(sendbuff);
  free(ioi1);

  dprint(DEBUG_TRACE, "Exiting df1_close.c\n");

  return 0;
}
