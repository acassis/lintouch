
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int get_object_config(_comm_header * comm, _path * path, _tag_detail * tag,
		       int debug)
{
  int x, start;
  int ret=0;
  int tagsize;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *cpf_buffer;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;
  _ioi_data *ioi1;
  _ioi_data *ioi2;
  _ioi_data ioiCM;

  dprint(DEBUG_TRACE, "get_object_config.c entered.\n");
  ioi2 = NULL;

  if (path == NULL)
  {
    CELLERROR(1,"path structure not allocated");
    return -1;
  }

  if (tag == NULL)
  {
    CELLERROR(2,"tag structure not allocated");
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
    CELLERROR(8,"Could not allocate memory for ioi1 buffer");
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


  buff->len = 0;

  msgbuff->data[msgbuff->len++] = GET_ATTRIBUTE_LIST;
  ioi1->ioiclass = OBJECT_CONFIG;
  ioi1->instance = tag->base;
  if (tag->id != 0)
  {
    ioi1->ioiclass = OBJECT_SUB_OBJECT;
    ioi1->instance = tag->base;
  }
  ioi1->member = -1;
  ioi1->point = -1;
  ioi1->attribute = -1;
  ioi1->tagname = NULL;
  ioi1->elem[0] = -1;
  ioi1->elem[1] = -1;
  ioi1->elem[2] = -1;
  if (tag->id != 0)
  {
    ioi2 = malloc(sizeof(_ioi_data));
    if (ioi2 == NULL)
    {
      CELLERROR(9,"Could not allocate memory for ioi2 buffer");
      free(buff);
      free(msgbuff);
      free(head);
      free(cpf_buffer);
      free(sendbuff);
      free(ioi1);
      return -1;
    }
    memset(ioi2, 0, sizeof(_ioi_data));
    ioi2->ioiclass = OBJECT_CONFIG;
    ioi2->instance = tag->id;
    ioi2->member = -1;
    ioi2->point = -1;
    ioi2->attribute = -1;
    ioi2->tagname = NULL;
    ioi2->elem[0] = -1;
    ioi2->elem[1] = -1;
    ioi2->elem[2] = -1;
  }

  ioi(msgbuff, ioi1, ioi2, debug);
  msgbuff->data[msgbuff->len++] = 3;
  msgbuff->data[msgbuff->len++] = 0;
  msgbuff->data[msgbuff->len++] = 1;
  msgbuff->data[msgbuff->len++] = 0;
  msgbuff->data[msgbuff->len++] = 2;
  msgbuff->data[msgbuff->len++] = 0;
  msgbuff->data[msgbuff->len++] = 3;
  msgbuff->data[msgbuff->len++] = 0;

  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Msgbuff = ");
    for (x = 0; x < msgbuff->len; x++)
      dprint(DEBUG_BUILD, "%02X ", msgbuff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }

  cpf_buffer->data[cpf_buffer->len++] = PDU_Unconnected_Send;
  ioiCM.ioiclass = CONNECTION_MANAGER;
  ioiCM.instance = FIRST_INSTANCE;
  ioiCM.member = -1;
  ioiCM.point = -1;
  ioiCM.attribute = -1;
  ioiCM.tagname = NULL;
  ioiCM.elem[0] = -1;
  ioiCM.elem[1] = -1;
  ioiCM.elem[2] = -1;

  ioi(cpf_buffer, &ioiCM, NULL, debug);
  settimeout(6, 0x9a, cpf_buffer, debug);
  cpf_buffer->data[cpf_buffer->len++] = msgbuff->len & 255;
  cpf_buffer->data[cpf_buffer->len++] = msgbuff->len / 0x100;

  makepath(path, msgbuff, debug);
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
  memset(head, 0, ENCAPS_Header_Length);
  memcpy(head, buff->data, ENCAPS_Header_Length);
  if (head->status != 0)
  {
    ret=-1;
    dprint(DEBUG_VALUES, "Get Object Config command returned an error.\n");
    CELLERROR(10, "Get Object Config command returned an error.");
  }
  if (ret)
  {
    if (ioi2 != NULL)
       free(ioi2);
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    free(ioi1);
    return -1;
  }

//buff = buff + ENCAPS_Header_Length;
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES,
	   "Got good reply to Get Object Config Command - %d\n",
	   buff->overall_len);
    for (x = 44; x < buff->overall_len; x++)
      dprint(DEBUG_VALUES, "%02X ", buff->data[x]);
    dprint(DEBUG_VALUES, "\n");
  }

  start = 50;
  tag->arraysize1 = buff->data[start] +
    (buff->data[start + 1] * 0x100) +
    (buff->data[start + 2] * 0x10000) + (buff->data[start + 3] * 0x1000000);
  start += 4;
  tag->arraysize2 = buff->data[start] +
    (buff->data[start + 1] * 0x100) +
    (buff->data[start + 2] * 0x10000) + (buff->data[start + 3] * 0x1000000);
  start += 4;
  tag->arraysize3 = buff->data[start] +
    (buff->data[start + 1] * 0x100) +
    (buff->data[start + 2] * 0x10000) + (buff->data[start + 3] * 0x1000000);


  start += 8;
  tag->type = buff->data[start] + (buff->data[start + 1] * 256);
  start += 6;
  tag->linkid = buff->data[start] +
    (buff->data[start + 1] * 0x100) +
    (buff->data[start + 2] * 0x10000) + (buff->data[start + 3] * 0x1000000);

  start += 8;
  tag->topbase = buff->data[start] + (buff->data[start + 1] * 256);

  start += 8;

  tagsize = getsize(tag);

  if (!tagsize)
  {
    CELLERROR(21,"Cannot allocate a tag of size 0");
    ret=0;
  }
  else
  {
    tag->data = malloc(tagsize);

    if (tag->data == NULL)
    {
      CELLERROR(11,"Could not allocate memory for tag data storage.");
      ret=-1;
    }
  }

  if (ioi2 != NULL)
     free(ioi2);

  free(buff);
  free(msgbuff);
  free(head);
  free(sendbuff);
  free(cpf_buffer);
  free(ioi1);

  dprint(DEBUG_TRACE, "get_object_config.c exited. ret=%d\n",ret);

  return ret;
}
