#include "sys.h"
#include "usart.h"	 
#include <stdio.h>
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os 使用	  
#endif
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
// /* 
//int fputc(int ch, FILE *f)
//{
//	 USART_ClearFlag(USART1,USART_FLAG_TC);
//	
//	USART_SendData(USART1, (uint8_t) ch);

//	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
//   
//    return ch;
//}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
//*/
 UsartRxTypeDef1 USARTStructure1;//UART1->数据结构
/****************************************************************************
    :void USART1_GPIO_Init(void)
***************************************************************/
void USART1_GPIO_Init(void)						//???????
{
	GPIO_InitTypeDef GPIO_InitStructure;		//??????
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
  	GPIO_Init(GPIOA, &GPIO_InitStructure);						
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		
  	GPIO_Init(GPIOA, &GPIO_InitStructure);					
}
/****************************************************************************
void USART1_Init(void)
****************************************************************************/
void USART1_Init(void)
{		
	USART_InitTypeDef UART_InitStructure;		//????

	//??????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//?????
	UART_InitStructure.USART_BaudRate            = 115200;					//boundrate->115200
	UART_InitStructure.USART_WordLength          = USART_WordLength_8b;				//databit=8
	UART_InitStructure.USART_StopBits            = USART_StopBits_1;				//stop=1
	UART_InitStructure.USART_Parity              = USART_Parity_No ;				//none
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//none
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	// mode->send/receive
	USART_Init(USART1, &UART_InitStructure);										//uart->init
}
/****************************************************************************
:void USART1_NVIC_Init(void)

****************************************************************************/
void USART1_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 						//
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//
	NVIC_Init(&NVIC_InitStructure);								//
}

/****************************************************************************
void USART1_RX_Buffer_init(void)
****************************************************************************/
void USART1_RX_Buffer_init(void)
{
	unsigned char i;

		USARTStructure1.RX_Flag = 0;
		USARTStructure1.RX_Len = 0;
		USARTStructure1.RX_TMEP_Len = 0;
		for(i=0; i<60; i++)
		{
//			USARTStructure1.RX_BUFF[i] = 0x00;
			USARTStructure1.RX_TEMP_BUFF[i] = 0x00;
		}
}
/****************************************************************************
void USART1_Config(void)
****************************************************************************/
void USART1_Config(void)
{
	USART1_Init();				
	USART1_GPIO_Init();			
	USART1_NVIC_Init();			
	USART1_RX_Buffer_init();	
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);				
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		
USART_ClearFlag(USART1,USART_FLAG_TC);	
	USART_Cmd(USART1, ENABLE);  								
}
/****************************************************************************
void USART1_SendByte(u8 Data)
****************************************************************************/
void USART1_SendByte(u8 Data)		   //???????
{
	USART_SendData(USART1, Data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
/****************************************************************************
 void USART1_SendString(u8* Data,u32 Len)
****************************************************************************/
void USART1_SendString(u8* Data,u32 Len)		   //?????
{
	u32 i=0;
	for(i=0;i<Len;i++)
  {    
		    USART_SendData(USART1, Data[i]);
		     //USART1->SR=0;//
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}
/****************************************************************************
void USART1_IRQHandler(void)
****************************************************************************/
void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	 unsigned char USART1_GetChar;
#ifdef SYSTEM_SUPPORT_OS	 	
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
  		 		USART_ClearITPendingBit(USART1, USART_IT_RXNE);							//????????
		USART1_GetChar = USART_ReceiveData(USART1);									//????????
     if(USART1_GetChar=='\b'){
					if(USARTStructure1.RX_TMEP_Len>0){
					USART1_SendByte('\b');
					USART1_SendByte(' ');
					USART1_SendByte('\b');
					USARTStructure1.RX_TMEP_Len=USARTStructure1.RX_TMEP_Len-1;
					 }
				   }
		else if(USART1_GetChar=='\n')                                     //???LF???(\n)
							{
							}
							
		else if(USART1_GetChar=='\r')                                //???CR???(\r)
			{
				USARTStructure1.RX_TEMP_BUFF[USARTStructure1.RX_TMEP_Len] ='\0';
				 USART1_SendByte('\r');
				 USART1_SendByte('\n');
					//USARTStructure1.RX_TMEP_Len = 0;
 					USARTStructure1.RX_Flag=1; 
				}
		else
		{
			if( USARTStructure1.RX_TMEP_Len < (sizeof(USARTStructure1.RX_TEMP_BUFF)-1) )
			{
			   USARTStructure1.RX_TEMP_BUFF[USARTStructure1.RX_TMEP_Len] = USART1_GetChar;
			   USARTStructure1.RX_TMEP_Len++; 
			}
			   USART1_SendByte(USART1_GetChar);
		}
     } 
#ifdef SYSTEM_SUPPORT_OS	 
	OSIntExit();  											 
#endif
} 


