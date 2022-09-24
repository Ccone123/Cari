#include "Lyndir_No.h"
#include "mbcrc.h"
#include "usart4.h"
#include "delay.h"
#include "usart.h"	
#include "ntc.h"
/***********************************************************
参数申明
************************************************************/
extern uint16_t usRegHoldingBuf[];
extern uint16_t usRegInputBuf[];
PPara_Data Lyndir_No;
const u8 STATUS_RX_BUF[20]={0}; 
u8 Zero_Calibration_RX_BUF[20]={0};
u8 Zero_CalFinish_Flag_RX_BUF[20]={0};
u8 Span_CalCoefficientR_RX_BUF[20]={0};
u8 Span_CalCoefficientW_RX_BUF[20]={0};
u8 Model_Status_RX_BUF[20]={0};
u8 No_Concentration_RX_BUF[20]={0};
u8 No_Pressure_RX_BUF[20]={0};
u8 No_Status_RX_BUF[20]={0};
/***********************************************************
************************************************************/
//读寄存器
//const u8 Model_Typical[8]={};
	
//const u8 Model_SerialN[8]={};
	
//const u8 Model_SoftVerion[8]={};
//进入零点校准	
const u8 Zero_Calibration[8]={0x22,0x06,0x00,0x40,0x00,0x01,0x8D,0x4E};
//零点校准完成标志读取
const u8 Zero_CalFinish_Flag[8]={0x22,0x03,0x00,0x4F,0x00,0x01,0x8E,0xB2};	

//const u8 Model_SpanValue[]={};
//读取量程校准系数
const u8 Span_CalCoefficientR[8]={0x22,0x03,0x00,0x50,0x00,0x02,0x49,0xC3};
//写入量程校准系数
u8 Span_CalCoefficientW[13]={0x22,0x10,0x00,0x50,0x00,0x02,0x04};

const u8  Model_Status[8]={0x22,0x03,0x00,0x8F,0x00,0x01,0xB2,0xB2};
//读取浓度值
const u8  No_Concentration[8]={0x22,0x03,0x00,0x90,0x00,0x02,0x75,0xC3};
//N0模块压力读取
const u8       No_Pressure[8]={0x22,0x03,0x00,0x9C,0x00,0x02,0x76,0x03};
//No模块状态读取
const u8         NO_Status[8]={0x22,0x03,0x00,0x8F,0X00,0X01,0xB2,0xB2};
//const u8 Pressure_Value[]={};	
//写相关参数
typedef union      
           {
						 float exdataFLOAT;
						  u8    tempdata[4];
					 }GET;
 GET GETDATA ={0};	
/***********************************************************
 function:void Read_Concentration_Value(void)
 读取浓度值
************************************************************/
 void Read_Concentration_Value(void)
 {
	 u16 calCRC;
	 u16 recCRC;
	 u8 count=2;
	 u8 i=0;
	 u8 rs485buf[20];
	 u8 revLen=0;
	 for(i=0;i<8;i++)
	 {
		 rs485buf[i]=No_Concentration[i];
	 }
	 RS485_Send_Data(rs485buf,8);
	 delay_ms(150);
	 RS485_Receive_Data(No_Concentration_RX_BUF,&revLen);	
	 if(  9 ==revLen)
	 {
		 calCRC=usMBCRC16(No_Concentration_RX_BUF,revLen-2);
		 recCRC=No_Concentration_RX_BUF[revLen-1]|(((u16)No_Concentration_RX_BUF[revLen-2])<<8);
		 if(calCRC==recCRC)
		 {
			 	  GETDATA.tempdata[0]=No_Concentration_RX_BUF[++count];   //81
					GETDATA.tempdata[1]=No_Concentration_RX_BUF[++count];
					GETDATA.tempdata[2]=No_Concentration_RX_BUF[++count];   //82
					GETDATA.tempdata[3]=No_Concentration_RX_BUF[++count]; 
			    Lyndir_No->Con_Old=GETDATA.exdataFLOAT;
			   // Lyndir_No->New_Con_Old=Lyndir_No->Con_Old-668.62*NTC.O2_ADC_V+806.18;
			 //    printf("%f@_@\r\n",Lyndir_No->Con_Old);
		 }
	 }
 }
 /***********************************************************
 function:void Read_NOPressure_Value(void)
 读取模块压力
************************************************************/
  void Read_NOPressure_Value(void)
 {
	 u16 calCRC;
	 u16 recCRC;
	 u8 count=2;
	 u8 i=0;
	 u8 rs485buf[20];
	 u8 revLen=0;
	 for(i=0;i<8;i++)
	 {
		 rs485buf[i]=No_Pressure[i];
	 }
	 RS485_Send_Data(rs485buf,8);
	 delay_ms(150);
	 RS485_Receive_Data(No_Pressure_RX_BUF,&revLen);	
	 if( 9 ==revLen)
	 {
		 calCRC=usMBCRC16(No_Pressure_RX_BUF,revLen-2);
		 recCRC=No_Pressure_RX_BUF[revLen-1]|(((u16)No_Pressure_RX_BUF[revLen-2])<<8);
		 if(calCRC==recCRC)
		 {
			 	  GETDATA.tempdata[0]=No_Pressure_RX_BUF[++count];   //81
					GETDATA.tempdata[1]=No_Pressure_RX_BUF[++count];
					GETDATA.tempdata[2]=No_Pressure_RX_BUF[++count];   //82
					GETDATA.tempdata[3]=No_Pressure_RX_BUF[++count]; 
			    Lyndir_No->No_Press=GETDATA.exdataFLOAT;
			    // printf("%f@_@\r\n",Lyndir_No->No_Press);
		 }
	 }
 }
 /***********************************************************
 function:void Read_NOStatus_Value(void)
 读取模块状态
************************************************************/
  void Read_NOStatus_Value(void)
 {
	 u16 calCRC;
	 u16 recCRC;
	 u8 count=2;
	 u8 i=0;
	 u8 rs485buf[20];
	 u8 revLen=0;
	 for(i=0;i<8;i++)
	 {
		 rs485buf[i]=NO_Status[i];
	 }
	 RS485_Send_Data(rs485buf,8);
	 delay_ms(150);
	 RS485_Receive_Data(No_Status_RX_BUF,&revLen);	
	 if( 7 ==revLen)
	 {
		 calCRC=usMBCRC16(No_Status_RX_BUF,revLen-2);
		 recCRC=No_Status_RX_BUF[revLen-1]|(((u16)No_Status_RX_BUF[revLen-2])<<8);
		 if(calCRC==recCRC)
		 {
			 	  Lyndir_No->No_Status=(No_Status_RX_BUF[3]<<8)|No_Status_RX_BUF[4];			  
			//     printf("%f@_@\r\n",Lyndir_No->No_Status);
		 }
	 }
 } 
 /***********************************************************
 function:void Zero_Calibration_Write(void)
写入零点校准开始
************************************************************/
 void Zero_Calibration_Write(void)
 {
	 u16 calCRC;
	 u16 recCRC;
//	 u8 count=2;
	 unsigned char i=0;
	 unsigned char rs485buf[20];
	 unsigned char revLen=0;
	 for(i=0;i<8;i++)
	 {
		 rs485buf[i]=Zero_Calibration[i];
	 }
	 RS485_Send_Data(rs485buf,8);
	 delay_ms(500);
	 RS485_Receive_Data(Zero_Calibration_RX_BUF,&revLen);	
	 if(8==revLen)
	 {
		 calCRC=usMBCRC16(Zero_Calibration_RX_BUF,revLen-2);
     recCRC=Zero_Calibration_RX_BUF[revLen-1]|(((u16)Zero_Calibration_RX_BUF[revLen-2])<<8);
	  if(calCRC==recCRC)
		{
			Lyndir_No->Zero_Cal=Zero_Calibration_RX_BUF[5];
			printf("Lyndir_No->Zero_Cal=%d@_@\r\n", Lyndir_No->Zero_Cal);
			Lyndir_No->Zero_CalFinish_Flag=0;
			printf("Lyndir_No->Zero_CalFinish_Flag=%d@_@\r\n", Lyndir_No->Zero_CalFinish_Flag);
		}
	 } 
 }
 /***********************************************************
 function:void Read_Zero_CalFinish_Flag(void)
读取零点校准完成标识
************************************************************/
 void Read_Zero_CalFinish_Flag(void)
 {
	 u16 calCRC;
	 u16 recCRC;
//	 u8 count=2;
	 unsigned char i=0;
	 unsigned char rs485buf[20];
	 unsigned char revLen=0;
	 for(i=0;i<8;i++)
     {
			 rs485buf[i]=Zero_CalFinish_Flag[i];
		 }	
    	RS485_Send_Data(rs485buf,8);
			delay_ms(500);
		RS485_Receive_Data(Zero_CalFinish_Flag_RX_BUF,&revLen);		 
    if(7==revLen)
		{
			calCRC=usMBCRC16(Zero_CalFinish_Flag_RX_BUF,revLen-2);
			recCRC = Zero_CalFinish_Flag_RX_BUF[revLen-1]|(((u16)Zero_CalFinish_Flag_RX_BUF[revLen-2])<<8);
			if(calCRC==recCRC)
			{
				Lyndir_No->Zero_CalFinish_Flag=Zero_CalFinish_Flag_RX_BUF[4];
		   	printf("Lyndir_No->Zero_CalFinish_Flag=%d@_@\r\n", Lyndir_No->Zero_CalFinish_Flag);
			}			
		}
 }
 /***********************************************************
 function:void Span_CalCoefficientR_Value(void)
 读取量程校准系数
************************************************************/
 void Span_CalCoefficientR_Value(void)
 {
	 u16 calCRC;
	 u16 recCRC;
	 u8 count=2;
	unsigned char i=0;
	unsigned char rs485buf[20];
	unsigned char revLen=0; 
	 for(i=0;i<8;i++)
	 {
		rs485buf[i]= Span_CalCoefficientR[i];
	 }
	 	RS485_Send_Data(rs485buf,8);
			delay_ms(500);
	 RS485_Receive_Data(Span_CalCoefficientR_RX_BUF,&revLen);	
	 if(9 == revLen)
	 {
		 calCRC=usMBCRC16(Span_CalCoefficientR_RX_BUF,revLen-2);	
		 recCRC = Span_CalCoefficientR_RX_BUF[revLen-1]|(((u16)Span_CalCoefficientR_RX_BUF[revLen-2])<<8);
	  if(calCRC==recCRC)
		{
				 	GETDATA.tempdata[0]=Span_CalCoefficientR_RX_BUF[++count];   //81
					GETDATA.tempdata[1]=Span_CalCoefficientR_RX_BUF[++count];
					GETDATA.tempdata[2]=Span_CalCoefficientR_RX_BUF[++count];   //82
					GETDATA.tempdata[3]=Span_CalCoefficientR_RX_BUF[++count]; 
			    Lyndir_No->Span_Old=GETDATA.exdataFLOAT;
			    Lyndir_No->Span_Old_Flag=0;//读取完毕一定要置零
			 	  printf("Lyndir_No->Span_Old=%f@_@\r\n", Lyndir_No->Span_Old);
		}
	 }	 
 }
 /***********************************************************
 function:void Span_CalCoefficientW_Value(void)
写入校准系数
************************************************************/
void Span_CalCoefficientW_Value(void)
{
	u16 calCRC;
	u16 recCRC;
	u16 scalCRC;
//	u8 count=2;
	  unsigned char i=0;
  	unsigned char rs485buf[20];
	  unsigned char revLen=0; 
   Lyndir_No->Span_New=CALCoefficient_Calulation(Lyndir_No->Span_Old);
	//Lyndir_No->Span_New=0.98;
	  GETDATA.exdataFLOAT=Lyndir_No->Span_New;  
		Span_CalCoefficientW[7]=GETDATA.tempdata[0];
		Span_CalCoefficientW[8]=GETDATA.tempdata[1];
		Span_CalCoefficientW[9]=GETDATA.tempdata[2];
		Span_CalCoefficientW[10]=GETDATA.tempdata[3];
	  scalCRC=usMBCRC16(Span_CalCoefficientW,11);
	  Span_CalCoefficientW[11]=(scalCRC>>8)&0xff;
	  Span_CalCoefficientW[12]=scalCRC&0xff;
	 
		 for(i=0;i<13;i++)
	 {
		  rs485buf[i]= Span_CalCoefficientW[i];
	 }
		RS485_Send_Data(rs485buf,13);//RS485_Send_Data(Span_CalCoefficientW,13);
			delay_ms(500);
	 RS485_Receive_Data(Span_CalCoefficientW_RX_BUF,&revLen);	
	 if(8==revLen)
	 {
		 calCRC=usMBCRC16(Span_CalCoefficientW_RX_BUF,revLen-2);	
	  	recCRC = Span_CalCoefficientW_RX_BUF[revLen-1]|(((u16)Span_CalCoefficientW_RX_BUF[revLen-2])<<8);
		 if(calCRC==recCRC)
		 {
			 Lyndir_No->Span_New_Flag=0;//读到数据后，记得清零。
			 	printf("Lyndir_No->Span_New_Flag=%d@_@\r\n", Lyndir_No->Span_New_Flag);
		 }	 
	 }	 
}
/***********************************************************
 function:void CALCoefficient_Calulation(void)
计算校准系数
************************************************************/
float CALCoefficient_Calulation(float Old)
{
	 float New;
	 New=Lyndir_No->Con_New*Old/Lyndir_No->Con_Old;
	 return New;
}
/***********************************************************
 function:void Calibrate_Function(void)
 校准模块
************************************************************/
 void Calibrate_Function(void)
 {
	 if(Lyndir_No->Zero_Cal_Flag==0x0001)
	 {
		 Lyndir_No->Zero_Cal=0; //置0为了接收
		 Zero_Calibration_Write();              //零点校准指令
		 if(Lyndir_No->Zero_Cal==0x0001)
		 {
			 Lyndir_No->Zero_Cal_Flag=0;		
		 }
	 }
	 //Lyndir_No->Zero_CalFinish_Flag读到1后一定要记得写零
	if((Lyndir_No->Zero_Cal==0x0001)&&(Lyndir_No->Zero_CalFinish_Flag==0)) 
	{
		   Read_Zero_CalFinish_Flag();
		   if(Lyndir_No->Zero_CalFinish_Flag==0x0001)
				   Lyndir_No->Zero_Cal=0;
	}
	 
	if(Lyndir_No->Span_Old_Flag==0x0001)
	{
		Span_CalCoefficientR_Value();	
	}
  if(Lyndir_No->Span_New_Flag==0x0001) 
	{
		Span_CalCoefficientW_Value();		
	}	 
 }

