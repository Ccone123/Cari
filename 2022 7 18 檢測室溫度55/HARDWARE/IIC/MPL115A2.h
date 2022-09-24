#ifndef _MPL115A2_H_	
#define _MPL115A2_H_
#include "sys.h"
#define Device_Address_W   0xC0
#define Device_Address_R   0xC1
#define Command_To_Convert 0x12
#define Command_To_Coefficient 0x04
#define Command_To_Data        0x00
#pragma pack(push)
#pragma pack(1)  
typedef struct{	
		 u8 a0MSB;
		 u8 a0LSB;
		 u8 b1MSB;
		 u8 b1LSB;
		 u8 b2MSB;
		 u8 b2LSB;
		 u8 c12MSB;
		 u8 c12LSB;
		 u8 PressureMSB;
		 u8 PressureLSB;
		 u8 TemperatureMSB;
		 u8 TemperatureLSB;
		 u16 Pressure;
		 u16 Temperature;
     int16_t a0; 
     int16_t b1;
     int16_t b2;
     int16_t c12;
	   float Co_a0;
	   float Co_b1;
	   float Co_b2;
   	 float Co_c12;
		 float Re_Pressure;
		 u16 Padc;
		 u16 Tadc;		 
}mpl;
#pragma apop
extern mpl MPL;
uint8_t Read_coefficients(mpl temp);
u8 Start_Conversion(void) ;
u8 Read_Result(mpl temp);
float Coefficient_Data_Precess_a0(mpl temp);
float Coefficient_Data_Precess_b1(mpl temp);
float Coefficient_Data_Precess_b2(mpl temp);
float Coefficient_Data_Precess_c12(mpl temp);
u16  Data_Precess_Padc(mpl temp);
u16  Data_Precess_Tadc(mpl temp); 
float ReadALL_Parameter(void);
float Calculating_Result(void);
#endif