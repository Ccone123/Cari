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
				 average1+=readFiltBuf[i];//求数据总和
			 }
		    average1=average1/(*readCnt); //求平均数
			// 变异系数求解
				 for(i=0;i<*readCnt; i++)
				 {
					SV_ZeroP+=  pow((readFiltBuf[i]-average1),2);//求平方差		 
				 } 
		      CV_ZeroP = sqrt ( SV_ZeroP/(*readCnt))/average1;//变异系数
				 if(isRand==1)
				    return average1;  //返回浓度
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
					 average1=average1/FLITE_NUM;  //取平均值
			   // 变异系数求解
				 for(i=0;i<*readCnt; i++)
				 {
					SV_ZeroP+=  pow((readFiltBuf[i]-average1),2);//求平方差		 
				 } 
		      CV_ZeroP = sqrt ( SV_ZeroP/(*readCnt))/average1;//变异系数
		    
        	 if(isRand==1)
				    return average1;  //返回浓度
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
											 average2+=readFiltBuf[i];//求数据总和
										 }
											average2=average2/(*readCnt); //求平均数			
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
											 average2=average2/5;  //取平均值
					
												return average2;  //返回浓度
										
						 }
					
	   }
		 else 
		 {}
 
	 }
	 
	 
	 