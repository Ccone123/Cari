#ifndef _IICEEPROM_H_	
#define _IICEEPROM_H_

#include "sys.h"

#define ADDR_24CXX        0xA0

#define SCLH         GPIOB->BSRR = GPIO_Pin_6
#define SCLL         GPIOB->BRR  = GPIO_Pin_6 
   
#define SDAH         GPIOB->BSRR = GPIO_Pin_7
#define SDAL         GPIOB->BRR  = GPIO_Pin_7

#define SCLread      GPIOB->IDR  & GPIO_Pin_6
#define SDAread      GPIOB->IDR  & GPIO_Pin_7




uint8_t TWI_Start(void);
void DelayMs(uint16_t ms);
void TWI_Stop(void);
void TWI_Ack(void);
void TWI_NoAck(void);
uint8_t TWI_WaitAck(void)  ;
void TWI_SendByte(uint8_t SendByte);
uint8_t TWI_ReceiveByte(void) ;
uint8_t TWI_WriteByte(uint8_t SendByte, uint16_t WriteAddress);
uint8_t TWI_ReadByte( uint16_t ReadAddress);
void I2C_EE_Init(void);

int ReadEpData(unsigned int adss);
int SaveEpData(unsigned int adss, int post);
uint8_t I2C_EE_BufferRead(uint8_t *pdin_data,uint16_t adr,uint8_t nbyte);
uint8_t I2C_EE_BufferWrite(uint8_t *psrc_data,uint16_t adr,uint8_t nbyte);
#endif

