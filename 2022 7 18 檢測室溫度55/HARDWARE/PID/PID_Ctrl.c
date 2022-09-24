#include "PID_Ctrl.h"
#include "lyndir_no.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "led.h"
struct _PID {
         double Set_Tempreture;
	       double Actual_Tempreture;
	       double err;
	       double err_last;
	       double err_next;
	       double Kp,Ki,Kd;
	       double integra;
}PID;
int PID_Time,PID_Step;
float PID_Value=55;
float TempretureRead;
unsigned char PID_Flag=1;
unsigned char PID_Start;
/*************************************************
**������:PID_Init
**����:PID��ʼ��
**ע�����
**init PID parameter 
		PID.Kp = 0.498;
		PID.Ki = 0.0002;
		PID.Kd = 0.158;
*************************************************/
void PID_Init(void)
{
   PID.Set_Tempreture=0;
	 PID.Actual_Tempreture=0;
	 PID.Kp=2;
	 PID.Ki=0.02;
	 PID.Kd=0.15;
	 PID.err = 0;
	 PID.err_last = 0;
   PID.err_next = 0;
	 PID.integra =0 ; 
	
}
/*************************************************
**������:PID_Calc
**����:PID����
**ע�����
*************************************************/
float PID_Calc(void)
{
   float PID_Position;
   TempretureRead=Lyndir_No->CHAM_TH4_Tem;//��ȡ�����¶�
   PID.Set_Tempreture = PID_Value;//72
	 if(TempretureRead>=(PID_Value+10)) //ѹ�������趨ѹ��3�������������PID����
		{		
			PID_Flag =0; 
			//ADDhot_COL=1;
			FANhot_COL=1;
			PID.err = 0;
	    PID.err_last = 0;
      PID.err_next = 0;
	    PID.integra =0 ;
		}
   else
	 {		 
		 PID_Flag=1;
	 }
	 
    PID.Actual_Tempreture=TempretureRead;
	  PID.err = PID.Set_Tempreture-PID.Actual_Tempreture;
 	if(fabs(PID.err)>=55)
		{
		PID.integra = 0;
		 PID.err = 0;
	   PID.err_last = 0;
     PID.err_next = 0;
	   PID.integra =0 ;
		}
		else
		{
			PID.integra +=  PID.err;                           //����ֵ
		}		
 //  PID_Position = PID.Kp*PID.err+PID.Ki*PID.integra+PID.Kd*(PID.err-PID.err_last);
	 PID_Position = PID.Kp*PID.err+PID.Ki*PID.integra+PID.Kd*(PID.err-PID.err_last);
	 PID.err_last = PID.err;  
   return((int)PID_Position); 
}


