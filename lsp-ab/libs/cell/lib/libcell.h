
/* 
   Revised 5/7/02 William Hays - CTI 

   5/7/2002 WRH  Added ms_timeout to comm_header for faster error comm loss handling
   5/7/2002 WRH  included tcp.h to support setsockopt changes in attach.c
*/
#ifdef WIN32
#define __EXPORT __declspec(dllexport)
#define strncasecmp(a,b,c) _strnicmp(a,b,c)
#include <windows.h>
#include <winsock.h>
#include <memory.h>
#include <stdarg.h>
#include "timeval.h"
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

#ifdef unix
typedef unsigned char byte;

#define __EXPORT
#define closesocket close
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#define OK 0
#define NOHOST -1
#define NOCONNECT -2
#define BADADDR -3

#define CONNECT_CMD 1
#define PCCC_CMD 7
#define DH_CMD 6

#define _ENET_HEADER_LEN 28
#define _CUSTOM_LEN 16
#define PCCC_DATA_LEN 9
#define PCCC_WORD_LEN 13
#define PCCC_MEM_LEN 14

#define PCCC_VERSION 4
#define PCCC_BACKLOG 5

#define ETHERNET 1
#define RS232 2
#define KT 3

#define READ_ONLY 1
#define READ_WRITE 3


#define PROGRAM_MODE 0
#define TEST_MODE 1
#define RUN_MODE 2
#define SLC_PROGRAM_MODE 1
#define SLC_RUN_MODE 6
#define SLC_TEST_CONT 7
#define SLC_TEST_SINGLE 8
#define SLC_TEST_DEBUG 9
#define MICRO_PROGRAM_MODE 1
#define MICRO_RUN_MODE 2


#define PLC5 1
#define PLC5250 2
#define SLC 3
#define MICRO 4




/* 

DEBUG stuff is here.  This defines a debug printing macro called dprint.
This function should work just like a simple printf statement that is
conditioned by an "if" statement.

NOTE: A debug level includes all lower debug levels.  For example: a debug 
level of DEBUG_TRACE will also turn on DEBUG_VALUES.

The various debug levels are as follows:
0 - NIL   	Print no debug info (default level)
1 - VALUES	Print values as they are discovered
2 - TRACE	Print entry and exit from various routines
3 - DATA	Print data as it is sent and received from the CL
4 - BUILD	Basically print everything, including the building of 
		Data packet information.
		
		
*/	

#ifndef WIN32
#define dprint(a,b,c...)   if (debug>=a) fprintf (stdout,b, ## c)
#endif

#ifdef WIN32
#ifndef HAVE_DPRINT
#define HAVE_DPRINT
__EXPORT extern int debug;
__EXPORT extern void dprint(int a,const char *b,...);
#endif
#endif

#define DEBUG_NIL 0
#define DEBUG_VALUES 1
#define DEBUG_TRACE 2
#define DEBUG_DATA 3
#define DEBUG_BUILD 4


#ifdef SPARC
#define BES(a)  ((unsigned short)((((unsigned short)(a))>>8) | (((unsigned short)(a))<<8)))
#define BEL(a)  ((BES(a & 0xffff)<< 16) | (BES(a>>16)))
#endif

#ifndef BES
#define BES(a)  a
#define BEL(a)  a
#endif

 
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#define DESCRIPTION_PROP 0x01
#define VALUE_PROP 0x02


/*              * * * Data Types * * *               */

#define CIP_PROGRAM 0x69
#define CIP_MAP 0x68
#define CIP_COUNTER 0x82
#define CIP_TIMER 0x83
#define CIP_PID 0x84
#define CIP_BOOL 0xc1
#define CIP_SINT 0xc2
#define CIP_INT 0xc3
#define CIP_DINT 0xc4
#define CIP_LINT 0xc5
#define CIP_USINT 0xc6
#define CIP_UINT 0xc7
#define CIP_UDINT 0xc8
#define CIP_ULINT 0xc9
#define CIP_REAL 0xca
#define CIP_LREAL 0xcb
#define CIP_STIME 0xcc
#define CIP_DATE 0xcd
#define CIP_TIME_OF_DAY 0xce
#define CIP_DATE_AND_TIME 0xcf
#define CIP_STRING 0xd0
#define CIP_BYTE 0xd1
#define CIP_WORD 0xd2
#define CIP_DWORD 0xd3
#define CIP_LWORD 0xd4
#define CIP_STRING2 0xd5
#define CIP_FTIME 0xd6
#define CIP_LTIME 0xd7
#define CIP_ITIME 0xd8
#define CIP_STRINGN 0xd9
#define CIP_SHORT_STRING 0xda
#define CIP_TIME 0xdb
#define CIP_EPATH 0xdc
#define CIP_ENGUNIT 0xdd

#define PID_EN 0x80000000
#define PID_CT 0x40000000
#define PID_CL 0x20000000
#define PID_PVT 0x10000000
#define PID_DOE 0x08000000
#define PID_SWM 0x04000000
#define PID_CA 0x02000000
#define PID_MO 0x01000000
#define PID_PE 0x00800000
#define PID_NDF 0x00400000
#define PID_NOBC 0x00200000
#define PID_NOZC 0x00100000
#define PID_INI 0x00008000
#define PID_SPOR 0x00004000
#define PID_OLL 0x00002000
#define PID_OLH 0x00001000
#define PID_EWD 0x00000800
#define PID_DVNA 0x00000400
#define PID_DVPA 0x00000200
#define PID_PVLA 0x00000100
#define PID_PVHA 0x00000080

#define TIMER_EN 0x80
#define TIMER_TT 0x40
#define TIMER_DN 0x20
#define TIMER_FS 0x10
#define TIMER_LS 0x08
#define TIMER_OV 0x04
#define TIMER_ER 0x02

#define COUNTER_CU 0x80
#define COUNTER_CD 0x40
#define COUNTER_DN 0x20
#define COUNTER_OV 0x10
#define COUNTER_UN 0x08

#define ALARM_ENABLE_IN 0x01
#define ALARM_ENABLE_OUT 0x01
#define ALARM_HH_ALARM 0x02
#define ALARM_H_ALARM 0x04
#define ALARM_L_ALARM 0x08
#define ALARM_LL_ALARM 0x10
#define ALARM_ROC_POS_ALARM 0x20
#define ALARM_ROC_NEG_ALARM 0x40
#define ALARM_INSTRUCT_FAULT 0x01
#define ALARM_DEADBAND_INV 0x02
#define ALARM_ROC_POS_INV 0x04
#define ALARM_ROC_NEG_INV 0x08
#define ALARM_ROC_PERIOD_INV 0x10

#define SEL_ENABLE_IN 0x01
#define SEL_SELECTOR_IN 0x02
#define SEL_ENABLE_OUT 0x01

#define IF16_ANALOG_GROUP_FAULT 0x10
#define IF16_CALIBRATING 0x20
#define IF16_CAL_FAULT 0x40

#define IF16_CHAN_CAL_FAULT 0x10
#define IF16_CHAN_UNDERRANGE 0x40
#define IF16_CHAN_OVERRANGE 0x20


/* These are the objects - things that you send commands to...  */

#define IDENTITY 0x01
#define ROUTER 0x02
#define CONNECTION 0x05
#define CONNECTION_MANAGER 0x06
#define REGISTER 0x07
#define BACKPLANE_DATA 0x66
#define OBJECT_SUB_OBJECT 0x68
#define OBJECT_CONFIG 0x6a
#define OBJECT_DETAILS 0x6b
#define STRUCT_LIST 0x6c
#define DATA_OBJECT 0x72
#define REMOTE_DH 0xa4
#define DHMAP 0xa5
#define DF1CMD 0xa6
#define PORT_OBJECT 0xf4

/* These are the "services" - commands by any other name...  */

#define GET_ATTRIBUTE_ALL 0x01
#define SET_ATTRIBUTE_ALL 0x02
#define GET_ATTRIBUTE_LIST 0x03
#define SET_ATTRIBUTE_LIST 0x04
#define RESET 0x05
#define START 0x06
#define STOP 0x07
#define CREATE 0x08
#define CIP_DELETE 0x09
#define MULTI_SERVICE 0x0a
#define APPLY_ATTRIBUTES 0x0d
#define GET_ATTRIBUTE_SINGLE 0x0e
#define SET_ATTRIBUTE_SINGLE 0x10
#define FIND_NEXT_OBJECT 0x11
#define RESTORE 0x15
#define SAVE 0x16
#define NOP 0x17
#define GET_MEMBER 0x18
#define SET_MEMBER 0x19
#define INSERT_MEMBER 0x1a
#define REMOVE_MEMBER 0x1b
#define CONN_KICK_TIMER 0x4b
#define CONN_OPEN 0x4c
#define CONN_CLOSE 0x4d
#define CONN_STOP 0x4e
#define CONN_CHANGE_START 0x4f
#define CONN_GET_STATUS 0x50
#define CONN_CHANGE_COMPLETE 0x51
#define CONN_AUDIT_CHANGE 0x52
#define LIST_ALL_OBJECTS 0x4b
#define DATA_TYPE_CONFIG 0x4c
#define DATA_READ_TAG 0x4c
#define DATA_WRITE_TAG 0x4d
#define OBJECT_RMW 0x4e
#define GET_SINGLE_PROP 0x4f
#define PUT_SINGLE_PROP 0x50
#define OBJECT_RMW_BYTE 0x51



#define MAX_INSTANCE 0x02
#define NUM_INSTANCES 0x03
#define ENTRY_PORT 0x08
#define ALL_PORTS 0x09
 
#define CELL_EIP 1
#define CELL_CSP 2
#define CELL_DHRIO 3
#define CELL_RS232 4



#define BASE_INSTANCE 0x00
#define FIRST_INSTANCE 0x01
#define SECOND_INSTANCE 0x02

/* TPC = Transport Class */

#define TPC_NULL 0x00
#define TPC_DUPLICATE_DETECT 0x01
#define TPC_ACKNOWLEDGED 0x02
#define TPC_VERIFIED 0x03
#define TPC_NONBLOCKING 0x04
#define TPC_FRAGMENTING 0x05
#define TPC_MULTICAST_FRAG 0x06

/* TPM = Trigger Mode */

#define TPM_CYCLIC 0x00
#define TPM_CHANGE_OF_STATE 0x01
#define TPM_APPLICATION 0x02

#define VENDOR_ID 721
#define VENDOR_SN 0xdeadbeef

#define CPH_Null 0x00
#define CPH_Connection_based 0xa1
#define CPH_Sequenced_address 0x8002
#define CPH_Transport_PDU 0xb1
#define CPH_Unconnected_message 0xb2
#define CPH_Sockinfo_ot 0x8000
#define CPH_Sockinfo_to 0x8001

#define PDU_Forward_Close 0x4e
#define PDU_Unconnected_Send 0x52
#define PDU_Forward_Open 0x54
#define PDU_GetConnectionData 0x56
#define PDU_SearchConnectionData 0x57
#define PDU_ExForwardOpen 0x59
#define PDU_GetConnectionOwner 0x5a



#define ENCAPS_NOP 0x00
#define ENCAPS_List_Targets 0x01
#define ENCAPS_List_Services 0x04
#define ENCAPS_ListIdentity 0x63
#define ENCAPS_List_Interfaces 0x64
#define ENCAPS_Register_Session 0x65
#define ENCAPS_UnRegister_Session 0x66
#define ENCAPS_SendRRData 0x6f
#define ENCAPS_SendUnitData 0x70
#define ENCAPS_Header_Length 24


#define SERVICE_Communications 0x100

#define CELL_DFLT_TIMEOUT 20000  /* Default sleep value (ms) */
#define CELL_MAX_TAGS 21000
#define CELL_MAX_PROGS 512
#define CELL_MAX_SBASE 256
#define CELL_MAX_RACKID 32
#define CELL_PKT_SIZE 256
#define CELL_TAG_LEN 64

#define DATA_Buffer_Length 20480

#pragma pack(1)


struct floatstuff
{
unsigned short data1;
unsigned short data2;
};

union cdata
{
unsigned short idata[2];
float fdata;
};

typedef struct 
{
byte protection;
short number;
byte type;
} _pccc_file;

typedef struct
{
byte sts;
byte extsts;
byte good;
} _pccc_results;


typedef struct
{
byte dst;
byte control;
byte src;
byte lsap;
byte cmd;
byte sts;
short tns;
byte fnc;
byte data[250];
} _pccc_send_data;


typedef struct
{
unsigned short dst;
unsigned short control;
unsigned short src;
byte cmd;
byte sts;
unsigned short tns;
byte fnc;
byte data[255];
} _pccc_normal;

typedef struct 
{
unsigned short dst;
unsigned short control;
unsigned short src;
byte cmd;
byte sts;
unsigned short tns;
byte fnc;
unsigned long addr;
byte count;
} _pccc_memread;

typedef struct
{
unsigned short dst;
unsigned short control;
unsigned short src;
byte cmd;
byte sts;
unsigned short tns;
byte fnc;
short offset;
short trans;
byte data[255];
} _pccc_word_read;

typedef struct {
byte mode;
byte plctype;
byte expansion;
unsigned long memsize;
byte revlevel;
byte dhnodecount;
byte ioaddress;
byte commparams;
unsigned short datatablefiles;
unsigned short datatypefiles;
byte forces;
byte memoryprotect;
byte badram;
} _plc525status;

typedef struct {
byte mode;
byte plctype;
byte expansion;
unsigned long memsize;
byte revlevel;
byte dhnodecount;
byte ioaddress;
byte commparams;
unsigned short datatablefiles;
unsigned short datatypefiles;
byte forces;
byte memoryprotect;
byte badram;
byte debugmode;
unsigned short holdpointfile;
unsigned short holdpointelement;
unsigned short editsecond;
unsigned short editminute;
unsigned short edithour;
unsigned short editday;
unsigned short editmonth;
unsigned short edityear;
byte cmdport;
} _plc5status;

typedef struct {
byte mode;
byte plctype;
byte expansion;
byte forces;
byte ioaddress;
unsigned short programsequence;
unsigned short datasequence;
unsigned short usersequence;
} _pyramidstatus;

typedef struct {
byte mode;
byte plctype;
byte expansion;
byte proctype;
byte revlevel;
byte name[11];
unsigned short error;
byte runmode;
unsigned short programid;
byte memsize;
byte ownernode;
} _slc501status;

typedef struct {
byte mode;
byte plctype;
byte expansion;
byte proctype;
byte revlevel;
byte name[11];
unsigned short error;
byte runmode;
byte runstatus;
unsigned short programid;
byte memsize;
byte ownernode;
} _slc503status;


struct plc5stat 
{
byte mode;
byte faulted;
byte download;
byte testedits;
byte plctype;
byte offline;
byte edithere;
byte type;
byte expansion_type;
byte interface;
byte series;
byte rev;
unsigned long memsize;
byte dhaddr;
byte io_addr;
byte io_conf;
unsigned short tablefiles;
unsigned short progfiles;
byte forces_here;
byte forces_on;
byte memprot;
byte mem_bad;
byte debug;
short hold_file;
short hold_element;
short edit_time_sec;
short edit_time_min;
short edit_time_hour;
short edit_time_day;
short edit_time_month;
short edit_time_year;
byte backup_active;
byte partner_active;
byte edit_resource_allocated;
byte outputs_reset;
byte sfc_forces_active;
short prog_change_count;
short data_change_count;
short user_data_change_count;
byte name[11];
short error_word;
byte comm_active;
byte protection_power_loss;
byte startup_protect_fault;
byte load_rom_on_error;
byte load_rom_always;
byte load_rom_and_run;
byte plc_halted;
byte locked;
byte first_scan;
short program_id;
byte program_owner;
byte cmdport;
};


struct ethernet_header
{
byte mode;
byte submode;
unsigned short pccc_length;
unsigned long conn;
unsigned long status;
byte custom[16];
byte df1_stuff[255];
};

struct custom_connect
{
short version;
short backlog;
byte junk[12];
};

struct custom_pccc
{
unsigned long request_id;
unsigned long name_id;
byte junk[8];
};

struct custom_route
{
unsigned long request_id;
unsigned long name_id;
byte pushwheel;
byte module;
byte channel;
byte protocol;
byte flags;
byte junk[3];
};




typedef struct 
{
byte sts;
byte extsts;
byte uploadable_seg_count;
long upload_seg_start[100];
long upload_seg_end[100];
byte comparable_seg_count;
long comparable_seg_start[100];
long comparable_seg_end[100];
} _pccc_upload_data;


typedef struct 
{
int module;
int area;
int subarea;
int section;
int file;
int element;
int subelement;
int mask;
int type;
int typelen;
int bit;
byte len;
byte floatdata;
byte data[24];
} _pccc_namedata;

typedef struct
{
unsigned short size_lo;
unsigned short size_hi;
unsigned short count;
byte write_priv;
byte read_priv;
byte type;
byte sts;
byte extsts;
} _pccc_size_data;

typedef struct 
{
_pccc_namedata *name;
short len;
byte floatdata;
unsigned short data[250];
} _pccc_data;

typedef struct {
byte node_id;
unsigned short junk1;
unsigned short junk2;
unsigned short junk3[6];
byte retry1;
byte retry2;
byte junk4[25];
byte node_count;
byte node_map[8];
unsigned short junk;
} _dhplus_map;

typedef struct{
unsigned short dst;
unsigned short junk1;
unsigned short src;
byte cmd;
byte sts;
unsigned short tns;
byte fnc;
byte df1data[255];
} _df1_cmd;

typedef struct {
unsigned short len;
byte data[512];
} _df1_senddata;

typedef struct{
unsigned short dst;
unsigned short junk1;
unsigned short src;
byte cmd;
byte sts;
unsigned short tns;
byte df1data[255];
} _df1_resp;

typedef struct{
unsigned long ot_conn_id;
unsigned long to_conn_id;
unsigned short conn_serial;
unsigned short vendor_id;
unsigned long orig_serial;
byte timeout_mul;
byte junk[3];
unsigned long ot_rpi;
unsigned short ot_param;
unsigned long to_rpi;
unsigned short to_param;
byte trigger;
} _df1_comm;

typedef struct{
byte cmd[512];
unsigned int df1_datalen;
unsigned int df1_port;
} _df1;

typedef struct{
_pccc_word_read cmd;
unsigned int df1_datalen;
unsigned int df1_port;
} _df1_word_read;

typedef struct
{
_pccc_namedata *name;
byte maintype;
byte type;
byte mainlen;
int typelen;
byte floatdata;
byte len;
short data[255];
} _pccc_typed_data;
typedef struct {
byte data[12];
} _bitmask;

typedef struct {
unsigned short type;
unsigned short length;
unsigned short version;
unsigned short flags;
byte name[16];
} _services;

typedef struct {
unsigned short type;
unsigned short arraysize;
char name[CELL_TAG_LEN];
byte *data;
} _element;

typedef struct {
unsigned long base;
unsigned short linkid;
unsigned short count;
unsigned short detailsize;
char name[CELL_TAG_LEN];
_element *data[64];
} _struct_base;

typedef struct {
unsigned short count;
_struct_base *base[CELL_MAX_SBASE];
} _struct_list;

typedef struct {
unsigned long base;
unsigned long linkid;
byte name[32];
} _prog_detail;

typedef struct {
_prog_detail *prog[CELL_MAX_PROGS];
int count;
} _prog_list;

typedef struct {
unsigned long topbase;
unsigned long base;
unsigned long alias_topbase;
unsigned long alias_base;
unsigned long id;
unsigned long alias_id;
unsigned long linkid;
unsigned long memory;
unsigned long alias_linkid;
unsigned short type;
unsigned short alias_type;
unsigned short displaytype;
unsigned short size;
unsigned short alias_size;
unsigned long arraysize1;
unsigned long arraysize2;
unsigned long arraysize3;
unsigned short datalen;
long dirty;
byte *data;	
byte name[CELL_TAG_LEN];
} _tag_detail;

typedef struct {
_tag_detail *tag[CELL_MAX_TAGS];
int count;
int reference;
} _tag_data;

typedef struct {
int count;
_tag_detail *tag[64];
} _tag_list;

typedef struct {
long ioiclass;
long instance;
long member;
long point;
long attribute;
byte *tagname;
long elem[3];
} _ioi_data;

typedef struct {
short len;
byte data[2000];
} _ioi_list;

typedef struct {
byte rx_bad_m;
byte err_threshold;
byte rx_bad_crc;
byte rx_bus_timeout;
byte tx_bad_crc;
byte tx_bus_timeout;
byte tx_retry;
byte status;
unsigned short address;
byte rev_major;
byte rev_minor;
unsigned long serial_number;
unsigned short rack_size;
} _backplane_data;

typedef struct {
short number1;
byte number2;
byte slot;
short number3;
short number4;
} _port;

typedef struct {
short family;
unsigned short port;
unsigned long address;
byte zero[8];
} _sock;

typedef struct {
unsigned short ID;
unsigned short type;
unsigned short product_code;
byte rev_hi;
byte rev_lo;
short status;
unsigned long serial;
byte namelen;
char name[32];
} _identity;

typedef struct {
int size;
int slot;
int cpulocation;
_identity *identity[CELL_MAX_RACKID];
} _rack;


typedef struct {
short number1;
short number2;
short len;
short number3;
short number4;
short number5;
unsigned short port;
unsigned long ip_addr;
short number6;
short number7;
short number8;
short number9;
char address[16];
} _interfaces;


typedef struct {
byte data[DATA_Buffer_Length];
unsigned short len;
unsigned short overall_len; 
} _data_buffer;

typedef struct {
int device1;
int device2;
int device3;
int device4;
int device5;
int device6;
int device7;
int device8;
} _path;

#define BUFFER_LENGTH DATA_Buffer_Length - ENCAPS_Header_Length

typedef struct {
unsigned short command;
unsigned short len;
unsigned long handle;
unsigned long status;
byte context[8];
unsigned long options;
} _encaps_header;

typedef struct {
unsigned long session_id;
unsigned long ms_timeout;  /* Communications layer timeout milliseconds */
unsigned short df1_tns;
unsigned short sequence;
int file_handle;
int commtype;
int retries;
byte *hostname;
byte context[8];
char error;
} _comm_header;


typedef struct {
unsigned long ctl;
float sp;
float kp;
float ki;
float kd;
float bias;
float maxs;
float mins;
float db;
float so;
float max0;
float min0;
float upd;
float pv;
float err;
float out;
float pvh;
float pvl;
float dvp;
float dvn;
float pvdb;
float dvdb;
float max1;
float min1;
float tie;
float maxcv;
float mincv;
float mintie;
float maxtie;
float data[17];
} _PID ;

typedef struct {
byte heartbeat[3];
byte ctl;
long preset;
long accumulator;
} _timer;

typedef struct {
byte junk[3];
byte ctl;
long preset;
long accumulator;
} _counter;

typedef struct {
long timelow;
long timehigh;
short type;
short code;
long info[8];
} _fault;

typedef struct {
long ctl;
float input;
float hhlimit;
float hlimit;
float llimit;
float lllimit;
float deadband;
float roc_pos_limit;
float roc_neg_limit;
float roc_period;
long alarms;
float roc;
long status;
} _alarm;


typedef struct {
long CTOffset[2];
long CurrentValue[2];
long year;
long month;
long date;
long hour;
long minute;
long seconds;
long micro;
} _wall_clock;

typedef struct {
short hmi_hb;
short hmi_last;
short ab_hb;
} _hmi_heartbeat;

typedef struct {
short current_state;
short destination_state;
short in_transition;
short abort_code;
short heartbeat;
long hs_data;
long sys_status;
} _state_data;

typedef struct {
long input;
float in1;
float in2;
long ctrl;
float out;
long junk;
} _select;


typedef struct {
long unk[3];
long prog_to_fault_en;
long fault_mode;
long fault_value;
long prog_mode;
long prog_value;
} _1756_do_ctrl;

typedef struct {
long fault;
long data;
} _1756_do_in;

typedef struct {
long fault;
long data;
} _1756_di_in;

typedef struct {
long data;
} _1756_do_out;


typedef struct {
long unk[3];
long diag_cos_disable;
byte filter_offon_07;
byte filter_onoff_07;
byte filter_offon_815;
byte filter_onoff_815;
byte filter_offon_1623;
byte filter_onoff_1623;
byte filter_offon_2431;
byte filter_onoff_2431;
long cos_onoff_en;
long cos_offon_en;
} _1756_di_ctrl;

typedef struct {
long slot_status_bits;
_1756_di_in slot[10];
} _1756_cnb_in_10slot;

typedef struct {
_1756_do_out slot[10];
} _1756_cnb_out_10slot;

typedef struct {
long slot_status_bits;
_1756_di_in slot[7];
} _1756_cnb_in_7slot;

typedef struct {
_1756_do_out slot[7];
} _1756_cnb_out_7slot;



typedef struct {
byte unk;
byte rangetype;
short digitalfilter;
float lowsignal;
float highsignal;
float lowengineering;
float highengineering;
float calbias;
} _analog_config;

typedef struct {
long unk[2];
byte unk_b;
byte module_filter;
short realtime_sample;
_analog_config ch_config[16];
} _1756_if16_ctrl;



typedef struct {
short chan_faults;
short mod_faults;
byte chan_status[16];
float chan_data[16];
} _1756_if16_in;




#pragma pack()


extern int cell_errno;
extern char cell_err_msg[256];
extern int cell_line;
extern char cell_file[256];


#define CELLERROR(num,msg) { cell_line=__LINE__; strcpy(cell_file,__FILE__); cell_errno = num; strcpy(cell_err_msg,msg); }


extern char *PROFILE_Names[];
extern char *ENCAPS_errors[];
extern int ENCAPS_errors_id[];
extern int ENCAPS_error_count;
extern char *pcccdatatypes[];
extern char *pcccaddrtypes[];
extern int pcccaddrtypescount;
extern char *cip_status[];
extern unsigned short cip_status_index[];
extern int cip_status_count;


__EXPORT extern void cell_geterror(int *error, char *s, int debug);
__EXPORT extern void cell_perror(const char *s, int debug);
__EXPORT extern int ioi ( _data_buffer *buff, _ioi_data *ioi1, _ioi_data *ioi2, int debug);
__EXPORT extern int list_interfaces (_comm_header *comm, int debug);
__EXPORT extern int list_services (_comm_header *comm, _services *services, int debug);
__EXPORT extern int readdata ( _data_buffer *buff, _comm_header *comm, int debug);
__EXPORT extern int senddata (const _data_buffer *buff, _comm_header *comm, int debug);
__EXPORT extern int nop (_comm_header *comm, int debug);
__EXPORT extern int register_session (_comm_header *comm, int debug);
__EXPORT extern int unregister_session (_comm_header *comm, int debug);
extern void fill_header (_comm_header *comm, _encaps_header *head, int debug);
__EXPORT extern void establish_connection (_comm_header *comm, _services *services, int debug);
__EXPORT extern void attach (_comm_header *comm, int debug);
__EXPORT extern int list_targets (_comm_header *comm, int debug);
extern int sendRRdata ( unsigned short timeout, _comm_header *comm, _encaps_header *head, _data_buffer *buff, int debug);
extern int sendConnecteddata ( unsigned short timeout, _comm_header *comm, _encaps_header *head, _data_buffer *buff, int debug);
extern void cpf_data (short datatype, _data_buffer *cpf_data, _data_buffer *buff, int debug);
extern void cpf (short addrtype, _data_buffer *addrbuff, short datatype, _data_buffer *databuff, _data_buffer *buff, int debug);
__EXPORT extern int get_backplane_data (_comm_header *comm, _backplane_data *backplane, _rack *rack, _path *path, int debug);
extern int unconnected_send (_data_buffer *buff, int debug);
extern int makepath (_path *path, _data_buffer *buff, int debug);
extern int settimeout (byte priority, byte timeout, _data_buffer *buff, int debug);
__EXPORT extern int get_device_data (_comm_header *comm, _path *path, _identity *identity, int debug);
__EXPORT extern int get_port_data (_comm_header *comm, _path *path, _port *port, int debug);
__EXPORT extern int who (_comm_header *comm, _rack *rack, _path *path, int debug);
__EXPORT extern int get_object_config_list (_comm_header *comm, _path *path, unsigned long base, _tag_data *tags, int debug);
__EXPORT extern int get_object_details_list (_comm_header *comm, _path *path, unsigned long base, _tag_data *tags, int debug);
__EXPORT extern int get_program_list (_comm_header *comm, _path *path, _prog_list *progs, int debug);
__EXPORT extern int get_program_details (_comm_header *comm, _path *path, _prog_detail *prog, int debug);
__EXPORT extern int get_object_config (_comm_header *comm, _path *path, _tag_detail *tag, int debug);
__EXPORT extern int get_object_details (_comm_header *comm, _path *path, _tag_detail *tag, int debug);
__EXPORT extern unsigned short getsize (_tag_detail *tag);
__EXPORT extern int get_element_size (int type);
__EXPORT extern int read_object_value (_comm_header *comm, _path *path, _tag_detail *tag, int debug);
__EXPORT extern int read_object_range_value (_comm_header *comm, _path *path, _tag_detail *tag, unsigned offset, unsigned count, int debug);
__EXPORT extern int write_object_value (_comm_header *comm, _path *path, _tag_detail *tag, int debug);
__EXPORT extern int write_object_range_value (_comm_header *comm, _path *path, _tag_detail *tag, unsigned offset, unsigned count, int debug);
__EXPORT extern int read_multi_object_value (_comm_header *comm, _path *path, _tag_list *list, int debug);
__EXPORT extern int get_struct_list (_comm_header *comm, _path *path, _struct_list *structs, int debug);
__EXPORT extern int get_struct_details (_comm_header *comm, _path *path, _struct_base *tag, int debug);
__EXPORT extern int get_struct_config (_comm_header *comm, _path *path, _struct_base *tag, int debug);
__EXPORT extern int aliascheck (_tag_detail *tag, _tag_data *mainlist, _tag_data *proglist, int debug);
__EXPORT extern int read_struct_element (_comm_header *comm, _path *path, _tag_detail *tag, _struct_list *structs, unsigned num, int debug);
__EXPORT extern int read_memory_value (_comm_header *comm, _path *path, _tag_detail *tag, int debug);
__EXPORT extern int read_tag (_comm_header *comm, _path *path, char *program, char *tagname, _tag_detail *tag, int debug);
__EXPORT extern int write_tag (_comm_header *comm, _path *path, char *program, char *tagname, _tag_detail *tag, int debug);
__EXPORT extern int object_bit_modify_word (_comm_header *comm, _path *path, _tag_detail *tag, _bitmask *andmask, _bitmask *ormask, byte count, int debug);
__EXPORT extern int object_bit_modify_byte (_comm_header *comm, _path *path, _tag_detail *tag, byte andmask, byte ormask, unsigned long offset, int debug);
__EXPORT extern int get_dh_map(_comm_header *comm, _path *path, _dhplus_map *data, int dhport, int debug);
__EXPORT extern int get_remote_dh(_comm_header *comm, _path *path, _data_buffer *data, int dhport, int debug);
__EXPORT extern int send_df1(_comm_header *comm, _df1_comm *df1_comm, _df1 *df1, _df1_resp *resp, int debug);
__EXPORT extern char *get_reply_status (_data_buffer *buff, int debug);
__EXPORT extern unsigned short get_object_size(_tag_detail *tag);

__EXPORT extern int df1_get_status (_comm_header *comm, _df1_comm *df1_comm, _df1_resp *resp, byte node, byte channel, int debug);
__EXPORT extern int df1_open (_comm_header *comm, _path *path, _df1_comm *df1_comm, int dh_channel, int debug);
__EXPORT extern int df1_close (_comm_header *comm, _path *path, _df1_comm *df1_comm, int debug);
__EXPORT extern int df1_word_read(_comm_header *comm, _df1_comm *df1_comm, char *reg, _df1_resp *resp, byte node, byte channel, int plctype, unsigned short count, int debug);
__EXPORT extern int df1_typed_read(_comm_header *comm, _df1_comm *df1_comm, char *reg, _df1_resp *resp, byte node, byte channel, int plctype, unsigned short count, int debug);
__EXPORT extern int df1_typed_write (_comm_header *comm, _df1_comm *df1_comm, char *reg, _df1_resp *resp, _df1_senddata *data, byte node, byte channel, int plctype, unsigned short coumt, int debug);
__EXPORT extern int df1_read_section_size (_comm_header *comm, _df1_comm *df1_comm, char *reg, _df1_resp *resp, byte node, byte channel, int plctype, int debug);
__EXPORT extern int df1_create_memory (_comm_header *comm, _df1_comm *df1, char *reg, _df1_resp *resp, byte node, byte channel, int plctype, int debug); 

__EXPORT extern int nameconv5(char *dataaddr, _pccc_namedata *name, int plctype, int debug);
