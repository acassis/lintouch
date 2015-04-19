
/* Revised 5/6/02 William Hays - CTI 

   5/7/2002 WRH Added setsockopt to disable Nagle algorithm, added ms_timeout init
 
*/

#include "libcell.h"



void attach(_comm_header *comm, int debug)
{
  struct hostent *hostinfo;
  struct sockaddr_in address;
  int result, len;
#ifdef WIN32
  WSADATA wsaData;
  WSAStartup (0x0101,&wsaData);
#endif

  dprint(DEBUG_TRACE, "Entering attach.c\n");

  comm->error = 0;

  hostinfo = gethostbyname(comm->hostname);
  if (!hostinfo)
  {
    comm->error = NOHOST;
    return;
  }
  dprint (DEBUG_TRACE, "Host name found.\n");
  
  comm->file_handle = socket(AF_INET, SOCK_STREAM, 0);
  if (comm->file_handle == -1)
  {
    comm->error = NOCONNECT;
    return;
  }
dprint (DEBUG_TRACE, "Socket set.\n");
  comm->ms_timeout=CELL_DFLT_TIMEOUT;

  address.sin_family = AF_INET;
  address.sin_addr = *(struct in_addr *) *hostinfo->h_addr_list;
  if (address.sin_addr.s_addr == 0)
  {
    comm->error = BADADDR;
    return;
  }
  address.sin_port = htons(0xaf12);
  result = 0;
  len = sizeof(address);
  result = connect(comm->file_handle, (struct sockaddr *) &address, len);
  if (result < 0)
  {
    address.sin_port = htons(0x2222);
    result = 0;
    len = sizeof(address);
    result = connect(comm->file_handle, (struct sockaddr *) &address, len);
    if (result < 0)
      {
      comm->error = NOCONNECT;
      comm->commtype = -1;
      return;
      }
    comm->commtype = CELL_CSP;
  }
  else comm->commtype = CELL_EIP;
  dprint (DEBUG_TRACE, "Connected...\n");

#ifndef WIN32
  /* Disable the Nagle algorithm for send colescing */
  len = 4096;
  if (setsockopt(comm->file_handle, IPPROTO_TCP, SO_RCVBUF, &len, sizeof(len)) == -1)
    {
    printf ("Could not set receive buffer size.\n");
    }
  len = 1;
  if (setsockopt(comm->file_handle, IPPROTO_TCP,
     TCP_NODELAY,&len,sizeof(len)) == -1)
  {
    /* Nothing really to do if this fails.. just continue */
  }
  dprint(DEBUG_TRACE, "Set Socket Options done.\n");
#endif
  comm->error = OK;
  dprint(DEBUG_TRACE, "Exiting attach.c\n");

  return;
}
