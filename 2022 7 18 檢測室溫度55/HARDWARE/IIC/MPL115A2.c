#include "MPL115A2.h"
#include "iic.h"
#include "math.h"
#include "delay.h"
#include "lyndir_no.h"
#include "kalmanoperation.h"
mpl MPL;
/***********************************************************
uint8_t Read_coefficients(void)
��ȡϵ��
************************************************************/
uint8_t Read_coefficients(mpl temp)
{
	uint8_t i;
	i=TWI_Start();
	if(i)
		return i;	
	TWI_SendByte(Device_Address_W); //д������
	if(!TWI_WaitAck())              //�ȴ��ӻ���ӦSDA
	{
		TWI_Stop();
		return 0;
	}
	TWI_SendByte(Command_To_Coefficient);//Command to Read"Coefficient data byte 1 HIgh byte"
		TWI_WaitAck();
		TWI_Start();                      //���·�����ʼλ����restart      
	 TWI_SendByte(Device_Address_R);    //�����ϵ��
	  TWI_WaitAck();
	 MPL.a0MSB=TWI_ReceiveByte();
	    TWI_Ack();
	 MPL.a0LSB=TWI_ReceiveByte();
	    TWI_Ack();
	 MPL.b1MSB=TWI_ReceiveByte();
	    TWI_Ack();
	 MPL.b1LSB=TWI_ReceiveByte();
	    TWI_Ack();
	 MPL.b2MSB=TWI_ReceiveByte();
	    TWI_Ack();
	 MPL.b2LSB=TWI_ReceiveByte();
	    TWI_Ack();
	 MPL.c12MSB=TWI_ReceiveByte();
	    TWI_Ack();
	 MPL.c12LSB=TWI_ReceiveByte();
	    TWI_NoAck();
		   TWI_Stop();   //ֹͣ
	  //  MPL=temp;      //�ṹ�帳ֵ
       return 1;
}
/***********************************************************
u8 Start_Conversion(void) 
��ʼת������
************************************************************/
u8 Start_Conversion(void) 
{
	uint8_t i;
	i=TWI_Start();
	if(i)
		return i;
	TWI_SendByte(Device_Address_W);  //д����
		if(!TWI_WaitAck())
		{
		 TWI_Stop(); 
		 return 0;
		}
	 TWI_SendByte(Command_To_Convert);
		 TWI_WaitAck();
	  TWI_SendByte(0x00);
		TWI_WaitAck();
		TWI_Stop(); 
	 return 0;	
}
/***********************************************************
u8 Start_Conversion(void) 
ת�����
************************************************************/
u8 Read_Result(mpl temp)
{
		uint8_t i;
	 
	i=TWI_Start();
	if(i)
		return i;
	
	TWI_SendByte(Device_Address_W);//д�Ĵ���
		if(!TWI_WaitAck())
	{
		TWI_Stop(); 
		return 0;
	}
	TWI_SendByte(Command_To_Data);//д���ݼĴ���
	TWI_WaitAck();
	TWI_Start();                    //���·�����ʼλ����restart   
	TWI_SendByte(Device_Address_R);//׼����ʼ������
	TWI_WaitAck();
	MPL.PressureMSB=TWI_ReceiveByte();
	TWI_Ack();
	MPL.PressureLSB=TWI_ReceiveByte();
		TWI_Ack();
	MPL.TemperatureMSB=TWI_ReceiveByte();
		TWI_Ack();
	MPL.TemperatureLSB=TWI_ReceiveByte();
	  TWI_NoAck();
		TWI_Stop();
  	return 1;
}
/***********************************************************
float Coefficient_Data_Precess_a0(mpl temp) 
a0ϵ��
************************************************************/
float Coefficient_Data_Precess_a0(mpl temp)
{
	char i;
	signed short int Data[2];
	float Conver_Result[2];
	MPL.a0=(MPL.a0MSB<<8)|MPL.a0LSB;
	if(MPL.a0&0x8000)
	{
	  Data[0]=~MPL.a0+1;
		Data[1]=(~MPL.a0+1)>>3;
	 for(i=1;i<=3;i++)
		 {
			Conver_Result[0]+= ((float)((Data[0]>>(3-i))&0x0001))*pow(2,0-i); 
		 }
	   	Conver_Result[1]=0-Data[1]-Conver_Result[0]; 
	}
	else
		{
			    Data[0]=MPL.a0;
					Data[1]=MPL.a0>>3;
			    for(i=1;i<=3;i++)
					{
						Conver_Result[0]+= ((float)((Data[0]>>(3-i))&0x0001))*pow(2,0-i); 
					}
			 Conver_Result[1]=Data[1]+Conver_Result[0];		
	  }
	
	
	  return 	Conver_Result[1];	
}	
/***********************************************************
float Coefficient_Data_Precess_b1(mpl temp) 
b1ϵ��
************************************************************/
float Coefficient_Data_Precess_b1(mpl temp)
{
	   char i;
	   signed short int Data[2];
	   float Conver_Result[2];
     MPL.b1=(MPL.b1MSB<<8)|(MPL.b1LSB);    //�ϲ�
      if(MPL.b1&0x8000)
        {
					Data[0]= ~MPL.b1+1;
					Data[1]=(~MPL.b1+1)>>13;
			
		for(i=1;i<=13;i++)		
			  {
     		Conver_Result[0]+= ((float)((Data[0]>>(13-i))&0x0001))*pow(2,0-i);
		    }		
				Conver_Result[1]=0-Data[1]-Conver_Result[0];  //���ս��				
			}	
	else 
				{
					Data[0]=MPL.b1;
					Data[1]=MPL.b1>>13;
					for(i=1;i<=13;i++)		
			  {
     		  Conver_Result[0]+= ((float)((Data[0]>>(13-i))&0x0001))*pow(2,0-i);
		    }	
				
				   Conver_Result[1]=Data[1]+Conver_Result[0];						
				}		
      return 	Conver_Result[1];	
}
/***********************************************************
float Coefficient_Data_Precess_b2(mpl temp) 
b2ϵ��
************************************************************/  
float Coefficient_Data_Precess_b2(mpl temp)
{
	   char i;
	   signed short int Data[2];
	   float Conver_Result[2];
	   MPL.b2=(MPL.b2MSB<<8)|MPL.b2LSB;
	    if(MPL.b2&0x8000)
			{
				Data[0]=~MPL.b2+1;
				Data[1]=(~MPL.b2+1)>>14;
			
			for(i=1;i<=14;i++)
			   {
					 Conver_Result[0]+= ((float)((Data[0]>>(14-i))&0x0001))*pow(2,0-i);
				 }
		   	Conver_Result[1]=0-Data[1]-Conver_Result[0];  //���ս��				 
     	}
	  else
			  {
			    Data[0]=MPL.b2;
					Data[1]=MPL.b2>>14;
					for(i=1;i<=14;i++)
					  {
							 Conver_Result[0]+= ((float)((Data[0]>>(14-i))&0x0001))*pow(2,0-i);
						}
						Conver_Result[1]=Data[1]+Conver_Result[0];
	     	}
	      return 	Conver_Result[1];		
}
/***********************************************************
float Coefficient_Data_Precess_c12(mpl temp) 
c12ϵ��
************************************************************/  
float Coefficient_Data_Precess_c12(mpl temp)
{
	   char i;
	   signed short int Data[2];
	   float Conver_Result[2];
	   MPL.c12=(MPL.c12MSB<<8)|MPL.c12LSB;
	   if(MPL.c12&0x8000)
		 {
		   Data[0]=((~MPL.c12)>>2)+1;
		   for(i=1;i<22;i++)
			 {
				Conver_Result[0]+=((float)((Data[0]>>(22-i))&0x00001))*pow(2,0-i) ;
			 }
			 Conver_Result[1]=0-Conver_Result[0]; 		 
		 }
		 else
		 {
			 Data[0]=MPL.c12>>2;
			 for(i=0;i<22;i++)
			   {
					 Conver_Result[0]+=((float)((Data[0]>>(22-i))&0x00001))*pow(2,0-i) ;
				 }
			  Conver_Result[1]=Conver_Result[0]; 	 
		 }
	   
		  return Conver_Result[1];	
}
/***********************************************************
u16 Coefficient_Data_Precess_Padc(mpl temp) 
Padc����
************************************************************/  
u16  Data_Precess_Padc(mpl temp) 
{
	   char i;
	   signed short int Data[2];
	   float Conver_Result[2];
	   MPL.Pressure=(MPL.PressureMSB<<8)|MPL.PressureLSB;
	   Data[0]=MPL.Pressure>>6;
	   return Data[0];
	
}
/***********************************************************
u16 Coefficient_Data_Precess_Tadc(mpl temp) 
Tadc����
************************************************************/  
u16   Data_Precess_Tadc(mpl temp) 
{
	   char i;
	   signed short int Data[2];
	   float Conver_Result[2];
	   MPL.Temperature=(MPL.TemperatureMSB<<8)|MPL.TemperatureLSB;
	   Data[0]=MPL.Temperature>>6;
	  return Data[0];
}

/***********************************************************
void ReadALL_Parameter(void)
���մ���ѹ����
************************************************************/ 
float ReadALL_Parameter(void)
{
       Read_coefficients(MPL);
	     delay_ms(5);
	     Start_Conversion();
	     delay_ms(5);
	     Read_Result(MPL);
	     delay_ms(5);
			 MPL.Co_a0=Coefficient_Data_Precess_a0(MPL);   //��ȡϵ��a0;
			 MPL.Co_b1=Coefficient_Data_Precess_b1(MPL);
			 MPL.Co_b2=Coefficient_Data_Precess_b2(MPL);
			 MPL.Co_c12=Coefficient_Data_Precess_c12(MPL);
			 MPL.Padc=Data_Precess_Padc(MPL); 
			 MPL.Tadc=Data_Precess_Tadc(MPL);	
  	  Calculating_Result();  //���������
	    Lyndir_No->Re_Pressure=kalman_deng(MPL.Re_Pressure);
	    return 0;
}
/***********************************************************
void Count_Result(void)
���մ���ѹ����
************************************************************/  
float Calculating_Result(void)
{
	  float Pcomp;
	  Pcomp=0;
	  MPL.Re_Pressure=(MPL.Co_a0+(MPL.Co_b1+MPL.Co_c12*MPL.Tadc)*MPL.Padc+MPL.Co_b2*MPL.Tadc)*((float)(115-50)/1023)+50;
    return MPL.Re_Pressure;
}






