#include "IICEEPROM.h"
#include "mbcrc.h"
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

void I2C_EE_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure; 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}
void TWI_delay(void)
{ 
   uint8_t i=10; 
   while(i--); 
   
}
/**************************************************************************
CYCLECOUNTER / 72000000 
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
//起始信号
uint8_t TWI_Start(void)  //起始信号。
{
	SDAH;
	SCLH;
	TWI_delay();
	if(!SDAread)return TWI_BUS_BUSY; 
	SDAL;
	TWI_delay();
	if(SDAread) return TWI_BUS_ERROR;
	SCLL;
	TWI_delay();
	return TWI_READY;
}
//***********************************************
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

//*************************

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
//************************
uint8_t TWI_ReceiveByte(void)  
{ 
	uint8_t i=8;
	uint8_t ReceiveByte=0;

	SDAH;    
	while(i--)
	{
		ReceiveByte <<= 1;      
		SCLL;
		TWI_delay();
		SCLH;
		TWI_delay(); 
		if(SDAread)
		{
			ReceiveByte |= 0x01;
		}
	}
	SCLL;
	return ReceiveByte;
}
uint8_t TWI_WriteByte(uint8_t SendByte, uint16_t WriteAddress)
{  
	uint8_t i;

	i = TWI_Start();
	if(i)
	return i;
	 
	TWI_SendByte( ADDR_24CXX & 0xFE);

	if(!TWI_WaitAck())
	{
	 TWI_Stop(); 
	 return 0;
	}

	TWI_SendByte(WriteAddress>>8);   //???   
	TWI_WaitAck(); 
	TWI_SendByte(WriteAddress);      //???
	TWI_WaitAck(); 
	TWI_SendByte(SendByte);       
	TWI_WaitAck();   
	TWI_Stop(); 
	return 3;
}

//*****************************************
uint8_t TWI_ReadByte( uint16_t ReadAddress)
{  
	uint8_t i,temp;
	i = TWI_Start();
	if(i)
	return i;
	 
	TWI_SendByte((ADDR_24CXX & 0xFE));
	if(!TWI_WaitAck())
	{
		TWI_Stop(); 
		return 0;
	}

	TWI_SendByte(ReadAddress>>8);        
	TWI_WaitAck();
	TWI_SendByte(ReadAddress);        
	TWI_WaitAck();
	TWI_Start();
	TWI_SendByte((ADDR_24CXX & 0xFE)|0x01);    
	TWI_WaitAck();

	//*pDat = TWI_ReceiveByte();
	temp = TWI_ReceiveByte();

	TWI_NoAck();

	TWI_Stop();
	return temp;
}


uint8_t I2C_EE_BufferWrite(uint8_t *psrc_data,uint16_t adr,uint8_t nbyte)
{
	uint8_t i;
	DelayMs(1);
	
	i =TWI_Start();
	if(i)
		return i;
	TWI_SendByte(ADDR_24CXX&0xFE);
	
	if(!TWI_WaitAck())
	{
		TWI_Stop();
		return 0;
	}
  TWI_SendByte(adr>>8);
	TWI_WaitAck();
	TWI_SendByte(adr);
	TWI_WaitAck();
	for(;nbyte!=0;nbyte--)
	   {
			 TWI_SendByte(*psrc_data);
			 TWI_WaitAck();
			 psrc_data++;
		 }
		TWI_Stop();
		DelayMs(20);  	
		return 0;
	
}
/***************************************************************************

***************************************************************************/
uint8_t I2C_EE_BufferRead(uint8_t *pdin_data,uint16_t adr,uint8_t nbyte)
{
	uint8_t i;
	i=TWI_Start();
	if(i)
		return i;
	TWI_SendByte((ADDR_24CXX & 0xFE));
	if(!TWI_WaitAck())
	{
		TWI_Stop();
		return 0;	
	}
	TWI_SendByte(adr>>8);
	TWI_WaitAck();
	TWI_SendByte(adr);
	TWI_WaitAck();
	TWI_Start();
	TWI_SendByte((ADDR_24CXX &0xFE)|0x01);
	TWI_WaitAck();
	 
	while(nbyte!=1)
	{
		*pdin_data =TWI_ReceiveByte();
		TWI_Ack();
		pdin_data++;
		nbyte--;	
	}
	*pdin_data=TWI_ReceiveByte();
	TWI_NoAck();
	TWI_Stop();
	return 0;
}
int SaveEpData(unsigned int adss, int post)
{
	unsigned char temp[10]={0};
	unsigned char rxtemp[10]={0};
	unsigned char i=0;
	unsigned short crc=0;
	unsigned char errorflag=0;
	if(post !=0)
	{
	temp[0]=adss>>8;
	temp[1]=adss;
	
	temp[2]=(post&0xff000000)>>24;
	temp[3]=(post&0x00ff0000)>>16;
	temp[4]=(post&0x0000ff00)>>8;
	temp[5]=(post&0x000000ff);
	
	crc=usMBCRC16(&temp[0],6);
	temp[6]=crc>>8;
	temp[7]=crc;
	I2C_EE_BufferWrite(&temp[0],adss,8);
	
	I2C_EE_BufferRead(&rxtemp[0],adss,8);
	errorflag=0;
	for(i=0;i<8;i++)
	{
		if(temp[i]!=rxtemp[i])
		{errorflag=1;}
	}
	if(errorflag==1)
	{
		I2C_EE_BufferWrite(&temp[0],adss,8);
		I2C_EE_BufferRead(&rxtemp[0],adss,8);
		for(i=0;i<8;i++)
		{
			if(temp[i]!=rxtemp[i])
			{return 1;}
		}
	}
	else 
	{
		return 0;
	}		
}
	else
	{
		return 2;
	}
	return 0;
}


int ReadEpData(unsigned int adss)
{
	unsigned char temp[10];
	int position;
	unsigned short Save_crc=0,Calc_crc=0;
	I2C_EE_BufferRead(&temp[0],adss,8);
	Save_crc=(temp[6]<<8)|temp[7];
	Calc_crc=usMBCRC16(&temp[0],6);
	if(Save_crc==Calc_crc)
	{
		position =((temp[2]<<24)|(temp[3]<<16)|(temp[4]<<8)|temp[5]);
		return position;
	}
	else
		return 0xf0000000;
	
}


