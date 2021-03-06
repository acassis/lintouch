#include "libcell.h"

int df1_typed_read (_comm_header *comm, _df1_comm *df1_comm, char *reg, _df1_resp *resp, 
  byte node, byte channel, int plctype, unsigned short count, int debug)
{

_df1 *df1;
_pccc_namedata *name;
_pccc_word_read *cmd;
int x,place;

dprint (DEBUG_TRACE,"Entering df1_typed_read\n");
if (df1_comm == NULL)
  {
  CELLERROR(1,"df1_comm struct not defined");
  return (-1);
  }
if (strlen(reg) < 1)
  {
  CELLERROR(2,"Invalid register to read.");
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

df1 = malloc(sizeof(_df1_word_read));
if (df1 == NULL)
  {
  CELLERROR(5,"Could not allocate DF1 struct");
  return (-5);
  }
name = malloc(sizeof(_pccc_namedata));
if (name == NULL)
  {
  CELLERROR(6,"Could not allocate name struct");
  return (-6);
  }

cmd = malloc(sizeof(_pccc_word_read));
if (cmd == NULL)
  {
  CELLERROR(8,"Could not allocate CMD struct");
  return (-8);
  }
if (resp == NULL)
  {
  CELLERROR(9,"Response struct not allocated");
  return (-9);
  }
  
memset (cmd, 0, sizeof(_pccc_word_read));
memset (df1,0,sizeof(_df1_word_read));
memset (name, 0, sizeof(_pccc_namedata));
memset (resp, 0, sizeof(_df1_resp));

place = 0;
df1->df1_port = channel;
cmd->cmd = 0x0f;
cmd->fnc = 0x68;
cmd->tns = comm->df1_tns;
cmd->dst = node;
cmd->offset = 0;



nameconv5(reg, name, plctype, debug);
memcpy (cmd->data, name->data, name->len);
place = name->len;

if (count == 0)
  count=1;
count = count * 2;
if (name->floatdata == TRUE)
  count = count * 2;
cmd->data[place++] = (byte)count;
if (plctype == PLC5250)
  cmd->trans = htons(count/2);
if (plctype != PLC5250)
  cmd->trans = (count);  
df1->df1_datalen = 16 + place;
memcpy (df1->cmd, cmd, 16+place);

x = send_df1 (comm, df1_comm, df1, resp, debug);
if (x < 0)
  printf ("Send_DF1 failed - status = %d.\n",x);

free(df1);
free(name);
free(cmd);
return x;
}
