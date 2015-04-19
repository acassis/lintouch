#include "libcell.h"

int df1_get_status (_comm_header *comm, _df1_comm *df1_comm, _df1_resp *resp, byte node, byte channel, int debug)
{

_df1 *df1;
_df1_cmd *cmd;
int x;

dprint (DEBUG_TRACE,"Entering df1_get_status\n");
if (df1_comm == NULL)
  {
  CELLERROR(1,"df1_comm struct not defined");
  return (-1);
  }
if (resp == NULL)
  {
  CELLERROR(2,"Resp struct not defined");
  return (-2);
  }
if (node > 63)
  {
  CELLERROR(3,"Node must be less than 64");
  return (-3);
  }
if ((channel < 1) || (channel > 2))
  {
  CELLERROR(4,"Channel must be 1 or 2");
  return (-4);
  }

df1 = malloc(sizeof(_df1));
if (df1 == NULL)
  {
  CELLERROR(5,"Could not allocate DF1 struct");
  return (-5);
  }
cmd = malloc(sizeof(_df1_cmd));
if(cmd == NULL)
  {
  CELLERROR(6,"Could not allocate CMD struct");
  return (-6);
  }
  
memset (cmd,0,sizeof(_df1_cmd));
memset (df1,0,sizeof(_df1));

df1->df1_port = channel;
df1->df1_datalen = 11;
cmd->cmd = 6;
cmd->fnc = 3;
cmd->tns = comm->df1_tns;
cmd->dst = node;
memcpy(df1->cmd, cmd, 11);

x = send_df1 (comm, df1_comm, df1, resp, debug);
if (x < 0)
  printf ("Send_DF1 failed - status = %d.\n",x);
free(df1);
free(cmd);
return x;
}
