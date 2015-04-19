#include "libcell.h"

void helpme(void)
{
  printf("Test program for ControlLogix Communications\n\n");
  printf("test [-d -q] hostname\n\n");
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




int main(int argc, char **argv)
{
  _comm_header *comm;
  _path *path;
  _services *services;
  _backplane_data *backplane;
  _rack *rack;
  _tag_data *configtags, *detailtags;
  _tag_data *program_tags[64];
  _tag_list *list;
  _prog_list *progs;
  _struct_list *structs;
  _dhplus_map *data;
  _df1_resp *resp;
  _df1_comm *df1_comm;
  struct timeval begin, end;
  long long begintime, endtime;
  int x, y, z, debug, program_count, quiet, result, count;
  int c1 = 0, c68 = 0, c69 = 0;
#ifdef WIN32
  int optind = 1;
  debug = DEBUG_DATA;
#endif

#ifndef WIN32
  debug = 0;
#endif

  program_count = 0;
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
  configtags = malloc(sizeof(_tag_data));
  if (configtags == NULL)
  {
    printf("Could not allocate memory for configtags structure.\n");
    free(path);
    free(backplane);
    free(comm);
    free(services);
    exit(-1);
  }
  detailtags = malloc(sizeof(_tag_data));
  if (detailtags == NULL)
  {
    printf("Could not allocate memory for detailtags structure.\n");
    free(path);
    free(backplane);
    free(comm);
    free(services);
    exit(-1);
  }
  progs = malloc(sizeof(_prog_list));
  if (progs == NULL)
  {
    printf("Could not allocate memory for program list structure.\n");
    free(detailtags);
    free(path);
    free(backplane);
    free(comm);
    free(services);
    exit(-1);
  }
  structs = malloc(sizeof(_struct_list));
  if (structs == NULL)
  {
    printf("Could not allocate memory for structure list.\n");
    exit(-1);
  }
  list = malloc(sizeof(_tag_list));
  if (list == NULL)
  {
    printf ("Could not allocate memory for tag list.\n");
    exit(-1);
  }
  data = malloc(sizeof(_dhplus_map));
  if (data == NULL)
    {
    printf ("Could not allocate dhplus map.\n");
    exit(-1);
    }
  resp = malloc(sizeof(_df1_resp));
  if (resp == NULL)
    {
    printf ("Could not allocate DF1 Command struct.\n");
    exit(-1);
    }
  df1_comm = malloc(sizeof(_df1_comm));
  if (df1_comm == NULL)
    {
    printf ("Could not allocate DF1 COMM struct.\n");
    exit(-1);
    }
    


  memset(services, 0, sizeof(_services));
  memset(comm, 0, sizeof(_comm_header));
  memset(backplane, 0, sizeof(_backplane_data));
  memset(rack, 0, sizeof(_rack));
  memset(configtags, 0, sizeof(_tag_data));
  memset(detailtags, 0, sizeof(_tag_data));
  memset(progs, 0, sizeof(_prog_list));
  memset(structs, 0, sizeof(_struct_list));
  memset(list, 0, sizeof(_tag_list));
  memset(data, 0, sizeof(_dhplus_map));
  memset(resp, 0, sizeof(_df1_resp));
  memset(df1_comm, 0, sizeof(_df1_comm));
  
  comm->hostname = argv[optind];
  if (quiet == FALSE)
    printf("Connecting to host %s\n", comm->hostname);
  establish_connection(comm, services, debug);
  if (comm->error != OK)
  {
    printf("An Error occured while connecting to host %s.\n", comm->hostname);
    exit(-1);
  }

  path->device1 = 1;
  path->device2 = 2;
  path->device3 = -1;
  path->device4 = -1;
  path->device5 = -1;
  path->device6 = -1;
  path->device7 = -1;
  path->device8 = -1;

  df1_open(comm, path, df1_comm, 1, debug);
  printf ("comm opened - id = %08lX\n",df1_comm->ot_conn_id);
while(1)
  {
  int node;
  memset (data,0,sizeof(_dhplus_map));
  x = get_dh_map(comm, path, data, 1, debug);
  if (x != 0)
    {
    printf ("problem with get_dh_map.\n");
    exit(-1);
    }
  system("clear");
  printf ("DHRIO Channel 1 (DH Node %d) has %d nodes\n",data->node_id, data->node_count);
  if (data->node_count > 1)
    for (x=0; x<8; x++)
      for (y=0; y<8; y++)
        {
        if (data->node_map[x] & (byte)pow(2,y))
          {
          node = (x*8)+y;
          printf ("Node %d - ",node);
          df1_get_status(comm, df1_comm, resp, node, 1,debug);
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
              result = df1_read_section_size (comm, df1_comm, "n7", resp, node, 1, PLC5, debug);
              printf ("Get Section Size: ");
              for (count=0; count < result - 10; count++)
                printf ("%02X ",resp->df1data[count]);
              printf ("\n");
              c1 = ((byte)resp->df1data[0] + ((byte)resp->df1data[1] * 256)) -1;
              if (c1 > 100) c1=100;
              printf ("Reading from N7:0 to N7:%d\n",c1);
              result = df1_word_read (comm, df1_comm, "N7:0", resp, node, 1, PLC5, c1, debug);
              printf ("Read Data: ");
              for (count=0; count < (result-10); count++)
                printf ("%02X ",resp->df1data[count]);
              printf ("\n");
              break;
            case 0xde:
              printf ("Pyramid   ");
              break;
            case 0xee:
              printf ("SLC/Micro ");
              break;
            default:
              printf ("Unk: %02X   ",resp->df1data[1]);
              break;
            }
          printf ("\n"); 
          }
        }
  printf ("\n");
/*  df1_close(comm, path, df1_comm, 1, debug);

  memset (data,0,sizeof(_dhplus_map));
  x = get_dh_map(comm, path, data, 2, 0);
  if (x != 0)
    {
    printf ("problem with get_dh_map.\n");
    exit(-1);
    }
  printf ("DHRIO Channel 2 (DH Node %d) has %d nodes\n",data->node_id, data->node_count);
  if (data->node_count > 1)
    for (x=0; x<8; x++)
      for (y=0; y<8; y++)
        {
        if (data->node_map[x] & (byte)pow(2,y))
          {
          node = (x*8)+y;
          printf ("Node %d \n",node);
          df1_get_status(comm, path, resp, node, 2, debug);
          }
        }
          
        

  printf ("\n");
 */
  sleep(1);
  }




  if (quiet == FALSE)
    printf("Got Session_ID = %ld\n", comm->session_id);
  get_backplane_data(comm, backplane, rack, path, debug);
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "RX_Bad_m :%02X     ", backplane->rx_bad_m);
    dprint(DEBUG_VALUES, "ERR+Threshold: %02X     ",
	   backplane->err_threshold);
    dprint(DEBUG_VALUES, "RX_Bad_CRC :%02X    ", backplane->rx_bad_crc);
    dprint(DEBUG_VALUES, "RX_Bus_Timeout :%02X\n", backplane->rx_bus_timeout);
    dprint(DEBUG_VALUES, "TX_Bad_CRC :%02X   ", backplane->tx_bad_crc);
    dprint(DEBUG_VALUES, "TX_Bus_Timeout :%02X   ",
	   backplane->tx_bus_timeout);
    dprint(DEBUG_VALUES, "TX_Retry :%02X\n", backplane->tx_retry);
    dprint(DEBUG_VALUES, "Status :%02X    ", backplane->status);
    dprint(DEBUG_VALUES, "Address :%04X\n", backplane->address);
    dprint(DEBUG_VALUES, "Major Rev :%02X  ", backplane->rev_major);
    dprint(DEBUG_VALUES, "Minor Rev :%02X  ", backplane->rev_minor);
    dprint(DEBUG_VALUES, "Serial Number :%08lX  ", backplane->serial_number);
    dprint(DEBUG_VALUES, "Backplane size :%d\n", backplane->rack_size);
  }
  who(comm, rack, NULL, debug);
  path->device1 = 1;
  path->device2 = rack->cpulocation;
  path->device3 = -1;
  path->device4 = -1;
  path->device5 = -1;
  path->device6 = -1;
  path->device7 = -1;
  path->device8 = -1;

  get_object_config_list(comm, path, 0, configtags, debug);
  get_object_details_list(comm, path, 0, detailtags, debug);
  get_program_list(comm, path, progs, debug);
/*  get_struct_list(comm, path, structs, debug);


printf ("Got %d structs in the program.\n",structs->count);
for (x=0;x<structs->count; x++)
	{
	printf ("Struct #%02d, type id = %08lX\n",x,structs->base[x]->base);
	get_struct_config (comm, path, structs->base[x], debug);
	get_struct_details (comm, path, structs->base[x], debug);
	printf ("Element count = %d    Detail size = %d\n",structs->base[x]->count, structs->base[x]->detailsize);
	printf ("Element link-id = %04X\n",structs->base[x]->linkid);
	printf ("Structure base name = %s\n",structs->base[x]->name);
	for (y=0;y<structs->base[x]->count;y++)
		{
		printf ("Element #%d, type = %04X, arraysize = %d name = %s\n",y,structs->base[x]->data[y]->type, structs->base[x]->data[y]->arraysize, structs->base[x]->data[y]->name);
		}
	printf ("----------------------------------------\n");
	}

exit;
*/
printf ("configtags count = %d, detailtags count = %d\n",configtags->count, detailtags->count);

y = detailtags->count;
if (configtags->count > y)
  y = configtags->count;

printf("Reading %d tags from ControlLogix...\n",y);
for (z = 0; z < y; z++)
  {
  int ret=0;
  if (z < detailtags->count)
    {
    ret = get_object_details(comm, path, detailtags->tag[z], debug);
    if (ret != 0)
      printf ("get_object_details errored out - %d.\n",ret);
    }
  if (z < configtags->count)
    {
    ret = get_object_config(comm, path, configtags->tag[z], debug);
    if (ret != 0)
      printf ("get_object_config errored out - %d.\n",ret);
    }
  }
printf("Checking for aliases...\n");

for (z = 0; z < detailtags->count; z++)
  aliascheck(detailtags->tag[z], NULL, detailtags, DEBUG_VALUES);

for (z = 0; z < y; z++)
  {

  if (strncmp ( detailtags->tag[z]->name, "chute_io_in",strlen(detailtags->tag[z]->name)))
    {
    while (1)
      {
      gettimeofday (&begin, NULL);
      read_object_value(comm, path, detailtags->tag[z], debug);
      gettimeofday (&end, NULL);
      for (x=0;x<detailtags->tag[z]->datalen;x++)
        printf ("%02X ",detailtags->tag[z]->data[x]);
      printf ("\n"); 
      begintime = (begin.tv_sec * 1000000) + begin.tv_usec;
      endtime = (end.tv_sec * 1000000) + end.tv_usec;
      printf ("Elapsed time: %lld\n",endtime - begintime);			
      }
    }			
  }
exit(0);
}
