#include "ntc.h"
#include "math.h"
#include "adc.h"
#include "Lyndir_No.h"
#include "led.h"
#include "delay.h"
#include "includes.h"
//Rt=R*EXP(B*(1/T1-1/T2))
//Rt������������T1�¶��µ���ֹ
//R������������T2�����µı����֯
//Bֵ�������������Ҫ����
//EXP��e��n�η�
//T1��T2ָ����K�ȼ��������¶ȣ�K��=273.15�������¶ȣ�+���϶�
ntc NTC;char  PID_Tem_startup;
u8 illumant_switch_flag;
float aa[10];
/***********************************************************
float Resistance_Rx(mpl temp) 
���������ֵ
************************************************************/
void Resistance_Rx(ntc temp)
{
  NTC.LD_TH1_Rt=(3.3*((float)ADC->LD_TH1/4096))/(Vref-(3.3*((float)ADC->LD_TH1/4096)));   //12λ,�޻�׼��ȡ֮3.3v,��ӵ�ѹ3V(Vrefk���Ը���)
  NTC.CHAM_TH4_Rt=(3.3*10*((float)ADC->CHAM_TH4/4096))/(Vref-(3.3*((float)ADC->CHAM_TH4/4096))); 
	NTC.PCB_TH5_Rt=(3.3*10*((float)ADC->PCB_TH5/4096))/(Vref-(3.3*((float)ADC->PCB_TH5/4096))); 
	NTC.PD_TH2_Rt=(3.3*((float)ADC->PD_TH2/4096))/(Vref-(3.3*((float)ADC->PD_TH2/4096))); 
	NTC.IN_TH3_Rt=(3.3*10*((float)ADC->IN_TH3/4096))/(Vref-(3.3*((float)ADC->IN_TH3/4096))); 
}
/***********************************************************
float Resistance_Rx(mpl temp) 
������¶�
************************************************************/
void NTC_thermistor (ntc temp)
{
    NTC.LD_TH1_Tem=   1/(log(NTC.LD_TH1_Rt/R)/B+1/T2) -273.15;
	  NTC.CHAM_TH4_Tem=1/( log(NTC.CHAM_TH4_Rt/R)/B+1/T2)-273.15;
	  NTC.PCB_TH5_Tem=1/( log(NTC.PCB_TH5_Rt/R)/B+1/T2)-273.15;
	  NTC.PD_TH2_Tem=1/( log(NTC.PD_TH2_Rt/R)/B+1/T2)-273.15;
	  NTC.IN_TH3_Tem=1/( log(NTC.IN_TH3_Rt/R)/B+1/T2)-273.15;	
	   Lyndir_No->LD_TH1_Tem=NTC.LD_TH1_Tem;
	   Lyndir_No->PD_TH2_Tem=NTC.PD_TH2_Tem;	
     Lyndir_No->IN_TH3_Tem=NTC.IN_TH3_Tem;
	   Lyndir_No->CHAM_TH4_Tem=NTC.CHAM_TH4_Tem;
	   Lyndir_No->PCB_TH5_Tem=NTC.PCB_TH5_Tem;
//			if(Lyndir_No->CHAM_TH4_Tem>50)
//					{
//						 FAN_COL=0;			
//					}
	
}
/***********************************************************
float Resistance_Rx(mpl temp) 
������¶�
************************************************************/
void NTC_TEM_Resurt(void)
{
   Resistance_Rx(NTC);
	 NTC_thermistor(NTC);
 
}
/***********************************************************
float Battery_Level_MS(mpl temp) 
�������ص���
************************************************************/
void Battery_Level_MS(ntc temp)
{
   NTC.Battery_Level=(3.3*((float)ADC->POW_ADC/4096))*6;//12~10
  	Lyndir_No->Battery_Level= NTC.Battery_Level;
	 if(NTC.Battery_Level<10)
	 {
		 delay_ms(100);
	   RUN1=1;
		 delay_ms(100);
		 RUN1=0;	 
	 }
	 else 
	 {
		delay_ms(500);
	   RUN1=1;
		 delay_ms(500);
		 RUN1=0;	  
	 }
}
/***********************************************************
float PD_Signal_Sampling(mpl temp) 
PD_ADC�źŲɼ�
************************************************************/
float PD_Signal_Sampling(ntc temp)
{
	 NTC.PD_ADC_V=3.3*(float)ADC->PD_ADC/4096;    //PD�źŲɼ�
	 return NTC.PD_ADC_V;
}
/***********************************************************
float Electrochemical_Oxygen_Acquisition(mpl temp) 
O2_ADC_V�绯ѧ���ɼ�
************************************************************/
void Electrochemical_Oxygen_Acquisition(ntc temp)
{
   NTC.O2_ADC_V=3.3*(float)ADC->O2_ADC/4096;
    Lyndir_No->Humity_O2_ADC_V=	NTC.O2_ADC_V;
	 Lyndir_No->Humity= NTC.O2_ADC_V*1/2.6 -(float)1/2.6; 
	
	if( NTC.O2_ADC_V<=2.51)
	 Lyndir_No->New_Humity=NTC.O2_ADC_V*0.4-0.498;
	else
	 Lyndir_No->New_Humity=NTC.O2_ADC_V*2.19-4.99;
	  
	
	
}
/***********************************************************
 function:void Calibrate_Function(void)
 У׼ģ��
************************************************************/
 void PD_Calibrate_Function(void)
 {
    
		                     
 
 }
/***********************************************************
 function:void Switch_Control_Function(void)
 ������Ԫ����
 	uint16_t   FAN_CONTROL;            //����
  uint16_t   FANhot_CONTROL;        //ɢ�ȷ���
  uint16_t   PUMP_CONTROL;           //����
  uint16_t   ADDhot_CONTROL;         //����Ƭ
  uint16_t   PPUMP_CONTROL;          //�䶯��
************************************************************/
void Switch_Control_Function(void)
 {
	
	 //PD��Դ
	  if(Lyndir_No->LD_CONTROL==0x0001)
		 {
			 LD_COL=0;     //������Դ
			Lyndir_No->LD_CONTROL=3;
		 }
		 		 
		 if(Lyndir_No->LD_CONTROL ==0x0002)
		 {
			   LD_COL=1; 
			 Lyndir_No->LD_CONTROL=4;
			 	
		 }
	//����	  
			  if(Lyndir_No->FAN_CONTROL ==0x0001)
		 {
			  FAN_COL=0;     //������Դ
			  Lyndir_No->FAN_CONTROL=3;
		 }
		 if(Lyndir_No->FAN_CONTROL ==0x0002)
		 {
			   FAN_COL=1; 
			   Lyndir_No->FAN_CONTROL=4;
		 } 	 
	//ɢ�ȷ���
		 		  if(Lyndir_No->FANhot_CONTROL ==0x0001)
		 {
			 // FANhot_COL=0;     //������Դ
			  Lyndir_No->FANhot_CONTROL=3;
		 }    
		 if(Lyndir_No->FANhot_CONTROL ==0x0002)
		 {
			  // FANhot_COL=1; 
			   Lyndir_No->FANhot_CONTROL=4;
		 }     
	//����
		  if(Lyndir_No->PUMP_CONTROL ==0x0001)
		 {
			  PUMP_COL=0;     //������Դ
			  Lyndir_No->PUMP_CONTROL=3;
		 }
		 if(Lyndir_No->PUMP_CONTROL ==0x0002)
		 {
			   PUMP_COL=1; 
			   Lyndir_No->PUMP_CONTROL=4;
		 } 
//����Ƭ
		  if(Lyndir_No->ADDhot_CONTROL ==0x0001)
		 {
			  //ADDhot_COL=0;     //������Դ
			  FANhot_COL=0;
			  Lyndir_No->ADDhot_CONTROL=3;
			  PID_Tem_startup=0;
		 }
		 if(Lyndir_No->ADDhot_CONTROL ==0x0002)
		 {
			  // ADDhot_COL=1; 
			   FANhot_COL=1;
			   Lyndir_No->ADDhot_CONTROL=4;
			  PID_Tem_startup=1;
		 } 
//�䶯��		
		  if(Lyndir_No->PPUMP_CONTROL ==0x0001)
		 {
			  PPUMP_COL=0;     //������Դ
			  Lyndir_No->PPUMP_CONTROL=3;
		 }
		 if(Lyndir_No->PPUMP_CONTROL ==0x0002)
		 {
			   PPUMP_COL=1; 
			   Lyndir_No->PPUMP_CONTROL=4;
		 }
 //���
     if(Lyndir_No->Duoji_CONTROL==0x0001) 
		 {
		   TIM_SetCompare1(TIM3,1500);
			 Lyndir_No->Duoji_CONTROL=0x0004;
		 }
		 else if(Lyndir_No->Duoji_CONTROL==0x0002) 
		 {
		    TIM_SetCompare1(TIM3,2500);
			 Lyndir_No->Duoji_CONTROL=0x0005;
		 }
		 else if(Lyndir_No->Duoji_CONTROL==0x0003) 
		 {
		   TIM_SetCompare1(TIM3,3500);
			 Lyndir_No->Duoji_CONTROL=0x0006;
		 }
		 else
		     ;
     		 
	 
 }
 /***********************************************************
 function:void Switch_Control_Function(void)
 ������Ԫ����

************************************************************/
 void  Tem_Control_Tuning(long Step)
 {
	 long temp=0;OS_ERR err;
	 temp=Step;
	 if(Step>0)
	 {
	     FANhot_COL=0;
       Lyndir_No->ADDhot_CONTROL=3;		 
		   OSTimeDlyHMSM(0,0,temp,0,OS_OPT_TIME_HMSM_NON_STRICT,&err); 
		   FANhot_COL=1;
       Lyndir_No->ADDhot_CONTROL=4;		 
	 }
	 if(Step<0)
	 {
	     FANhot_COL=1;                    //FANhot_COL
		   Lyndir_No->ADDhot_CONTROL=4;
		   OSTimeDlyHMSM(0,0,fabs(temp),0,OS_OPT_TIME_HMSM_NON_STRICT,&err);
		   FANhot_COL=0; 	
       Lyndir_No->ADDhot_CONTROL=3;		 
	 }
	 
	 
 }
 