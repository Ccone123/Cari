#ifndef _IICSSC_H_	
#define _IICSSC_H_
#include "sys.h"
#define Zero_Point   0
#define Zero_Flow    1.04
#define SCLHSSC         GPIOB->BSRR = GPIO_Pin_10
#define SCLLSSC         GPIOB->BRR  = GPIO_Pin_10
   
#define SDAHSSC         GPIOB->BSRR = GPIO_Pin_11
#define SDALSSC         GPIOB->BRR  = GPIO_Pin_11

#define SCLreadSSC      GPIOB->IDR  & GPIO_Pin_10
#define SDAreadSSC      GPIOB->IDR  & GPIO_Pin_11

#pragma pack(push)
#pragma pack(1) 
typedef struct {
         float K;           
	       float b;    
}flow;
#pragma apop
extern flow FLOW;
void I2CSSC_IInit(void);
void TWI_delaySSC(void);
void DelayMsSSC(uint16_t ms);
uint8_t TWI_StartSSC(void);
void TWI_StopSSC(void);
void TWI_AckSSC(void);
void TWI_NoAckSSC(void);
uint8_t TWI_WaitAckSSC(void);
void TWI_SendByteSSC(uint8_t SendByte);
uint8_t TWI_ReceiveByteSSC(void);
uint8_t Change_DeviceID(void);

uint8_t Change_DeviceID1(void);
uint8_t Read_PressureSSC_Experment (void);
#endif

