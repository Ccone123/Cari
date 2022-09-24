#ifndef _NTC_H_	
#define _NTC_H_
#include "sys.h"
#define Vref 3
#define B  3950
#define R  12.490   //K
#define T2 (20+273.15)
#pragma pack(push)
#pragma pack(1) 
typedef struct {
   float LD_TH1_Tem;     //LD���¶�          
	 float CHAM_TH4_Tem;   //�̳������¶�    
   float PD_ADC_V;     //PD�ź�       
   float PT_ADC_Rt;       //�����¶��ź�               PB.1
   float POW_ADC;      //��ص�������źţ�PC.0     PC.1
   float O2_ADC_V;     //�����ź�          PB.0     PC.0
	float PCB_TH5_Tem;    //�������¶�        
	float PD_TH2_Tem;     //PD�¶�           
	float IN_TH3_Tem;     //��������¶Ȳɼ� 
	float PT_ADC_Tem;       //�����¶��ź�   
  float LD_TH1_Rt;  //���������ֵ
	float CHAM_TH4_Rt;//���������ֵ
	float PCB_TH5_Rt; //���������ֵ
	float PD_TH2_Rt;  //���������ֵ
	float IN_TH3_Rt;  //���������ֵ
	float Battery_Level;
}ntc;
#pragma apop
extern ntc NTC;
extern char  PID_Tem_startup;
void Resistance_Rx(ntc temp);
void NTC_thermistor (ntc temp);
void NTC_TEM_Resurt(void);
void Battery_Level_MS(ntc temp);
float PD_Signal_Sampling(ntc temp);
void Electrochemical_Oxygen_Acquisition(ntc temp);
void PD_Calibrate_Function(void);
void Switch_Control_Function(void);
void  Tem_Control_Tuning(long Step);
#endif
