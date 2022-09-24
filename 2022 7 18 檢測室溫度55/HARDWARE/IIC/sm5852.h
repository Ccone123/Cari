#ifndef _SM5852_H_	
#define _SM5852_H_
#include "sys.h"

#define SM5852_Address_W  95
#define SM5852_Address_R  96
#define SM5852_Register   0x84  //80~
#pragma pack(push)
#pragma pack(1)  
typedef struct {
	u8 Uncorrected_Pressure_LSB;
	u8 Uncorrected_Pressure_MSB;
	u8 Supply_Voltage_LSB;
	u8 Supply_Voltage_MSB;
	u16 Uncorrected_Pressure;
	float Pressure;
}sm;
#pragma apop
extern sm SM;
void 	ConverttoKPa(sm temp);
void SM5852_Air_Pressure_Resurt(void);
#endif
