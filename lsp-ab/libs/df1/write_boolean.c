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
int write_boolean(int plctype, char *straddress, int *value)
{
	int error=ERROR_READ_INTEGER;
	TThree_Address_Fields address;
	byte posit;
	word valuebool;
	word mask;
	
	if ((error = calc_address(straddress,&address))!=SUCCES)
		return error;
	posit=address.s_eleNumber;
	if (plctype==SLC){
		mask = 0x0001<<posit;
		if (*value)
			valuebool = 0x0001<<posit;
		else
			valuebool = 0x0000;
		error=write_AB(address,valuebool,mask);
	}
	return error;
}
