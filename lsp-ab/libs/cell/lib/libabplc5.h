
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

#define OK 0
#define NOHOST -1
#define BADADDR -2
#define NOCONNECT -3
#define BADCMDRET -1

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
byte dst;
byte control;
byte src;
byte lsap;
byte cmd;
byte sts;
short tns;
byte fnc;
} _pccc_normal;

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
long addr;
byte count;
} _pccc_memread;

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
short offset;
short trans;
byte data[240];
} _pccc_word_read;

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
short size_lo;
short size_hi;
short count;
byte write_priv;
byte read_priv;
byte type;
byte sts;
byte extsts;
} _pccc_size_data;

typedef struct 
{
struct namedata name;
short len;
byte floatdata;
unsigned short data[250];
} _pccc_data;

typedef struct
{
struct namedata name;
byte maintype;
byte type;
byte mainlen;
int typelen;
byte floatdata;
byte len;
short data[255];
} _pccc_typed_data;

#pragma pack()
