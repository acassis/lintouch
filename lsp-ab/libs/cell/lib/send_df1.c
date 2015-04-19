
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int send_df1(_comm_header *comm,_df1_comm *df1_comm, _df1 *df1,
		     _df1_resp *resp, int debug)
{
  int x,ret=0;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *cpf_buffer;
  _data_buffer *cpf_address;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;

  dprint(DEBUG_TRACE, "send_df1.c entered.\n");

  if (df1_comm == NULL)
  {
    CELLERROR(1,"df1_comm structure not allocated");
    return -1;
  }
  if (df1 == NULL)
  {
    CELLERROR(2,"df1 structure not allocated");
    return -1;
  }
  if (resp == NULL)
  {
    CELLERROR(3,"df1 response structure not allocated");
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
  
  cpf_address = malloc(sizeof(_data_buffer));
  if (cpf_address == NULL)
    {
    CELLERROR(8,"Could not allocate memory for CPF Address buffer");
    free (buff);
    free (msgbuff);
    free (head);
    free (cpf_buffer);
    free (sendbuff);
    return -1;
    }
    

  memset(buff, 0, sizeof(_data_buffer));
  memset(msgbuff, 0, sizeof(_data_buffer));
  memset(head, 0, sizeof(_encaps_header));
  memset(cpf_buffer, 0, sizeof(_data_buffer));
  memset(sendbuff, 0, sizeof(_data_buffer));
  memset(cpf_address, 0, sizeof(_data_buffer));
  
  cpf_address->data[0] = (byte)(df1_comm->ot_conn_id & 255);
  cpf_address->data[1] = (byte)(df1_comm->ot_conn_id >> 8) & 255;
  cpf_address->data[2] = (byte)(df1_comm->ot_conn_id >> 16) & 255;
  cpf_address->data[3] = (byte)(df1_comm->ot_conn_id >> 24) & 255;
  cpf_address->len = 4;
  
  msgbuff->len = 0;



/* msgbuff contains the Sequence number, the length, and the df1 command */

  msgbuff->data[msgbuff->len++] = comm->sequence & 255;
  msgbuff->data[msgbuff->len++] = comm->sequence / 256;
  comm->sequence++;
  msgbuff->data[msgbuff->len++] = 0;
  msgbuff->data[msgbuff->len++] = 0;
  
  memcpy (msgbuff->data + msgbuff->len, &df1->cmd, df1->df1_datalen);
  msgbuff->len += df1->df1_datalen;
    
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Msgbuff = ");
    for (x = 0; x < msgbuff->len; x++)
      dprint(DEBUG_BUILD, "%02X ", msgbuff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }



  memcpy(cpf_buffer->data + cpf_buffer->len, msgbuff->data, msgbuff->len);
  cpf_buffer->len += msgbuff->len;

  sendConnecteddata(10, comm, head, buff, debug);
  cpf(CPH_Connection_based, cpf_address, CPH_Transport_PDU, cpf_buffer, buff, debug);

  head->len = buff->len;
  memcpy(sendbuff->data, head, ENCAPS_Header_Length);
  sendbuff->overall_len = ENCAPS_Header_Length;
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "Loading sendbuffer with command data.\n");
    for (x = 0; x < buff->len; x++)
      dprint(DEBUG_VALUES, "%02X ", buff->data[x]);
    dprint(DEBUG_VALUES, "\n");
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
    dprint(DEBUG_VALUES, "send_df1 command returned an error.\n");
    CELLERROR(9,"Send DF1 command returned an error");
  }
  if (ret)
  {
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    return -1;
  }

//buff = buff + ENCAPS_Header_Length;
  if (buff->overall_len > 48)
  {
    dprint (DEBUG_TRACE, "copying %d bytes data to resp struct\n",buff->overall_len - 48);
   // memset(resp, 0, sizeof(_df1_resp));
    memmove(resp, &buff->data[48], buff->overall_len-48);


//    data->overall_len = buff->overall_len - 44;
  }
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "Got good reply to Send DF1 Command - %d\n",
	   buff->overall_len);
    for (x = 48; x < buff->overall_len; x++)
      dprint(DEBUG_VALUES, "%02X ", buff->data[x]);
    dprint(DEBUG_VALUES, "\n");
  }
  x = buff->overall_len - 48;
  free(buff);
  free(msgbuff);
  free(head);
  free(cpf_buffer);
  free(sendbuff);
  free(cpf_address);
  dprint(DEBUG_TRACE, "Exiting send_df1.c\n");

  return x;
}
