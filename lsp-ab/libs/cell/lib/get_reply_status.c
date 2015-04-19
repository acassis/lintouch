#include "libcell.h"

char cell_error_message[64];

char *get_reply_status (_data_buffer *buff, int debug)
{
unsigned short x, errnum, found = 0;

dprint (DEBUG_TRACE, "Get Reply Status entered\n");

if (buff == NULL)
  {
  dprint (DEBUG_TRACE, "buff struct is null\n");
  return NULL;
  }

errnum = (unsigned short)buff->data[42];
dprint (DEBUG_TRACE, "Looking for errnum = 0x%04X\n",errnum);

if (errnum == 0x01ff)
  {
  errnum = (unsigned short)buff->data[44];
  dprint (DEBUG_TRACE, "Extended errnum = 0x%04X\n",errnum);
  }


memset (&cell_error_message,0,64);

for (x=0; x< cip_status_count; x++)
  {
  if (cip_status_index[x] == errnum)
    {
    found = 1;
    dprint (DEBUG_TRACE, "Found Status Message %d\n",x);
    sprintf (cell_error_message, "%s",cip_status[x]);
    }
  }
if (found == 0)
  {
  sprintf (cell_error_message, "Could not find error message for 0x%04X\n",errnum);
  dprint (DEBUG_TRACE, "Could not find message for errnum 0x%04X\n",errnum);
  }

dprint (DEBUG_TRACE, "get_reply_status exiting\n");
return cell_error_message;
}
