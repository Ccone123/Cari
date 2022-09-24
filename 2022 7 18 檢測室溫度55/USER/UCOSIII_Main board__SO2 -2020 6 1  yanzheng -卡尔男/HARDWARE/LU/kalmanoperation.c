#include "kalmanoperation.h"
//#include "equipment.h"
#include "math.h"

float X_k,_X_k,Y_k,P_k,_P_k,Kg,R;// X_k���������Ż�ֵ,_X_kԤ��ֵ,Y_kʵ�ʲ���ֵ,P_k����Э����,_P_k����Э����,Kg����������,Rѭ��������
float gammam=0.1,A;//gammam��������A�ٽ�����
float Q=0.1;//������
char j=1;
float I_celiang=0;


//������Ҫ���㺯����leixingָ���㾫��: 1 is sensitive, 0 is rough
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
					
					
					if(fabs((celiang-_X_k))>A)//���������ֵ��������Ų���ֵ
					{
							X_k = Kg*_X_k +( 1-Kg) * celiang;
					}
					else//û�г������������Ԥ��ֵ
					{
							X_k = _X_k + Kg * (celiang - _X_k);
					}
			
					P_k = (1 - Kg) * _P_k;

		 }
		 I_celiang=celiang;
 
     return  I_celiang;

}
    


