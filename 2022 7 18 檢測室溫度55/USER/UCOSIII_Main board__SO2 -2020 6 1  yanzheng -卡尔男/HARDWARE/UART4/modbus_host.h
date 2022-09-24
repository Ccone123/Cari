#ifndef _MODBUS_HOST_H
#define _MODBUS_HOST_H
#include "sys.h"
#define RS485_TX_EN		PCout(9)	//485模式控制.0,接收;1,发送.
#define USART2BaudRate		9600		
#define USART4_RX_BUFF_SIZEMAX 64
#define USART4_TX_BUFF_SIZEMAX 64
typedef struct                                                          //
{
 unsigned char RX_Flag;                                                 // 
 unsigned char RX_TMEP_Len;  
 unsigned char RX_Len;                                                  //  
 unsigned char RX_TEMP_BUFF[USART4_RX_BUFF_SIZEMAX];                    //????
 char RX_BUFF[USART4_RX_BUFF_SIZEMAX];                     		//????
}
UsartRxTypeDef4;
extern UsartRxTypeDef4 USARTStructure4;
typedef struct 
{
   // u8 *Modbus_HoldReg[100];	
	  u8 tempdata[12];
} Receive;
extern Receive Receivedata;

void Timer7_Init(void);
void RS485_Initt(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);
void USART4_SendString(char * Data,u32 Len)	;
#endif
