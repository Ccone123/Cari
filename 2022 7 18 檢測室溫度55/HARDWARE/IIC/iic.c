#include "iic.h"
#include "delay.h"
#include "sm5852.h"
#include "lyndir_no.h"
#include "kalmanoperation.h"
enum ENUM_TWI_REPLY
{
	TWI_NACK=0
	,TWI_ACK=1
};
enum ENUM_TWI_BUS_STATE
{
		TWI_READY=0
	,TWI_BUS_BUSY=1
	,TWI_BUS_ERROR=2
};
/**************************************************************************
FUNCTION:VOID I2C_IInit(VOID)
初始化
***************************************************************************/
void I2C_IInit(void)
{
		 GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;//pb6,pb.7
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	 GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/**************************************************************************
void TWI_delay(void)
***************************************************************************/
void TWI_delay(void)
{ 
   uint8_t i=10; 
   while(i--);    
}
void TWI_delaySS(void)
{ 
   uint8_t i=10; 
   while(i--);    
}
void TWI_delay1(void)
{ 
   uint8_t i=10; 
   while(i--);    
}
/**************************************************************************
void DelayMs(uint16_t ms)
***************************************************************************/
void DelayMs(uint16_t ms)
{
	uint16_t iq0;
	uint16_t iq1;
	for(iq0 = ms; iq0 > 0; iq0--)
	{
		for(iq1 = 11998; iq1 > 0; iq1--); // ( (6*iq1+9)*iq0+15 ) / 72000000 
  
	}
}
/**************************************************************************
uint8_t TWI_Start(void)
起始信号
***************************************************************************/
uint8_t TWI_Start(void)
{
	SDAH;
	SCLH;
	TWI_delay();
	if(!SDAread)return TWI_BUS_BUSY;
	SDAL;
	TWI_delay();
	SCLL;
	TWI_delay();
	return TWI_READY;
}
/**************************************************************************
void TWI_Stop(void)
停止信号
***************************************************************************/
void TWI_Stop(void)
{
  SDAL;
  SCLL;
	TWI_delay();
	SCLH;
	TWI_delay();
	SDAH;
	TWI_delay();
}
/**************************************************************************
void TWI_Ack(void)
应答信号
***************************************************************************/
void TWI_Ack(void)
{
	SCLL;
	TWI_delay();
	SDAL;
	TWI_delay();
	SCLH;
	TWI_delay();
	SCLL;
	TWI_delay();
}
void TWI_Ack1(void)
{
	SCLL;
	TWI_delay1();
	SDAL;
	TWI_delay1();
	SCLH;
	TWI_delay1();
	SCLL;
	TWI_delay1();
}
/**************************************************************************
void TWI_NoAck(void)
非应答信号
***************************************************************************/
void TWI_NoAck(void)
{
	SCLL;
	TWI_delay();
	SDAH;
	TWI_delay();
	SCLH;
	TWI_delay();
	SCLL;
	TWI_delay();	
}
/**************************************************************************
uint8_t TWI_WaitAck(void)
等从机回复信号
***************************************************************************/
uint8_t TWI_WaitAck(void)
{
	SCLL;
	TWI_delay();
	SDAH;
	TWI_delay();
	SCLH;
	TWI_delay();
	if(SDAread)
	{
		SCLL;
		return 0;
	}
	SCLL;
	return 1;
}
/**************************************************************************
void TWI_SendByte(uint8_t SendByte)
发送一个字节的数据
***************************************************************************/
void TWI_SendByte(uint8_t SendByte)
{
  uint8_t i=8;
	while(i--)
	{
		SCLL;
		TWI_delay();
		if(SendByte&0x80)
			   SDAH;
		else 
			   SDAL;
	   	SendByte<<=1;
		  TWI_delay();
	   	SCLH;
		  TWI_delay();
	}
  SCLL;
}
/**************************************************************************
uint8_t TWI_ReceiveByte(void)
接收一个字节的数据
***************************************************************************/	
uint8_t TWI_ReceiveByte(void)
{
  uint8_t i=8;
	uint8_t ReceiveByte=0;
	SDAH;
	while(i--)
	{
		ReceiveByte<<=1;
		SCLL;
		TWI_delay();
		SCLH;
		TWI_delay();
		if(SDAread)
		{
			ReceiveByte|=0x01;
		}
		
	}
	SCLL;
	return ReceiveByte;	
}


/**************************************************************************
IIC协议给SM5852
***************************************************************************/	
uint8_t Read_Pressure (void)
{
  uint8_t i;

					
		     i=  TWI_Start();
			    if(i)
		        return i;						
			   TWI_SendByte(0xbe);
		   	if(!TWI_WaitAck())              //等待从机回应SDA
					{
						TWI_Stop();
						return 0;
					}  
				  TWI_SendByte(0x80);					
          TWI_WaitAck();
           TWI_delaySS();				
						TWI_Start(); 
					TWI_SendByte(0xbf);
			  	 TWI_WaitAck();  			
         SM.Uncorrected_Pressure_LSB=TWI_ReceiveByte();		
           TWI_Ack();
         SM.Uncorrected_Pressure_MSB=TWI_ReceiveByte();						
	  	     TWI_NoAck();
					 TWI_Ack();
				   TWI_Stop();		
//					delay_ms(5);					
		     i= TWI_Start();
			    if(i)
		        return i;	 
			    TWI_SendByte(0xbe);
		   	if(!TWI_WaitAck())              //等待从机回应SDA
					{
						TWI_Stop();
						return 0;
					}  
				  TWI_SendByte(0x81);					
          TWI_WaitAck();
           TWI_delaySS();				
						TWI_Start(); 
					TWI_SendByte(0xbf);
			  	 TWI_WaitAck();  			
           SM.Uncorrected_Pressure_MSB=TWI_ReceiveByte();		
	  	     TWI_NoAck();
				   TWI_Stop();			
				SM.Uncorrected_Pressure = (( SM.Uncorrected_Pressure_MSB&0x3F)<<6)|( SM.Uncorrected_Pressure_LSB&0x3F);
				  SM.Pressure=((float)((( SM.Uncorrected_Pressure_MSB&0x3F)<<6)|( SM.Uncorrected_Pressure_LSB&0x3F))/0xfff)*2.1;
			
					return 0;			
	
	
}








