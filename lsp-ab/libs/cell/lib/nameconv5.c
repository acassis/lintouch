\
// This is an example program for communicating with an Allen Bradley PLC-5 over
// Ethernet.  This is only to demonstrate the methodology needed.  This program is not
// necessarily useful for any other purpose.

//  Copyright (1999) by Ron Gage 


/* This routine will convert the ASCII representation of the PLC5
address you are trying to read into the binary format used as a
part of the DF1 protocol.  It returns 0 on success, 1 on failure. */

#include "libcell.h"

int nameconv5(char *dataaddr, _pccc_namedata *name, int plctype, int debug)

{

int start,type,bitdata,try,l,junk;
unsigned int x;
byte tempname[3];
memset (name, 0, sizeof(_pccc_namedata)); 
bitdata=-1;
name->mask=0; 
name->module=-1; 
name->area=-1; 
name->subarea=-1; 
name->section=-1; 
name->bit=-1;
name->file=-1; 
name->element=-1;
name->subelement=-1; 
name->floatdata=FALSE;
memset (&tempname, 0, sizeof(tempname));
switch (plctype)
	{
	case PLC5:
		dprint (DEBUG_TRACE,"Encoding %s for PLC5.\n",dataaddr);
		start=1;
		type = -1;
		tempname[0]=dataaddr[0];
		if (isalpha(dataaddr[1]))
			{
			dprint (DEBUG_TRACE,"Saw second character as alpha.\n");
			start=2;
			tempname[1]=dataaddr[1];
			}
		for (x=0;x<(unsigned)pcccaddrtypescount;x++)
			{
			try = strncasecmp(tempname,pcccaddrtypes[x],start);
			if (try==0)
				{
				type = x;
				dprint (DEBUG_TRACE,"type = %d\n",type);
				x = pcccaddrtypescount;
				}
			}
		if (type == -1)
			return (-1);
		switch (type)
			{
			case 0:		// Output
				name->section = 0;
				name->file = 0;
				name->type = 1;
				name->typelen = 2;
				break;
			case 1:		// Input
				name->section = 0;
				name->file = 1;
				name->type = 1;
				name->typelen = 2;
				break;
			case 2:		// System Registers
				name->section = 0;
				name->file = 2;
				name->type = 4;
				name->typelen = 2;
				break;
			case 3:		// Binary Registers
				name->section = 0;
				name->type = 1;
				name->typelen = 2;
				break;
			case 4:		// Timers
				name->section = 0;
				name->type = 5;
				name->typelen = 2;
				break;
			case 5:		// Counters
				name->section = 0;
				name->type = 6;
				name->typelen = 2;
				break;
			case 6:		// Control Registers
				name->section = 0;
				name->type = 7;
				name->typelen = 2;
				break;
			case 7:		// Integer Registers
				name->section = 0;
				name->type = 4;
				name->typelen = 2;
				break;
			case 8:		// IEEE Floating Point Registers
				name->section = 0;
				name->floatdata = TRUE;
				name->type = 8;
				name->typelen = 4;
				break;
			case 9:		// ASCII Data
				name->section = 0;
				name->type = 3;
				name->typelen = 1;
				break;
			case 10:	// BCD Data
				name->section = 0;
				name->type = 16;
				name->typelen = 2;
				break;
			case 11:	// Block Transfer Data
				name->section = 0;
				name->type = 7;
				name->typelen = 2;
				break;
			case 12:	// Long Integers
				name->section = 0;
				name->type = 4;
				name->typelen = 4;
				break;
			case 13:	// Message Control
				name->section = 0;
				name->type = 7;
				name->typelen = 2;
				break;
			case 14:	// PID Control
				name->section = 0;
				name->type = 7;
				name->typelen = 2;
				break;
			case 15:	// SC - ??
				name->section = 0;
				name->type = 4;
				name->typelen = 4;
				break;
			case 16:	// String data
				name->section = 0;
				name->type = 3;
				name->typelen = 1;
				break;
			case 17:	// PLC Program name
				name->section = 1;
				name->type = 3;
				name->typelen = 1;
				break;
			case 18:	// Rung data
				name->section = 1;
				name->type = 0x0d;
				break;
			case 19:	// Force table Outputs
				name->section = 2;
				name->file=0;
				name->type = 1;
				name->typelen = 2;
				break;
			case 20:	// Force Table Inputs
				name->section = 2;
				name->file=1;
				name->type = 1;
				name->typelen = 2;
				break;
			case 21:	// Section 3 data
				name->section = 3;
				name->type = 4;
				name->typelen = 2;
				break;
			case 22:	// Section 4 data
				name->section = 4;
				name->type = 4;
				name->typelen = 2;
				break;
			case 23:	// Section 5 data
				name->section = 5;
				name->type = 4;
				name->typelen = 2;
				break;
			case 24:	// Section 6 data
				name->section = 6;
				name->type = 4;
				name->typelen = 2;
				break;
			case 25:	// Force table as a whole
				name->section = 2;
				name->type = 1;
				name->typelen = 1;
				break;
			}
			
		dprint (DEBUG_TRACE,"Data Section = %d, type = %d\n",name->section, name->type);
			
		if (isdigit(dataaddr[start]))
			{
			name->file = atoi(&dataaddr[start]);
			dprint (DEBUG_TRACE,"File is %d\n",name->file);
			}			
			dprint (DEBUG_TRACE,"Start = %d, strlen = %d\n",start,strlen(dataaddr));
						
		for (x=start;x<strlen(dataaddr);x++)
			{
			dprint (DEBUG_TRACE,"x = %d\n",x);
			switch (dataaddr[x])
				{
				case ':':
				case ';':
					x++;
					if (isdigit(dataaddr[x]))
						name->element = atoi(&dataaddr[x]);
						
					break;
				case '.':
				case '/':
					x++;
					if (isdigit(dataaddr[x]))
						{
						if (name->element == -1)
							name->element = 0;
						bitdata = atoi(&dataaddr[x]);
						name->bit = bitdata % 16;
						name->element += (bitdata/16);
						}
		
					if ( bitdata == -1)
						dprint (DEBUG_TRACE,"SubElement Found.\n");
					if (bitdata == -1)
						{
						name->subelement=0;
						l=strlen(dataaddr) - x;
						if (strncasecmp (&dataaddr[x],"acc",l) == 0 )
							name->subelement = 2;
						if (strncasecmp (&dataaddr[x],"pre",l) == 0 )
							name->subelement = 1;
						if (strncasecmp (&dataaddr[x],"len",l) == 0 )
							name->subelement = 1;
						if (strncasecmp (&dataaddr[x],"pos",l) == 0 )
							name->subelement = 2;
						if (strncasecmp (&dataaddr[x],"en",l) == 0 )
							name->bit = 13;
						if (strncasecmp (&dataaddr[x],"tt",l) == 0 )
							name->bit = 14;
						if (strncasecmp (&dataaddr[x],"dn",l) == 0 )
							name->bit = 15;
						x = strlen(dataaddr)-1;
						}
					}
			}
	
			dprint (DEBUG_TRACE,"Element is now %d\n",name->element);
			dprint (DEBUG_TRACE,"Bit is %d\n",name->bit);
			dprint (DEBUG_TRACE,"Extracted bit data was %d\n",bitdata);


		name->len = 1;
		if (name->section != -1)
			{
			name->data[name->len++] = name->section;
			name->data[0] = name->data[0] | 1;
			}
		if (name->file != -1)
			{
			name->data[0] = name->data[0] | 2;
			if (name->file >= 255)
				{
				name->data[name->len++] = 0xff;
				name->data[name->len++] = (name->file & 255);
				name->data[name->len++] = (name->file / 256);
				}
			if (name->file < 255)
				{
				name->data[name->len++] = name->file;
				}
			}
		if (name->element != -1)
			{
			name->data[0] = name->data[0] | 4;
			if (name->element >= 255)
				{
				name->data[name->len++] = 0xff;
				name->data[name->len++] = (name->element & 255);
				name->data[name->len++] = (name->element / 256);
				}
			if (name->element < 255)
				{
				name->data[name->len++] = name->element;
				}
			}
		if (name->subelement != -1)
			{
			name->data[0] = name->data[0] | 8;
			name->data[name->len++] = name->subelement;
			}
		if (debug != 0)
			{
			dprint (DEBUG_TRACE, "%s -> ",dataaddr);
			for (x=0; x<name->len; x++)
				dprint (DEBUG_TRACE,"%02X ", name->data[x]);
			dprint (DEBUG_TRACE,"\n");
			}
		return 0;

// -------------------------------PLC5/250 Decoding----------------------
	
	case PLC5250:
		start=0;
		if (isdigit(dataaddr[0]))
			{
			name->module = atoi(&dataaddr[0]);
			if (name->module > 0)
				name->module = name->module + 30;
			start=1;
			name->area = 2; // identifies data section
			name->subarea = 0;
			}
			
		for (x=start;x<strlen(dataaddr);x++)
			{
			junk = toupper(dataaddr[x]);
			switch (junk)
				{
				case 'O':
					name->module = 1;
					name->type = 4;
					name->area = 0;
					break;
				case 'I':
					name->module = 1;
					name->area = 1;
					name->type = 4;
					break;
				case 'B':
					name->section=0;
					name->type = 1;
					x++;
					name->file = atoi(&dataaddr[x]);
					break;
				case 'N':
					name->section=1;
					name->type = 4;
					x++;
					name->file = atoi(&dataaddr[x]);
					break;
				case 'F':
					name->section=4;
					name->type = 8;
					x++;
					name->floatdata=TRUE;
					name->file = atoi(&dataaddr[x]);
					break;
				case 'T':
					name->section=5;
					name->type = 5;
					x++;
					name->file = atoi(&dataaddr[x]);
					break;
				case 'C':
					name->section=6;
					name->type = 6;
					x++;
					name->file = atoi(&dataaddr[x]);
					break;
				case 'R':
					name->section=7;
					name->type = 7;
					x++;
					name->file = atoi(&dataaddr[x]);
					break;
				case ':':
				case ';':
					if (name->module == 1)
						name->subarea = atoi(&dataaddr[++x]);
					if (name->module != 1)
						name->element = atoi(&dataaddr[++x]);
					break;
				case '.':
				case '/':
					x++;
					l=strlen(dataaddr) - x;
					if (strncasecmp (&dataaddr[x],"en",l) == 0 )
						name->subelement=0;
					if (strncasecmp (&dataaddr[x],"tt",l) == 0)
						name->subelement=1;
					if (strncasecmp (&dataaddr[x],"dn",l) == 0)
						name->subelement=2;
					if (strncasecmp (&dataaddr[x],"pre",l) == 0)
						{
						if (name->section == 5)
							name->subelement=3;
						if (name->section == 6)
							name->subelement=5;
						}
					if (strncasecmp (&dataaddr[x],"acc",l) == 0)
						{
						if (name->section == 5)
							name->subelement=4;
						if (name->section == 6)
							name->subelement=6;
						}
					if (strncasecmp (&dataaddr[x],"cu",l) == 0)
						name->subelement=0;
					if (strncasecmp (&dataaddr[x],"cd",l) == 0)
						name->subelement=1;
					if (strncasecmp (&dataaddr[x],"dn",l) == 0)
						name->subelement=2;
					if (strncasecmp (&dataaddr[x],"ov",l) == 0)
						name->subelement=3;
					if (strncasecmp (&dataaddr[x],"un",l) == 0)
						name->subelement=4;
					x = strlen(dataaddr) -1;
											
				}
			}
		if (name->subelement != -1)
			name->type = 4;

		dprint (DEBUG_TRACE,"module = %d\n",name->module);
		dprint (DEBUG_TRACE,"area = %d\n",name->area);
		dprint (DEBUG_TRACE,"subarea = %d\n",name->subarea);
		dprint (DEBUG_TRACE,"section = %d\n",name->section);
		dprint (DEBUG_TRACE,"file = %d\n",name->file);
		dprint (DEBUG_TRACE,"element = %d\n",name->element);
		dprint (DEBUG_TRACE,"subelement = %d\n",name->subelement);
			
		name->len = 1;

		if ((name->module != -1) && (name->module != 0))
			{
			dprint (DEBUG_TRACE,"Adding Module\n");
			name->data[name->len++]=name->module;
			name->data[0] = name->data[0] | 1;
			}
		if ((name->area != -1) && (name->area != 0))
			{
			dprint (DEBUG_TRACE,"Adding Area\n");
			name->data[name->len++]=name->area;
			name->data[0] = name->data[0] | 2;
			}
		if ((name->subarea != -1) && (name->subarea != 0))
			{
			dprint (DEBUG_TRACE,"Adding Subarea\n");
			name->data[name->len++]=name->subarea;
			name->data[0] = name->data[0] | 4;
			}
		if ((name->section != -1) && (name->section != 0))
			{
			dprint (DEBUG_TRACE,"Adding Section\n");
			name->data[name->len++]=name->section;
			name->data[0] = name->data[0] | 8;
			}
		if ((name->file != -1) && (name->file != 0))
			{
			dprint (DEBUG_TRACE,"Adding File\n");
			name->data[0] = name->data[0] | 16;
			if (name->file >= 255)
				{
				name->data[name->len++] = 0xff;
				name->data[name->len++] = (name->file & 255);
				name->data[name->len++] = (name->file / 256);
				}
			if (name->file < 255)
				{
				name->data[name->len++] = name->file;
				}
			}

		if (name->element != -1)
			{
			dprint (DEBUG_TRACE,"Adding Element\n");
			name->data[0] = name->data[0] | 32;
			if (name->element >= 255)
				{
				name->data[name->len++] = 0xff;
				name->data[name->len++] = (name->element & 255);
				name->data[name->len++] = (name->element / 256);
				}
			if (name->element < 255)
				{
				name->data[name->len++] = name->element;
				}
			}
			
		if (name->subelement != -1)
			{
			dprint (DEBUG_TRACE,"Adding Subelement\n");
			name->data[0] = name->data[0] | 64;
			name->data[name->len++] = name->subelement;
			}

		if(debug != 0 )
			{
			dprint (DEBUG_TRACE,"name->len = %d\n",name->len);
			for (x=0;x<name->len;x++)
				dprint (DEBUG_TRACE,"%02X  ",name->data[x]);
			dprint (DEBUG_TRACE,"\n");
			}

		return 0;

//------------------------- SLC 5/05 Encoding ----------------------

	case SLC:
		name->file=-1;
		name->element=-1;
		name->subelement=-1;
		name->section=-1;
		
		
		for (x=0;x<strlen(dataaddr);x++)
			{
			junk = toupper(dataaddr[x]);
			switch (junk)
				{
				case 'O':
					name->type = 0x8b;
					name->file = 0;
					name->typelen = 2;
					break;
				case 'I':
					name->type = 0x8c;
					name->file = 1;
					name->typelen = 2;
					break;
				case 'S':
					x++;
					name->type = 0x84;
					name->typelen = 2;
					if (toupper(dataaddr[x] == 'T'))
						{
						name->type=0x8d;
						name->typelen = 0x54;
						x++;
						}
					name->file = atoi(&dataaddr[x]);
					break;
				case 'B':
					x++;
					name->type = 0x85;
					name->file = atoi(&dataaddr[x]);
					name->typelen = 2;
					break;
				case 'T':
					x++;
					name->type = 0x86;
					name->file = atoi(&dataaddr[x]);
					name->typelen = 2;
					break;
				case 'C':
					x++;
					name->type = 0x87;
					name->file = atoi(&dataaddr[x]);
					name->typelen = 2;
					break;
				case 'R':
					x++;
					name->type = 0x88;
					name->file = atoi(&dataaddr[x]);
					name->typelen = 2;
					break;
				case 'N':
					x++;
					name->type = 0x89;
					name->file = atoi(&dataaddr[x]);
					name->typelen=2;
					break;
				case 'F':
					x++;
					name->type = 0x8a;
					name->file = atoi(&dataaddr[x]);
					name->floatdata=TRUE;
					name->typelen = 4;
					break;
				case 'A':
					x++;
					name->type = 0x8e;
					name->file = atoi(&dataaddr[x]);
					name->typelen = 1;
					break;
				case 'D':
					x++;
					name->type = 0x8f;
					name->file = atoi(&dataaddr[x]);
					name->typelen = 2;
					break;
				
				case ':':
				case ';':
					name->element = atoi(&dataaddr[++x]);
					break;
				case 'P':   //special case to read program name from PLC.
					name->section  = 1;
					name->file = 7;
					name->element = 0;
					break;
				case '.':
				case '/':
					x++;
					l=strlen(dataaddr) - x;
					if (strncasecmp (&dataaddr[x],"acc",l) == 0 )
						name->subelement = 2;
					if (strncasecmp (&dataaddr[x],"pre",l) == 0 )
						name->subelement = 1;
					if (strncasecmp (&dataaddr[x],"len",l) == 0 )
						name->subelement = 1;
					if (strncasecmp (&dataaddr[x],"pos",l) == 0 )
						name->subelement = 2;
					x = strlen(dataaddr)-1;
				}
			}
	
		name->len = 1;
		if (name->file != -1)
			{
			name->data[0] = name->data[0] | 2;
			name->data[name->len++] = name->file;
			}
		if (name->section != -1)
			{
			name->data[name->len++] = name->section;
			name->data[0] = name->data[0] | 1;
			}
		if (name->element != -1)
			{
			name->data[0] = name->data[0] | 4;
			name->data[name->len++] = name->element;
			}
		if (name->subelement != -1)
			{
			name->data[0] = name->data[0] | 8;
			name->data[name->len++] = name->subelement;
			}
		return 0;
		
	}
return -1;
}
