#include "kalmanoperation.h"
//#include "equipment.h"
#include "math.h"

float X_k,_X_k,Y_k,P_k,_P_k,Kg,R;// X_k代表最优优化值,_X_k预测值,Y_k实际测量值,P_k先验协方差,_P_k后验协方差,Kg卡尔曼增益,R循环超参数
float gammam=0.1,A;//gammam超参数，A临界增量
float Q=0.1;//超参数
char j=1;
float I_celiang=0;


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
				 _X_k= X_k;
				 _P_k= P_k+ Q;
				 R=gammam*sqrt(_X_k);
				 Kg = _P_k / (P_k + R);
						A=6;
					
					
					if(fabs((celiang-_X_k))>A)//如果超过阈值则更加相信测量值
					{
							X_k = Kg*_X_k +( 1-Kg) * celiang;
					}
					else//没有超过则更加相信预测值
					{
							X_k = _X_k + Kg * (celiang - _X_k);
					}
			
					P_k = (1 - Kg) * _P_k;

		 }
		 I_celiang=celiang;
 
     return  I_celiang;

}
    


