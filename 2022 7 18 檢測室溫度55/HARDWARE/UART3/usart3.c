#include "delay.h"
#include "usart3.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"	
#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////
//串口3驱动代码	 
//修改日期:2020/7/7
//版本：V1.0
//Copyright(C) 天瑞仪器
//////////////////////////////////////////////////////////////////////////////////
//串口接收缓存区
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN];
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN];
vu16 USART3_RX_STA=0;

/********************************************************************************	 
	//初始化IO
//bound:波特率
*********************************************************************************/ 
void usart3_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //串口3时钟使能

	USART_DeInit(USART3);//复位串口3
			 //USART3_TX   PB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11
	
	 USART_InitStructure.USART_BaudRate=bound;
	 USART_InitStructure.USART_WordLength=USART_WordLength_8b;//字长为8位数据格式
	 USART_InitStructure.USART_StopBits=USART_StopBits_1;//一个停止位
	 USART_InitStructure.USART_Parity=USART_Parity_No;//无奇偶校验位
	 USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无硬件数据流控制
	 USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;//收发模式
	 USART_Init(USART3, &USART_InitStructure); //初始化串口	3
	
	 USART_Cmd(USART3, ENABLE); 
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断 	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	
	TIM7_Int_Init(99,7199);		//10ms中断
	USART3_RX_STA=0;		//清零
	TIM_Cmd(TIM7,DISABLE);			//关闭定时器7

}
/********************************************************************************	 
//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
*********************************************************************************/ 
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART3,USART3_TX_BUF[j]); 
	} 
}
/********************************************************************************	 
	中断	
*********************************************************************************/ 
void USART3_IRQHandler(void)
{
	u8 res;
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)
	{
		res =USART_ReceiveData(USART3);
		if((USART3_RX_STA&(1<<15))==0)
		{
			 if(USART3_RX_STA<USART3_MAX_RECV_LEN)
			 {
				 TIM_SetCounter(TIM7,0);
				 if(USART3_RX_STA==0)
				 {
					 TIM_Cmd(TIM7,ENABLE);
				 }
				 USART3_RX_BUF[USART3_RX_STA++]=res;
			 }else
			 {
				 USART3_RX_STA|=1<<15;
			 }
	  }
   }
}


/****************************************************************************
void USART3_SendByte(u8 Data)  
发送字节
****************************************************************************/
void USART3_SendByte(u8 Data)		   //???????
{
	    USART_SendData(USART3, Data);
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}

/****************************************************************************
void USART3_SendString(char * Data,u32 Len)	
发送字符串
****************************************************************************/
void USART3_SendString(char * Data,u32 Len)		   //?????
{
	u32 i=0;

	for(i=0;i<Len;i++)
    {    
		USART_SendData(USART3, Data[i]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	}

}



