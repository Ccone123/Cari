#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
/********************************************************************************	 
						  
*********************************************************************************/ 
#define   FAN_COL      PBout(6)   //风扇
#define   LD_COL       PBout(5)   //PD
#define   FANhot_COL   PBout(4)   //散热风扇

#define   PUMP_COL     PBout(3)   //气泵
#define   ADDhot_COL   PAout(0)   //冷凝片
#define   PPUMP_COL    PAout(1)   //蠕动泵
#define   RUN1         PAout(15)  //
void LED_Init(void);  //初始化	
void Base_GPIO_Init(void);
#endif
