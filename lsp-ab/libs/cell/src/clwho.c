#include "libcell.h"

void helpme(void)
{
  printf("Rack Device Enumeration program for Controllogix.\n\n");
  printf("who [-d -q] hostname\n\n");
  printf("-q	  = quiet mode.  Print almost nothing.\n");
  printf("-d	  = Increase debug mode.  Each -d increments debug mode value.\n");
  printf("	    Mode 0 - (default) Print no debug information.\n");
  printf("	    Mode 1 - Print ControlLogix Values as they are discovered.\n");
  printf("	    Mode 2 - Print entry and exit info for each routine.\n");
  printf("	    Mode 3 - Print data packet as sent and received from the CL.\n");
  printf("	    Mode 4 - Print data packet building inforation.\n");
  printf("	    A higher debug mode includes all lower modes.\n\n");
  printf("hostname = FQDN or IP address of ControlLogix PLC to talk to.\n\n");

}

void interpret (_df1_resp *resp)
{
switch (resp->df1data[1])
  {
  case 0xeb: /* we are a PLC-5 family */
    switch (resp->df1data[2])
      {
      case 0x15:
        printf ("PLC 5/40B ");
        break;
      case 0x22:
        printf ("PLC 5/10  ");
        break;
      case 0x23:
        printf ("PLC 5/60  ");
        break;
      case 0x28:
        printf ("PLC 5/40L ");
        break;
      case 0x29:
        printf ("PLC 5/60L ");
        break;
      case 0x31:
        printf ("PLC 5/11  ");
        break;
      case 0x32:
        printf ("PLC 5/20  ");
        break;
      case 0x33:
        printf ("PLC 5/30  ");
        break;
      case 0x4a:
        printf ("PLC 5/20E ");
        break;
      case 0x4b:
        printf ("PLC 5/40E ");
        break;
      case 0x55:
        printf ("PLC 5/25  ");
        break;
      case 0x59:
        printf ("PLC 5/80E ");
        break;
      default:
        printf ("Unk %02X    ",resp->df1data[2]);
      }
    switch (resp->df1data[0] & 7)
      {
      case 0:
        printf ("Program Load  ");
        break;
      case 2:
        printf ("Run Mode      ");
        break;
      case 4:
        printf ("Rem Prog Mode ");
        break;
      case 5:
        printf ("Remote Test   ");
        break;
      case 6:
        printf ("Remote Run    ");
        break;
      default:
        printf ("UNK - 0x%02X       ",resp->df1data[0] & 7);
        break;
      }
    if (resp->df1data[0] & 8)
      printf ("FAULTED");
    printf ("\n");
    break;
  case 0xde:
    printf ("Pyramid \n");
    break;
  case 0xee:
    switch(resp->df1data[3])
      {
      case 0x1a:
        printf ("SLC 5/00 ");
        break;
      case 0x1b:
        printf ("SLC 5/01 ");
        break;
      case 0x25:
        printf ("SLC 5/02 ");
        break;
      case 0x49:
        printf ("SLC 5/03 ");
        break;
      case 0x58:
        printf ("ML 1000  ");
        break;
      case 0x5b:
        printf ("SLC 5/04 ");
        break;
      case 0x76:
        printf ("CL DHRIO ");
        break;
      default:
        printf ("UNK %02X  ",resp->df1data[3]);
        break;
      }
    if (resp->df1data[3] != 0x76)
      {
      switch (resp->df1data[18])
        {
        case 0:
          printf ("download mode  ");
          break;
        case 1:
          printf ("Remote Program ");
          break;
        case 3:
          printf ("Idle SUS Instr ");
          break;
        case 4:
          printf ("Reserved 0x04  ");
          break;
        case 5:
          printf ("Reserved 0x05  ");
          break;
        case 6:
          printf ("Remote RUN     ");
          break;
        case 7:
          printf ("Test Cont Scan ");
          break;
        case 8:
          printf ("Test Sngl Scan ");
          break;
        case 9:
          printf ("Test Cont Step ");
          break;
        case 16:
          printf ("Download mode  ");
          break;
       case 17:
         printf ("Program Mode   ");
         break;
       case 27:
         printf ("Idle SUS Instr ");
         break;
       case 30:
         printf ("Run Mode       ");
         break;
       default:
         printf ("Reserved 0x%02X  ",resp->df1data[18]);
         break;
       }
     }
   if (resp->df1data[19] & 34)
     printf ("FAULTED");
   printf ("\n");
   break;
 default:
   printf ("Unk: %02X   \n",resp->df1data[1]);
   break;
 }
}



int main(int argc, char **argv)
{
  _comm_header *comm;
  _path *path;
  _services *services;
  _backplane_data *backplane;
  _rack *rack;
  int x, y, z, debug, channel, quiet, count;
#ifdef WIN32
  int optind = 1;
#endif

  debug = 0;
  quiet = FALSE;
  if (argc == 1)
  {
    helpme();
    exit(1);
  }
#ifndef WIN32
  while ((x = getopt(argc, argv, "dhq?")) != -1)
  {
    switch (x)
    {
    case 'd':
      debug++;
      break;
    case 'q':
      quiet = TRUE;
      break;
    case 'h':
    case '?':
      helpme();
      exit(1);
    }
  }
#endif



  if (argv[optind] == NULL)
  {
    printf("Can not connect - need a host name to connect to.\n");
    exit(-1);
  }

  rack = malloc(sizeof(_rack));
  if (rack == NULL)
  {
    printf("Could not allocate memory for rack structure.\n");
    exit(-1);
  }

  comm = malloc(sizeof(_comm_header));
  if (comm == NULL)
  {
    printf("Could not allocate memory for comm header.\n");
    exit(-1);
  }
  services = malloc(sizeof(_services));
  if (services == NULL)
  {
    printf("Could not allocate memory for services structure.\n");
    free(comm);
    exit(-1);
  }

  backplane = malloc(sizeof(_backplane_data));
  if (backplane == NULL)
  {
    printf("Could not allocate memory for backplane structure.\n");
    free(comm);
    free(services);
    exit(-1);
  }
  path = malloc(sizeof(_path));
  if (path == NULL)
  {
    printf("Could not allocate memory for path structure.\n");
    free(backplane);
    free(comm);
    free(services);
    exit(-1);
  }

  memset(services, 0, sizeof(_services));
  memset(comm, 0, sizeof(_comm_header));
  memset(backplane, 0, sizeof(_backplane_data));
  memset(rack, 0, sizeof(_rack));

  comm->hostname = argv[optind];
  if (quiet == FALSE)
    printf("Connecting to host %s\n", comm->hostname);
  establish_connection(comm, services, debug);
  if (comm->error != OK)
  {
    printf("An Error occured while connecting to host %s.\n", comm->hostname);
    exit(-1);
  }

  path->device1 = -1;
  path->device2 = -1;
  path->device3 = -1;
  path->device4 = -1;
  path->device5 = -1;
  path->device6 = -1;
  path->device7 = -1;
  path->device8 = -1;

  if (quiet == FALSE)
    printf("Got Session_ID = %ld\n", comm->session_id);
  get_backplane_data(comm, backplane, rack, path, debug);
    printf( "RX_Bad_m :%02X     ", backplane->rx_bad_m);
    printf( "ERR+Threshold: %02X     ",
	   backplane->err_threshold);
    printf( "RX_Bad_CRC :%02X    ", backplane->rx_bad_crc);
    printf( "RX_Bus_Timeout :%02X\n", backplane->rx_bus_timeout);
    printf( "TX_Bad_CRC :%02X   ", backplane->tx_bad_crc);
    printf( "TX_Bus_Timeout :%02X   ",
	   backplane->tx_bus_timeout);
    printf( "TX_Retry :%02X\n", backplane->tx_retry);
    printf( "Status :%02X    ", backplane->status);
    printf( "Address :%04X\n", backplane->address);
    printf( "Major Rev :%02X  ", backplane->rev_major);
    printf( "Minor Rev :%02X  ", backplane->rev_minor);
    printf( "Serial Number :%08lX  ", backplane->serial_number);
    printf( "Backplane size :%d\n", backplane->rack_size);
  who(comm, rack, NULL, debug);
  printf
  ("-----------------------------------------------------------------------\n");

  for (x=0;x<rack->size;x++)
    {
    char temp[33];
    memset (&temp,0,33);
    strncpy (temp, rack->identity[x]->name, 32);
    printf ("Slot %d - %s\n",x,temp);
    printf ("ID = %02X   type = %02X - ",rack->identity[x]->ID, rack->identity[x]->type);
    if (rack->identity[x]->type < 32)
      printf ("  - %s\n",PROFILE_Names[rack->identity[x]->type]);
    else
      printf ("  - Unknown Type.\n");
    printf ("Product Code = %02X    Revision = %d.%02d\n", rack->identity[x]->product_code, rack->identity[x]->rev_hi, rack->identity[x]->rev_lo);
    printf ("Status - %04X    Serial Number: %08lX\n",rack->identity[x]->status, rack->identity[x]->serial);
    printf ("-----------------------------------------------------------------------\n");
    }

for (x=0;x<rack->size;x++)
  {
  printf ("Looking at slot %d\n",x);
  if ((rack->identity[x]->type == 0x0c) && (rack->identity[x]->product_code == 0x07))
    {
    _identity id;
    
    /* CNB Card Found - let's scan it...*/
    printf ("CNB card found in slot %d, scanning channel A...\n",x);
    channel = 2;
    count = 0;
    for (y=0;y<99;y++)
      {
      memset (&id,0,sizeof(_identity));
      path->device1 = 1;
      path->device2 = x;
      path->device3 = channel;
      path->device4 = y;
      path->device5 = -1;
      path->device6 = -1;
      path->device7 = -1;
      path->device8 = -1;
      get_device_data (comm, path, &id, debug);
      if ((id.type != 0) && (id.ID != 0))
        {
        char temp[33];
        
        printf ("Device found at address %d\n",y);
        memset (&temp,0,33);
        strncpy (temp, id.name, 32);
        printf ("%s\n",temp);
        printf ("ID = %02X   type = %02X - ",id.ID, id.type);
        if (id.type < 32)
          printf ("  - %s\n",PROFILE_Names[id.type]);
        else
          printf ("  - Unknown Type.\n");
        printf ("Product Code = %02X    Revision = %d.%02d\n", id.product_code, id.rev_hi, id.rev_lo);
        printf ("Status - %04X    Serial Number: %08lX\n",id.status, id.serial);
        if ((id.type == 0x0c) && (id.product_code == 0x07))
          {
          if (id.serial == rack->identity[x]->serial)
            {
            printf ("This is our network connection.\n");
            printf ("-----------------------------------------------------------------------\n");
            }
          else
            {
            _backplane_data remotebackplane;
            _rack remoterack;
          
	    memset (&remotebackplane,0,sizeof(_backplane_data));
	    memset (&remoterack,0,sizeof(_rack));
	    
	    printf ("* * * Remote Rack Found - Scanning...\n");
            get_backplane_data (comm, &remotebackplane, &remoterack, path, debug);
            printf( "RX_Bad_m :%02X     ", remotebackplane.rx_bad_m);
            printf( "ERR+Threshold: %02X     ",
	 	remotebackplane.err_threshold);
            printf( "RX_Bad_CRC :%02X    ", remotebackplane.rx_bad_crc);
            printf( "RX_Bus_Timeout :%02X\n", remotebackplane.rx_bus_timeout);
            printf( "TX_Bad_CRC :%02X   ", remotebackplane.tx_bad_crc);
            printf( "TX_Bus_Timeout :%02X   ",
		remotebackplane.tx_bus_timeout);
            printf( "TX_Retry :%02X\n", remotebackplane.tx_retry);
            printf( "Status :%02X    ", remotebackplane.status);
            printf( "Address :%04X\n", remotebackplane.address);
            printf( "Major Rev :%02X  ", remotebackplane.rev_major);
            printf( "Minor Rev :%02X  ", remotebackplane.rev_minor);
            printf( "Serial Number :%08lX  ", remotebackplane.serial_number);
            printf( "Backplane size :%d\n", remotebackplane.rack_size);
            printf ("-----------------------------------------------------------------------\n");
	    printf ("Scanning Remote Rack Slots......\n");
	    who (comm, &remoterack, path, debug);

            for (z=0;z<remotebackplane.rack_size;z++)
	      {
	      _identity remotecard;
	      char tempname[33];
	      
	      memset (&tempname,0,33);
	      memset (&remotecard,0,sizeof(_identity));
	      path->device5 = 1;
	      path->device6 = z;
	      get_device_data (comm, path, &remotecard, debug);
              strncpy (tempname, remotecard.name, 32);
              printf ("%s\n",tempname);
              printf ("ID = %02X   type = %02X - ",remotecard.ID, remotecard.type);
              if (remotecard.type < 32)
                printf ("  - %s\n",PROFILE_Names[remotecard.type]);
              else
                printf ("  - Unknown Type.\n");
              printf ("Product Code = %02X    Revision = %d.%02d\n", remotecard.product_code, remotecard.rev_hi, remotecard.rev_lo);
              printf ("Status - %04X    Serial Number: %08lX\n",remotecard.status, remotecard.serial);
              printf ("-----------------------------------------------------------------------\n");
              }
            }
          }
        }
      }
    }
  if ((rack->identity[x]->type == 0x0c) && (rack->identity[x]->product_code == 0x12))
    {
    _df1_comm *df1_comm;
    _dhplus_map *dhmap;
    _df1_resp *resp;
    int node;
    
    df1_comm = malloc(sizeof(_df1_comm));
    dhmap = malloc(sizeof(_dhplus_map));
    resp = malloc(sizeof(_df1_resp));
    
    memset (df1_comm, 0, sizeof(_df1_comm));
    memset (dhmap, 0, sizeof(_dhplus_map));
    memset (resp, 0, sizeof(_df1_resp));
         
    path->device1 = 1;
    path->device2 = x;
    path->device3 = -1;
    path->device4 = -1;
    path->device5 = -1;
    path->device6 = -1;
    path->device7 = -1;
    path->device8 = -1;
    df1_open(comm, path, df1_comm, 1, debug);
    get_dh_map(comm, path, dhmap, 1, debug);
    printf ("DHRIO module found.\nChannel 1 - slot: %d, node: %d, total nodes: %d\n",x,dhmap->node_id, dhmap->node_count);
    if (dhmap->node_count > 1)
      for (y=0; y<8; y++)
        for (z=0; z<8; z++)
        {
        if (dhmap->node_map[y] & (byte)pow(2,z))
          {
          node = (y*8)+z;
          printf ("Node %d - ",node);
          df1_get_status(comm, df1_comm, resp, node, 1,debug);
          interpret (resp);
          }
        }
//      sleep(1);
      memset (df1_comm, 0, sizeof(_df1_comm));
      df1_open(comm, path, df1_comm, 2, debug);
      get_dh_map(comm, path, dhmap, 2, debug);
      printf ("Channel 2 - slot: %d, node: %d, total nodes: %d\n", x, dhmap->node_id, dhmap->node_count);
      if (dhmap->node_count > 1)
        for (y=0; y<8; y++)
          for (z=0; z<8; z++)
            {
            if (dhmap->node_map[y] & (byte)pow(2,z))
              {
              node = (y*8)+z;
              printf ("Node %d - ",node);
              df1_get_status(comm, df1_comm, resp, node, 2, debug);
              interpret (resp);
              }           
            }
   
    }
  }

  closesocket(comm->file_handle);
  exit(0);
}
