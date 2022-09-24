#include "modbus_host.h"
#include "sys.h"
//#include "mbcrc.h"
#include "delay.h"
u8 RS485_Addr=0x32;//����ַ
u8 RS485_FrameFlag=0;//֡������־
UsartRxTypeDef4 USARTStructure4;	//���ڽṹ��

Receive Receivedata;
void PtotocolAnalysis (void);
/*********************************************************************
void RS485_Initt(u32 bound)
********************************************************************/
//��ʼ��uart4 modbus�ӻ�
void RS485_Initt(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	
	//UART4_TX pc.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//UART4_RX	  PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	//UART_EN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;						//PD4 485 ??
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//?????50M
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//???????????	
	GPIO_Init(GPIOC, &GPIO_InitStructure);		
		
	USART_DeInit(UART4);
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	
	USART_Init(UART4, &USART_InitStructure); 
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4, ENABLE);
//	Timer7_Init();//��ʱ��7��ʼ�������ڼ��ӿ���ʱ��
	RS485_TX_EN=0; //ʹ�ܽ���
}
/************************************************************************************
void Timer7_Init(void)
*********************************************************************************/
//��ʱ��7��ʼ��
void Timer7_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//tim7sʱ��ʹ��
	
	//tim7��ʼ������
	TIM_TimeBaseStructure.TIM_Period = 4*10;
	TIM_TimeBaseStructure.TIM_Prescaler =7200; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); 
	TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );
	//TIM7�жϷ�������
	TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
		
}
/****************************************************************************************
void RS485_SendData(u8 *buff ,u8 len)
*********************************************************************************************/
//����N���ֽ�����
//buff������ȥ�׵�ַ
//len:���͵��ֽ���
void RS485_Send_Data(u8 *buf,u8 len)
{
	  u8 t;
	  RS485_TX_EN=1;			//����Ϊ����ģʽ
  	delay_us(1000);
  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		//USART_ClearFlag(UART4, USART_FLAG_TC);
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);//�ȴ�Ϊ��
		USART_SendData(UART4,buf[t]);
		  	  
	}	 
    while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);	

	   
	 RS485_TX_EN=0;				//����Ϊ����ģʽ	
}

void USART4_SendString(char * Data,u32 Len)	
{
	u32 i=0;
	 RS485_TX_EN=1;
	delay_us(1000);
	for(i=0;i<Len;i++)
	{
		USART_SendData(UART4,Data[i]);
		  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
	}
		   
	 RS485_TX_EN=0;				//����Ϊ����ģʽ	
}

/************************************************************************
void UART4_IRQHandler(void) 
************************************************************************/
void UART4_IRQHandler(void) 
{
unsigned char USART4_GetChar;
if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
		USART4_GetChar=USART_ReceiveData(UART4);
	}
   if(USARTStructure4.RX_TMEP_Len<64)
	 {
		 USARTStructure4.RX_TEMP_BUFF[USARTStructure4.RX_TMEP_Len]=USART4_GetChar;
		 USARTStructure4.RX_TMEP_Len++;
		 
		 // TIM_SetCounter(TIM2,0);//�����յ�һ���µ��ַ�,����ʱ��7��λΪ0
	   	//TIM_Cmd(TIM2,ENABLE);//��ʼ��ʱ
	 }
		   

}

/***************************************************************************************
void TIM7_IRQHandler(void)
*******************************************************/
void TIM7_IRQHandler(void)
{                                                                   
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
	{
			TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
			TIM_Cmd(TIM7,DISABLE);
			//RS485_TX_EN=1;
			RS485_FrameFlag=1;
	}
}

//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=USARTStructure4.RX_TMEP_Len;
	u8 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==USARTStructure4.RX_TMEP_Len&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USARTStructure4.RX_TEMP_BUFF[i];	
		}		
		*len=USARTStructure4.RX_TMEP_Len;	//��¼�������ݳ���
		USARTStructure4.RX_TMEP_Len=0;		//����
	}
}






