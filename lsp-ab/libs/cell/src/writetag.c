#include "libcell.h"
#include <stdlib.h>

void helpme(void)
{
printf ("writetag (processor) tagname {program name} (value)\n\n");
printf ("Allows you to write data to a specific tag in the specified ControlLogix PLC.\n\n");
printf ("(processor) = ip address or domain name of target PLC.\n");
printf ("tagname = the tag you wish to read from the PLC.\n");
printf ("{program name} = optional program name to find the tag in.\n");
printf ("(value) = the numeric data you want to write.\n\n");
return;
}



int main(int argc, char **argv)
{
_comm_header *comm;
_tag_detail *tag;
_path *path;
_rack *rack;
_services *services;
//_backplane_data *backplane;
int result,x,debug=4;

result = 0;

  while ((x = getopt(argc, argv, "dh?")) != -1)
  {
    switch (x)
    {
    case 'd':
      debug++;
      break;
    case 'h':
    case '?':
      helpme();
      exit(1);
    }
  }


	
// Note that this is quick and dirty - no error checking...

tag = malloc(sizeof(_tag_detail));
memset(tag,0,sizeof(_tag_detail));

tag->data = malloc(128);
memset (tag->data,0,128);

rack = malloc(sizeof(_rack));
memset (rack,0,sizeof(_rack));

//backplane = malloc(sizeof(_backplane_data));
//memset (backplane,0,sizeof(_backplane_data));

path = malloc(sizeof(_path));
memset (path,0,sizeof(_path));

services = malloc(sizeof(_services));
memset (services,0,sizeof(_services));

comm = malloc(sizeof(_comm_header));
memset (comm,0,sizeof(_comm_header));

dprint (DEBUG_TRACE,"setting plc name %d %s.\n",strlen(argv[1]), argv[1]);
comm->hostname = argv[1];
dprint (DEBUG_TRACE,"attaching to plc\n");

establish_connection (comm, services, 0);
if (comm->error != 0)
	{
	printf ("Could not attach to %s\n",argv[1]);
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

//get_backplane_data(comm, backplane, rack, path, 0);
dprint (DEBUG_TRACE,"polling PLC rack layout\n");
who(comm, rack, NULL, 0);
path->device1 =1;
path->device2 = rack->cpulocation;
dprint (DEBUG_TRACE,"writing tag.\n");
if (argc == 4)
	{
	long junk;
	junk = atoi(argv[3]);
	tag->data[0] = (byte)(junk & 255);
	tag->data[1] = (byte)(junk >> 8);
	tag->size = 1;
	tag->type = 0xc3;
	result = write_tag(comm, path, NULL, argv[2], tag, 0);
	}
if (argc == 5)
	{
	tag->data[0] = (byte)(atoi(argv[4]) & 255);
	tag->data[1] = (byte)(atoi(argv[4])>>8);
	tag->size = 1;
	tag->type = 0xc3;
	result = write_tag(comm, path, argv[3], argv[2], tag, 0);
	}
if (result != 0)
	{
	printf ("writing tag %s failed - does it exist?\n\n",argv[2]);
	exit(-1);
	}
for (x=0; x<tag->datalen; x++)
	printf ("%02X ",tag->data[x]);
printf ("\n\n");
exit(0);
}

