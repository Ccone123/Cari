#include "kalmanoperation.h"
//#include "equipment.h"
#include "math.h"

float X_k,_X_k,Y_k,P_k,_P_k,Kg,R;// X_k���������Ż�ֵ,_X_kԤ��ֵ,Y_kʵ�ʲ���ֵ,P_k����Э����,_P_k����Э����,Kg����������,Rѭ��������
float gammam=0.1,A;//gammam��������A�ٽ�����
float Q=0.1;//������

//������Ҫ���㺯����leixingָ���㾫��: 1 is sensitive, 0 is rough
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
						A=0.02457*_X_k+2.743;//AֵԽ��������Խ��this formula is from test results
					}
					
					if(fabs((celiang-_X_k))>A)//���������ֵ��������Ų���ֵ
					{
							X_k = Kg*_X_k +( 1-Kg) * celiang;
					}
					else//û�г������������Ԥ��ֵ
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

