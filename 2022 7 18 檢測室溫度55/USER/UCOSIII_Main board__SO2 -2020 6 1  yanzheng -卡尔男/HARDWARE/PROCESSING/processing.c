#include "processing.h"
#include "modbus_host.h"
#include "delay.h"
#include "mbcrc.h"
#include "stdio.h"
#include "string.h"
#include "communication.h"
#include "USART3.h"
#include "stdlib.h"
//#include "Timer.h"
#include "math.h"
#include "IICEEPROM.h"
#include "malloc.h"	 
#include "includes.h" 
#include "variance.h"
#include "datawr.h"
#include "calibration.h"
#include "experiment.h"
#include "kalmanoperation.h"
extern int aaa;
extern u16 TimeMeas_Rocord;
float elmResultBuf[ELM_CNT][101] = {{0},{0},{0}};
u8 reqErrCnt[ELM_CNT] ={0,0,0,0,0,0,0,0};
extern char Record_First;
extern OS_MUTEX OSUart4Used_Mutex;
u8 StopFlg=0;
SO2CONCERNRATION SO2CONCERNRATIONCAL; //S02相关重要参数保存
extern int TimeMeasCount;
const char string[2][6]={"ppb","ug/m3"};


//u8 TH_RX_BUF[20]={0}; //读当前状态
//u8 SO2_RX_BUF[20]={0};
u8 PMTDriveVal_RX_BUF[64]={0};
u8 TotalPara_RX_BUF[64]={0};


const char Record_Data[]={0xEE, 0x82, 0xFF, 0xFC ,0xFF ,0xFF };
const u16 ADDress2[]={0x0195,0x0197,0x0199,0x019B,0x01A9,0x01AB,0x01AD,0x01B1,0x01B7,0x01C3,0x01C9};
const u8 CALREGNUM[]={     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2};
//readdevice readdeviceoperate;
//writedevice writedeviceoperate;
//writedeviceFlag writedeviceoperateFlag;
const u16 ADDress3[]={0x0235, 0x0237, 0x0239,0x023B,0x023D};
const u8 DEVREGNUM[]={     2,     2,     2,     2,       2};			
readwarm readwarmingINF;
READSCRRENFLAG READSCRREN;
u8  instrumentcommuaddress; 
float SO2concentration=0;//S02的浓度,du
//"so2 浓度的一个建立"
float SO2ConCenTration=0;//S02浓度测试
char categroy_change=1;
float SO2ConCenTration_AfterCal=0;
float SO2ConCenTration_AfterCal_W=0;

float SO2SD=0;//S02标准差
float DCPMESampVL=0;//暗电流采样状态下，PMT采样电压
float REFVOL_PD=0;//参考光强PD
float REFVOL_PMT=0;//参考暗电流PD
float OVERALLD=0;//归一化值
float GASFLOW=0;//气体流量,单位:sccm
float SampGasP=0;//采样气体的压力
float CaseGasPRESSURE=0;//反应室气体压力
float CaseTEM=0;//反应室温度
float CRAteTEM=0;//ji机箱温度
float TECTEM=0;//TEC温度
float FiveMin_AVG=0;
float FiveMin_AVG_dis=0;
float PMT_DriValage=0;
float Change_Lamp_Val=0;
average AVG;
 readparameter parameter;  		
u8 RecorDataMin_Flag=1;
u8 RecorDataHour_Flag=0;
u8 RecorData5Min_Flag=0;
u16 RecorDataday_Flag=0;

//仪器参数设置相关变量
const u16 ADDress1[]={0x00FB,0x0102,0x0106,0x0108,0x010A,0x010C,0x0116,0x0118,0x011A,0x011C,0x011E,0x0120,0x0129,0x0132,0x013B};						 
const u8    REGNUM[]={     7,     4,     2,     2,     2,     2,     2,     2,     2,     2,     2,     9,     9,     9,     2};

//读寄存器
const u8              STATUS_TX_BUF[8]={0x32,0x03,0x00,0x78,0x00,0x01,0x01,0xD0};//当前状态

const u8                 SO2_TX_BUF[8]={0x32,0x03,0x00,0x79,0x00,0x02,0x10,0x11};//S02的浓度
const u8               SO2SD_TX_BUF[8]={0x32,0x03,0x00,0x7F,0x00,0x02,0xF0,0x10};//S02标准差

const u8           PMTDriveVal_TX_BUF[8]={0xFF,0x03,0x01,0x1C,0x00,0x04,0x91,0xED};//都PMT驱动电压与更换灯电压
const u8            Total_Parameter[8]={0xFF,0x03,0x00,0x81,0x00,0x16,0x81,0xF2};//多参数读取


const u8 STATUS_RX_BUF[64]={0}; 
//读仪器器件操作流程
const u8    deviceopratestatus_TX_BUF[6]={0x32,0x03,0x02,0x35,0x00,0x02}; 
const u8         samplingvalve_TX_BUF[6]={0x32,0x03,0x02,0x37,0x00,0x02}; 
const u8    Lightsourceshutter_TX_BUF[6]={0x32,0x03,0x02,0x39,0x00,0x02}; 
const u8              PMTlight_TX_BUF[6]={0x32,0x03,0x02,0x3B,0x00,0x02}; 
const u8     caseheatingswitch_TX_BUF[6]={0x32,0x03,0x02,0x3D,0x00,0x02};

//写相关参数
typedef union 
           {
						 float exdataFLOAT;
						  u8    tempdata[4];
					 }GET;
 GET GETDATA ={0};

 void Read_Total_Parameter (void)
 {
	u16 calCRC;
	u16 recCRC;
//	u16 RegNum;
//	u16 byteNum;
	u8 count=2;
	unsigned char i=0;
	unsigned char rs485buf[20];
	unsigned char revLen=0; 
	  for(i=0;i<8;i++)
			{
				rs485buf[i]=Total_Parameter[i];// 填充发送缓冲器
			}	
			RS485_Send_Data(rs485buf,8);
			delay_ms(500);
		RS485_Receive_Data(TotalPara_RX_BUF,&revLen);	
	   //  byteNum=TotalPara_RX_BUF[2];
	     if(49 == revLen)	
			 {
				calCRC=usMBCRC16(TotalPara_RX_BUF,revLen-2);	
			  recCRC = TotalPara_RX_BUF[revLen-2]|(((u16)TotalPara_RX_BUF[revLen-1])<<8);
		   if(calCRC == recCRC)	
			 { 
				 	GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];   //81
					GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];   //82
					GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
 SO2CONCERNRATIONCAL.PMTSampVL=GETDATA.exdataFLOAT;	//，PMT采样电压	 
					GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
 //DCPMESampVL=GETDATA.exdataFLOAT;	//暗电流采样状态下，PMT采样电压
				  GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
				  GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
				  GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
				  GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
 REFVOL_PD=GETDATA.exdataFLOAT;		//参考光强//GASFLOW,SO2CONCERNRATIONCAL.PMTSampVL,CaseTEM,TECTEM,REFVOL_PD,SO2ConCenTration
				   GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
				 	 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
           GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
				   GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
//REFVOL_PMT=GETDATA.exdataFLOAT;			//参考暗电流		
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
				   GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
//OVERALLD=GETDATA.exdataFLOAT;					//归一化值	 
				   GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
				 	 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
				 	 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
				   GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
GASFLOW=GETDATA.exdataFLOAT;					//气体流量,单位:sccm		
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
SampGasP=GETDATA.exdataFLOAT;					//采样气体的压力	
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
CaseGasPRESSURE=GETDATA.exdataFLOAT;					//反应室气体压力	 
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
CaseTEM=GETDATA.exdataFLOAT;		 	//反应室温度
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
CRAteTEM=GETDATA.exdataFLOAT;			 	//ji机箱温度
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
           GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
 TECTEM=GETDATA.exdataFLOAT;	//PMT温度			 
			 }
			 }				 
 }
 //读取驱动电压与更换灯电压
void PMT_Dri_Valage(void)
{
  u16 calCRC;
	u16 recCRC;
	u16 RegNum;
//	u16 byteNum;
//	u8 count=2;
	unsigned char i=0;
	unsigned char rs485buf[20];
	unsigned char revLen=0; 
  for(i=0;i<8;i++)
			{
				rs485buf[i]=PMTDriveVal_TX_BUF[i];// 填充发送缓冲器
			}	
			RS485_Send_Data(rs485buf,8);
			delay_ms(500);
		RS485_Receive_Data(PMTDriveVal_RX_BUF,&revLen);	
	    // byteNum=PMTDriveVal_RX_BUF[2];	
			 if(13 == revLen)	
			 {
				 calCRC=usMBCRC16(PMTDriveVal_RX_BUF,revLen-2);	
			  recCRC = PMTDriveVal_RX_BUF[revLen-2]|(((u16)PMTDriveVal_RX_BUF[revLen-1])<<8);
				 if(calCRC == recCRC)	
				 {
				  PMT_DriValage=((PMTDriveVal_RX_BUF[3]<<8)|PMTDriveVal_RX_BUF[4]);                                   //
					 
					GETDATA.tempdata[3]=PMTDriveVal_RX_BUF[7];
					GETDATA.tempdata[2]=PMTDriveVal_RX_BUF[8];
					GETDATA.tempdata[1]=PMTDriveVal_RX_BUF[9];
					GETDATA.tempdata[0]=PMTDriveVal_RX_BUF[10];
					 Change_Lamp_Val=GETDATA.exdataFLOAT;
				 }
			 }
}
 
 
 
 
void readscreen0(void)
{
	unsigned char i=0;
//	unsigned char rs485buf[64];
   OS_ERR err;
	char len1,len2,len3,len4,len5,len6,len7,len8,len9,len10,len11,len12;
   char FTOCHAR[20][20];  
//  float x1,x2,y1,y2;	
	
	if((READSCRREN.SCRREN0FLAG==1)||(READSCRREN.FristGetPowerFlag==0))
	{		
		 delay_ms(2); 	
	   len2=sprintf(FTOCHAR[2],"%.3f",CRAteTEM);        //机箱温度
		 len3=sprintf(FTOCHAR[3],"%.3f",CaseTEM);         //反应室温度
		 len4=sprintf(FTOCHAR[4],"%.3f",REFVOL_PD);       //参考光强
		 len5=sprintf(FTOCHAR[5],"%.3f",FiveMin_AVG_dis);       //采样压力
		 len6=sprintf(FTOCHAR[6],"%.3f",SampGasP);//采样气体压力
		 len7=sprintf(FTOCHAR[7],"%.3f",GASFLOW);  //气体流量    
		
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
		USART3_SendByte(0x12);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x00);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x02);			
		USART3_SendByte(len2>>8);	
		USART3_SendByte(len2&0xff);
			 for(i=0;i<len2;i++)
			{
				USART3_SendByte(FTOCHAR[2][i]);
			}
		USART3_SendByte(0x00);	
		USART3_SendByte(0x03);
		USART3_SendByte(len3>>8);	
		USART3_SendByte(len3&0xff);
					 for(i=0;i<len3;i++)
			{
				USART3_SendByte(FTOCHAR[3][i]);
			}
			
		USART3_SendByte(0x00);	
		USART3_SendByte(0x4);
		USART3_SendByte(len4>>8);	
		USART3_SendByte(len4&0xff);
					 for(i=0;i<len4;i++)
			{
				USART3_SendByte(FTOCHAR[4][i]);
			}
		USART3_SendByte(0x00);	
		USART3_SendByte(0x05);
		USART3_SendByte(len5>>8);	
		USART3_SendByte(len5&0xff);
					 for(i=0;i<len5;i++)
			{
				USART3_SendByte(FTOCHAR[5][i]);
			}
		USART3_SendByte(0x00);	
		USART3_SendByte(0x06);
		USART3_SendByte(len6>>8);	
		USART3_SendByte(len6&0xff);
					 for(i=0;i<len6;i++)
			{
				USART3_SendByte(FTOCHAR[6][i]);
			}
		USART3_SendByte(0x00);	
		USART3_SendByte(0x07);
		USART3_SendByte(len7>>8);	
		USART3_SendByte(len7&0xff);
					 for(i=0;i<len7;i++)
			{
				USART3_SendByte(FTOCHAR[7][i]);
			}
	    	END_CMD();
		
}
	 if(READSCRREN.SCRREN1FLAG==1)   //详细参数
	{	
	    	REFVOL_PD=  kalman(REFVOL_PD);
		   len1=sprintf(FTOCHAR[1],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL);      //测量光强
		
    	 len2=sprintf(FTOCHAR[2],"%.3f",DCPMESampVL);     //测量暗电流
		
  		 len3=sprintf(FTOCHAR[3],"%.3f",REFVOL_PD);       //参考光强
		
  		 len4=sprintf(FTOCHAR[4],"%.3f",REFVOL_PMT);      //参考暗电流
		
  		 len5=sprintf(FTOCHAR[5],"%.3f",GASFLOW);//气体流量
		
  		 len6=sprintf(FTOCHAR[6],"%.3f",SampGasP);       //采样压力	
	
  		 len7=sprintf(FTOCHAR[7],"%.3f",CaseGasPRESSURE);    //反应室压力
	
       len8=sprintf(FTOCHAR[8],"%.3f",CaseTEM); //反应室温度

       len9=sprintf(FTOCHAR[9],"%.3f",CRAteTEM);     //机箱温度
		   len10=sprintf(FTOCHAR[10],"%.3f",TECTEM);      //TEC温度
			if(Test_Catagry!=123456)
			{
				 len11=sprintf(FTOCHAR[11],"%.3f",SO2CONCERNRATIONCAL.SO2K);     //K
				 len12=sprintf(FTOCHAR[12],"%.3f",SO2CONCERNRATIONCAL.SO2B);      //B
			}
		  else
			{
     		  FTOCHAR[11][0]=' ';
				  FTOCHAR[12][0]=' ';
			   	len11=1;
				  len12=1;
			}
				 
    	OSMutexPend (&OSUart4Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
		   delay_ms(2); 
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //多控件发送的头
  		 USART3_SendByte(0x00);	USART3_SendByte(0x01);  	                   //界面1
  		 USART3_SendByte(0x00);	USART3_SendByte(0x01);                      //键控号1，测量光强
  		 USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//字符个数
					 for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[1][i]);                //发送测量光强
					}
					
//					
			USART3_SendByte(0x00);	  USART3_SendByte(0x02);                      //键控号2，测量暗电流
			USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//字符个数		
				 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[2][i]);                //发送数据 测量暗电流
					}	
//					
//					
			USART3_SendByte(0x00);	USART3_SendByte(0x03);                      //键控号3 参考光强
			USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//字符个数		
				 for(i=0;i<len3;i++)
					{
						USART3_SendByte(FTOCHAR[3][i]);                //发送数据 参考光强
					}	
//		
	    USART3_SendByte(0x00);	  USART3_SendByte(0x04);                      //键控号4，参考暗电流
			USART3_SendByte(len4>>8);	USART3_SendByte(len4&0xff);	//字符个数		
				 for(i=0;i<len4;i++)
					{
						USART3_SendByte(FTOCHAR[4][i]);                //发送数据 参考暗电流
					}			

			 USART3_SendByte(0x00);	  USART3_SendByte(0x05);                      //键控号5，  //气体流量
			USART3_SendByte(len5>>8);	USART3_SendByte(len5&0xff);	//字符个数		
				 for(i=0;i<len5;i++)
					{
						USART3_SendByte(FTOCHAR[5][i]);                //发送数据 气体流量
					}		

	  USART3_SendByte(0x00);	  USART3_SendByte(0x06);                      //键控号6，采样压力
			USART3_SendByte(len6>>8);	USART3_SendByte(len6&0xff);	//字符个数		
				 for(i=0;i<len6;i++)
					{
						USART3_SendByte(FTOCHAR[6][i]);                //发送数据 采样压力	
					}		

      USART3_SendByte(0x00);	  USART3_SendByte(0x07);                      //键控号07，反应室压力
			USART3_SendByte(len7>>8);	USART3_SendByte(len7&0xff);	//字符个数		
				 for(i=0;i<len7;i++)
					{
						USART3_SendByte(FTOCHAR[7][i]);                //发送数据 采样压力	
					}		
					
     USART3_SendByte(0x00);	  USART3_SendByte(0x08);                      //键控号8，反应室温度
			USART3_SendByte(len8>>8);	USART3_SendByte(len8&0xff);	//字符个数		
				 for(i=0;i<len8;i++)
					{
						USART3_SendByte(FTOCHAR[8][i]);                //发送数据 反应室温度
					}		
					
		  USART3_SendByte(0x00);	  USART3_SendByte(0x09);                      //键控号09，机箱温度
			USART3_SendByte(len9>>8);	USART3_SendByte(len9&0xff);	//字符个数		
				 for(i=0;i<len9;i++)
					{
						USART3_SendByte(FTOCHAR[9][i]);                //发送数据 机箱温度
					}		
					
			  USART3_SendByte(0x00);	  USART3_SendByte(0x0A);                      //键控号10，TEC温度
			USART3_SendByte(len10>>8);	USART3_SendByte(len10&0xff);	//字符个数		
				 for(i=0;i<len10;i++)
					{
						USART3_SendByte(FTOCHAR[10][i]);                //发送数据 TEC温度
					}		
					
	  USART3_SendByte(0x00);	  USART3_SendByte(0x0B);                      //键控号11，K
			USART3_SendByte(len11>>8);	USART3_SendByte(len11&0xff);	//字符个数		
				 for(i=0;i<len11;i++)
					{
						USART3_SendByte(FTOCHAR[11][i]);                //发送K
					}		
	     USART3_SendByte(0x00);	  USART3_SendByte(0x0C);                      //键控号12，B
			USART3_SendByte(len12>>8);	USART3_SendByte(len12&0xff);	//字符个数		
				 for(i=0;i<len12;i++)
					{
						USART3_SendByte(FTOCHAR[12][i]);                //发送B
					}		
							
					END_CMD();		
	OSMutexPost(&OSUart4Used_Mutex,OS_OPT_POST_NONE,&err);	
					
	}
	
}
	

int Calibration_SO2ConCenTration(void)
{
	 OS_ERR err;
	 float x1,x2,y1,y2;	
	
    char FTOCHAR[10][20],len1,len2,len3,len4,i; 
	
  if(READSCRREN.SCRREN6FLAG==1)   //标定界面
	{
			reqErrCnt[0]=0;//滑动滤波参数清零，0点个数
			reqErrCnt[1]=0;//滑动滤波参数清零，标气个数
			reqErrCnt[4]=0;//滑动滤波参数清零，0点个数
			reqErrCnt[5]=0;//滑动滤波参数清零，标气个数
//    OSIntEnter();
//				 	USART3_SendByte(0xEE);  //标定过程中
//		      USART3_SendByte(0xB1);		 
//		      USART3_SendByte(0x23);
//				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
//		      USART3_SendByte(0x00);		USART3_SendByte(0x61);
//	        USART3_SendByte(0x01);
//				  END_CMD();
//			    OSIntExit();
	//	READSCRREN.Record_SO2BKVFinshFlag=0;//每24小时产生一个正常稳态数值标志,jia校准时需要将此值清零
		len1=sprintf(FTOCHAR[6],"%.2f",GASFLOW); //GASFLOWq气体流量
		len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL); //PMT光强
	  	delay_ms(2); 
		OSIntEnter();  
		USART3_SendByte(0xEE);//EE B1 10 00 01 00 17 31 31 31 31 FF FC FF FF 
		USART3_SendByte(0xB1);
		USART3_SendByte(0x10);
		USART3_SendByte(0x00);	USART3_SendByte(0x06);
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
		USART3_SendByte(0x00);	USART3_SendByte(0x06);
	  USART3_SendByte(0x00);	USART3_SendByte(0x0F);
				for(i=0;i<len2;i++)
			{
				USART3_SendByte(FTOCHAR[7][i]);
			}
	    	END_CMD();
		OSIntExit();  //阻止被打断
			
			
	 if(READSCRREN.ZeroCALFLAG==1) //开始零点校准
	 {
		 SO2CONCERNRATIONCAL.PMTSampVL_ZERO=SO2CONCERNRATIONCAL.PMTSampVL;//跟新零点。(X1,0)
		 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL_ZERO); //PMT光强mv
		 delay_ms(2); 
		  OSIntEnter();
		 USART3_SendByte(0xEE);//EE B1 10 00 01 00 04 FF FC FF FF 
		 USART3_SendByte(0xB1);
		 USART3_SendByte(0x10);
		 USART3_SendByte(0x00);	USART3_SendByte(0x06);
		 USART3_SendByte(0x00);	USART3_SendByte(0x04);
		 	for(i=0;i<len2;i++)
			{
				USART3_SendByte(FTOCHAR[7][i]);
			}
				      END_CMD();	
          OSIntExit(); 			
      		   	delay_ms(2); 
			    OSIntEnter();
				 	USART3_SendByte(0xEE);  //标定过程中
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
		      USART3_SendByte(0x00);		USART3_SendByte(0x37);
	        USART3_SendByte(0x01);
				  END_CMD();
			    OSIntExit(); 
		     	TimeMeasCount=300; //等待5分钟
			 while((StopFlg==0)&&TimeMeasCount)
			 {
					//跟新零点光强 
				 len1=sprintf(FTOCHAR[6],"%.2f",GASFLOW);//1E
				 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL); //PMT光强0F
				// len3=sprintf(FTOCHAR[8],"%.3f",SO2ConCenTration_AfterCal); //PMT光强0E
				    delay_ms(2); 
				 OSIntEnter(); 
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //多控件发送的头
  		 USART3_SendByte(0x00);	USART3_SendByte(0x06);  	                   //界面1
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
//			USART3_SendByte(0x00);	USART3_SendByte(0x0E);                      //键控号0E 参考光强
//			USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//字符个数		
//				 for(i=0;i<len3;i++)
//					{
//						USART3_SendByte(FTOCHAR[8][i]);                //发送数据 参考光强
//					}		
					END_CMD();		
				 OSIntExit();
					
					SO2CONCERNRATIONCAL.PMTSampVL_ZERO =filtStack (&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[4],&reqErrCnt[4],1);//跟新零点。(X1,0	
			   
					OSIntEnter(); 
					 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL_ZERO); //PMT光强mv
					 delay_ms(2); 
					 USART3_SendByte(0xEE);//EE B1 10 00 01 00 04 FF FC FF FF 
					 USART3_SendByte(0xB1);
					 USART3_SendByte(0x10);
					 USART3_SendByte(0x00);	USART3_SendByte(0x06);
					 USART3_SendByte(0x00);	USART3_SendByte(0x04);
						for(i=0;i<len2;i++)
						{
							USART3_SendByte(FTOCHAR[7][i]);
						}
				      END_CMD();
							OSIntExit(); 
         
						SO2CONCERNRATIONCAL.COV_ZERO =filtStack (&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[0],&reqErrCnt[0],0);
			   	OSTimeDly(200,OS_OPT_TIME_PERIODIC,&err);	//200个时间节拍，1S更新一次
						if((SO2CONCERNRATIONCAL.COV_ZERO<0.02)&&(SO2CONCERNRATIONCAL.COV_ZERO>0)) 
		     {
		       READSCRREN.ZeroCALFLAG=0;
				   delay_ms(2); 
				   USART3_SendByte(0xEE);  //校准成功
		       USART3_SendByte(0xB1);		 
		       USART3_SendByte(0x23);
				   USART3_SendByte(0x00);		USART3_SendByte(0x06);
		       USART3_SendByte(0x00);		USART3_SendByte(0x37);
	         USART3_SendByte(0x02);
				   END_CMD();		
          	TimeMeasCount=0;				 
		     }
			 }	

				if(SO2CONCERNRATIONCAL.COV_ZERO>0.02) 
		     {
		   		READSCRREN.ZeroCALFLAG=0;
				  delay_ms(2); 
				 	USART3_SendByte(0xEE);  //校准shi失败
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
		      USART3_SendByte(0x00);		USART3_SendByte(0x37);
	        USART3_SendByte(0x03);
				  END_CMD();
		     }
		}
	
	 if((READSCRREN.StadagasCALFLAG==1)&&(READSCRREN.SO2STARDCON_INPUT_Flag==1))//一个是标气开始标志位，一个是浓度输入完毕标志
	 {
		 //更新标气光强
		 	SO2CONCERNRATIONCAL.PMTSampVL_STADARD=SO2CONCERNRATIONCAL.PMTSampVL;//(X2,Y2)
		 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL_STADARD); //PMT光强mv	
		 delay_ms(2); 
		 
		 USART3_SendByte(0xEE);//EE B1 10 00 01 00 09 FF FC FF FF 
		 USART3_SendByte(0xB1);
		 USART3_SendByte(0x10);
		 USART3_SendByte(0x00);	USART3_SendByte(0x06);
		 USART3_SendByte(0x00);	USART3_SendByte(0x09);
		 	for(i=0;i<len2;i++)
			{
				USART3_SendByte(FTOCHAR[7][i]);
			}
				END_CMD();
			
			 delay_ms(2); 
			USART3_SendByte(0xEE);  //标定过程中
			USART3_SendByte(0xB1);		 
			USART3_SendByte(0x23);
			USART3_SendByte(0x00);		USART3_SendByte(0x06);
			USART3_SendByte(0x00);		USART3_SendByte(0x0A);
			USART3_SendByte(0x01);
			END_CMD();			
			TimeMeasCount=300; //等待5分钟		
      while((StopFlg==0)&&TimeMeasCount)
			 {	
				 len1=sprintf(FTOCHAR[6],"%.2f",GASFLOW);//1E
				 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL); //PMT光强0F
				 //len3=sprintf(FTOCHAR[8],"%.3f",SO2ConCenTration_AfterCal); //PMT光强0E
				    delay_ms(2); 
				 
				  OSIntEnter();
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //多控件发送的头
  		 USART3_SendByte(0x00);	USART3_SendByte(0x06);  	                   //界面1
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
//			USART3_SendByte(0x00);	USART3_SendByte(0x0E);                      //键控号0E 参考光强
//			USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//字符个数		
//				 for(i=0;i<len3;i++)
//					{
//						USART3_SendByte(FTOCHAR[8][i]);                //发送数据 参考光强
//					}		
					END_CMD();		
					OSIntExit(); 
					
					 SO2CONCERNRATIONCAL.PMTSampVL_STADARD =filtStack (&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[5],&reqErrCnt[5],1);

					 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL_STADARD); //PMT光强mv	
					  delay_ms(2); 
					  
					   OSIntEnter();
					   USART3_SendByte(0xEE);//EE B1 10 00 01 00 09 FF FC FF FF 
						 USART3_SendByte(0xB1);
						 USART3_SendByte(0x10);
						 USART3_SendByte(0x00);	USART3_SendByte(0x06);
						 USART3_SendByte(0x00);	USART3_SendByte(0x09);
							for(i=0;i<len2;i++)
							{
								USART3_SendByte(FTOCHAR[7][i]);
							}
								END_CMD();
							OSIntExit();
							
	        SO2CONCERNRATIONCAL.COV_STANDARD =filtStack (&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[1],&reqErrCnt[1],0);
			   	OSTimeDly(200,OS_OPT_TIME_PERIODIC,&err);	//200个时间节拍，1S更新一次		
  		
				if((SO2CONCERNRATIONCAL.COV_STANDARD<0.02)&&(SO2CONCERNRATIONCAL.COV_STANDARD>0)) 
		     {
		       //READSCRREN.ZeroCALFLAG=0;
					 READSCRREN.StadagasCALFLAG=0;//校准成功
				   delay_ms(2); 
				  	USART3_SendByte(0xEE);  //校准成功
		       USART3_SendByte(0xB1);		 
		       USART3_SendByte(0x23);
				   USART3_SendByte(0x00);		USART3_SendByte(0x06);
		       USART3_SendByte(0x00);		USART3_SendByte(0x0A);
	         USART3_SendByte(0x02);
				   END_CMD();		
          	TimeMeasCount=0;	
					  READSCRREN.CACULATEKBFLAG=1; //标气校准完成代表所有都已经完成
		     }
			 }	

				if(SO2CONCERNRATIONCAL.COV_STANDARD>0.02) 
		     {
		   		READSCRREN.StadagasCALFLAG=0;
				  delay_ms(2); 
				 	USART3_SendByte(0xEE);  //校准shi失败
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
		      USART3_SendByte(0x00);		USART3_SendByte(0x0A);
	        USART3_SendByte(0x03);
				  END_CMD();
		     }
	 }

//		 //此刻数据开始计算K,B值		  
	 	 if(READSCRREN.CACULATEKBFLAG==1)
		 {
			 READSCRREN.CACULATEKBFLAG=0;
			  x1=SO2CONCERNRATIONCAL.PMTSampVL_ZERO;
        y1=0;
			 x2=SO2CONCERNRATIONCAL.PMTSampVL_STADARD;
       y2=SO2CONCERNRATIONCAL.SO2STARDCON_INPUT; 
			 SO2CONCERNRATIONCAL.SO2K =(y2-y1)/(x2-x1);              //计算K.B值
			 SO2CONCERNRATIONCAL.SO2B =-SO2CONCERNRATIONCAL.SO2K*x1;		
       //校准完成保存自动校准功能的标志位			 
  //	 SO2CONCERNRATIONCAL.Seek_SO2BGV[0]=SO2CONCERNRATIONCAL.SO2K*SO2CONCERNRATIONCAL.PMTSampVL+SO2CONCERNRATIONCAL.SO2B;		 
		  len1=sprintf(FTOCHAR[8],"%.3f",SO2CONCERNRATIONCAL.SO2K);        //浮点数转化成字符
		  len2=sprintf(FTOCHAR[9],"%.3f",SO2CONCERNRATIONCAL.SO2B);         //
			  delay_ms(2);
			 OSIntEnter();
				USART3_SendByte(0xEE);
				USART3_SendByte(0xB1);	
				USART3_SendByte(0x12);
				USART3_SendByte(0x00);	
				USART3_SendByte(0x06);
				USART3_SendByte(0x00);	//K位置
				USART3_SendByte(0x0C);			
				USART3_SendByte(len1>>8);	
				USART3_SendByte(len1&0xff);
					 for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[8][i]);
					}
				USART3_SendByte(0x00);//b位置	
				USART3_SendByte(0x0D);
				USART3_SendByte(len2>>8);	
				USART3_SendByte(len2&0xff);
							 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[9][i]);
					}
	    	  END_CMD();
					OSIntExit();
						delay_ms(2);
		//在下位机存k,b
					OSIntEnter();
   			 SaveEpData(0x00,SO2CONCERNRATIONCAL.SO2K*1000); //8个字节。address：0~7;
				 SaveEpData(0x08,SO2CONCERNRATIONCAL.SO2B*1000);// 8g个字节。address:8~15		
//					Var.AfterCALFlag=1;
//					SaveEpData(360,Var.AfterCALFlag);
					OSIntExit();
					Event_Record(1);//校准完成，记录一下校准记录
		   }
		 READSCRREN.IntialonceFlag=0;//只发送一次，等待校准
		 } 
	else
	   {  
			 if(READSCRREN.IntialonceFlag==0)
			 {
				READSCRREN.IntialonceFlag=1;
			 	 OSIntEnter(); 
			     delay_ms(2); 
				 	USART3_SendByte(0xEE);  //等待校准
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
		      USART3_SendByte(0x00);		USART3_SendByte(0x37);
	        USART3_SendByte(0x00);
				  END_CMD();
		    //	OSIntExit();
			   	   	delay_ms(2);
			   //  OSIntEnter();
				 	USART3_SendByte(0xEE);  //等待校准
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
		      USART3_SendByte(0x00);		USART3_SendByte(0x0A);
	        USART3_SendByte(0x00);
				  END_CMD();
			    OSIntExit();
			 }
		 }
}

int Read_SO2ConCenTration(void)
{
	   u8 *pp,So2Len,i=1,j;
	   pp=(u8*)mymalloc(20);
			if(Test_Catagry==123456) //多点校准
					{
						for(i=1;i<Startup;i++)
								 {
									 if(SO2CONCERNRATIONCAL.PMTSampVL<=AMulti[i].PMTSampVL)
												 {
																SO2ConCenTration=AMulti[i].k*SO2CONCERNRATIONCAL.PMTSampVL  + AMulti[i].b ;      
												 }											 
												 break;
								 }
						if(SO2CONCERNRATIONCAL.PMTSampVL>AMulti[Startup-1].PMTSampVL)
							     SO2ConCenTration=AMulti[Startup-1].k*SO2CONCERNRATIONCAL.PMTSampVL  + AMulti[Startup-1].b ; 
					
					}
			else      //线性校准
						 SO2ConCenTration=SO2CONCERNRATIONCAL.SO2K *SO2CONCERNRATIONCAL.PMTSampVL  +SO2CONCERNRATIONCAL.SO2B;
			 
//			 if(SO2ConCenTration<-20)
//						 SO2ConCenTration=0;	
			 if(Var.Add_AfterCALFlag==0x01)//加入offset value;
		{
			
	
		  SO2ConCenTration_AfterCal=filtStack(&SO2ConCenTration,elmResultBuf[2],&reqErrCnt[2],1)+Var.OffSet;
			
		}
		else
		{
		  SO2ConCenTration_AfterCal=filtStack(&SO2ConCenTration,elmResultBuf[2],&reqErrCnt[2],1);
			
		}
		
	    SO2ConCenTration_AfterCal_W = SO2ConCenTration_AfterCal*2.62;

			 if(categroy_change==2)
			 {	
					So2Len=sprintf(pp,"%.3f",SO2ConCenTration_AfterCal_W);
			 }
			 else
			 {
					So2Len=sprintf(pp,"%.3f",SO2ConCenTration_AfterCal);
			 }
		
			 OSIntEnter(); 
     USART3_SendByte(0xEE); USART3_SendByte(0xB1);   USART3_SendByte(0x10);                    //EE B1 10 00 01 00 09 FF FC FF FF 	
		 USART3_SendByte(0x00);	USART3_SendByte(0x00);   USART3_SendByte(0x00);	USART3_SendByte(0x01); //screen 00, control 01,0x15
		 	for(i=0;i<So2Len;i++)
			{ 
				USART3_SendByte(pp[i]);
			}
				END_CMD();
      OSIntExit();	
		
      for(j=1;j<10;j++)	
     {			
			   delay_ms(2);
			 OSIntEnter(); 
			 USART3_SendByte(0xEE); USART3_SendByte(0xB1);   USART3_SendByte(0x10);                    //EE B1 10 00 01 00 09 FF FC FF FF 	
		 USART3_SendByte(0x00);	USART3_SendByte(j);   USART3_SendByte(0x00);	USART3_SendByte(0x0E); //screen 00, control 01,0x15
		 	for(i=0;i<So2Len;i++)
			{
				USART3_SendByte(pp[i]);
			}
				END_CMD();	
			OSIntExit();	
		}
				 OSIntEnter(); 
		USART3_SendByte(0xEE); USART3_SendByte(0xB1);   USART3_SendByte(0x10);                    //EE B1 10 00 01 00 09 FF FC FF FF 	
		 USART3_SendByte(0x00);	USART3_SendByte(0x0B);   USART3_SendByte(0x00);	USART3_SendByte(0x0E); //screen 00, control 01,0x15
		 	for(i=0;i<So2Len;i++)
			{
				USART3_SendByte(pp[i]);
			}
				END_CMD();	
			OSIntExit();	
	     myfree(pp);
	    return 0;
}	
	
void Set_Warn(void)
{
	unsigned char i=0;
//	unsigned char rs485buf[64];
//   OS_ERR err;
	char W_len1,W_len2,W_len3,W_len4,W_len5,W_len6,W_len7,W_len8,W_len9,W_len10,W_len11,W_len12,W_len13,W_len14,W_len15,W_len16,W_len17,W_len18,W_len19,W_len20;
		char FTOCHAR1[21][10]={{0,0}};  
	     W_len1=sprintf(FTOCHAR1[1],"%.3f",Input.CRAteTEM_LowerLimit);     //机箱温度上下限
    	 W_len2=sprintf(FTOCHAR1[2],"%.3f",Input.CRAteTEM_UpperLimit);   	
  		 W_len3=sprintf(FTOCHAR1[3],"%.3f",Input.TECTEM_LowerLimit);     //PMT温度上下限
  		 W_len4=sprintf(FTOCHAR1[4],"%.3f",Input.TECTEM_UpperLimit);     
  		 W_len5=sprintf(FTOCHAR1[5],"%.3f",Input.CaseTEM_LowerLimit);//反应室温度上下限
  		 W_len6=sprintf(FTOCHAR1[6],"%.3f",Input.CaseTEM_UpperLimit);       
  		 W_len7=sprintf(FTOCHAR1[7],"%.3f",Input.SampGasP_LowerLimit);    //采样压力
       W_len8=sprintf(FTOCHAR1[8],"%.3f",Input.SampGasP_UpperLimit); 
       W_len9=sprintf(FTOCHAR1[9],"%.3f",Input.CaseGasPRESSURE_LowerLimit);   //采样压力   
		   W_len10=sprintf(FTOCHAR1[10],"%.3f",Input.CaseGasPRESSURE_UpperLimit);      
			 W_len11=sprintf(FTOCHAR1[11],"%.3f",Input.GASFLOW_LowerLimit);    //气体流量 
		   W_len12=sprintf(FTOCHAR1[12],"%.3f",Input.GASFLOW_UpperLimit);    
	     W_len13=sprintf(FTOCHAR1[13],"%.3f",Input.PMTSampVL_LowerLimit);   //测量光强
		   W_len14=sprintf(FTOCHAR1[14],"%.3f",Input.PMTSampVL_UpperLimit);     
			 W_len15=sprintf(FTOCHAR1[15],"%.3f",Input.REFVOL_PD_LowerLimit);  //参考光强 
		   W_len16=sprintf(FTOCHAR1[16],"%.3f",Input.REFVOL_PD_UpperLimit);     
	     W_len17=sprintf(FTOCHAR1[17],"%.3f",Input.REFVOL_PMT_LowerLimit);   //参考暗电流 
		   W_len18=sprintf(FTOCHAR1[18],"%.3f",Input.REFVOL_PMT_UpperLimit);     
			 W_len19=sprintf(FTOCHAR1[19],"%.3f",Input.VEF_LowerLimit);         //基准光源
		   W_len20=sprintf(FTOCHAR1[20],"%.3f",Input.VEF_UpperLimit);    
	     	OSIntEnter();                
	//	OSMutexPend (&OSUart4Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
				delay_ms(10); 
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //多控件发送的头
  		 USART3_SendByte(0x00);	USART3_SendByte(0x08);  	                   //界面1
  		 USART3_SendByte(0x00);	USART3_SendByte(0x02);                      //键控号2,机箱温度上下限
  		 USART3_SendByte(W_len1>>8);	USART3_SendByte(W_len1&0xff);	//字符个数
					 for(i=0;i<W_len1;i++)
					{
						USART3_SendByte(FTOCHAR1[1][i]);                     //      机箱温度下限
					}
					
//					
			USART3_SendByte(0x00);	  USART3_SendByte(0x05);          //控件号5    
			USART3_SendByte(W_len2>>8);	USART3_SendByte(W_len2&0xff);	
				 for(i=0;i<W_len2;i++)
					{
						USART3_SendByte(FTOCHAR1[2][i]);                   //机箱温度上限
					}	
//					
//					
			USART3_SendByte(0x00);	USART3_SendByte(0x09);                              //控件号 9          
			USART3_SendByte(W_len3>>8);	USART3_SendByte(W_len3&0xff);	
				 for(i=0;i<W_len3;i++)
					{
						USART3_SendByte(FTOCHAR1[3][i]);                   //PMT温度上下限
					}	
//		
	    USART3_SendByte(0x00);	  USART3_SendByte(0x0A);                     
			USART3_SendByte(W_len4>>8);	USART3_SendByte(W_len4&0xff);   
				 for(i=0;i<W_len4;i++)
					{
						USART3_SendByte(FTOCHAR1[4][i]);                //PMT温度上下限
					}			

			 USART3_SendByte(0x00);	  USART3_SendByte(26);                     
			USART3_SendByte(W_len5>>8);	USART3_SendByte(W_len5&0xff);
				 for(i=0;i<W_len5;i++)
					{
						USART3_SendByte(FTOCHAR1[5][i]);              
					}		

	  USART3_SendByte(0x00);	  USART3_SendByte(27);                    
			USART3_SendByte(W_len6>>8);	USART3_SendByte(W_len6&0xff);	
				 for(i=0;i<W_len6;i++)
					{
						USART3_SendByte(FTOCHAR1[6][i]);               
					}		

      USART3_SendByte(0x00);	  USART3_SendByte(17);                     
			USART3_SendByte(W_len7>>8);	USART3_SendByte(W_len7&0xff);
				 for(i=0;i<W_len7;i++)
					{
						USART3_SendByte(FTOCHAR1[7][i]);               
					}		
					
     USART3_SendByte(0x00);	  USART3_SendByte(18);                      
			USART3_SendByte(W_len8>>8);	USART3_SendByte(W_len8&0xff);		
				 for(i=0;i<W_len8;i++)
					{
						USART3_SendByte(FTOCHAR1[8][i]);               
					}		
					
		  USART3_SendByte(0x00);	  USART3_SendByte(13);                     
			USART3_SendByte(W_len9>>8);	USART3_SendByte(W_len9&0xff);			
				 for(i=0;i<W_len9;i++)
					{
						USART3_SendByte(FTOCHAR1[9][i]);                
					}		
					
			  USART3_SendByte(0x00);	  USART3_SendByte(15);                     
			USART3_SendByte(W_len10>>8);	USART3_SendByte(W_len10&0xff);		
				 for(i=0;i<W_len10;i++)
					{
						USART3_SendByte(FTOCHAR1[10][i]);               
					}		
					
	  USART3_SendByte(0x00);	  USART3_SendByte(31);                    
			USART3_SendByte(W_len11>>8);	USART3_SendByte(W_len11&0xff);	
				 for(i=0;i<W_len11;i++)
					{
						USART3_SendByte(FTOCHAR1[11][i]);                
					}		
	     USART3_SendByte(0x00);	  USART3_SendByte(32);                   
			USART3_SendByte(W_len12>>8);	USART3_SendByte(W_len12&0xff);	
				 for(i=0;i<W_len12;i++)
					{
						USART3_SendByte(FTOCHAR1[12][i]);               
					}		
	     USART3_SendByte(0x00);	  USART3_SendByte(36);                     
			USART3_SendByte(W_len13>>8);	USART3_SendByte(W_len13&0xff);		
				 for(i=0;i<W_len13;i++)
					{
						USART3_SendByte(FTOCHAR1[13][i]);               
					}		
					
		     USART3_SendByte(0x00);	  USART3_SendByte(38);                     
			USART3_SendByte(W_len14>>8);	USART3_SendByte(W_len14&0xff);	
				 for(i=0;i<W_len14;i++)
					{
						USART3_SendByte(FTOCHAR1[14][i]);              
					}		
					
	     USART3_SendByte(0x00);	  USART3_SendByte(40);                     
			USART3_SendByte(W_len15>>8);	USART3_SendByte(W_len15&0xff);	
				 for(i=0;i<W_len15;i++)
					{
						USART3_SendByte(FTOCHAR1[15][i]);               
					}		
					
	     USART3_SendByte(0x00);	  USART3_SendByte(41);                     
			USART3_SendByte(W_len16>>8);	USART3_SendByte(W_len16&0xff);		
				 for(i=0;i<W_len16;i++)
					{
						USART3_SendByte(FTOCHAR1[16][i]);                
					}		
					
		     USART3_SendByte(0x00);	  USART3_SendByte(43);                     
			USART3_SendByte(W_len17>>8);	USART3_SendByte(W_len17&0xff);	
				 for(i=0;i<W_len17;i++)
					{
						USART3_SendByte(FTOCHAR1[17][i]);               
					}		
	     USART3_SendByte(0x00);	  USART3_SendByte(44);                     
			USART3_SendByte(W_len18>>8);	USART3_SendByte(W_len18&0xff);		
				 for(i=0;i<W_len18;i++)
					{
						USART3_SendByte(FTOCHAR1[18][i]);               
					}	
		     USART3_SendByte(0x00);	  USART3_SendByte(46);                    
			USART3_SendByte(W_len19>>8);	USART3_SendByte(W_len19&0xff);	
				 for(i=0;i<W_len19;i++)
					{
						USART3_SendByte(FTOCHAR1[19][i]);              
					}		
			     USART3_SendByte(0x00);	  USART3_SendByte(47);                    
			USART3_SendByte(W_len20>>8);	USART3_SendByte(W_len20&0xff);	
				 for(i=0;i<W_len20;i++)
					{
						USART3_SendByte(FTOCHAR1[20][i]);             
					}							
					END_CMD();		
	//	OSMutexPost(&OSUart4Used_Mutex,OS_OPT_POST_NONE,&err);
					OSIntExit(); 
}

void Read_SetWarn(void)  //读取报警设置的上下限
{
	         Input.CRAteTEM_LowerLimit  =      (float)ReadEpData(30)/1000;  //机箱温度上下限
	         Input.CRAteTEM_UpperLimit  =      (float)ReadEpData(38)/1000;  //
						Input.TECTEM_LowerLimit   =      (float)ReadEpData(46)/1000;  
						Input.TECTEM_UpperLimit   =      (float)ReadEpData(54)/1000;  
						Input.CaseTEM_LowerLimit =       (float)ReadEpData(62)/1000;  
						Input.CaseTEM_UpperLimit=        (float)ReadEpData(70)/1000;  
						Input.SampGasP_LowerLimit=       (float)ReadEpData(78)/1000;  
						Input.SampGasP_UpperLimit=       (float)ReadEpData(86)/1000;  
						Input.CaseGasPRESSURE_LowerLimit=(float)ReadEpData(94)/1000;  
						Input.CaseGasPRESSURE_UpperLimit=(float)ReadEpData(102)/1000;  
						Input.GASFLOW_LowerLimit=         (float)ReadEpData(110)/1000;  
						Input.GASFLOW_UpperLimit=         (float)ReadEpData(118)/1000;  
						Input.PMTSampVL_LowerLimit=       (float)ReadEpData(128)/1000;  
						Input.PMTSampVL_UpperLimit=       (float)ReadEpData(136)/1000;  
						Input.REFVOL_PD_LowerLimit=       (float)ReadEpData(144)/1000;  
						Input.REFVOL_PD_UpperLimit=       (float)ReadEpData(152)/1000;  
						Input.REFVOL_PMT_LowerLimit=      (float)ReadEpData(160)/1000;  
						Input.REFVOL_PMT_UpperLimit=      (float)ReadEpData(168)/1000;  
						Input.VEF_LowerLimit=             (float)ReadEpData(176)/1000;  
						Input.VEF_UpperLimit=             (float)ReadEpData(184)/1000;  
}
void CheckWarn (void)
{
	         //机箱温度
						if((Input.CRAteTEM_LowerLimit <CRAteTEM )&&  ( CRAteTEM<Input.CRAteTEM_UpperLimit))
						{
								if(readwarmingINF.warmingcaseTEM_Flag==1)
								{
									 delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x00);		                    //数据00解除.机箱报警
									 END_CMD();
									Icon_Display(0);  //消除报警后的绿色通行
									 OSIntExit();	
									 readwarmingINF.warmingcaseTEM_Flag=0; 	
								}	
						}
						else 
						{
								 if(readwarmingINF.warmingcaseTEM_Flag==0)
								 {
									 delay_ms(10);
                   OSIntEnter(); 									 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //告警设置的头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x00);		                    //数据00.机箱报警
									 END_CMD(); 
									 Icon_Display(1);   //报警后的红色警报
									   OSIntExit();
									 readwarmingINF.warmingcaseTEM_Flag=1;
								 }
	          	}
						
					//PMT温度		
	      if((Input.TECTEM_LowerLimit <TECTEM)&&(TECTEM<Input.TECTEM_UpperLimit))
				{
					if(readwarmingINF.warmingPMTTEM_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);		                    //数据01，PMT温度解除报警
									 END_CMD();
							 Icon_Display(0);  //消除报警后的绿色通行
									 OSIntExit();	
						readwarmingINF.warmingPMTTEM_Flag=0;
					}
				}
				else
				{
					if(readwarmingINF.warmingPMTTEM_Flag==0)
					{
						    delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);		                    //数据01，PMT温度报警
									 END_CMD();
							 Icon_Display(1);   //报警后的红色警报
									 OSIntExit();
						  readwarmingINF.warmingPMTTEM_Flag=1;
					}
				}
       //反应室温度		
    if((Input.CaseTEM_LowerLimit <CaseTEM)&&(CaseTEM<Input.CaseTEM_UpperLimit))
				{
					
					if(readwarmingINF.warmingcasereactionTEM_Flag==1)
					{
						      delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x02);		                    //数据02.反应室温度解除报警
									 END_CMD();
							Icon_Display(0);;  //消除报警后的绿色通行
									 OSIntExit();	
						 readwarmingINF.warmingcasereactionTEM_Flag=0;
					}
				}
				else
				{
					if(readwarmingINF.warmingcasereactionTEM_Flag==0)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x02);		                    //数据02.反应室温度报警
									 END_CMD();
							  Icon_Display(1);  //报警后的红色警报
									 OSIntExit();	
						   readwarmingINF.warmingcasereactionTEM_Flag=1;
					}
				}
//采样压力
			  if((Input.SampGasP_LowerLimit <SampGasP)&&(SampGasP<Input.SampGasP_UpperLimit))
				{
					if(readwarmingINF.warmingSamplingpressure_Flag==1)
					{
						      delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x03);		                    //数据03.采样压力解除报警
									  END_CMD();
						  	Icon_Display(0);  //消除报警后的绿色通行
									 OSIntExit();	
						  readwarmingINF.warmingSamplingpressure_Flag=0;
					}
				}
				else
				{
					if(readwarmingINF.warmingSamplingpressure_Flag==0)
					{
						       delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x03);		                    //数据03.采样压力报警
									  END_CMD();
							  Icon_Display(1);   //报警后的红色警报
									 OSIntExit();	
						      readwarmingINF.warmingSamplingpressure_Flag=1;
					}
				}	
//反应室压力
			  if((Input.CaseGasPRESSURE_LowerLimit <CaseGasPRESSURE)&&(CaseGasPRESSURE<Input.CaseGasPRESSURE_UpperLimit))
				{
					if(readwarmingINF.casereactionpressure_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x04);		                    //数据00.反应室压力解除
									 END_CMD();
							  Icon_Display(0);  //消除报警后的绿色通行
									 OSIntExit();	
						readwarmingINF.casereactionpressure_Flag=0;
					}
				}
				else
				{
					if(readwarmingINF.casereactionpressure_Flag==0)
					{
						       delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x04);		                    //数据00.反应室压力报警
									 END_CMD();
							  Icon_Display(1);   //报警后的红色警报
									 OSIntExit();	
						readwarmingINF.casereactionpressure_Flag=1;
					}
				}		
//采样流量
 if((Input.GASFLOW_LowerLimit <GASFLOW)&&(GASFLOW<Input.GASFLOW_UpperLimit))
				{
					if(readwarmingINF.warmingSamplingflow_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);		                    //数据05，采样流量报警解除
									 END_CMD();
							Icon_Display(0);;  //消除报警后的绿色通行
									 OSIntExit();	
						readwarmingINF.warmingSamplingflow_Flag=0;
					}
				}
				else
				{
					if(readwarmingINF.warmingSamplingflow_Flag==0)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);		                    //数据05，采样流量报警
									 END_CMD();
							  Icon_Display(1);   //报警后的红色警报
									 OSIntExit();	
						readwarmingINF.warmingSamplingflow_Flag=1;
					}
				}		
//测量光强
	if((Input.PMTSampVL_LowerLimit <SO2CONCERNRATIONCAL.PMTSampVL)&&(SO2CONCERNRATIONCAL.PMTSampVL<Input.PMTSampVL_UpperLimit))
				{
					if(readwarmingINF.warminglightintensityVolt_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x06);		                    //数据06，测量光强解除报警
									 END_CMD();
						  	Icon_Display(0); //消除报警后的绿色通行
									 OSIntExit();	
						readwarmingINF.warminglightintensityVolt_Flag=0;
					}
				}
				else
				{
					if(readwarmingINF.warminglightintensityVolt_Flag==0)
					{
						       delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x06);		                    //数据06，测量光强报警
									 END_CMD();
							  Icon_Display(1);  //报警后的红色警报
									 OSIntExit();	
						       readwarmingINF.warminglightintensityVolt_Flag=1;
					}
				}			
//参考光强
if((Input.REFVOL_PD_LowerLimit <REFVOL_PD)&&(REFVOL_PD<Input.REFVOL_PD_UpperLimit))
				{
					if(readwarmingINF.warmingREFVOL_PD_Flag==1)
					{
						     delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x07);		                    //数据07,参考光强报警解除
									 END_CMD();
						Icon_Display(0);  //消除报警后的绿色通行
									 OSIntExit();	
						readwarmingINF.warmingREFVOL_PD_Flag=0;
					}
				}
				else
				{
					if(readwarmingINF.warmingREFVOL_PD_Flag==0)
					{
						       delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x07);		                    //数据07,参考光强报警
									 END_CMD();
							 Icon_Display(1);   //报警后的红色警报
									 OSIntExit();	
						readwarmingINF.warmingREFVOL_PD_Flag=1;
					}
				}			

//参考暗电流
if((Input.REFVOL_PMT_LowerLimit <REFVOL_PMT)&&(REFVOL_PMT<Input.REFVOL_PMT_UpperLimit))
				{
					if(readwarmingINF.warmingREFVOL_PMT_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x08);		                    //数据08，参考暗电流警告解除
									 END_CMD();
							Icon_Display(0);  //消除报警后的绿色通行
									 OSIntExit();	
						readwarmingINF.warmingREFVOL_PMT_Flag=0;
					}
				}
				else
				{
					if(readwarmingINF.warmingREFVOL_PMT_Flag==0)
					{
							    delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
									 USART3_SendByte(0x00);	USART3_SendByte(0x08);		                    //数据08，参考暗电流警告
									 END_CMD();
							  Icon_Display(1);   //报警后的红色警报
									 OSIntExit();	
						readwarmingINF.warmingREFVOL_PMT_Flag=1;
					}
				}			
//基准光强
			//	if((Input.VEF_LowerLimit <Change_Lamp_Val)&&(Change_Lamp_Val<Input.VEF_UpperLimit))
			   if(Input.VEF_LowerLimit <=Change_Lamp_Val)
				  {
							if(readwarmingINF.warmingChangePD_Flag==1)
							{
											 delay_ms(10); 
											 OSIntEnter(); 
											 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //解除告警头文件
											 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
											 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
											 USART3_SendByte(0x00);	USART3_SendByte(0x09);		                    //数据09，基准光强解除报警
											 END_CMD();
									Icon_Display(0);  //消除报警后的绿色通行
											 OSIntExit();	
											 readwarmingINF.warmingChangePD_Flag=0;
					}
				}
				else
				{
					if(readwarmingINF.warmingChangePD_Flag==0)
					{
						 delay_ms(10); 
											 OSIntEnter(); 
											 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //解除告警头文件
											 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //界面5
											 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //控件1
											 USART3_SendByte(0x00);	USART3_SendByte(0x09);		                    //数据09，基准光强报警
											 END_CMD();
							   Icon_Display(1);  //报警后的红色警报
											 OSIntExit();	
						readwarmingINF.warmingChangePD_Flag=1;
					}
				}		

}



void   Record_Data_Task(void)
{	
		OS_ERR err;
	 FuckingYou_New_Pathname();	//创建新文档，每天4点创建	
				//AVG.Min_Ave_Value=0;
				  TimeMeas_Rocord=60;
				  RecorDataMin_Flag=1;
        	while(TimeMeas_Rocord&&(StopFlg==0))
					{
						OSTimeDly(200,OS_OPT_TIME_PERIODIC,&err);	//200个时间节拍，1S更新一次			
					}	
					OSIntEnter();
        FuckingYou_New_Writedata_Parament(GASFLOW,SO2CONCERNRATIONCAL.PMTSampVL,CaseTEM,TECTEM,REFVOL_PD,SO2ConCenTration,Var.OffSet,switch_status);
					OSIntExit(); 
				RecorData5Min_Flag++; //每分钟加一次
				RecorDataHour_Flag++;	
				RecorDataday_Flag++;
				AVG.Min_Ave_Value=AVG.Min_Total_Value/60; //分钟平均值
				OSIntEnter();
        FuckingYou_New_Writedata_Min(AVG.Min_Ave_Value);//分钟值数据保存	
					OSIntExit(); 		
			 if(	categroy_change==1)
				 FiveMin_AVG_dis = filtStack(&AVG.Min_Ave_Value,elmResultBuf[3],&reqErrCnt[3],3);		
			else
					FiveMin_AVG_dis = filtStack(&AVG.Min_Ave_Value,elmResultBuf[3],&reqErrCnt[3],3)*2.62; 
			
				AVG.FiveMin_Aver+=AVG.Min_Total_Value;  //5分钟总值
					AVG.Min_Total_Value=0;aaa=0		;		 
				if(RecorData5Min_Flag==5)
				{
					RecorData5Min_Flag=0;
					FiveMin_AVG=AVG.FiveMin_Aver/5/60; //5分钟平均值
					AVG.FiveMin_Aver=0;
					OSIntEnter();
					FuckingYou_New_Writedata_5MIN(FiveMin_AVG);
					OSIntExit(); 
				}		
				AVG.Hour_Total_Value+=AVG.Min_Ave_Value; //一小时的总值
			if(RecorDataHour_Flag==60)
				{
					RecorDataHour_Flag=0;
					AVG.Hour_Ave_Value=AVG.Hour_Total_Value/60; //一小时的平均值
						OSIntEnter();
					FuckingYou_New_Writedata_Hour(AVG.Hour_Ave_Value);
						OSIntExit(); 	
					AVG.Hour_Total_Value=0;
				}		
				AVG.Day_Total_Value+=AVG.Min_Ave_Value;
		  	if(RecorDataday_Flag==1440)
				{
					RecorDataday_Flag=0;
					AVG.Day_Ave_Value=AVG.Day_Total_Value/60/24; //一天的平均值
						OSIntEnter();
					FuckingYou_New_Writedata_day(AVG.Day_Ave_Value);
						OSIntExit(); 	
					AVG.Day_Total_Value=0;
				}							
}

//暗电流测试
void dark_current(void)
{
	DCPMESampVL= SO2CONCERNRATIONCAL.PMTSampVL;
	REFVOL_PMT=REFVOL_PD ;  //参考暗电流
}

u8 yure_jiemian(void)
{
	unsigned char i=0;
//	unsigned char rs485buf[64];
  // u8 err; 
	static char a,b,c,d,e,f;
	char len1,len2,len3,len4,len5,len6;
   char FTOCHAR[10][10];  

if((a==0)||(b==0)||(c==0)||(d==0)||(e==0)||(f==0))
{	
  	 Read_Total_Parameter();
		 len1=sprintf(FTOCHAR[0],"%.3f",CaseTEM);        //反应室温度 //REFVOL_PD
		 len2=sprintf(FTOCHAR[1],"%.3f",TECTEM);         //pmt内部温度
		 len3=sprintf(FTOCHAR[2],"%.3f",CRAteTEM);       //机箱温度
		 len4=sprintf(FTOCHAR[3],"%.3f",SampGasP);       //采样压力
		 len5=sprintf(FTOCHAR[4],"%.3f",CaseGasPRESSURE);//反应室压力
		 len6=sprintf(FTOCHAR[5],"%.3f",REFVOL_PD);      //参考光强
		    delay_ms(2); 
	  USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
		USART3_SendByte(0x12);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0F);			
		USART3_SendByte(len1>>8);	
		USART3_SendByte(len1&0xff);
			 for(i=0;i<len1;i++)
			{
				USART3_SendByte(FTOCHAR[0][i]);
			}
   	USART3_SendByte(0x00);	
		USART3_SendByte(0x10);
		USART3_SendByte(len2>>8);	
		USART3_SendByte(len2&0xff);
					 for(i=0;i<len2;i++)
			{
				USART3_SendByte(FTOCHAR[1][i]);
			}
		USART3_SendByte(0x00);	
		USART3_SendByte(0x11);
		USART3_SendByte(len3>>8);	
		USART3_SendByte(len3&0xff);
					 for(i=0;i<len3;i++)
			{
				USART3_SendByte(FTOCHAR[2][i]);
			}
	USART3_SendByte(0x00);	
		USART3_SendByte(0x12);
		USART3_SendByte(len4>>8);	
		USART3_SendByte(len4&0xff);
					 for(i=0;i<len4;i++)
			{
				USART3_SendByte(FTOCHAR[3][i]);
			}
		USART3_SendByte(0x00);	
		USART3_SendByte(0x13);
		USART3_SendByte(len5>>8);	
		USART3_SendByte(len5&0xff);
					 for(i=0;i<len5;i++)
			{
				USART3_SendByte(FTOCHAR[4][i]);
			}
		USART3_SendByte(0x00);	
		USART3_SendByte(0x14);
		USART3_SendByte(len6>>8);	
		USART3_SendByte(len6&0xff);
					 for(i=0;i<len6;i++)
			{
				USART3_SendByte(FTOCHAR[5][i]);
			}
	    	END_CMD();
		}
	
if(d==0)
{
if((45<CaseTEM)&&(CaseTEM<55))//反应室温度
	{
		 	delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x08);
		USART3_SendByte(0x05);
		 END_CMD();
		d++;
	}
	else
	{
			delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x08);
		USART3_SendByte(0x01);
		 END_CMD();
		
				delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x08);
		USART3_SendByte(0x02);
		 END_CMD();
		 		delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x08);
		USART3_SendByte(0x03);
		 END_CMD();
		  delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x08);
		USART3_SendByte(0x04);
		 END_CMD();
	}
}
if(e==0)
{
	if((5<TECTEM)&&(TECTEM<12))//TEC温度,PMT内部温度
	{
		 	delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x09);
		USART3_SendByte(0x05);
		 END_CMD();
		e++;
	}
	else
	{
		delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x09);
		USART3_SendByte(0x01);
		 END_CMD();
		
				delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x09);
		USART3_SendByte(0x02);
		 END_CMD();
		 		delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x09);
		USART3_SendByte(0x03);
		 END_CMD();
		  delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x09);
		USART3_SendByte(0x04);
		 END_CMD();
	}
}

if(f==0)
{
	if((5<CRAteTEM)&&(CRAteTEM<55))//ji机箱温度
	{
				 	delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);		 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x05);
		 END_CMD();
		f++;
		
	}
	else
	{
		delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x01);
		 END_CMD();
		
				delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x02);
		 END_CMD();
		 		delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x03);
		 END_CMD();
		  delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x04);
		 END_CMD();
	}
}
	if(a==0)	
	{		
	if((0<SampGasP)&&(SampGasP<18))//采样气体的压力
	{
					 	delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);		 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0B);
		USART3_SendByte(0x05);
		 END_CMD();
		a++;
	}
	else
	{
			delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0B);
		USART3_SendByte(0x01);
		 END_CMD();
		
				delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0B);
		USART3_SendByte(0x02);
		 END_CMD();
		 		delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0B);
		USART3_SendByte(0x03);
		 END_CMD();
		  delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0B);
		USART3_SendByte(0x04);
		 END_CMD();
	}
}
	if(b==0)
	{
	if((7<=CaseGasPRESSURE)&&(CaseGasPRESSURE<=14))////反应室气体压力
	{
					 	delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);		 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0C);
		USART3_SendByte(0x05);
		 END_CMD();
		b++;
	}
	else
	{
			delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0C);
		USART3_SendByte(0x01);
		 END_CMD();
		
				delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0C);
		USART3_SendByte(0x02);
		 END_CMD();
		 		delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0C);
		USART3_SendByte(0x03);
		 END_CMD();
		  delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0C);
		USART3_SendByte(0x04);
		 END_CMD();
	}
}
	
if(c==0)
{
	//if((1<=REFVOL_PD)&&(REFVOL_PD<=4)) //参考光强PD状态显示
			if((0.8<=REFVOL_PD)&&(REFVOL_PD<=4)) //参考光强PD状态显示 2020 5 25 
	{
			delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);		 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x07);
		USART3_SendByte(0x05);
		 END_CMD();
		c++;
	}
	else
	{
		delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x07);
		USART3_SendByte(0x01);
		 END_CMD();
		
				delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x07);
		USART3_SendByte(0x02);
		 END_CMD();
		 		delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x07);
		USART3_SendByte(0x03);
		 END_CMD();
		  delay_ms(100); 
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
			 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0A);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x07);
		USART3_SendByte(0x04);
		 END_CMD();
	}
	
}

if((a==1)&&(b==1)&&(c==1)&&(d==1)&&(e==1)&&(f==1))
{
	 //EE B1 00 00 00 FF FC FF FF //切换置0画面
	  delay_ms(12); 
	  USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
		USART3_SendByte(0x00);	
		USART3_SendByte(0x00);
		USART3_SendByte(0x00);
	   END_CMD();
   	return 0;
}
	
}

void Display_Sampling_Switch (char x)
{
	char i;
	for(i=0;i<12;i++)
	{
   	USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(i);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x61);
		USART3_SendByte(x);
		 END_CMD();
	}
}


void Icon_Display (char x)//EE B1 23 00 00 01 2C 00 FF FC FF FF 
{
	char i;
	for(i=0;i<13;i++)
	{
	  USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	 
		USART3_SendByte(0x23);
		USART3_SendByte(0x00);	
		USART3_SendByte(i);
		USART3_SendByte(0x01);	
		USART3_SendByte(0x2c);
		USART3_SendByte(x);
		 END_CMD();	
	}
	  
}
void Test_Uint(const char *str)//const char string[2][6]={"ppb","ug/m3"};
{
		char i,j,len;
	  len = strlen(str);
	
		USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	 
		USART3_SendByte(0x10);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x00);
		USART3_SendByte(0x00);	
		USART3_SendByte(0x0B);
    for(j=0;j<len;j++)
		{
			USART3_SendByte(str[j]);
		}
		 END_CMD();
	  
	for(i=0;i<12;i++)
	{
	  USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	 
		USART3_SendByte(0x10);
		USART3_SendByte(0x00);	
		USART3_SendByte(i);
		USART3_SendByte(0x01);	
		USART3_SendByte(0x2D);
    for(j=0;j<len;j++)
		{
			USART3_SendByte(str[j]);
		}
		 END_CMD();	
	}  
}

