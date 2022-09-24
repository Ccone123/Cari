#include "iicssc.h"
#include "delay.h"
#include "iic.h"
#include "sm5852.h"
#include "lyndir_no.h"
#include "kalmanoperation.h"
flow FLOW;
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
void I2CSSC_IInit(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10| GPIO_Pin_11;//pb6,pb.7
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}
/**************************************************************************
void TWI_delay(void)
***************************************************************************/
void TWI_delaySSC(void)
{ 
   uint8_t i=30; 
   while(i--);    
}
/**************************************************************************
void DelayMs(uint16_t ms)
***************************************************************************/
void DelayMsSSC(uint16_t ms)
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
uint8_t TWI_StartSSC(void)
{
		
	SDAHSSC;
	SCLHSSC;
	TWI_delaySSC();
	if(!SDAreadSSC)return TWI_BUS_BUSY;
	SDALSSC;
	TWI_delaySSC();
	SCLLSSC;
	TWI_delaySSC();
	return TWI_READY;
}


/**************************************************************************
void TWI_Stop(void)
停止信号
***************************************************************************/
void TWI_StopSSC(void)
{
  SDALSSC;
  SCLLSSC;
	TWI_delaySSC();
	SCLHSSC;
	TWI_delaySSC();
	SDAHSSC;
	TWI_delaySSC();
}
/**************************************************************************
void TWI_Ack(void)
应答信号
***************************************************************************/
void TWI_AckSSC(void)
{
	SCLLSSC;
	TWI_delaySSC();
	SDALSSC;
	TWI_delaySSC();
	SCLHSSC;
	TWI_delaySSC();
	SCLLSSC;
	TWI_delaySSC();
}
/**************************************************************************
void TWI_NoAck(void)
非应答信号
***************************************************************************/
void TWI_NoAckSSC(void)
{
	SCLLSSC;
	TWI_delaySSC();
	SDAHSSC;
	TWI_delaySSC();
	SCLHSSC;
	TWI_delaySSC();
	SCLLSSC;
	TWI_delaySSC();	
}
/**************************************************************************
uint8_t TWI_WaitAck(void)
***************************************************************************/
uint8_t TWI_WaitAckSSC(void)
{
	SCLLSSC;
	TWI_delaySSC();
	SDAHSSC;
	TWI_delaySSC();
	SCLHSSC;
	TWI_delaySSC();
	if(SDAreadSSC)
	{
		SCLLSSC;
		return 0;
	}
	SCLLSSC;
	return 1;
}






/**************************************************************************
void TWI_SendByte(uint8_t SendByte)
发送一个字节的数据
***************************************************************************/
void TWI_SendByteSSC(uint8_t SendByte)
{
  uint8_t i=8;
	while(i--)
	{
		SCLLSSC;
		TWI_delaySSC();
		if(SendByte&0x80)
			   SDAHSSC;
		else 
			   SDALSSC;
	   	SendByte<<=1;
		  TWI_delaySSC();
	   	SCLHSSC;
		  TWI_delaySSC();
	}
     SCLLSSC;
}


/**************************************************************************
uint8_t TWI_ReceiveByte(void)
接收一个字节的数据
***************************************************************************/	
uint8_t TWI_ReceiveByteSSC(void)
{
  uint8_t i=8;
	uint8_t ReceiveByte=0;
	SDAHSSC;
	while(i--)
	{
		ReceiveByte<<=1;
		SCLLSSC;
		TWI_delaySSC();
		SCLHSSC;
		TWI_delaySSC();
		if(SDAreadSSC)
		{
			ReceiveByte|=0x01;
		}
	}
	SCLLSSC;
	return ReceiveByte;	
}
uint8_t Read_PressureSSC_Experment (void)
{
	       uint8_t i;
					DelayMsSSC(5);	
		     i=  TWI_StartSSC();
			    if(i)
		        return i;	 
			   TWI_SendByteSSC(0xbe);
		   	if(!TWI_WaitAckSSC())              //等待从机回应SDA
					{
						TWI_StopSSC();
						return 0;
					}  
				  TWI_SendByteSSC(0x80);					
          TWI_WaitAckSSC();
           TWI_delaySSC();				
						TWI_StartSSC(); 
					TWI_SendByteSSC(0xbf);
			  	 TWI_WaitAckSSC();  			
          SM.Uncorrected_Pressure_LSB=TWI_ReceiveByteSSC();				
	  	     TWI_NoAckSSC();
				   TWI_StopSSC();		
      	DelayMsSSC(5);
		  i=  TWI_StartSSC();
			    if(i)
		        return i;	 
			   TWI_SendByteSSC(0xbe);
		   	if(!TWI_WaitAckSSC())              //等待从机回应SDA
					{
						TWI_StopSSC();
						return 0;
					}  
				  TWI_SendByteSSC(0x81);					
          TWI_WaitAckSSC();
           TWI_delaySSC();				
						TWI_StartSSC(); 
					TWI_SendByteSSC(0xbf);
			  	 TWI_WaitAckSSC();  			
          SM.Uncorrected_Pressure_MSB=TWI_ReceiveByteSSC();		
	  	     TWI_NoAckSSC();
				   TWI_StopSSC();							
	       SM.Uncorrected_Pressure = (( SM.Uncorrected_Pressure_MSB&0x3F)<<6)|( SM.Uncorrected_Pressure_LSB&0x3F);
				  SM.Pressure=((float)((( SM.Uncorrected_Pressure_MSB&0x3F)<<6)|( SM.Uncorrected_Pressure_LSB&0x3F))/0xfff)*2.1;
        if(Lyndir_No->PUMP_CONTROL==3)					
			    Lyndir_No->Uncorrected_Pressure=kalman(SM.Pressure);
				else
					 Lyndir_No->Uncorrected_Pressure=0;

}

uint8_t Change_DeviceID(void)
{
 uint8_t i;
					DelayMsSSC(5);	
		     i=  TWI_StartSSC();
			    if(i)
		        return i;	 
			   TWI_SendByteSSC(0xbe);
		   	if(!TWI_WaitAckSSC())              //等待从机回应SDA
					{
						TWI_StopSSC();
						return 0;
					} 
       TWI_SendByteSSC(0x5F);					
          TWI_WaitAckSSC();					
				  TWI_SendByteSSC(0x8F);					
          TWI_WaitAckSSC();
					  TWI_SendByteSSC(0x01);					
          TWI_WaitAckSSC();
							
					
					
					
	  	     TWI_NoAckSSC();
				   TWI_StopSSC();							
}

uint8_t Change_DeviceID1(void)
{
 uint8_t i;
					DelayMsSSC(5);	
		     i=  TWI_StartSSC();
			    if(i)
		        return i;	 
			   TWI_SendByteSSC(0xbe);
		   	if(!TWI_WaitAckSSC())              //等待从机回应SDA
					{
						TWI_StopSSC();
						return 0;
					} 				
			 TWI_SendByteSSC(0x5F);					
          TWI_WaitAckSSC();					
				  TWI_SendByteSSC(0x3F);					
          TWI_WaitAckSSC();
					  TWI_SendByteSSC(0xFF);					
          TWI_WaitAckSSC();		
	         delay_ms(130);
					 TWI_SendByteSSC(0x5F);					
          TWI_WaitAckSSC();					
				  TWI_SendByteSSC(0x8F);					
          TWI_WaitAckSSC();
					  TWI_SendByteSSC(0x00);					
          TWI_WaitAckSSC();
					
			  TWI_SendByteSSC(0x5F);					
          TWI_WaitAckSSC();					
				  TWI_SendByteSSC(0x3F);					
          TWI_WaitAckSSC();
					  TWI_SendByteSSC(0xBB);					
          TWI_WaitAckSSC();							
	  	     TWI_NoAckSSC();
				   TWI_StopSSC();							
}







