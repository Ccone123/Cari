#include "sm5852.h"
#include "iic.h"
#include "delay.h"
#include "includes.h"
#include "iicssc.h"
/***********************************************************
float Read_Pressure(mpl temp) 
��ȡѹ��ֵ
************************************************************/

sm SM;

/***********************************************************
void 	ConverttoKPa(sm temp)
ת��ΪKPa
************************************************************/
void 	ConverttoKPa(sm temp)
{  //0~fff
	SM.Pressure= 2.1*(float)SM.Uncorrected_Pressure/0xfff;	
}
/***********************************************************
float SM5852_Air_Pressure_Resurt(void) 
ת��ΪKPa
************************************************************/
void SM5852_Air_Pressure_Resurt(void)
{
 Read_PressureSSC_Experment();

}