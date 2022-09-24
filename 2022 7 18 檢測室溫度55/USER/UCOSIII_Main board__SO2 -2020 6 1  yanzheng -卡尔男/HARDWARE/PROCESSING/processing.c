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
SO2CONCERNRATION SO2CONCERNRATIONCAL; //S02�����Ҫ��������
extern int TimeMeasCount;
const char string[2][6]={"ppb","ug/m3"};


//u8 TH_RX_BUF[20]={0}; //����ǰ״̬
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
float SO2concentration=0;//S02��Ũ��,du
//"so2 Ũ�ȵ�һ������"
float SO2ConCenTration=0;//S02Ũ�Ȳ���
char categroy_change=1;
float SO2ConCenTration_AfterCal=0;
float SO2ConCenTration_AfterCal_W=0;

float SO2SD=0;//S02��׼��
float DCPMESampVL=0;//����������״̬�£�PMT������ѹ
float REFVOL_PD=0;//�ο���ǿPD
float REFVOL_PMT=0;//�ο�������PD
float OVERALLD=0;//��һ��ֵ
float GASFLOW=0;//��������,��λ:sccm
float SampGasP=0;//���������ѹ��
float CaseGasPRESSURE=0;//��Ӧ������ѹ��
float CaseTEM=0;//��Ӧ���¶�
float CRAteTEM=0;//ji�����¶�
float TECTEM=0;//TEC�¶�
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

//��������������ر���
const u16 ADDress1[]={0x00FB,0x0102,0x0106,0x0108,0x010A,0x010C,0x0116,0x0118,0x011A,0x011C,0x011E,0x0120,0x0129,0x0132,0x013B};						 
const u8    REGNUM[]={     7,     4,     2,     2,     2,     2,     2,     2,     2,     2,     2,     9,     9,     9,     2};

//���Ĵ���
const u8              STATUS_TX_BUF[8]={0x32,0x03,0x00,0x78,0x00,0x01,0x01,0xD0};//��ǰ״̬

const u8                 SO2_TX_BUF[8]={0x32,0x03,0x00,0x79,0x00,0x02,0x10,0x11};//S02��Ũ��
const u8               SO2SD_TX_BUF[8]={0x32,0x03,0x00,0x7F,0x00,0x02,0xF0,0x10};//S02��׼��

const u8           PMTDriveVal_TX_BUF[8]={0xFF,0x03,0x01,0x1C,0x00,0x04,0x91,0xED};//��PMT������ѹ������Ƶ�ѹ
const u8            Total_Parameter[8]={0xFF,0x03,0x00,0x81,0x00,0x16,0x81,0xF2};//�������ȡ


const u8 STATUS_RX_BUF[64]={0}; 
//������������������
const u8    deviceopratestatus_TX_BUF[6]={0x32,0x03,0x02,0x35,0x00,0x02}; 
const u8         samplingvalve_TX_BUF[6]={0x32,0x03,0x02,0x37,0x00,0x02}; 
const u8    Lightsourceshutter_TX_BUF[6]={0x32,0x03,0x02,0x39,0x00,0x02}; 
const u8              PMTlight_TX_BUF[6]={0x32,0x03,0x02,0x3B,0x00,0x02}; 
const u8     caseheatingswitch_TX_BUF[6]={0x32,0x03,0x02,0x3D,0x00,0x02};

//д��ز���
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
				rs485buf[i]=Total_Parameter[i];// ��䷢�ͻ�����
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
 SO2CONCERNRATIONCAL.PMTSampVL=GETDATA.exdataFLOAT;	//��PMT������ѹ	 
					GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
 //DCPMESampVL=GETDATA.exdataFLOAT;	//����������״̬�£�PMT������ѹ
				  GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
				  GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
				  GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
				  GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
 REFVOL_PD=GETDATA.exdataFLOAT;		//�ο���ǿ//GASFLOW,SO2CONCERNRATIONCAL.PMTSampVL,CaseTEM,TECTEM,REFVOL_PD,SO2ConCenTration
				   GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
				 	 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
           GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
				   GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
//REFVOL_PMT=GETDATA.exdataFLOAT;			//�ο�������		
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
				   GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
//OVERALLD=GETDATA.exdataFLOAT;					//��һ��ֵ	 
				   GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
				 	 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
				 	 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
				   GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
GASFLOW=GETDATA.exdataFLOAT;					//��������,��λ:sccm		
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
SampGasP=GETDATA.exdataFLOAT;					//���������ѹ��	
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
CaseGasPRESSURE=GETDATA.exdataFLOAT;					//��Ӧ������ѹ��	 
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
CaseTEM=GETDATA.exdataFLOAT;		 	//��Ӧ���¶�
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
CRAteTEM=GETDATA.exdataFLOAT;			 	//ji�����¶�
					 GETDATA.tempdata[3]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[2]=TotalPara_RX_BUF[++count];
					 GETDATA.tempdata[1]=TotalPara_RX_BUF[++count];
           GETDATA.tempdata[0]=TotalPara_RX_BUF[++count];
 TECTEM=GETDATA.exdataFLOAT;	//PMT�¶�			 
			 }
			 }				 
 }
 //��ȡ������ѹ������Ƶ�ѹ
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
				rs485buf[i]=PMTDriveVal_TX_BUF[i];// ��䷢�ͻ�����
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
	   len2=sprintf(FTOCHAR[2],"%.3f",CRAteTEM);        //�����¶�
		 len3=sprintf(FTOCHAR[3],"%.3f",CaseTEM);         //��Ӧ���¶�
		 len4=sprintf(FTOCHAR[4],"%.3f",REFVOL_PD);       //�ο���ǿ
		 len5=sprintf(FTOCHAR[5],"%.3f",FiveMin_AVG_dis);       //����ѹ��
		 len6=sprintf(FTOCHAR[6],"%.3f",SampGasP);//��������ѹ��
		 len7=sprintf(FTOCHAR[7],"%.3f",GASFLOW);  //��������    
		
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
	 if(READSCRREN.SCRREN1FLAG==1)   //��ϸ����
	{	
	    	REFVOL_PD=  kalman(REFVOL_PD);
		   len1=sprintf(FTOCHAR[1],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL);      //������ǿ
		
    	 len2=sprintf(FTOCHAR[2],"%.3f",DCPMESampVL);     //����������
		
  		 len3=sprintf(FTOCHAR[3],"%.3f",REFVOL_PD);       //�ο���ǿ
		
  		 len4=sprintf(FTOCHAR[4],"%.3f",REFVOL_PMT);      //�ο�������
		
  		 len5=sprintf(FTOCHAR[5],"%.3f",GASFLOW);//��������
		
  		 len6=sprintf(FTOCHAR[6],"%.3f",SampGasP);       //����ѹ��	
	
  		 len7=sprintf(FTOCHAR[7],"%.3f",CaseGasPRESSURE);    //��Ӧ��ѹ��
	
       len8=sprintf(FTOCHAR[8],"%.3f",CaseTEM); //��Ӧ���¶�

       len9=sprintf(FTOCHAR[9],"%.3f",CRAteTEM);     //�����¶�
		   len10=sprintf(FTOCHAR[10],"%.3f",TECTEM);      //TEC�¶�
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
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //��ؼ����͵�ͷ
  		 USART3_SendByte(0x00);	USART3_SendByte(0x01);  	                   //����1
  		 USART3_SendByte(0x00);	USART3_SendByte(0x01);                      //���غ�1��������ǿ
  		 USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//�ַ�����
					 for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[1][i]);                //���Ͳ�����ǿ
					}
					
//					
			USART3_SendByte(0x00);	  USART3_SendByte(0x02);                      //���غ�2������������
			USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//�ַ�����		
				 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[2][i]);                //�������� ����������
					}	
//					
//					
			USART3_SendByte(0x00);	USART3_SendByte(0x03);                      //���غ�3 �ο���ǿ
			USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//�ַ�����		
				 for(i=0;i<len3;i++)
					{
						USART3_SendByte(FTOCHAR[3][i]);                //�������� �ο���ǿ
					}	
//		
	    USART3_SendByte(0x00);	  USART3_SendByte(0x04);                      //���غ�4���ο�������
			USART3_SendByte(len4>>8);	USART3_SendByte(len4&0xff);	//�ַ�����		
				 for(i=0;i<len4;i++)
					{
						USART3_SendByte(FTOCHAR[4][i]);                //�������� �ο�������
					}			

			 USART3_SendByte(0x00);	  USART3_SendByte(0x05);                      //���غ�5��  //��������
			USART3_SendByte(len5>>8);	USART3_SendByte(len5&0xff);	//�ַ�����		
				 for(i=0;i<len5;i++)
					{
						USART3_SendByte(FTOCHAR[5][i]);                //�������� ��������
					}		

	  USART3_SendByte(0x00);	  USART3_SendByte(0x06);                      //���غ�6������ѹ��
			USART3_SendByte(len6>>8);	USART3_SendByte(len6&0xff);	//�ַ�����		
				 for(i=0;i<len6;i++)
					{
						USART3_SendByte(FTOCHAR[6][i]);                //�������� ����ѹ��	
					}		

      USART3_SendByte(0x00);	  USART3_SendByte(0x07);                      //���غ�07����Ӧ��ѹ��
			USART3_SendByte(len7>>8);	USART3_SendByte(len7&0xff);	//�ַ�����		
				 for(i=0;i<len7;i++)
					{
						USART3_SendByte(FTOCHAR[7][i]);                //�������� ����ѹ��	
					}		
					
     USART3_SendByte(0x00);	  USART3_SendByte(0x08);                      //���غ�8����Ӧ���¶�
			USART3_SendByte(len8>>8);	USART3_SendByte(len8&0xff);	//�ַ�����		
				 for(i=0;i<len8;i++)
					{
						USART3_SendByte(FTOCHAR[8][i]);                //�������� ��Ӧ���¶�
					}		
					
		  USART3_SendByte(0x00);	  USART3_SendByte(0x09);                      //���غ�09�������¶�
			USART3_SendByte(len9>>8);	USART3_SendByte(len9&0xff);	//�ַ�����		
				 for(i=0;i<len9;i++)
					{
						USART3_SendByte(FTOCHAR[9][i]);                //�������� �����¶�
					}		
					
			  USART3_SendByte(0x00);	  USART3_SendByte(0x0A);                      //���غ�10��TEC�¶�
			USART3_SendByte(len10>>8);	USART3_SendByte(len10&0xff);	//�ַ�����		
				 for(i=0;i<len10;i++)
					{
						USART3_SendByte(FTOCHAR[10][i]);                //�������� TEC�¶�
					}		
					
	  USART3_SendByte(0x00);	  USART3_SendByte(0x0B);                      //���غ�11��K
			USART3_SendByte(len11>>8);	USART3_SendByte(len11&0xff);	//�ַ�����		
				 for(i=0;i<len11;i++)
					{
						USART3_SendByte(FTOCHAR[11][i]);                //����K
					}		
	     USART3_SendByte(0x00);	  USART3_SendByte(0x0C);                      //���غ�12��B
			USART3_SendByte(len12>>8);	USART3_SendByte(len12&0xff);	//�ַ�����		
				 for(i=0;i<len12;i++)
					{
						USART3_SendByte(FTOCHAR[12][i]);                //����B
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
	
  if(READSCRREN.SCRREN6FLAG==1)   //�궨����
	{
			reqErrCnt[0]=0;//�����˲��������㣬0�����
			reqErrCnt[1]=0;//�����˲��������㣬��������
			reqErrCnt[4]=0;//�����˲��������㣬0�����
			reqErrCnt[5]=0;//�����˲��������㣬��������
//    OSIntEnter();
//				 	USART3_SendByte(0xEE);  //�궨������
//		      USART3_SendByte(0xB1);		 
//		      USART3_SendByte(0x23);
//				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
//		      USART3_SendByte(0x00);		USART3_SendByte(0x61);
//	        USART3_SendByte(0x01);
//				  END_CMD();
//			    OSIntExit();
	//	READSCRREN.Record_SO2BKVFinshFlag=0;//ÿ24Сʱ����һ��������̬��ֵ��־,jiaУ׼ʱ��Ҫ����ֵ����
		len1=sprintf(FTOCHAR[6],"%.2f",GASFLOW); //GASFLOWq��������
		len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL); //PMT��ǿ
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
		OSIntExit();  //��ֹ�����
			
			
	 if(READSCRREN.ZeroCALFLAG==1) //��ʼ���У׼
	 {
		 SO2CONCERNRATIONCAL.PMTSampVL_ZERO=SO2CONCERNRATIONCAL.PMTSampVL;//������㡣(X1,0)
		 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL_ZERO); //PMT��ǿmv
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
				 	USART3_SendByte(0xEE);  //�궨������
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
		      USART3_SendByte(0x00);		USART3_SendByte(0x37);
	        USART3_SendByte(0x01);
				  END_CMD();
			    OSIntExit(); 
		     	TimeMeasCount=300; //�ȴ�5����
			 while((StopFlg==0)&&TimeMeasCount)
			 {
					//��������ǿ 
				 len1=sprintf(FTOCHAR[6],"%.2f",GASFLOW);//1E
				 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL); //PMT��ǿ0F
				// len3=sprintf(FTOCHAR[8],"%.3f",SO2ConCenTration_AfterCal); //PMT��ǿ0E
				    delay_ms(2); 
				 OSIntEnter(); 
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //��ؼ����͵�ͷ
  		 USART3_SendByte(0x00);	USART3_SendByte(0x06);  	                   //����1
  		 USART3_SendByte(0x00);	USART3_SendByte(0x1E);                      //���غ�1E,GASFLOW,1E
  		 USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//�ַ�����
					 for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[6][i]);                //GASFLOW
					}		
			USART3_SendByte(0x00);	  USART3_SendByte(0x0F);                      //���غ�0F������������
			USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//�ַ�����		
				 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[7][i]);                //SO2CONCERNRATIONCAL.PMTSampVL
					}		
//			USART3_SendByte(0x00);	USART3_SendByte(0x0E);                      //���غ�0E �ο���ǿ
//			USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//�ַ�����		
//				 for(i=0;i<len3;i++)
//					{
//						USART3_SendByte(FTOCHAR[8][i]);                //�������� �ο���ǿ
//					}		
					END_CMD();		
				 OSIntExit();
					
					SO2CONCERNRATIONCAL.PMTSampVL_ZERO =filtStack (&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[4],&reqErrCnt[4],1);//������㡣(X1,0	
			   
					OSIntEnter(); 
					 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL_ZERO); //PMT��ǿmv
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
			   	OSTimeDly(200,OS_OPT_TIME_PERIODIC,&err);	//200��ʱ����ģ�1S����һ��
						if((SO2CONCERNRATIONCAL.COV_ZERO<0.02)&&(SO2CONCERNRATIONCAL.COV_ZERO>0)) 
		     {
		       READSCRREN.ZeroCALFLAG=0;
				   delay_ms(2); 
				   USART3_SendByte(0xEE);  //У׼�ɹ�
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
				 	USART3_SendByte(0xEE);  //У׼shiʧ��
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
		      USART3_SendByte(0x00);		USART3_SendByte(0x37);
	        USART3_SendByte(0x03);
				  END_CMD();
		     }
		}
	
	 if((READSCRREN.StadagasCALFLAG==1)&&(READSCRREN.SO2STARDCON_INPUT_Flag==1))//һ���Ǳ�����ʼ��־λ��һ����Ũ��������ϱ�־
	 {
		 //���±�����ǿ
		 	SO2CONCERNRATIONCAL.PMTSampVL_STADARD=SO2CONCERNRATIONCAL.PMTSampVL;//(X2,Y2)
		 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL_STADARD); //PMT��ǿmv	
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
			USART3_SendByte(0xEE);  //�궨������
			USART3_SendByte(0xB1);		 
			USART3_SendByte(0x23);
			USART3_SendByte(0x00);		USART3_SendByte(0x06);
			USART3_SendByte(0x00);		USART3_SendByte(0x0A);
			USART3_SendByte(0x01);
			END_CMD();			
			TimeMeasCount=300; //�ȴ�5����		
      while((StopFlg==0)&&TimeMeasCount)
			 {	
				 len1=sprintf(FTOCHAR[6],"%.2f",GASFLOW);//1E
				 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL); //PMT��ǿ0F
				 //len3=sprintf(FTOCHAR[8],"%.3f",SO2ConCenTration_AfterCal); //PMT��ǿ0E
				    delay_ms(2); 
				 
				  OSIntEnter();
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //��ؼ����͵�ͷ
  		 USART3_SendByte(0x00);	USART3_SendByte(0x06);  	                   //����1
  		 USART3_SendByte(0x00);	USART3_SendByte(0x1E);                      //���غ�1E,GASFLOW,1E
  		 USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//�ַ�����
					 for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[6][i]);                //GASFLOW
					}
			USART3_SendByte(0x00);	  USART3_SendByte(0x0F);                      //���غ�0F������������
			USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//�ַ�����		
				 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[7][i]);                //SO2CONCERNRATIONCAL.PMTSampVL
					}			
//			USART3_SendByte(0x00);	USART3_SendByte(0x0E);                      //���غ�0E �ο���ǿ
//			USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//�ַ�����		
//				 for(i=0;i<len3;i++)
//					{
//						USART3_SendByte(FTOCHAR[8][i]);                //�������� �ο���ǿ
//					}		
					END_CMD();		
					OSIntExit(); 
					
					 SO2CONCERNRATIONCAL.PMTSampVL_STADARD =filtStack (&SO2CONCERNRATIONCAL.PMTSampVL , elmResultBuf[5],&reqErrCnt[5],1);

					 len2=sprintf(FTOCHAR[7],"%.3f",SO2CONCERNRATIONCAL.PMTSampVL_STADARD); //PMT��ǿmv	
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
			   	OSTimeDly(200,OS_OPT_TIME_PERIODIC,&err);	//200��ʱ����ģ�1S����һ��		
  		
				if((SO2CONCERNRATIONCAL.COV_STANDARD<0.02)&&(SO2CONCERNRATIONCAL.COV_STANDARD>0)) 
		     {
		       //READSCRREN.ZeroCALFLAG=0;
					 READSCRREN.StadagasCALFLAG=0;//У׼�ɹ�
				   delay_ms(2); 
				  	USART3_SendByte(0xEE);  //У׼�ɹ�
		       USART3_SendByte(0xB1);		 
		       USART3_SendByte(0x23);
				   USART3_SendByte(0x00);		USART3_SendByte(0x06);
		       USART3_SendByte(0x00);		USART3_SendByte(0x0A);
	         USART3_SendByte(0x02);
				   END_CMD();		
          	TimeMeasCount=0;	
					  READSCRREN.CACULATEKBFLAG=1; //����У׼��ɴ������ж��Ѿ����
		     }
			 }	

				if(SO2CONCERNRATIONCAL.COV_STANDARD>0.02) 
		     {
		   		READSCRREN.StadagasCALFLAG=0;
				  delay_ms(2); 
				 	USART3_SendByte(0xEE);  //У׼shiʧ��
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
		      USART3_SendByte(0x00);		USART3_SendByte(0x0A);
	        USART3_SendByte(0x03);
				  END_CMD();
		     }
	 }

//		 //�˿����ݿ�ʼ����K,Bֵ		  
	 	 if(READSCRREN.CACULATEKBFLAG==1)
		 {
			 READSCRREN.CACULATEKBFLAG=0;
			  x1=SO2CONCERNRATIONCAL.PMTSampVL_ZERO;
        y1=0;
			 x2=SO2CONCERNRATIONCAL.PMTSampVL_STADARD;
       y2=SO2CONCERNRATIONCAL.SO2STARDCON_INPUT; 
			 SO2CONCERNRATIONCAL.SO2K =(y2-y1)/(x2-x1);              //����K.Bֵ
			 SO2CONCERNRATIONCAL.SO2B =-SO2CONCERNRATIONCAL.SO2K*x1;		
       //У׼��ɱ����Զ�У׼���ܵı�־λ			 
  //	 SO2CONCERNRATIONCAL.Seek_SO2BGV[0]=SO2CONCERNRATIONCAL.SO2K*SO2CONCERNRATIONCAL.PMTSampVL+SO2CONCERNRATIONCAL.SO2B;		 
		  len1=sprintf(FTOCHAR[8],"%.3f",SO2CONCERNRATIONCAL.SO2K);        //������ת�����ַ�
		  len2=sprintf(FTOCHAR[9],"%.3f",SO2CONCERNRATIONCAL.SO2B);         //
			  delay_ms(2);
			 OSIntEnter();
				USART3_SendByte(0xEE);
				USART3_SendByte(0xB1);	
				USART3_SendByte(0x12);
				USART3_SendByte(0x00);	
				USART3_SendByte(0x06);
				USART3_SendByte(0x00);	//Kλ��
				USART3_SendByte(0x0C);			
				USART3_SendByte(len1>>8);	
				USART3_SendByte(len1&0xff);
					 for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[8][i]);
					}
				USART3_SendByte(0x00);//bλ��	
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
		//����λ����k,b
					OSIntEnter();
   			 SaveEpData(0x00,SO2CONCERNRATIONCAL.SO2K*1000); //8���ֽڡ�address��0~7;
				 SaveEpData(0x08,SO2CONCERNRATIONCAL.SO2B*1000);// 8g���ֽڡ�address:8~15		
//					Var.AfterCALFlag=1;
//					SaveEpData(360,Var.AfterCALFlag);
					OSIntExit();
					Event_Record(1);//У׼��ɣ���¼һ��У׼��¼
		   }
		 READSCRREN.IntialonceFlag=0;//ֻ����һ�Σ��ȴ�У׼
		 } 
	else
	   {  
			 if(READSCRREN.IntialonceFlag==0)
			 {
				READSCRREN.IntialonceFlag=1;
			 	 OSIntEnter(); 
			     delay_ms(2); 
				 	USART3_SendByte(0xEE);  //�ȴ�У׼
		      USART3_SendByte(0xB1);		 
		      USART3_SendByte(0x23);
				  USART3_SendByte(0x00);		USART3_SendByte(0x06);
		      USART3_SendByte(0x00);		USART3_SendByte(0x37);
	        USART3_SendByte(0x00);
				  END_CMD();
		    //	OSIntExit();
			   	   	delay_ms(2);
			   //  OSIntEnter();
				 	USART3_SendByte(0xEE);  //�ȴ�У׼
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
			if(Test_Catagry==123456) //���У׼
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
			else      //����У׼
						 SO2ConCenTration=SO2CONCERNRATIONCAL.SO2K *SO2CONCERNRATIONCAL.PMTSampVL  +SO2CONCERNRATIONCAL.SO2B;
			 
//			 if(SO2ConCenTration<-20)
//						 SO2ConCenTration=0;	
			 if(Var.Add_AfterCALFlag==0x01)//����offset value;
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
	     W_len1=sprintf(FTOCHAR1[1],"%.3f",Input.CRAteTEM_LowerLimit);     //�����¶�������
    	 W_len2=sprintf(FTOCHAR1[2],"%.3f",Input.CRAteTEM_UpperLimit);   	
  		 W_len3=sprintf(FTOCHAR1[3],"%.3f",Input.TECTEM_LowerLimit);     //PMT�¶�������
  		 W_len4=sprintf(FTOCHAR1[4],"%.3f",Input.TECTEM_UpperLimit);     
  		 W_len5=sprintf(FTOCHAR1[5],"%.3f",Input.CaseTEM_LowerLimit);//��Ӧ���¶�������
  		 W_len6=sprintf(FTOCHAR1[6],"%.3f",Input.CaseTEM_UpperLimit);       
  		 W_len7=sprintf(FTOCHAR1[7],"%.3f",Input.SampGasP_LowerLimit);    //����ѹ��
       W_len8=sprintf(FTOCHAR1[8],"%.3f",Input.SampGasP_UpperLimit); 
       W_len9=sprintf(FTOCHAR1[9],"%.3f",Input.CaseGasPRESSURE_LowerLimit);   //����ѹ��   
		   W_len10=sprintf(FTOCHAR1[10],"%.3f",Input.CaseGasPRESSURE_UpperLimit);      
			 W_len11=sprintf(FTOCHAR1[11],"%.3f",Input.GASFLOW_LowerLimit);    //�������� 
		   W_len12=sprintf(FTOCHAR1[12],"%.3f",Input.GASFLOW_UpperLimit);    
	     W_len13=sprintf(FTOCHAR1[13],"%.3f",Input.PMTSampVL_LowerLimit);   //������ǿ
		   W_len14=sprintf(FTOCHAR1[14],"%.3f",Input.PMTSampVL_UpperLimit);     
			 W_len15=sprintf(FTOCHAR1[15],"%.3f",Input.REFVOL_PD_LowerLimit);  //�ο���ǿ 
		   W_len16=sprintf(FTOCHAR1[16],"%.3f",Input.REFVOL_PD_UpperLimit);     
	     W_len17=sprintf(FTOCHAR1[17],"%.3f",Input.REFVOL_PMT_LowerLimit);   //�ο������� 
		   W_len18=sprintf(FTOCHAR1[18],"%.3f",Input.REFVOL_PMT_UpperLimit);     
			 W_len19=sprintf(FTOCHAR1[19],"%.3f",Input.VEF_LowerLimit);         //��׼��Դ
		   W_len20=sprintf(FTOCHAR1[20],"%.3f",Input.VEF_UpperLimit);    
	     	OSIntEnter();                
	//	OSMutexPend (&OSUart4Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
				delay_ms(10); 
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //��ؼ����͵�ͷ
  		 USART3_SendByte(0x00);	USART3_SendByte(0x08);  	                   //����1
  		 USART3_SendByte(0x00);	USART3_SendByte(0x02);                      //���غ�2,�����¶�������
  		 USART3_SendByte(W_len1>>8);	USART3_SendByte(W_len1&0xff);	//�ַ�����
					 for(i=0;i<W_len1;i++)
					{
						USART3_SendByte(FTOCHAR1[1][i]);                     //      �����¶�����
					}
					
//					
			USART3_SendByte(0x00);	  USART3_SendByte(0x05);          //�ؼ���5    
			USART3_SendByte(W_len2>>8);	USART3_SendByte(W_len2&0xff);	
				 for(i=0;i<W_len2;i++)
					{
						USART3_SendByte(FTOCHAR1[2][i]);                   //�����¶�����
					}	
//					
//					
			USART3_SendByte(0x00);	USART3_SendByte(0x09);                              //�ؼ��� 9          
			USART3_SendByte(W_len3>>8);	USART3_SendByte(W_len3&0xff);	
				 for(i=0;i<W_len3;i++)
					{
						USART3_SendByte(FTOCHAR1[3][i]);                   //PMT�¶�������
					}	
//		
	    USART3_SendByte(0x00);	  USART3_SendByte(0x0A);                     
			USART3_SendByte(W_len4>>8);	USART3_SendByte(W_len4&0xff);   
				 for(i=0;i<W_len4;i++)
					{
						USART3_SendByte(FTOCHAR1[4][i]);                //PMT�¶�������
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

void Read_SetWarn(void)  //��ȡ�������õ�������
{
	         Input.CRAteTEM_LowerLimit  =      (float)ReadEpData(30)/1000;  //�����¶�������
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
	         //�����¶�
						if((Input.CRAteTEM_LowerLimit <CRAteTEM )&&  ( CRAteTEM<Input.CRAteTEM_UpperLimit))
						{
								if(readwarmingINF.warmingcaseTEM_Flag==1)
								{
									 delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x00);		                    //����00���.���䱨��
									 END_CMD();
									Icon_Display(0);  //�������������ɫͨ��
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
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //�澯���õ�ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x00);		                    //����00.���䱨��
									 END_CMD(); 
									 Icon_Display(1);   //������ĺ�ɫ����
									   OSIntExit();
									 readwarmingINF.warmingcaseTEM_Flag=1;
								 }
	          	}
						
					//PMT�¶�		
	      if((Input.TECTEM_LowerLimit <TECTEM)&&(TECTEM<Input.TECTEM_UpperLimit))
				{
					if(readwarmingINF.warmingPMTTEM_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);		                    //����01��PMT�¶Ƚ������
									 END_CMD();
							 Icon_Display(0);  //�������������ɫͨ��
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
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);		                    //����01��PMT�¶ȱ���
									 END_CMD();
							 Icon_Display(1);   //������ĺ�ɫ����
									 OSIntExit();
						  readwarmingINF.warmingPMTTEM_Flag=1;
					}
				}
       //��Ӧ���¶�		
    if((Input.CaseTEM_LowerLimit <CaseTEM)&&(CaseTEM<Input.CaseTEM_UpperLimit))
				{
					
					if(readwarmingINF.warmingcasereactionTEM_Flag==1)
					{
						      delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x02);		                    //����02.��Ӧ���¶Ƚ������
									 END_CMD();
							Icon_Display(0);;  //�������������ɫͨ��
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
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x02);		                    //����02.��Ӧ���¶ȱ���
									 END_CMD();
							  Icon_Display(1);  //������ĺ�ɫ����
									 OSIntExit();	
						   readwarmingINF.warmingcasereactionTEM_Flag=1;
					}
				}
//����ѹ��
			  if((Input.SampGasP_LowerLimit <SampGasP)&&(SampGasP<Input.SampGasP_UpperLimit))
				{
					if(readwarmingINF.warmingSamplingpressure_Flag==1)
					{
						      delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x03);		                    //����03.����ѹ���������
									  END_CMD();
						  	Icon_Display(0);  //�������������ɫͨ��
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
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x03);		                    //����03.����ѹ������
									  END_CMD();
							  Icon_Display(1);   //������ĺ�ɫ����
									 OSIntExit();	
						      readwarmingINF.warmingSamplingpressure_Flag=1;
					}
				}	
//��Ӧ��ѹ��
			  if((Input.CaseGasPRESSURE_LowerLimit <CaseGasPRESSURE)&&(CaseGasPRESSURE<Input.CaseGasPRESSURE_UpperLimit))
				{
					if(readwarmingINF.casereactionpressure_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x04);		                    //����00.��Ӧ��ѹ�����
									 END_CMD();
							  Icon_Display(0);  //�������������ɫͨ��
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
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x04);		                    //����00.��Ӧ��ѹ������
									 END_CMD();
							  Icon_Display(1);   //������ĺ�ɫ����
									 OSIntExit();	
						readwarmingINF.casereactionpressure_Flag=1;
					}
				}		
//��������
 if((Input.GASFLOW_LowerLimit <GASFLOW)&&(GASFLOW<Input.GASFLOW_UpperLimit))
				{
					if(readwarmingINF.warmingSamplingflow_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);		                    //����05�����������������
									 END_CMD();
							Icon_Display(0);;  //�������������ɫͨ��
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
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);		                    //����05��������������
									 END_CMD();
							  Icon_Display(1);   //������ĺ�ɫ����
									 OSIntExit();	
						readwarmingINF.warmingSamplingflow_Flag=1;
					}
				}		
//������ǿ
	if((Input.PMTSampVL_LowerLimit <SO2CONCERNRATIONCAL.PMTSampVL)&&(SO2CONCERNRATIONCAL.PMTSampVL<Input.PMTSampVL_UpperLimit))
				{
					if(readwarmingINF.warminglightintensityVolt_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x06);		                    //����06��������ǿ�������
									 END_CMD();
						  	Icon_Display(0); //�������������ɫͨ��
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
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x06);		                    //����06��������ǿ����
									 END_CMD();
							  Icon_Display(1);  //������ĺ�ɫ����
									 OSIntExit();	
						       readwarmingINF.warminglightintensityVolt_Flag=1;
					}
				}			
//�ο���ǿ
if((Input.REFVOL_PD_LowerLimit <REFVOL_PD)&&(REFVOL_PD<Input.REFVOL_PD_UpperLimit))
				{
					if(readwarmingINF.warmingREFVOL_PD_Flag==1)
					{
						     delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x07);		                    //����07,�ο���ǿ�������
									 END_CMD();
						Icon_Display(0);  //�������������ɫͨ��
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
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x07);		                    //����07,�ο���ǿ����
									 END_CMD();
							 Icon_Display(1);   //������ĺ�ɫ����
									 OSIntExit();	
						readwarmingINF.warmingREFVOL_PD_Flag=1;
					}
				}			

//�ο�������
if((Input.REFVOL_PMT_LowerLimit <REFVOL_PMT)&&(REFVOL_PMT<Input.REFVOL_PMT_UpperLimit))
				{
					if(readwarmingINF.warmingREFVOL_PMT_Flag==1)
					{
						delay_ms(10); 
									 OSIntEnter(); 
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x08);		                    //����08���ο�������������
									 END_CMD();
							Icon_Display(0);  //�������������ɫͨ��
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
									 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
									 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
									 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
									 USART3_SendByte(0x00);	USART3_SendByte(0x08);		                    //����08���ο�����������
									 END_CMD();
							  Icon_Display(1);   //������ĺ�ɫ����
									 OSIntExit();	
						readwarmingINF.warmingREFVOL_PMT_Flag=1;
					}
				}			
//��׼��ǿ
			//	if((Input.VEF_LowerLimit <Change_Lamp_Val)&&(Change_Lamp_Val<Input.VEF_UpperLimit))
			   if(Input.VEF_LowerLimit <=Change_Lamp_Val)
				  {
							if(readwarmingINF.warmingChangePD_Flag==1)
							{
											 delay_ms(10); 
											 OSIntEnter(); 
											 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x51); //����澯ͷ�ļ�
											 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
											 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
											 USART3_SendByte(0x00);	USART3_SendByte(0x09);		                    //����09����׼��ǿ�������
											 END_CMD();
									Icon_Display(0);  //�������������ɫͨ��
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
											 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x50); //����澯ͷ�ļ�
											 USART3_SendByte(0x00);	USART3_SendByte(0x05);  	                   //����5
											 USART3_SendByte(0x00);	USART3_SendByte(0x01);                       //�ؼ�1
											 USART3_SendByte(0x00);	USART3_SendByte(0x09);		                    //����09����׼��ǿ����
											 END_CMD();
							   Icon_Display(1);  //������ĺ�ɫ����
											 OSIntExit();	
						readwarmingINF.warmingChangePD_Flag=1;
					}
				}		

}



void   Record_Data_Task(void)
{	
		OS_ERR err;
	 FuckingYou_New_Pathname();	//�������ĵ���ÿ��4�㴴��	
				//AVG.Min_Ave_Value=0;
				  TimeMeas_Rocord=60;
				  RecorDataMin_Flag=1;
        	while(TimeMeas_Rocord&&(StopFlg==0))
					{
						OSTimeDly(200,OS_OPT_TIME_PERIODIC,&err);	//200��ʱ����ģ�1S����һ��			
					}	
					OSIntEnter();
        FuckingYou_New_Writedata_Parament(GASFLOW,SO2CONCERNRATIONCAL.PMTSampVL,CaseTEM,TECTEM,REFVOL_PD,SO2ConCenTration,Var.OffSet,switch_status);
					OSIntExit(); 
				RecorData5Min_Flag++; //ÿ���Ӽ�һ��
				RecorDataHour_Flag++;	
				RecorDataday_Flag++;
				AVG.Min_Ave_Value=AVG.Min_Total_Value/60; //����ƽ��ֵ
				OSIntEnter();
        FuckingYou_New_Writedata_Min(AVG.Min_Ave_Value);//����ֵ���ݱ���	
					OSIntExit(); 		
			 if(	categroy_change==1)
				 FiveMin_AVG_dis = filtStack(&AVG.Min_Ave_Value,elmResultBuf[3],&reqErrCnt[3],3);		
			else
					FiveMin_AVG_dis = filtStack(&AVG.Min_Ave_Value,elmResultBuf[3],&reqErrCnt[3],3)*2.62; 
			
				AVG.FiveMin_Aver+=AVG.Min_Total_Value;  //5������ֵ
					AVG.Min_Total_Value=0;aaa=0		;		 
				if(RecorData5Min_Flag==5)
				{
					RecorData5Min_Flag=0;
					FiveMin_AVG=AVG.FiveMin_Aver/5/60; //5����ƽ��ֵ
					AVG.FiveMin_Aver=0;
					OSIntEnter();
					FuckingYou_New_Writedata_5MIN(FiveMin_AVG);
					OSIntExit(); 
				}		
				AVG.Hour_Total_Value+=AVG.Min_Ave_Value; //һСʱ����ֵ
			if(RecorDataHour_Flag==60)
				{
					RecorDataHour_Flag=0;
					AVG.Hour_Ave_Value=AVG.Hour_Total_Value/60; //һСʱ��ƽ��ֵ
						OSIntEnter();
					FuckingYou_New_Writedata_Hour(AVG.Hour_Ave_Value);
						OSIntExit(); 	
					AVG.Hour_Total_Value=0;
				}		
				AVG.Day_Total_Value+=AVG.Min_Ave_Value;
		  	if(RecorDataday_Flag==1440)
				{
					RecorDataday_Flag=0;
					AVG.Day_Ave_Value=AVG.Day_Total_Value/60/24; //һ���ƽ��ֵ
						OSIntEnter();
					FuckingYou_New_Writedata_day(AVG.Day_Ave_Value);
						OSIntExit(); 	
					AVG.Day_Total_Value=0;
				}							
}

//����������
void dark_current(void)
{
	DCPMESampVL= SO2CONCERNRATIONCAL.PMTSampVL;
	REFVOL_PMT=REFVOL_PD ;  //�ο�������
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
		 len1=sprintf(FTOCHAR[0],"%.3f",CaseTEM);        //��Ӧ���¶� //REFVOL_PD
		 len2=sprintf(FTOCHAR[1],"%.3f",TECTEM);         //pmt�ڲ��¶�
		 len3=sprintf(FTOCHAR[2],"%.3f",CRAteTEM);       //�����¶�
		 len4=sprintf(FTOCHAR[3],"%.3f",SampGasP);       //����ѹ��
		 len5=sprintf(FTOCHAR[4],"%.3f",CaseGasPRESSURE);//��Ӧ��ѹ��
		 len6=sprintf(FTOCHAR[5],"%.3f",REFVOL_PD);      //�ο���ǿ
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
if((45<CaseTEM)&&(CaseTEM<55))//��Ӧ���¶�
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
	if((5<TECTEM)&&(TECTEM<12))//TEC�¶�,PMT�ڲ��¶�
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
	if((5<CRAteTEM)&&(CRAteTEM<55))//ji�����¶�
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
	if((0<SampGasP)&&(SampGasP<18))//���������ѹ��
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
	if((7<=CaseGasPRESSURE)&&(CaseGasPRESSURE<=14))////��Ӧ������ѹ��
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
	//if((1<=REFVOL_PD)&&(REFVOL_PD<=4)) //�ο���ǿPD״̬��ʾ
			if((0.8<=REFVOL_PD)&&(REFVOL_PD<=4)) //�ο���ǿPD״̬��ʾ 2020 5 25 
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
	 //EE B1 00 00 00 FF FC FF FF //�л���0����
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

