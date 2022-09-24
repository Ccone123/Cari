#ifndef _IIC_H_	
#define _IIC_H_
#include "sys.h"
#define SCLH         GPIOB->BSRR = GPIO_Pin_10
#define SCLL         GPIOB->BRR  = GPIO_Pin_10 
   
#define SDAH         GPIOB->BSRR = GPIO_Pin_11
#define SDAL         GPIOB->BRR  = GPIO_Pin_11

#define SCLread      GPIOB->IDR  & GPIO_Pin_10
#define SDAread      GPIOB->IDR  & GPIO_Pin_11
void I2C_IInit(void);
void TWI_delay(void);
void TWI_delaySS(void);
void DelayMs(uint16_t ms);
uint8_t TWI_Start(void);
void TWI_Stop(void);
void TWI_Ack(void);
void TWI_NoAck(void);
uint8_t TWI_WaitAck(void);
void TWI_SendByte(uint8_t SendByte);
uint8_t TWI_ReceiveByte(void);
uint8_t Read_Pressure (void);
#endif

