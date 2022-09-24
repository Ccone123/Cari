#include "led.h"  
/********************************************************************************	 
#define FAN_COL      PBout(6);   //风扇
#define LD_COL       PBout(5);   //
#define FANhot_COL   PBout(4);   //散热风扇
#define PUMP_COL     PBout(3);   //气泵
#define ADDhot_COL   PAout(0);   //冷凝片
#define PPUMP_COL    PAout(1);   //蠕动泵
#define RUN          PAout(15);  						  
*********************************************************************************/ 
/********************************************************************************	 
	void Base_GPIO_Init(void)			
*********************************************************************************/ 
void Base_GPIO_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;	

 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB,PE端口时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);			//根据设定参数初始化GPIOB.5

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_15;	
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
  GPIO_SetBits(GPIOB,GPIO_Pin_3);						 //PB.5 输出高
  GPIO_SetBits(GPIOB,GPIO_Pin_4);
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
//	GPIO_SetBits(GPIOB,GPIO_Pin_6);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
//	GPIO_SetBits(GPIOA,GPIO_Pin_15);
	 FAN_COL=1;
	 LD_COL=1;
	  RUN1=0;
		ADDhot_COL=0; //FANhot_COL=0;
}
 





