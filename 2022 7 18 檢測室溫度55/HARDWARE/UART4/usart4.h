#ifndef _USART4_H_	
#define _USART4_H_
#include "sys.h"  
#define RS485_TX_EN		PCout(12)	//485模式控制.0,接收;1,发送.
#define USART4BaudRate 115200 
//接收缓冲区容量
#define USART4_RX_BUFF_SIZEMAX 100
#define USART4_TX_BUFF_SIZEMAX 100

typedef struct
{
	unsigned char RX_Flag;
	unsigned char RX_Len;
	unsigned char RX_TMEP_Len; 
	unsigned char RX_TEMP_BUFF[USART4_RX_BUFF_SIZEMAX];  
	         char RX_BUFF[USART4_RX_BUFF_SIZEMAX];
}UsartRxTypeDef4;
extern UsartRxTypeDef4 USARTStructure4;
typedef struct
{
	u8 tempdata[12];
}Receive;
extern Receive Receivedata;


void RS485_Initt(u32 bound);			
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);
void USART4_SendString(char * Data,u32 Len)	;


#endif
