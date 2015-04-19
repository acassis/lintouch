
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"
#include <errno.h>


int readdata(_data_buffer * buff, _comm_header * comm, int debug)
{
  int x,bump;
  int exp_len;
#ifndef WIN32
  fd_set s_readfds;
  int nfds,ret;
  struct timeval s_timeout;
#endif

  dprint(DEBUG_TRACE, "--- readdata.c REVISED TIMEOUT entered at %ld\n",time(NULL));

  memset(buff, 0, DATA_Buffer_Length);

#ifndef WIN32
  FD_ZERO(&s_readfds);

  FD_SET(comm->file_handle,&s_readfds);

  nfds = comm->file_handle+1;

  s_timeout.tv_sec = comm->ms_timeout / 1000;
  s_timeout.tv_usec = (comm->ms_timeout % 1000) * 1000;

  ret = select (nfds, &s_readfds, NULL, NULL, &s_timeout);

  if (ret == 0)
  {
    dprint(DEBUG_TRACE, "--- readdata.c error timestamp %ld errno %d\n",
       time(NULL),errno);
    CELLERROR(1,"Communications Timeout");
    /* Communications timeout */
    return -1;
  }
  else if (ret == -1)
  {
    /* Error during communications */
    CELLERROR(2,"Communications Error");
    return -1;
  }
  buff->overall_len = read(comm->file_handle, buff->data, DATA_Buffer_Length);
#endif

#ifdef WIN32
  buff->overall_len = recv(comm->file_handle, buff->data, DATA_Buffer_Length, 0);
#endif

  exp_len = sizeof(_encaps_header) + buff->data[2] + (buff->data[3]*256);
  dprint (DEBUG_TRACE, "expected length = %d, received length = %d\n",exp_len, buff->overall_len);
  if (exp_len != buff->overall_len)
    {
    #ifdef WIN32
      bump = recv(comm->file_handle, &buff->data[buff->overall_len], DATA_Buffer_Length, 0);
    #endif
    
    #ifndef WIN32
      bump = read(comm->file_handle, &buff->data[buff->overall_len], DATA_Buffer_Length);
    #endif
    
    buff->overall_len += bump;
    dprint (DEBUG_TRACE, "Final received length: %d\n",buff->overall_len);
    }  


  if (buff->overall_len < 1)
  {
    /* Read error */
    CELLERROR(3,"Communications Read Error");
    return -1;
  }

  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_DATA, "Received back %d bytes:\n",buff->overall_len);
    for (x = 0; x < buff->overall_len; x++)
      dprint(DEBUG_DATA, "%02X ", buff->data[x]);
    dprint(DEBUG_DATA, "\n");
  }
  dprint(DEBUG_TRACE, "readdata.c exited.\n");
  return 0;
}
