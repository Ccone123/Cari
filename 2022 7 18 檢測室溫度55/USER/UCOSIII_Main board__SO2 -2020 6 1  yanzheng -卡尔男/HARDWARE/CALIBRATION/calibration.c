#include "calibration.h"
#include "processing.h"
#include "includes.h" 
#include "rtc.h"
#include "variance.h"
#include "IICEEPROM.h"
#include "delay.h"
#include "USART3.h"
#include "communication.h"
extern int TimeMeasCount; 
Multi Multi_points;
Multi AMulti[10];
PMulti PMulti_points;
unsigned  char  Startup;
 unsigned char Count_Input;
unsigned  char numm;
static fun_ptr1     fun1;
//fun1=display;
void Multi_points_Calibration (void)
{
	 OS_ERR err;
	  float x1,x2,y1,y2,x3,y3;
    char FTOCHAR[10][20],len1,len2,len3,len4,i; 
    fun1=display;
	if(READSCRREN.SCRREN11FLAG==1)
	{
		len1=sprintf(FTOCHAR[6],"%.2f",GASFLOW); //GASFLOWq气体流量
		len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL); //PMT光强
	  	delay_ms(2); 
		OSIntEnter();  
		USART3_SendByte(0xEE);//EE B1 10 00 01 00 17 31 31 31 31 FF FC FF FF 
		USART3_SendByte(0xB1);
		USART3_SendByte(0x10);
		USART3_SendByte(0x00);	USART3_SendByte(0x0B);
	  USART3_SendByte(0x00);	USART3_SendByte(0x1E);
				for(i=0;i<len1;i++)
			{
				USART3_SendByte(FTOCHAR[6][i]);
			}
	    	END_CMD();
			  delay_ms(2); 
				USART3_SendByte(0xEE);//EE B1 10 00 01 00 17 31 31 31 31 FF FC FF FF 
		USART3_SendByte(0xB1);
		USART3_SendByte(0x10);
		USART3_SendByte(0x00);	USART3_SendByte(0x0B);
	  USART3_SendByte(0x00);	USART3_SendByte(0x0F);
				for(i=0;i<len2;i++)
			{
				USART3_SendByte(FTOCHAR[7][i]);
			}
	    	END_CMD();
	     	OSIntExit();  //阻止被打断
		
		if(AMulti[0].Startup==1)
		{
			reqErrCnt[6]=0;//滑动滤波参数清零，0点个数
			reqErrCnt[7]=0;
				   	delay_ms(2); 
			    OSIntEnter();
				 	USART3_SendByte(0xEE);  //标定过程中
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x0B);
		      USART3_SendByte(0x00);		USART3_SendByte(0x37);
	        USART3_SendByte(0x01);
				  END_CMD();
			    OSIntExit(); 
			
		   TimeMeasCount=300; //等待5分钟
			 while(TimeMeasCount)	
			 {
				 //跟新零点光强 
				 len1=sprintf(FTOCHAR[6],"%.2f",GASFLOW);//1E
				 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL); //PMT光强0F
				 len3=sprintf(FTOCHAR[8],"%.3f",SO2ConCenTration_AfterCal); //PMT光强0E
				  
				 OSIntEnter(); 
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //多控件发送的头
  		 USART3_SendByte(0x00);	USART3_SendByte(0x0B);  	                   //界面11
  		 USART3_SendByte(0x00);	USART3_SendByte(0x1E);                      //键控号1E,GASFLOW,1E
  		 USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//字符个数
					 for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[6][i]);                //GASFLOW
					}		
				USART3_SendByte(0x00);	  USART3_SendByte(0x0F);                      //键控号0F，测量暗电流
				USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//字符个数		
				 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[7][i]);                //SO2CONCERNRATIONCAL.PMTSampVL
					}		
			USART3_SendByte(0x00);	USART3_SendByte(0x0E);                      //键控号0E 参考光强
			USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//字符个数		
				 for(i=0;i<len3;i++)
					{
						USART3_SendByte(FTOCHAR[8][i]);                //发送数据 参考光强
					}		
					END_CMD();		
				 OSIntExit();	 
				 AMulti[0].PMTSampVL=filtStack (&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[6],&reqErrCnt[6],1);//测量光强
				 AMulti[0].COV=filtStack(&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[7],&reqErrCnt[7],0);
				 	OSTimeDly(50,OS_OPT_TIME_PERIODIC,&err);
				 if(AMulti[0].COV>0&&AMulti[0].COV<1)
					 { 
						 TimeMeasCount=0;	  //校准成功
						 AMulti[0].Startup=0;
					 len2=sprintf(FTOCHAR[7],"%.3f",AMulti[0].PMTSampVL); //PMT光强mv
					 len3=sprintf(FTOCHAR[8],"%.1f",AMulti[0].SO2Soncentration_Input);  
					 delay_ms(2); 
						 	OSIntEnter(); 
					 USART3_SendByte(0xEE);//EE B1 10 00 01 00 04 FF FC FF FF 
					 USART3_SendByte(0xB1);
					 USART3_SendByte(0x10);
					 USART3_SendByte(0x00);	USART3_SendByte(0x0B);
					 USART3_SendByte(0x00);	USART3_SendByte(0x66);
						for(i=0;i<len2;i++)
						{
							USART3_SendByte(FTOCHAR[7][i]);
						}
				      END_CMD();
						  delay_ms(2); 
					 USART3_SendByte(0xEE);//EE B1 10 00 01 00 04 FF FC FF FF 
					 USART3_SendByte(0xB1);
					 USART3_SendByte(0x10);
					 USART3_SendByte(0x00);	USART3_SendByte(0x0B);
					 USART3_SendByte(0x00);	USART3_SendByte(0x65);
						for(i=0;i<len2;i++)
						{
							USART3_SendByte(FTOCHAR[8][i]);
						}
				      END_CMD();
						  delay_ms(2); 
				  	USART3_SendByte(0xEE);  //校准成功
		       USART3_SendByte(0xB1);		 
		       USART3_SendByte(0x23);
				   USART3_SendByte(0x00);		USART3_SendByte(0x0B);
		       USART3_SendByte(0x00);		USART3_SendByte(0x37);
	         USART3_SendByte(0x02);
				   END_CMD();	
						SaveEpData(328,Startup); //保存Startup
						SaveEpData(336,Count_Input); //
						 	//SaveEpData(328,Startup); //保存Startup
						SaveEpData(344, AMulti[0].PMTSampVL);
							OSIntExit();  	 
					 }
			 }
			 if(AMulti[0].COV>1)
			 {
				 AMulti[0].Startup=0;  //校准失败			 
				 		  delay_ms(2); 
				  	USART3_SendByte(0xEE);  //校准失败
		       USART3_SendByte(0xB1);		 
		       USART3_SendByte(0x23);
				   USART3_SendByte(0x00);		USART3_SendByte(0x0B);
		       USART3_SendByte(0x00);		USART3_SendByte(0x37);
	         USART3_SendByte(0x03);
				   END_CMD();					 
							OSIntExit();
			 }	
		}
	 else if(AMulti[Startup-1].Startup==1) 
	 {    
		  	reqErrCnt[6]=0;//滑动滤波参数清零，0点个数
			  reqErrCnt[7]=0; 
		   	delay_ms(2); 
			    OSIntEnter();
				 	USART3_SendByte(0xEE);  //标定过程中
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x0B);
		      USART3_SendByte(0x00);		USART3_SendByte(0x37);
	        USART3_SendByte(0x01);
				  END_CMD();
			    OSIntExit(); 
		 
		  	TimeMeasCount=300; //等待5分钟
			 while(TimeMeasCount)	
			 {
				 // 更新校准点光强
				 len1=sprintf(FTOCHAR[6],"%.2f",GASFLOW);//1E
				 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL); //PMT光强0F
				 len3=sprintf(FTOCHAR[8],"%.3f",SO2ConCenTration_AfterCal); //PMT光强0E
				  delay_ms(2); 
				 OSIntEnter(); 
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //多控件发送的头
  		 USART3_SendByte(0x00);	USART3_SendByte(0x0B);  	                   //界面11
  		 USART3_SendByte(0x00);	USART3_SendByte(0x1E);                      //键控号1E,GASFLOW,1E
  		 USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//字符个数
					 for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[6][i]);                //GASFLOW
					}		
			USART3_SendByte(0x00);	  USART3_SendByte(0x0F);                      //键控号0F，测量暗电流
			USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//字符个数		
				 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[7][i]);                //SO2CONCERNRATIONCAL.PMTSampVL
					}		
			USART3_SendByte(0x00);	USART3_SendByte(0x0E);                      //键控号0E 参考光强
			USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//字符个数		
				 for(i=0;i<len3;i++)
					{
						USART3_SendByte(FTOCHAR[8][i]);                //发送数据 参考光强
					}		
					END_CMD();		
				 OSIntExit();	 
					
					
				 
				 
		  AMulti[Startup-1].PMTSampVL =filtStack (&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[6],&reqErrCnt[6],1);//测量光强
					
		  AMulti[Startup-1].COV=filtStack (&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[7],&reqErrCnt[7],0);
		  	OSTimeDly(50,OS_OPT_TIME_PERIODIC,&err);
				 if(AMulti[Startup-1].COV>0&&AMulti[Startup-1].COV<1)
					 { 
						 TimeMeasCount=0;	  //校准成功
						 AMulti[Startup-1].Startup=0;
						AMulti[Startup-1].CACULATEKBFLAG =1;
						 	  delay_ms(2); 
				  	USART3_SendByte(0xEE);  //校准成功
		       USART3_SendByte(0xB1);		 
		       USART3_SendByte(0x23);
				   USART3_SendByte(0x00);		USART3_SendByte(0x0B);
		       USART3_SendByte(0x00);		USART3_SendByte(0x37);
	         USART3_SendByte(0x02);
				   END_CMD();	
					 }
			} 
			 	if (AMulti[Startup-1].COV>1)
			 {
				 AMulti[Startup-1].Startup=0;  //校准失败
				 	 		  delay_ms(2); 
				    OSIntEnter();
				   USART3_SendByte(0xEE);  //校准失败
		       USART3_SendByte(0xB1);		 
		       USART3_SendByte(0x23);
				   USART3_SendByte(0x00);		USART3_SendByte(0x0B);
		       USART3_SendByte(0x00);		USART3_SendByte(0x37);
	         USART3_SendByte(0x03);
				   END_CMD();					 
					 OSIntExit();
				 Startup--;
				 Count_Input--;// 清空不作数
			 }			
	 }
	 else{;}
	 if(AMulti[Startup-1].CACULATEKBFLAG ==1)
	 {
	     	sort_struct(Startup,1);
       
  		 AMulti[Startup-1].CACULATEKBFLAG =0;
		 
				 for(i=0;i<Startup-1;i++)
					 {
						x1=AMulti[i].PMTSampVL;;
						y1=AMulti[i].SO2Soncentration_Input;
						x2=AMulti[i+1].PMTSampVL;
						y2=AMulti[i+1].SO2Soncentration_Input;   
						AMulti[i+1].k =(y2-y1)/(x2-x1);           //k值
						AMulti[i+1].b =y2-AMulti[i+1].k*x2;  //b值
						OSIntEnter();	
						SaveEpData(384+(i)*8*2,AMulti[i+1].k*1000); //b保存K，B。
						SaveEpData(392+(i)*8*2,AMulti[i+1].b*1000);//	
						
						SaveEpData(512+(i)*8*2,AMulti[i+1].SO2Soncentration_Input*1000); //b保存K，B。
						SaveEpData(520+(i)*8*2,AMulti[i+1].PMTSampVL*1000);
						 
						OSIntExit();
						}
					 	SaveEpData(328,Startup); //保存Startup
						SaveEpData(336,Count_Input); //
			      SaveEpData(344, AMulti[0].PMTSampVL*1000);
					  display();
 						
	 }
	}
}

  //  InsertSort_InputSoncentration(Count_Input,numm);
//直接插入排序函数
void InsertSort_InputSoncentration( int n,char Num)// AMulti[Count_Input++].SO2Soncentration_Input
{
	char i;float x; char flag;
	float *a=NULL;
	for(i=0;i<n;i++)
	{
		a[i]=AMulti[i].SO2Soncentration_Input;
	}

	for(i=0;i<n-1;i++)
	{
		if(a[i]>a[n-1])
		{
			Num=i;
			break;
			flag=1;
			}	
	}	
	if(flag==1)
	{
   x = a[n-1];
	for(i=n-1;i>Num;i--)
	{
	   a[i]=a[i-1];
		AMulti[i].PMTSampVL= AMulti[i-1].PMTSampVL;
	}
	a[Num]=x;
	 flag=0;
 }
	for(i=0;i<n;i++)
	{
	AMulti[i].SO2Soncentration_Input=a[i];
	}
}


void sort_struct (int m,char Num)
{
	 int i,j;
	for (i=0;i<m;i++)
	    {
				for(j=0;j<m-i-1;j++)
			{
				if(AMulti[j].SO2Soncentration_Input>AMulti[j+1].SO2Soncentration_Input)
				{
					Multi_points=AMulti[j+1];
					AMulti[j+1]=AMulti[j];
					AMulti[j]=Multi_points;
				}
			}
			}
}
void display (void)
{
	int i,j; OS_ERR err;
	  float x1,x2,y1,y2,x3,y3;
    char FTOCHAR[5][20],len1,len2,len3,len4;
	for(j=0;j<Startup;j++)
	   {
          if(j==0)
					{ 
						 len1=sprintf(FTOCHAR[1],"%.2f",AMulti[0].SO2Soncentration_Input);
						 len2=sprintf(FTOCHAR[2],"%.3f",AMulti[0].PMTSampVL);
							  delay_ms(2); 
				 OSIntEnter(); 
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //多控件发送的头
  		 USART3_SendByte(0x00);	USART3_SendByte(0x0B);  	                   //界面11
  		 USART3_SendByte(0x00);	USART3_SendByte(101+0*4);                      //键控号101 浓度
  		 USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//字符个数
					 for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[1][i]);                //测量光强
					}		
			USART3_SendByte(0x00);	  USART3_SendByte(102+0*4);                      //102
			USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//字符个数		
				 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[2][i]);                //SO2CONCERNRATIONCAL.PMTSampVL
					}		
					END_CMD();		
				 OSIntExit();	
					    }						
		    else 
				{
					  len1=sprintf(FTOCHAR[1],"%.2f",AMulti[j].SO2Soncentration_Input);
						len2=sprintf(FTOCHAR[2],"%.3f",AMulti[j].PMTSampVL);
						len3=sprintf(FTOCHAR[3],"%.2f",AMulti[j].k);
						len4=sprintf(FTOCHAR[4],"%.2f",AMulti[j].b);
					  delay_ms(2); 
				 OSIntEnter(); 
						 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //多控件发送的头
						 USART3_SendByte(0x00);	USART3_SendByte(0x0B);  	                   //界面11
						 USART3_SendByte(0x00);	USART3_SendByte(101+j*4);                      //键控号浓度101+i*4
						 USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//字符个数
								 for(i=0;i<len1;i++)
								{
									USART3_SendByte(FTOCHAR[1][i]);                //测量光强
								}		
						USART3_SendByte(0x00);	  USART3_SendByte(102+j*4);                      //，测量暗电流102+i*4
						USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//字符个数		
							 for(i=0;i<len2;i++)
								{
									USART3_SendByte(FTOCHAR[2][i]);                //K
								}		
						USART3_SendByte(0x00);	USART3_SendByte(103+j*4);                      //键控号103+i*4
						USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//字符个数		
							 for(i=0;i<len3;i++)
								{
									USART3_SendByte(FTOCHAR[3][i]);                //发送数据 
								}		
									USART3_SendByte(0x00);	USART3_SendByte(104+j*4);                      //b 104+i*4
						USART3_SendByte(len4>>8);	USART3_SendByte(len4&0xff);	//字符个数		
							 for(i=0;i<len4;i++)
								{
									USART3_SendByte(FTOCHAR[4][i]);                //发送数据 
								}			
					    END_CMD();		
				 OSIntExit();			
				}
			}
				  for (i=Startup;i<8;i++)
	            {
							  OSIntEnter(); 
						 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //多控件发送的头
						 USART3_SendByte(0x00);	USART3_SendByte(0x0B);  	                   //界面11
					
						 USART3_SendByte(0x00);	USART3_SendByte(101+i*4);                      //键控号浓度101+i*4
						 USART3_SendByte(0x00);	USART3_SendByte(0x00);	//字符个数
							
						USART3_SendByte(0x00);	  USART3_SendByte(102+i*4);                      //，测量暗电流102+i*4
						USART3_SendByte(0x00);	  USART3_SendByte(0x00);	//字符个数		
							
						USART3_SendByte(0x00);	USART3_SendByte(103+i*4);                      //键控号103+i*4
						USART3_SendByte(0x00);	USART3_SendByte(0x00);	//字符个数		
							
						USART3_SendByte(0x00);	USART3_SendByte(104+i*4);                      //b 104+i*4
						USART3_SendByte(0x00);	USART3_SendByte(0x00);	//字符个数								
					     END_CMD();		
				       OSIntExit();		
							}	 
}

 void Delete_sort(u8 cout)//i=
 {
	 u8 i,j,k; float x1,x2,y1,y2,x3,y3;
	  for(j=i+1;j<Startup;j++)
	 {
		  AMulti[j-1]=AMulti[j];
	 } 
	    Startup--;
	    Count_Input--;
	 
	 	 for(i=cout-1;i<Startup-1;i++)
					 {
						x1=AMulti[i].PMTSampVL;;
						y1=AMulti[i].SO2Soncentration_Input;
						x2=AMulti[i+1].PMTSampVL;
						y2=AMulti[i+1].SO2Soncentration_Input;   
						AMulti[i+1].k =(y2-y1)/(x2-x1);           //k值
						AMulti[i+1].b =y2-AMulti[i+1].k*x2;  //b值
						OSIntEnter();	
						SaveEpData(384+(i)*8*2,AMulti[i+1].k*1000); //b保存K，B。最大512个字节
						SaveEpData(392+(i)*8*2,AMulti[i+1].b*1000);//	
					  
						SaveEpData(512+(i)*8*2,AMulti[i+1].SO2Soncentration_Input*1000); //b保存K，B。
						SaveEpData(520+(i)*8*2,AMulti[i+1].PMTSampVL*1000); 
						OSIntExit();
						}
					 		SaveEpData(328,Startup); //保存Startup
					  	SaveEpData(336,Count_Input); 
	      display();
	 
 }
	 
void Cover_Fun(u8 cout)//cout=3
{
	 u8 i,j,k; float x1,x2,y1,y2,x3,y3;
	  for(j=cout+1;j<Startup;j++)
	 {
		  AMulti[j-1]=AMulti[j];
	 } 
	    Startup--;
	    Count_Input--;
	 
	 	 for(i=cout-1;i<Startup-1;i++)
					 {
						x1=AMulti[i].PMTSampVL;;
						y1=AMulti[i].SO2Soncentration_Input;
						x2=AMulti[i+1].PMTSampVL;
						y2=AMulti[i+1].SO2Soncentration_Input;   
						AMulti[i+1].k =(y2-y1)/(x2-x1);           //k值
						AMulti[i+1].b =y2-AMulti[i+1].k*x2;  //b值
						}					
	      display();
}
 void press_button(void)//EE B1 10 00 0B 00 32 00 FF FC FF FF 初始化时把按键恢复初始状态
 {
	   USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
		USART3_SendByte(0x10);	
		USART3_SendByte(0x00);
		USART3_SendByte(0x0B);
	 	USART3_SendByte(0x00);
		USART3_SendByte(0x32);
	 	USART3_SendByte(0x00);
	   END_CMD();
	 delay_ms(2);
	    USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
		USART3_SendByte(0x10);	
		USART3_SendByte(0x00);
		USART3_SendByte(0x06);
	 	USART3_SendByte(0x00);
		USART3_SendByte(0x32);
	 	USART3_SendByte(0x00);
	   END_CMD();
	 
 }
 
 
 
 
 
 
 