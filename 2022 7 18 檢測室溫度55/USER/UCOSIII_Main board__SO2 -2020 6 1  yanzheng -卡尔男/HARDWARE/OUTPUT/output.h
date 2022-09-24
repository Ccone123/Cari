#ifndef __OUTPUT_H
#define __OUTPUT_H	

#include "sys.h"
typedef struct
{
	char HoldingregStart;
	char iRegIndex;
	
}flag;
extern flag WFlag;
void poiters (void);
void ReadValue_InputBuff(void);
void ReadValue_HoldingBuf(void);
void WriteValue_HoldingBuf(char StartRegx,char IndexNumx);
void data_transmitter(char StartReg,char IndexNum);
#endif
