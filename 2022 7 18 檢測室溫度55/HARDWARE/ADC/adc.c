#include "adc.h"
#include "delay.h"
__IO uint16_t ADCConvertedValue[M*N]; 
adc ADC;
uint16_t ADC_BUFF[M];

/***********************************************************
void ADC_GPIO_Init(void)
ADC GPIO初始化
************************************************************/ 
void ADC_GPIO_Init(void)
{
	//LD_TH1     LD板温度               PA.6
	//PD_ADC     PD信号                 PA.7
	//CHAM_TH4   烟尘气室温度           PB.0
	//PT_ADC     制冷温度信号           PB.1
	//O2_ADC     氧气信号               PC.0
	//POW_ADC    电池电量监测信号，     PC.1
	//PCB_TH5    机箱内温度             PC.2
	//PD_TH2     PD温度                 PC.4
	//IN_TH3     PD温度                 PC.5
  GPIO_InitTypeDef GPIO_InitStructure;//ADC_SoftwareStartConvCmd(ADC1, ENABLE)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
}
/***********************************************************
void ADC_DMA_Config(void)
DMA初始化
************************************************************/
void ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure; 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 
	DMA_DeInit(DMA1_Channel1); 
	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)ADCConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = M*N; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  
	DMA_Cmd(DMA1_Channel1, ENABLE); 
}
/***********************************************************
void ADC1_Init(void)
ADC1初始化
************************************************************/
void ADC1_Init(void)
{
   ADC_InitTypeDef ADC_InitStructure;  
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
	 
	  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;  
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  
    ADC_InitStructure.ADC_NbrOfChannel = M;  
    ADC_Init(ADC1, &ADC_InitStructure);  
	//
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_6,1, ADC_SampleTime_239Cycles5);  
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7,2, ADC_SampleTime_239Cycles5);  
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_8,3, ADC_SampleTime_239Cycles5);  
		ADC_RegularChannelConfig(ADC1, ADC_Channel_9,4, ADC_SampleTime_239Cycles5);  
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10,5, ADC_SampleTime_239Cycles5);  
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_11,6, ADC_SampleTime_239Cycles5);  
		ADC_RegularChannelConfig(ADC1, ADC_Channel_12,7, ADC_SampleTime_239Cycles5);  
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14,8, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15,9, ADC_SampleTime_239Cycles5);    
    ADC_DMACmd(ADC1, ENABLE); 
	  ADC_Cmd(ADC1, ENABLE); 
	  ADC_ResetCalibration(ADC1); 
		while(ADC_GetResetCalibrationStatus(ADC1));
		ADC_StartCalibration(ADC1); 
		while(ADC_GetCalibrationStatus(ADC1)) {};
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); 
}
/***********************************************************
void ADC_CONFIG(void)
ADC_CONFIG 
************************************************************/
void ADC_CONFIG(void)
{
  ADC_GPIO_Init();
	ADC_DMA_Config();
	ADC1_Init();
}
/***********************************************************
void Data_ConvertedValue(void)
数据转化
************************************************************/
void Data_ConvertedValue(void)
{
   u8 i=0,j=0;
	uint16_t sum[9]={0};
  for(j=0;j<M;j++)    //m=9
	     {
				 for(i=0;i<N;i++)//n=8
				 {
					 sum[j]+=ADCConvertedValue[j+i*M];
				 }
				    ADC_BUFF[j]=sum[j]/8;
			 }
}


