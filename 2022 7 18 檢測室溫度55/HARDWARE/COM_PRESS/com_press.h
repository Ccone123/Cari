#ifndef _CMD_PROCESS_H
#define _CMD_PROCESS_H
#include "sys.h"

#pragma pack(push)
#pragma pack(1)                           //按字节对齐
typedef struct
{
   char SerialNumber[8]; 
	 u16  SerialValue;
	 char Version[6];	
	 u32 CC;
}CTRL_MSG,*PCTRL_MSG;

#pragma apop

#pragma pack(push)
#pragma pack(1) 

typedef struct {
   float x;     //LD板温度          
	 float y;   //烟尘气室温度    
   float z;     //PD信号       

}xyz;
#pragma apop
extern xyz XYZ;


void Parament_update(void);
void Parament_flash(void);
void Data_Update(void);
#endif
