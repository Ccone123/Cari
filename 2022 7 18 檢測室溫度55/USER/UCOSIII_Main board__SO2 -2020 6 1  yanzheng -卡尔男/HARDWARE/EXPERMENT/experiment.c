#include "processing.h"
#include "experiment.h"
#include "communication.h"
#include "includes.h"
#include "rtc.h"
#include "IICEEPROM.h"
#include "delay.h"
#include "USART3.h"
 VAR Var;
void Experiment (void)
{
		OS_ERR err;
	static char  i =0,flag=0;
	if(Var.AfterCALFlag==1)
	{
		Var.OffSet =0;
	   KuaiMen_Switch(0x01,0x01,0x01,0x02);switch_status=1;//采样校准阀
	   Display_Sampling_Switch(1);         //
		 OSTimeDlyHMSM(0,10,0,0,OS_OPT_TIME_PERIODIC,&err);
		  Var.BackGroundValue=FiveMin_AVG_dis;
			SaveEpData(368,Var.BackGroundValue*1000);
		 KuaiMen_Switch(0x00,0x00,0x00,0x02);switch_status=0;
		 Display_Sampling_Switch(0); 
		 Var.AfterCALFlag=0x02;  //用于自校准功能  
	   SaveEpData(360,Var.AfterCALFlag);
		OSTimeDlyHMSM(0,1,0,0,OS_OPT_TIME_PERIODIC,&err);
	}
	    RTC_Get();
		if((calendar.hour==00)&&(flag==0)&&(Var.AfterCALFlag==0x02))
	{		
    Var.OffSet =0;	//校准时把补偿去掉	
		KuaiMen_Switch(0x01,0x01,0x01,0x02);switch_status=1;//采样校准阀
		Display_Sampling_Switch(1);
		OSTimeDlyHMSM(0,10,0,0,OS_OPT_TIME_PERIODIC,&err);
		Var.CalValue=FiveMin_AVG_dis;
		Var.OffSet = Var.BackGroundValue-Var.CalValue;
		SaveEpData(376,Var.OffSet*1000);
		KuaiMen_Switch(0x00,0x00,0x00,0x02);switch_status=0;
		Display_Sampling_Switch(0); 
     flag=1;		
	}
      if((calendar.hour==0x01)&&(flag==1))
			{
			   flag=0;
			}	
}

 void Auto_Cal_0701(unsigned int controlbutton ,char i)//EE B1 10 00 0B 00 32 00 FF FC FF FF 初始化时把按键恢复初始状态
 {
	  delay_ms(2);
	  USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
		USART3_SendByte(0x10);	
		USART3_SendByte(0x00);
		USART3_SendByte(0x07);
	 	USART3_SendByte(controlbutton>>8);
		USART3_SendByte(controlbutton&0xff);
	 	USART3_SendByte(i);
	   END_CMD();	 
 }
 void Button_Control (unsigned int screen , unsigned int control , char state)
 {
    USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
		USART3_SendByte(0x10);	
		USART3_SendByte(screen>>8);
		USART3_SendByte(screen&0xff);
	 	USART3_SendByte(control>>8);
		USART3_SendByte(control&0xff);
	 	USART3_SendByte(state);
	   END_CMD();	 
 }