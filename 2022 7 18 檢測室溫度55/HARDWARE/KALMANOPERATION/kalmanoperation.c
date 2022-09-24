#include "kalmanoperation.h"
#include "math.h"
#include "lyndir_no.h"
float X_k,_X_k,Y_k,P_k,_P_k,Kg,R;// X_k代表最优优化值,_X_k预测值,Y_k实际测量值,P_k先验协方差,_P_k后验协方差,Kg卡尔曼增益,R循环超参数
float X_k2,_X_k2,Y_k2,P_k2,_P_k2,Kg2,R2;


//Q=0.000001;
float X_k1,_X_k1,Y_k1,P_k1,_P_k1,Kg1,R1;
float gammam=1,	A=10;//gammam超参数，A临界增量
float gammam1=1,	A1=10;//gammam超参数，A临界增量
float gammam2=1,	A2=10;
float Q=0.0001;//超参数
float Q1=0.0001;//超参数
float Q2=0.0001;
char j=1;
char j1=1;
char j2=1;
float I_celiang=0;
float I_celiang1=0;
float I_celiang2=0;
char Flag_Clear;
char Flag_Clear1;
char Flag_Clear2;
float Sum_=0;
float Sum_1=0;
float Sum_2=0;
float AVR=0;
float AVR1=0;
float AVR2=0;
u32 k_count=0;
u32 k_count1=0;
u32 k_count2=0;
//这是主要计算函数，leixing指计算精度: 1 is sensitive, 0 is rough
float kalman(float celiang)
{
		 if(j == 1)   // at the first cycle, the initial values of kalman cycle
		 {
				 X_k= celiang;
				 P_k= 2;
				 _X_k= X_k;
				 _P_k = P_k + Q;
				 R=gammam*sqrt(_X_k);
				 Kg = _P_k / (P_k + R);
				 X_k = _X_k + Kg * (celiang - _X_k);
				 P_k = (1 - Kg) * _P_k;
			  j=0;
		 }
	  else
		 { 
			 if(Lyndir_No->Calman_inital==0x01   )
			 {
				    X_k=1.285;
				 Lyndir_No->Calman_inital=2;				 
			 }
				 _X_k= X_k;
				 _P_k= P_k+ Q;
				 R=gammam*sqrt(_X_k);
				 Kg = _P_k / (P_k + R);					
//					if(fabs((celiang-_X_k))>A)//如果超过阈值则更加相信测量值
//					{
//							X_k = Kg*_X_k +( 1-Kg) * celiang;
//					}
//					else//没有超过则更加相信预测值
//					{
							X_k = _X_k + Kg * (celiang - _X_k);
				//	}
					
		   	P_k = (1 - Kg) * _P_k;

		 }
		 I_celiang=X_k;
 
     return  I_celiang;
}
    
float kalman_deng(float celiang)
{
		 if(j1 == 1)   // at the first cycle, the initial values of kalman cycle
		 {
				 X_k1= celiang;
				 P_k1= 2;
				 _X_k1= X_k1;
				 _P_k1 = P_k1 + Q1;
				 R1=gammam1*sqrt(_X_k1);
				 Kg1 = _P_k1 / (P_k1 + R1);
				 X_k1 = _X_k1 + Kg1 * (celiang - _X_k1);
				 P_k1 = (1 - Kg1) * _P_k1;
			  j1=0;
		 }
	  else
		 { 
				 _X_k1= X_k1;
				 _P_k1= P_k1+ Q1;
				 R1=gammam1*sqrt(_X_k1);
				 Kg1 = _P_k1 / (P_k1 + R1);
					
					
			
							X_k1 = _X_k1 + Kg1 * (celiang - _X_k1);
		
	
		   	P_k1 = (1 - Kg1) * _P_k1;

		 }
		 I_celiang1=X_k1;
 
     return  I_celiang1;
}
    
float Kalman_PD(float celiang)
{
		 if(j2 == 1)   // at the first cycle, the initial values of kalman cycle
		 {
				 X_k2= celiang;
				 P_k2= 2;
				 _X_k2= X_k2;
				 _P_k2 = P_k2 + Q2;
				 R1=gammam2*sqrt(_X_k2);
				 Kg2 = _P_k2 / (P_k2 + R2);
				 X_k2 = _X_k2 + Kg2 * (celiang - _X_k2);
				 P_k2 = (1 - Kg2) * _P_k2;
			  j2=0;
		 }
	  else
		 { 
				 _X_k2= X_k2;
				 _P_k2= P_k2+ Q2;
				 R2=gammam2*sqrt(_X_k2);
				 Kg2 = _P_k2 / (P_k2 + R2);
					
					
			
							X_k2 = _X_k2 + Kg2 * (celiang - _X_k2);
		
	
		   	P_k2 = (1 - Kg2) * _P_k2;

		 }
		 I_celiang2=X_k2;
 
     return  I_celiang2;
}




