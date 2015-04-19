#include "libcell.h"

#ifdef WIN32
int debug;
void dprint(int a,const char *b,...)  
{
	va_list vl; 
	va_start (vl,b); 
	if (debug>=a)  
		vfprintf(stdout,b,vl); 
	va_end(vl); 
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason, LPVOID lpReserved)
{
return TRUE;
}

#endif

char *ENCAPS_errors[] = { "Success",
  "Sender issued an unsupported command",
  "Receiver ran out of memory to handle command",
  "Incorrect Data in command (encaps section)",
  "Invalid Session Handle used.",
  "Invalid message Length",
  "Unsupported Protocol Revision"
};
int ENCAPS_errors_id[] = { 0, 1, 2, 3, 0x64, 0x65, 0x69 };
int ENCAPS_error_count = 7;
char *PROFILE_Names[] = { "Generic Device",
  "Not Defined - 0x01",
  "A/C Drives",
  "Motor Overload Device",
  "Limit Switch",
  "Inductive Prox Switch",
  "Photoelectric Sensor",
  "General Purpose Discreet I/O",
  "Not Defined - 0x08",
  "Resolver",
  "Analog I/O Device - 0x0a",
  "Not Defined - 0x0b",
  "Communications Adaptor",
  "Not Defined - 0x0d",
  "Control Processor",
  "Not Defined - 0x0f",
  "Position Controller",
  "Not Defined - 0x11",
  "Not Defined - 0x12",
  "DC Drive",
  "Not Defined - 0x14",
  "Contactor",
  "Motor Starter",
  "Soft Motor Starter",
  "Human Machine Interface",
  "Not Defined - 0x19",
  "Not Defined - 0x1a" "Pneumatic Valves",
  "Vacuum Pressure Gauge",
  "Not Defined - 0x1d",
  "Not Defined - 0x1e",
  "Not Defined - 0x1f",
  "Not Defined - 0x20",
  "Not Defined - 0x21",
  "Not Defined - 0x22",
  "Not Defined - 0x23",
  "Not Defined - 0x24",
  "Not Defined - 0x25",
  "Not Defined - 0x26",
  "Not Defined - 0x27",
  "Not Defined - 0x28",
  "Not Defined - 0x29",
  "Not Defined - 0x2a",
  "Not Defined - 0x2b",
  "Not Defined - 0x2c",
  "Not Defined - 0x2d",
  "Not Defined - 0x2e",
  "Not Defined - 0x2f",
  "Not Defined - 0x30",
  "Not Defined - 0x31",
  "ControlNet Physical Layer"
};

char *pccc_errors[] = {
	"No Error",
	"Illegal Command or Format",
	"Host has a problem and will not communicate",
	"Remote Node Host is missing, disconnected, or shutdown",
	"Host could not complete function due to hardware fault",
	"Addressing problem or memory protected rungs",
	"Function not allowed due to command protection selection",
	"Processor is in Program Mode",
	"Compatability mode file missing or communication zone problem",
	"Remote node cannot buffer command",
	"Wait ACK (1775-KA buffer full)",
	"Remote Node problem due to download",
	"Wait ACK (1775-KA buffer full)",
	"Not Used 1",
	"Not Used 2",
	"Extended error data received"};

char *slcmodes[] = {
	"Download",
	"Remote Program",
	"reserved - 2",
	"Idle - SUS command",
	"reserved - 4",
	"reserved - 5",
	"Run",
	"Test - Continuous Scan",
	"Test - Single Scan",
	"Test - Single Step",
	"reserved - 10",
	"reserved - 11",
	"reserved - 12",
	"reserved - 13",
	"reserved - 14",
	"reserved - 15",
	"Download",
	"Program",
	"reserved - 18",
	"reserved - 19",
	"reserved - 20",
	"reserved - 21",
	"reserved - 22",
	"reserved - 23",
	"reserved - 24",
	"reserved - 25",
	"reserved - 26",
	"Idle - SUS command",
	"reserved - 28",
	"reserved - 29",
	"Run"};
	


char *plc5runmode[] = {
	"Program Load",
	"Not Used 1",
	"Run Mode",
	"Not Used 2",
	"Remote Program Load",
	"Remote Test",
	"Remote Run",
	"Not Used 3"};

char *plc_id[] = {
	"1785 - L40B (PLC5/40 ) ",
	"1785 - LT4  (PLC5/10 ) ",
	"1785 - L60B (PLC5/60 ) ",
	"1785 - L40L (PLC5/40L) ",
	"1785 - L60L (PLC5/60L) ",
	"1785 - L11B (PLC5/11 ) ",
	"1785 - L20B (PLC5/20 ) ",
	"1785 - L30B (PLC5/30 ) ",
	"1785 - L20E (PLC5/20E) ",
	"1785 - L40E (PLC5/40E) ",
	"1785 - L80B (PLC5/25 ) ",
	"1785 - L80E (PLC5/80E) ",
	"1785 - xxxx (SoftLogix PLC)"};
int id_count=17;

int plc_index[] = 	{0x15, 0x22, 0x23, 0x28, 0x29, 0x31, 0x32, 0x33,
			 0x4a, 0x4b, 0x55, 0x59, 0x7b};

char *pccc_ext_errors[] = {
	"No Error",
	"A field has an illegal value",
	"Less Levels specified in address than minimum for any address",
	"More Levels specified in address than system supports",
	"Symbol Not Found",
	"Symbol is of improper format",
	"Address doesn't point to something usable",
	"File is wrong size",
	"Can not complete request, situation has changed.",
	"Data or file is too large",
	"Transaction size plus word address is too large",
	"Access denied, improper privilege",
	"Condition can not be generated",
	"Condition already exists",
	"Command cannot be executed",
	"Histogram Overflow",
	"No access",
	"Illegal Data Type",
	"Invalid parameter or data type",
	"Address reference exists to deleted area",
	"Command execution failure for unknown reason",
	"Data Conversion error",
	"Scanner not able to communicate with 1771 rack adaptor",
	"Type Mismatch",
	"1771 module response was not valid",
	"Duplicated label",
	"Remote Rack fault",
	"Timeout",
	"Unknown error",
	"File is open, another node owns it",
	"Another node is program owner",
	"Reserved",
	"Reserved",
	"Data table element protection violation",
	"Temporary internal problem"};


char *pcccdatatypes[] = 	{"bit",
			"bit string",
			"byte string",
			"integer",
			"timer",
			"counter",
			"control",
			"IEEE floating point",
			"array (byte)",
			"not defined - 10",
			"not defined - 11",
			"not defined - 12",
			"Rung data",
			"not defined - 14",
			"address data",
			"BCD"};

char *pcccaddrtypes[] = 	{"O",	//Output
			"I",	//Input
			"S",	//Status
			"B",	//Binary
			"T",	//Timer
			"C",	//Counter
			"R",	//Control
			"N",	//Integer
			"F",	//Float
			"A",	//ASCII
			"D",	//BCD
			"BT",	//Block Transfer
			"L",	//Long Integer
			"MG",	//Message
			"PD",	//PID
			"SC",	// ??
			"ST",	//String
			"PN",	//PLC Name
			"RG",	//Rung
			"FO",	//Output Force Table
			"FI",	//Input Force Table
			"XA",	//Section 3 File
			"XB",	//Section 4 File
			"XC",	//Section 5 File
			"XD",	//Section 6 File
			"FF"};	// Force File Section

int pcccaddrtypescount = 26;

char *cip_status[] = {
		"Success",
		"Connection Failure",
		"Resource Unavailable",
		"Invalid Parameter Value",
		"Path Segment Error",
		"Path Destination Unknown",
		"Partial Transfer",
		"Connection Lost",
		"Service Not Supported",
		"Invalid Attribute Value",
		"Attribute List Error",
		"Already In Requested State/Mode",
		"Object State Conflict",
		"Object Already Exists",
		"Attribute Not Settable",
		"Privilege Violation",
		"Device State Conflict",
		"Reply Data Too Large",
		"Fragmentation Of A Primative Value",
		"Not Enough Data",
		"Attribute Not Supported",
		"Too Much Data",
		"Object Does Not Exist",
		"Service Fragmentation Sequence Not In Progress"
		"No Stored Attribute Data",
		"Store Operation Failure",
		"Routing Failure, Request Packet Too Large",
		"Routing Failure, Response Packet Too Large",
		"Missing Attribute List Entry Data",
		"Invalid Attribute Value List",
		"Embedded Service Error",
		"Vendor Specific Error",
		"Invalid Parameter",
		"Write Once error - already wrote",
		"Invalid Reply Received",
		"Key Failure In Path",
		"Path Size Invalid",
		"Unexpected Attribute In List",
		"Invalid Member ID",
		"Member Not Settable",
		"Group 2 Only Server General Failure",
		"Beginning Offset Beyond End Of Template",
		"Tried To Read Beyond End Of Data",
		"Data Type Mismatch"};

unsigned short cip_status_index[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
			0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
			0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20, 0x21, 0x22, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2104, 0x2105,
			0x2107 };

int cip_status_count = 44;			
