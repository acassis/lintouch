
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int sendConnecteddata(unsigned short timeout, _comm_header * comm,
		_encaps_header * head, _data_buffer * buff, int debug)
{
  dprint(DEBUG_TRACE, "sendConnecteddata.c entered.  buff->len = %d\n", buff->len);
  if (head == NULL)
  {
    CELLERROR(1,"Could not allocate memory for encaps header");
    return -1;
  }
  if (buff == NULL)
  {
    CELLERROR(2,"Buffer for sendConnecteddata.c not allocated.");
    return -1;
  }

  memset(head, 0, ENCAPS_Header_Length);
  memset(buff, 0, DATA_Buffer_Length);
  fill_header(comm, head, debug);
  head->command = ENCAPS_SendUnitData;
  buff->data[buff->len += 4] = 0;
  buff->data[buff->len++] = (byte)timeout;
  buff->len++;
  dprint(DEBUG_TRACE,
	 "SendConnectedData.c exited - added %d timeout value to packet.\n",
	 timeout);

  return 0;

}
