#ifndef __CALIBRATION_H
#define __CALIBRATION_H	

#include "sys.h"
 extern unsigned char  Startup;
 extern unsigned char Count_Input;
 extern unsigned char numm;
 typedef void (*fun_ptr1)(void);
#pragma pack(push)
#pragma pack(1) 
typedef struct     //待测相关参数
{	
 float SO2Soncentration_Input;
 float k;
 float b;
 float PMTSampVL;
 unsigned char  Startup;  //启动
 unsigned char  Count_Input;
 float COV;
 char  Delete_Flag;
  u8 CACULATEKBFLAG;
}Multi,*PMulti;
#pragma  apop;
extern Multi Multi_points;
extern  Multi AMulti[10];
void InsertSort_InputSoncentration( int n,char Num);
void sort_struct (int m,char Num);
void display (void);
void Multi_points_Calibration (void);
void Delete_sort(u8 cout);
void Cover_Fun(u8 cout);
 void press_button(void);
#endif
