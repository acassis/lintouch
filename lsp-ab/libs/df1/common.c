/*  df1 (C) 2002 Stéphane LEICHT (leicht@free.fr) http://www.treewin.com
 *
 * 	This program is free software; you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation; either version 2, or (at your option)
 * 	any later version.
 *
 * 	This program is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

#include "df1.h"

extern int file;

/***********************************************************************/
word bytes2word(byte lowb, byte highb)
{
	word w;
	char c[2];//={lowb,highb};
	c[0]=lowb;
	c[1]=highb;
	memcpy(&w,c,sizeof(w));
	return w;
}	

/***********************************************************************/
int add_word2buffer(TBuffer * buffer, word value) /* return new buffer size */
{
	memcpy(buffer->data+buffer->size, &value,sizeof(value));
	buffer->size += sizeof(value);
	return buffer->size;
}	


/***********************************************************************/
int add_byte2buffer(TBuffer * buffer, byte value) /* return new buffer size */
{
	memcpy(buffer->data+buffer->size, &value,sizeof(value));
	buffer->size += sizeof(value);
	return buffer->size;
}	

/***********************************************************************/
int add_data2buffer(TBuffer * buffer, void * data, byte size) /* return new buffer size */
{
	memcpy(buffer->data+buffer->size, data,size);
	buffer->size += size;
	return buffer->size;
}	

/***********************************************************************/
/* Add DLE if DLE exist in buffer **************************************/
int add_data2bufferWithDLE(TBuffer * buffer, TMsg msg) /* return new buffer size */
{
	byte  i;
	byte databyte[262];
	memcpy(&databyte, &msg,sizeof(msg));
	for (i=0;i<msg.size+6;i++)
	 {
	  if (databyte[i]==DLE)
	   add_byte2buffer(buffer,DLE);
 	  add_byte2buffer(buffer,databyte[i]);
	 }	
	return buffer->size;
}

/***********************************************************************/
int is_timeout(int start_time)
{
	if ((time ((time_t *) 0)-start_time)>TIME_OUT)
		return ERROR_TIMEOUT;
	else 
		return SUCCES;
}

/***********************************************************************/
void print_symbol(byte c)
{
	switch (c)
	{
		case STX :
			puts("STX\n");
			break;
		case ETX :
			puts("ETX\n");
			break;	
		case ENQ :
			puts("ENQ\n");
			break;
		case ACK :
			puts("ACK\n");
			break;		
		case NAK :
			puts("NAK\n");
			break;		
		case DLE :
			puts("DLE\n");
			break;		
		default : printf("??:%02X\n",c);
	}
}
