#include "usart4.h"
#include "delay.h"
unsigned char  Rx4_String[USART4_TX_BUFF_SIZEMAX];
unsigned char Rx4_Flag;
UsartRxTypeDef4 USARTStructure4;

Receive Receivedata;
/****************************************************************************
*名称:void USART4_Init(void)
* 功能：串口初始化
* 入口参数：无
* 出口参数：无
* 说明：无
****************************************************************************/
void RS485_Initt(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
  //UART4_TX pc.10
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	//UART4_RX PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	//UART_EN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
		USART_DeInit(UART4);
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);
		USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(UART4, &USART_InitStructure); 
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4, ENABLE);
	RS485_TX_EN=0; //使能接收
}
/****************************************************************************
* 名称:void RS485_Send_Data(u8 *buf,u8 len)
* 功能：发送数据包
* 入口参数：array,len
* 出口参数：无
* 说明：无
****************************************************************************/
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN=1;
	delay_us(1000);
	 for(t=0;t<len;t++)
	{
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);//等待为空
		USART_SendData(UART4,buf[t]);
	}
	 while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET){};		
    RS485_TX_EN=0;	//设置为接收模式		
}
/****************************************************************************
* 名称:void USART4_SendString(u8 Data)
*  功能：发送数据包
* 入口参数：无
* 出口参数: 无			   
****************************************************************************/
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
/****************************************************************************
void UART4_IRQHandler(void) 		   
****************************************************************************/
void UART4_IRQHandler(void)
{
	unsigned char USART4_GetChar;
	if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)
	{
				USART_ClearITPendingBit(UART4, USART_IT_RXNE);	
		USART4_GetChar=USART_ReceiveData(UART4);
	}
	  if(USARTStructure4.RX_TMEP_Len<64)
	 {
		 USARTStructure4.RX_TEMP_BUFF[USARTStructure4.RX_TMEP_Len]=USART4_GetChar;
		 USARTStructure4.RX_TMEP_Len++;
		 
		  //TIM_SetCounter(TIM2,0);//当接收到一个新的字符,将定时器7复位为0
	   	//TIM_Cmd(TIM2,ENABLE);//开始计时
	 }
}



/*******************************************************************************
* Function Name  : RS485_Receive_Data
* Description    : 串口2发送一字节
*******************************************************************************/
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


/*******************************************************************************
* Function Name  : 
* Description    : 字符串发送函数（内部包含lrc校验,0D0A）   开始':' 结尾'\0'
*******************************************************************************/



/*******************************************************************************
* Function Name  : 
* Description    : 中断接收
	
*******************************************************************************/





/*******************************************************************************
* Function Name  : 
* Description    : 字符串接受后校验lrc返回标志位
	
*******************************************************************************/







