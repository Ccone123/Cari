#ifndef __EXPERIMENT_H
#define __EXPERIMENT_H
#pragma pack(push)
#pragma pack(1) 
typedef struct     //������ز���
{	
float BackGroundValue;  //��¼����ֵ
char AfterCALFlag;      //У׼���־λ
float CalValue;
float OffSet;
char Add_AfterCALFlag;//����OFFSET����Ũ��
}VAR;
#pragma  apop;
extern VAR Var;
 void Auto_Cal_0701(unsigned int controlbutton ,char i);
void Experiment (void);
void Button_Control (unsigned int screen , unsigned int control , char state);
#endif
