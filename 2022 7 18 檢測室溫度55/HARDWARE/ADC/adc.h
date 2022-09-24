#ifndef _ADC_H
#define _ADC_H
#include "sys.h"
#define N 8 //每个通道采集8次
#define M 9 //总共8个通道

#pragma pack(push)
#pragma pack(1)     
typedef struct {
	uint16_t LD_TH1;     //LD板温度          PC.1     PA.6
	uint16_t PD_ADC;     //PD信号            PB.1     PA.7
	uint16_t CHAM_TH4;   //烟尘气室温度      PC.4     PB.0
	uint16_t PT_ADC;     //制冷温度信号               PB.1
	uint16_t O2_ADC;     //氧气信号          PB.0     PC.0
	uint16_t POW_ADC;    //电池电量监测信号，PC.0     PC.1
	uint16_t PCB_TH5;    //机箱内温度        pc.5     PC.2
	uint16_t PD_TH2;     //PD温度            PC.2     PC.4
	uint16_t IN_TH3;     //PD温度            PC.3     PC.5
} *adc;
#pragma apop
extern adc ADC;

void ADC_GPIO_Init(void);
void ADC_DMA_Config(void);
void ADC1_Init(void);
void Data_ConvertedValue(void);
void ADC_CONFIG(void);
#endif