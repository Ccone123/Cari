#include "USART3.h"
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os 使用	  
#endif
#include "cmd_queue.h"

UsartRxTypeDef3 USARTStructure3;
unsigned char USART3_TX_BUFF[USART3_TX_BUFF_SIZEMAX];  
unsigned char USART3_flag=0;
/**********************************************************************
* name :void USART3_GPIO_Init(void)
*****************************************************************************/
void USART3_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;//串口发送引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;					     //配置PB.11为接受引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//
}
/**********************************************************************
   名称：void USART3_Init(void)
********************************************************************************/
void USART3_Init(void)
{
		USART_InitTypeDef UART_InitStructure; //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	  UART_InitStructure.USART_BaudRate = USART3BaudRate;
	  UART_InitStructure.USART_WordLength=USART_WordLength_8b;
    UART_InitStructure.USART_StopBits=USART_StopBits_1;
	  UART_InitStructure.USART_Parity = USART_Parity_No ;
    UART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
    UART_InitStructure.USART_Mode= USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &UART_InitStructure);//initial serial interface
}
/*******************************************************************************************
  void USART3 NVIC_Init(void)
***********************************************************************************/
void USART3_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;//中断控制器变量
	NVIC_InitStructure.NVIC_IRQChannel =USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  NVIC_Init(&NVIC_InitStructure);			
}
/******************************************************************************************
void USART3_RX_Buffer_Clear(void)
****************************************************************************************/
void USART3_RX_Buffer_Clear(void)	
{

	USARTStructure3.RX3_Len = 0;
		
}
/*************************************************************************************
 void USART3_Config(void)
*********************************************************************************/
void USART3_Config(void)
{
	USART3_Init();
	USART3_GPIO_Init();
	//USART3_GPIO_Init();
	USART3_NVIC_Init();
	USART3_RX_Buffer_Clear();	
	USART_ClearITPendingBit(USART3, USART_IT_RXNE);	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
	USART_Cmd(USART3, ENABLE);
}
/****************************************************************************
void USART3_SendByte(u8 Data)		   
****************************************************************************/
void USART3_SendByte(u8 Data)
{
	USART_SendData(USART3, Data);
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
//		USART_ClearFlag(USART3, USART_FLAG_TC);
//	USART_SendData(USART3, Data);
//  while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
	
}
/****************************************************************************
:void USART3_SendString(u8* Data,u32 Len)		 
****************************************************************************/
void USART3_SendString(char * Data,u32 Len)	
{
	u32 i=0;
	for(i=0;i<Len;i++)
	{
		USART_SendData(USART3,Data[i]);
		  while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	}
}
/***********************************************************************************
void USART3_IRQHandler(void)
****************************************************************************************/
void USART3_IRQHandler(void)
{
	//unsigned char i; 2020 4 21
	unsigned char USART3_Getchar;
	#ifdef SYSTEM_SUPPORT_OS	 	
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3,USART_IT_RXNE) !=RESET)
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		USART3_Getchar =USART_ReceiveData(USART3);
		 queue_push(USART3_Getchar);
	
		
      
	}
		
		
		
		

	#ifdef SYSTEM_SUPPORT_OS	 
	OSIntExit();  											 
#endif
	
}






