#include "com_press.h"
#include "stdio.h"
#include "string.h"
#include "lyndir_no.h"
#include "adc.h"
#include "math.h"
#include "MPL115A2.h"
extern uint16_t usRegHoldingBuf[];
extern uint16_t usRegInputBuf[];
extern u16 ADC_BUFF[];
/***********************************************************
参数申明
************************************************************/
 PCTRL_MSG PC;
 CTRL_MSG  pcc;
 CTRL_MSG  *PCC;
 
 
/***********************************************************
  void Data_Update(void)
	数据对齐
************************************************************/
void Data_Update(void)
{
	
  Lyndir_No=(PPara_Data)usRegHoldingBuf;//低16位在前（第八位在后高八位在前），高16位在后，
	ADC=(adc)ADC_BUFF;
	PC=(PCTRL_MSG)usRegInputBuf;
	PC->SerialNumber[0]='A';
	PC->SerialNumber[1]='Y';
	PC->SerialNumber[2]='Q';
	PC->SerialNumber[3]='N';
	PC->SerialNumber[4]='N';
	PC->SerialNumber[5]='I';
	PC->SerialNumber[6]='.';
	PC->SerialNumber[7]='O';
	PC->SerialValue=0x01;
	PC->Version[0]='E';
	PC->Version[1]='V';
	PC->Version[2]='.';
	PC->Version[3]='R';
	PC->Version[4]='1';
	PC->Version[5]='0';
	PC->CC=0x66778899;
}



 /***********************************************************
  参数申明
************************************************************/

 /***********************************************************
  参数申明
************************************************************/




/***********************************************************
 function:void Read_Concentration_Value(void)
************************************************************/
void Parament_update(void) //4129999a,29419a99,999a4129
{
//	PC=(PCTRL_MSG)buff;
//	PC->cmd_head=0x1234;
//	PC->cmd_type=0x2345;
//	PC->control_id=0x3456;
//	PC->control_type=0x4567;
//	PC->ctrl_msg=0x5678;
//	PC->screen_id=0x6789;
//  b=strlen((const char*)PC);
////   buff[0]=0xf9;
////	 buff[1]=0xb3;	
//////	 buff[0]=0xf9;
//////	 buff[1]=0x;	
////	Data[0]=(int16_t)((buff[1]<<8)|buff[0]>>13);
////	Data[0]=Data[0];
//	      fushua0();
}
 void Parament_flash(void)
 {	 
//	  buff[0]= *(char*)PC;
//	  buff[1]= *((char*)PC+1);
//	  buff[2]= *((char*)PC+2);
//	  buff[3]= *((char*)PC+3);
//	  buff[4]= *((char*)PC+4);
//	  buff[5]= *((char*)PC+5);
//	  buff[6]= *((char*)PC+6);
//	  buff[7]= *((char*)PC+7);
//	  c=strlen((const char*)PC);		
 }
 