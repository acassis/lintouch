
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int
nop (_comm_header * comm, int debug)
{
  _encaps_header *head;
  _data_buffer *buff;
  int ret;

  dprint (DEBUG_TRACE, "--- nop.c entered\n");

  head = malloc (sizeof (_encaps_header));
  if (head == NULL)
    {
      CELLERROR (1, "Could not allocate memory for nop.");
      return -1;
    }

  buff = malloc (sizeof (_data_buffer));
  if (buff == NULL)
    {
      CELLERROR (2, "Could not allocate memory for transmit buffer.");
      free (head);
      return -1;
    }


  memset (buff, 0, sizeof (_data_buffer));
  memset (head, 0, sizeof (_encaps_header));

  fill_header (comm, head, debug);

  head->command = ENCAPS_NOP;

  memcpy (buff->data, head, ENCAPS_Header_Length);
  buff->overall_len = ENCAPS_Header_Length;
  ret = senddata (buff, comm, debug);
  if (!ret)
    ret = readdata (buff, comm, debug);

  free (head);
  free (buff);

  dprint (DEBUG_TRACE, "nop.c exited.\n");
  return ret;
}
