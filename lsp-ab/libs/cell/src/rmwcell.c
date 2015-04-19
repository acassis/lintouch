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
  int x, y, z, debug, program_count, quiet, result;
  _tag_detail *bittag;
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



  memset(services, 0, sizeof(_services));
  memset(comm, 0, sizeof(_comm_header));
  memset(backplane, 0, sizeof(_backplane_data));
  memset(rack, 0, sizeof(_rack));
  memset(configtags, 0, sizeof(_tag_data));
  memset(detailtags, 0, sizeof(_tag_data));
  memset(progs, 0, sizeof(_prog_list));
  memset(structs, 0, sizeof(_struct_list));
  memset(list, 0, sizeof(_tag_list));

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

	printf ("configtags count = %d, detailtags count = %d\n",
		configtags->count, detailtags->count);

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
		if( strcmp( detailtags->tag[z]->name, "PMIS_B30" ) == 0 )
		{
			bittag = detailtags->tag[z];
			break;
		}
  }

while (1)
{
int offset, index, bit;
byte andmask, ormask;

/* this section should flash PMIS_B30[2].3 every second */

/* We need to convert the array index into a byte offset. */

index = 2;
bit = 3;

offset = bittag->size * index;
if (bit > 7)
  {
  offset++;
  bit-=8;
  }
andmask = 0xff;
ormask = pow(2,bit);
object_bit_modify_byte(comm, path, bittag, andmask, ormask, offset, debug);  /* turn it on */
sleep(1);
andmask = ~ormask;
ormask = 0;
object_bit_modify_byte(comm, path, bittag, andmask, ormask, offset, debug); /* turn it off */
sleep(1);
}


  closesocket(comm->file_handle);
  exit(0);
}
