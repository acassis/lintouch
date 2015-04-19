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

//*********************************************************
int write_float(int plctype, char *straddress, float *value)
{
	int error=ERROR_WRITE;
	TThree_Address_Fields address;
	
	if ((error = calc_address(straddress,&address))!=SUCCES)
		return error;
	if (plctype==SLC)
		error=write_AA(address,value,sizeof(*value));
	return error;
}
