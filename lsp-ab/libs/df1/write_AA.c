/*  df1 (C) 2002 St�phane LEICHT (leicht@free.fr) http://www.treewin.com
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

extern word tns;

/***********************************************************************/
/* Cmd:0F Fnc:AA > write 3 address fields in SLC500                    */
/* Write Protected Typed Logical                                       */
/***********************************************************************/
int write_AA(TThree_Address_Fields address, void *value, byte size) 
{
	TCmd cmd;
	TMsg sendMsg,rcvMsg;
	int error=ERROR_WRITE_AA;
	
	bzero(&sendMsg,sizeof(sendMsg));
	bzero(&rcvMsg,sizeof(rcvMsg));
	bzero(&cmd,sizeof(cmd));
	sendMsg.dst = DEST;
	sendMsg.src = SOURCE;
	sendMsg.cmd = 0x0F;
	sendMsg.sts = 0x00;
	sendMsg.tns = ++tns;
	cmd.fnc = 0xAA;
	cmd.size = address.size;
	cmd.fileNumber = address.fileNumber;
	cmd.fileType = address.fileType;
	cmd.eleNumber = address.eleNumber;
	cmd.s_eleNumber = address.s_eleNumber;
	memcpy(&sendMsg.data,&cmd,sizeof(cmd));
	sendMsg.size = sizeof(cmd);
	memcpy(&sendMsg.data[sendMsg.size],value,cmd.size);
	sendMsg.size += cmd.size; 
	if ((error=send_DF1(sendMsg))!=0)
		return error;
	if ((error=rcv_DF1(&rcvMsg))!=0)
		return error;
	if (rcvMsg.tns!=sendMsg.tns)
		return ERROR_TNS_MISMATCH;
	if (rcvMsg.sts!=0)
		return rcvMsg.sts;
	return SUCCES;
}
