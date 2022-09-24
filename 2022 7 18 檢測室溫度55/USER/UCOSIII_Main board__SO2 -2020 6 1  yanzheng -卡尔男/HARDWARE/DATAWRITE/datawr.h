#ifndef __DATAWR_H
#define __DATAWR_H	

#include "sys.h"

extern u32  RevNameS_D;
extern u32  RevNameE_D;
extern u32  RevNameS3_D;
extern u32  RevNameE3_D;
int GetStringNum( char *cmd);
void FuckingYou_New_Pathname(void) ;
void FuckingYou_New_Readdata_min(void);
void FuckingYou_New_Readdata_5MIN(void); 
void FuckingYou_New_Readdata_hour(void);
void FuckingYou_New_Readdata_day(void);
void FuckingYou_New_Readdata1(void);
void FuckingYou_New_Readdata1_Hour(void); //ÇúÏßÍ¼
void FuckingYou_New_Writedata_Min(float data);
void FuckingYou_New_Writedata_5MIN(float data);
void FuckingYou_New_Writedata_Hour(float data);
void FuckingYou_New_Writedata_day(float data);
void FuckingYou_New_Writedata_Parament(float _data1,float _data2,float _data3,float _data4,float _data5,float _data6,float _data7,float _data8);

char *strRemov(char* dst,  char* src, char ch );
void Event_Record(char event_type);

#endif


