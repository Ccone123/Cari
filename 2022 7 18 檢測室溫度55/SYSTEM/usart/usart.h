#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
#define USART1BaudRate		115200	   			  												
#define USART1_RX_BUFF_SIZEMAX 100
#define USART1_TX_BUFF_SIZEMAX 100

typedef struct                                                      
{
 unsigned char RX_Flag;                                                
 unsigned char RX_TMEP_Len;   
 unsigned char RX_Len;                                                  
 unsigned char RX_TEMP_BUFF[USART1_RX_BUFF_SIZEMAX];                                        	
}
UsartRxTypeDef1;
extern UsartRxTypeDef1 USARTStructure1;	   	
void USART1_Config(void);		  								
void USART1_RX_Buffer_Clear(void); 						
void USART1_SendByte(u8 Data);								
void USART1_SendString(u8* Data,u32 Len);				
#endif


