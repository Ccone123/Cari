#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
/********************************************************************************	 
						  
*********************************************************************************/ 
#define   FAN_COL      PBout(6)   //����
#define   LD_COL       PBout(5)   //PD
#define   FANhot_COL   PBout(4)   //ɢ�ȷ���

#define   PUMP_COL     PBout(3)   //����
#define   ADDhot_COL   PAout(0)   //����Ƭ
#define   PPUMP_COL    PAout(1)   //�䶯��
#define   RUN1         PAout(15)  //
void LED_Init(void);  //��ʼ��	
void Base_GPIO_Init(void);
#endif
