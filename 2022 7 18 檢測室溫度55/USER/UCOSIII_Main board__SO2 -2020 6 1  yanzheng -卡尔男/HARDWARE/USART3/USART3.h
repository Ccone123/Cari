#ifndef __USART3_H_	
#define __USART3_H_
#include "sys.h"

#define USART3BaudRate		9600
//
#define USART3_RX_BUFF_SIZEMAX 64
#define USART3_TX_BUFF_SIZEMAX 64
typedef struct                                                          //
{
  unsigned char RX3_Flag;
	unsigned char RX3_TMEP_Len;
	unsigned char RX3_Len;
	unsigned char RX3_TEMP_BUFF[USART3_RX_BUFF_SIZEMAX];
	char RX3_BUFF[USART3_RX_BUFF_SIZEMAX];                       		//
}UsartRxTypeDef3;
extern UsartRxTypeDef3 USARTStructure3;

void USART3_SendByte(u8 Data);
void USART3_SendString(char * Data,u32 Len);	
void USART3_Config(void);

#endif
