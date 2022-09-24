#include "static.h"
/*******************************************************************************
* Function Name  :
* Description    : 
********************************************************************************/
unsigned char const Ascill_16[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
unsigned char ChrToInt(char chr)
{
	unsigned char n=0;
	if(chr>='0'&&chr<='9')
	{
		n=chr-'0';
	}
	else if(chr>='a'&&chr<='f')
	{
		n=chr-'a'+10;
	}
	  return n;
}