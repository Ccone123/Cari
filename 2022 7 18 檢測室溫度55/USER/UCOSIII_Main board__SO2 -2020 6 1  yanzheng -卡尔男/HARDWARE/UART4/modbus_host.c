#include "modbus_host.h"
#include "sys.h"
//#include "mbcrc.h"
#include "delay.h"
u8 RS485_Addr=0x32;//机地址
u8 RS485_FrameFlag=0;//帧结束标志
UsartRxTypeDef4 USARTStructure4;	//串口结构体

Receive Receivedata;
void PtotocolAnalysis (void);
/*********************************************************************
void RS485_Initt(u32 bound)
********************************************************************/
//初始化uart4 modbus从机
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
//	Timer7_Init();//定时器7初始化，用于监视空闲时间
	RS485_TX_EN=0; //使能接收
}
/************************************************************************************
void Timer7_Init(void)
*********************************************************************************/
//定时器7初始化
void Timer7_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//tim7s时钟使能
	
	//tim7初始化设置
	TIM_TimeBaseStructure.TIM_Period = 4*10;
	TIM_TimeBaseStructure.TIM_Prescaler =7200; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); 
	TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );
	//TIM7中断分组配置
	TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
		
}
/****************************************************************************************
void RS485_SendData(u8 *buff ,u8 len)
*********************************************************************************************/
//发送N个字节数据
//buff：发送去首地址
//len:发送的字节数
void RS485_Send_Data(u8 *buf,u8 len)
{
	  u8 t;
	  RS485_TX_EN=1;			//设置为发送模式
  	delay_us(1000);
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		//USART_ClearFlag(UART4, USART_FLAG_TC);
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);//等待为空
		USART_SendData(UART4,buf[t]);
		  	  
	}	 
    while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);	

	   
	 RS485_TX_EN=0;				//设置为接收模式	
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
		   
	 RS485_TX_EN=0;				//设置为接收模式	
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
		 
		 // TIM_SetCounter(TIM2,0);//当接收到一个新的字符,将定时器7复位为0
	   	//TIM_Cmd(TIM2,ENABLE);//开始计时
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

//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=USARTStructure4.RX_TMEP_Len;
	u8 i=0;
	*len=0;				//默认为0
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==USARTStructure4.RX_TMEP_Len&&rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USARTStructure4.RX_TEMP_BUFF[i];	
		}		
		*len=USARTStructure4.RX_TMEP_Len;	//记录本次数据长度
		USARTStructure4.RX_TMEP_Len=0;		//清零
	}
}






