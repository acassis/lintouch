#include "libcell.h"

unsigned short get_object_size(_tag_detail *tag)
{

unsigned short ret;

if (tag == NULL)
	return 0;
switch ((byte)tag->type)
	{
	case 0xc1:
	case 0xc2:
	case 0xc6:
	case 0xd0:
	case 0xda:
		ret = 1;
		break;
	case 0xc3:
	case 0xc7:
	case 0xd2:
	case 0xd5:
	case 0xd8:
		ret = 2;
		break;
	case 0xc4:
	case 0xc8:
	case 0xca:
	case 0xd3:
	case 0xd7:
		ret = 4;
		break;
	case 0xc5:
	case 0xc9:
	case 0xcb:
	case 0xd4:
		ret = 8;
		break;
	default:
		ret=0;
	}
return ret;
}
