#ifndef _ADC_H
#define _ADC_H
#include "sys.h"
#define N 8 //ÿ��ͨ���ɼ�8��
#define M 9 //�ܹ�8��ͨ��

#pragma pack(push)
#pragma pack(1)     
typedef struct {
	uint16_t LD_TH1;     //LD���¶�          PC.1     PA.6
	uint16_t PD_ADC;     //PD�ź�            PB.1     PA.7
	uint16_t CHAM_TH4;   //�̳������¶�      PC.4     PB.0
	uint16_t PT_ADC;     //�����¶��ź�               PB.1
	uint16_t O2_ADC;     //�����ź�          PB.0     PC.0
	uint16_t POW_ADC;    //��ص�������źţ�PC.0     PC.1
	uint16_t PCB_TH5;    //�������¶�        pc.5     PC.2
	uint16_t PD_TH2;     //PD�¶�            PC.2     PC.4
	uint16_t IN_TH3;     //PD�¶�            PC.3     PC.5
} *adc;
#pragma apop
extern adc ADC;

void ADC_GPIO_Init(void);
void ADC_DMA_Config(void);
void ADC1_Init(void);
void Data_ConvertedValue(void);
void ADC_CONFIG(void);
#endif