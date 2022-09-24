#include "usart4.h"
#include "delay.h"
unsigned char  Rx4_String[USART4_TX_BUFF_SIZEMAX];
unsigned char Rx4_Flag;
UsartRxTypeDef4 USARTStructure4;

Receive Receivedata;
/****************************************************************************
*����:void USART4_Init(void)
* ���ܣ����ڳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵������
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
	RS485_TX_EN=0; //ʹ�ܽ���
}
/****************************************************************************
* ����:void RS485_Send_Data(u8 *buf,u8 len)
* ���ܣ��������ݰ�
* ��ڲ�����array,len
* ���ڲ�������
* ˵������
****************************************************************************/
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN=1;
	delay_us(1000);
	 for(t=0;t<len;t++)
	{
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);//�ȴ�Ϊ��
		USART_SendData(UART4,buf[t]);
	}
	 while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET){};		
    RS485_TX_EN=0;	//����Ϊ����ģʽ		
}
/****************************************************************************
* ����:void USART4_SendString(u8 Data)
*  ���ܣ��������ݰ�
* ��ڲ�������
* ���ڲ���: ��			   
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
	 RS485_TX_EN=0;				//����Ϊ����ģʽ	
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
		 
		  //TIM_SetCounter(TIM2,0);//�����յ�һ���µ��ַ�,����ʱ��7��λΪ0
	   	//TIM_Cmd(TIM2,ENABLE);//��ʼ��ʱ
	 }
}



/*******************************************************************************
* Function Name  : RS485_Receive_Data
* Description    : ����2����һ�ֽ�
*******************************************************************************/
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


/*******************************************************************************
* Function Name  : 
* Description    : �ַ������ͺ������ڲ�����lrcУ��,0D0A��   ��ʼ':' ��β'\0'
*******************************************************************************/



/*******************************************************************************
* Function Name  : 
* Description    : �жϽ���
	
*******************************************************************************/





/*******************************************************************************
* Function Name  : 
* Description    : �ַ������ܺ�У��lrc���ر�־λ
	
*******************************************************************************/







