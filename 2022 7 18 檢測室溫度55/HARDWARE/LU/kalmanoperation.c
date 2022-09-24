#include "kalmanoperation.h"
//#include "equipment.h"
#include "math.h"

float X_k,_X_k,Y_k,P_k,_P_k,Kg,R;// X_k代表最优优化值,_X_k预测值,Y_k实际测量值,P_k先验协方差,_P_k后验协方差,Kg卡尔曼增益,R循环超参数
float gammam=0.1,A;//gammam超参数，A临界增量
float Q=0.1;//超参数

//这是主要计算函数，leixing指计算精度: 1 is sensitive, 0 is rough
u32 kalman(float celiang)
{
		 j++; 
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
		 }
		 if(j>1)
		 { 
				 _X_k= X_k;
				 _P_k= P_k+ Q;
				 R=gammam*sqrt(_X_k);
				 Kg = _P_k / (P_k + R);
				 
					if(_X_k<10)// safety threshold
					{
						A=5;
					}
					else
					{
						A=0.02457*_X_k+2.743;//A值越大灵敏度越低this formula is from test results
					}
					
					if(fabs((celiang-_X_k))>A)//如果超过阈值则更加相信测量值
					{
							X_k = Kg*_X_k +( 1-Kg) * celiang;
					}
					else//没有超过则更加相信预测值
					{
							X_k = _X_k + Kg * (celiang - _X_k);
					}
			
					P_k[i] = (1 - Kg[i]) * _P_k[i];

				j=1;//rest	
		 }
		 I_ceiang=celiang;
  }
return I_celiang;
}

