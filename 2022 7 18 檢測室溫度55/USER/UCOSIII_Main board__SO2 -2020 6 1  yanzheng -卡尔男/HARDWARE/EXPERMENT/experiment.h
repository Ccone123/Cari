#ifndef __EXPERIMENT_H
#define __EXPERIMENT_H
#pragma pack(push)
#pragma pack(1) 
typedef struct     //待测相关参数
{	
float BackGroundValue;  //记录本底值
char AfterCALFlag;      //校准后标志位
float CalValue;
float OffSet;
char Add_AfterCALFlag;//加入OFFSET进入浓度
}VAR;
#pragma  apop;
extern VAR Var;
 void Auto_Cal_0701(unsigned int controlbutton ,char i);
void Experiment (void);
void Button_Control (unsigned int screen , unsigned int control , char state);
#endif
