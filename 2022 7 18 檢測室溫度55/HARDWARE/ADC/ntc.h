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
   float LD_TH1_Tem;     //LD板温度          
	 float CHAM_TH4_Tem;   //烟尘气室温度    
   float PD_ADC_V;     //PD信号       
   float PT_ADC_Rt;       //制冷温度信号               PB.1
   float POW_ADC;      //电池电量监测信号，PC.0     PC.1
   float O2_ADC_V;     //氧气信号          PB.0     PC.0
	float PCB_TH5_Tem;    //机箱内温度        
	float PD_TH2_Tem;     //PD温度           
	float IN_TH3_Tem;     //入口烟气温度采集 
	float PT_ADC_Tem;       //制冷温度信号   
  float LD_TH1_Rt;  //待测电阻阻值
	float CHAM_TH4_Rt;//待测电阻阻值
	float PCB_TH5_Rt; //待测电阻阻值
	float PD_TH2_Rt;  //待测电阻阻值
	float IN_TH3_Rt;  //待测电阻阻值
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
