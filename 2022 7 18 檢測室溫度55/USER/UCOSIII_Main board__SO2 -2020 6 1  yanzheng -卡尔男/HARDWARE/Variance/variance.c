#include "variance.h"
#include "math.h"
#include "stdlib.h"


float filtStack (float *source , float readFiltBuf[],u8 * readCnt,u8 isRand)
   {
		 float CV_ZeroP;float *Paverage=NULL;float *Paverage2=NULL;
		 int i=0; 
		 float average1=0; float SV_ZeroP;
		 float average2=0;
		
	if((isRand==1)||	(isRand==0) )
	{
		 if(*readCnt<FLITE_NUM)
		 {
			 readFiltBuf[*readCnt] = *source;
			 *readCnt=*readCnt+1;
		 
		 for(i=0;i<*readCnt;i++)    
		   {
				 average1+=readFiltBuf[i];//�������ܺ�
			 }
		    average1=average1/(*readCnt); //��ƽ����
			// ����ϵ�����
				 for(i=0;i<*readCnt; i++)
				 {
					SV_ZeroP+=  pow((readFiltBuf[i]-average1),2);//��ƽ����		 
				 } 
		      CV_ZeroP = sqrt ( SV_ZeroP/(*readCnt))/average1;//����ϵ��
				 if(isRand==1)
				    return average1;  //����Ũ��
					 else
						return 0;
			} 
		 else 
		 {
					 for( i = FLITE_NUM -1 ; i>=0 ; i--)
					{
						readFiltBuf[i+1] = readFiltBuf[i];
				

					}
					readFiltBuf[0] = *source;
			
					for( i = 0 ; i<FLITE_NUM; i++)
					{
						average1 += readFiltBuf[i] ; 
					}
					 average1=average1/FLITE_NUM;  //ȡƽ��ֵ
			   // ����ϵ�����
				 for(i=0;i<*readCnt; i++)
				 {
					SV_ZeroP+=  pow((readFiltBuf[i]-average1),2);//��ƽ����		 
				 } 
		      CV_ZeroP = sqrt ( SV_ZeroP/(*readCnt))/average1;//����ϵ��
		    
        	 if(isRand==1)
				    return average1;  //����Ũ��
					 else	 
				     return CV_ZeroP;	 
		 }
	 }
	   else if(isRand==3)
		 {
						if(*readCnt<5)
							 {
										 readFiltBuf[*readCnt] = *source;
										 *readCnt=*readCnt+1;
									 average2=0;
									 for(i=0;i<*readCnt;i++)    
										 {
											 average2+=readFiltBuf[i];//�������ܺ�
										 }
											average2=average2/(*readCnt); //��ƽ����			
													return average2;
								}  

            else 
						{
							 	 for( i = 5 -1 ; i>=0 ; i--)
											{
												readFiltBuf[i+1] = readFiltBuf[i];
											}
											readFiltBuf[0] = *source;
									       average2=0;
											for( i = 0 ; i<5; i++)
											{
												average2 += readFiltBuf[i] ; 
											}
											 average2=average2/5;  //ȡƽ��ֵ
					
												return average2;  //����Ũ��
										
						 }
					
	   }
		 else 
		 {}
 
	 }
	 
	 
	 