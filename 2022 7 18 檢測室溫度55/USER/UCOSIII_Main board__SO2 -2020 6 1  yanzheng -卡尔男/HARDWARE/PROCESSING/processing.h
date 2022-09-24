#ifndef _PROCESSING_H
#define _PROCESSING_H
#include "sys.h"
extern float elmResultBuf[8][101] ;
extern u8 reqErrCnt[8];
extern float FiveMin_AVG_dis;
extern const char string[2][6];
extern const u16 ADDress3[];
extern float SO2ConCenTration;		

extern float SO2concentration;//S02��Ũ��,du
//"so2 Ũ�ȵ�һ������"
extern float SO2ConCenTration;//S02Ũ�Ȳ���
extern float SO2ConCenTration_AfterCal;//����ƽ���Ժ��Ũ��
extern float SO2SD;//S02��׼��
extern float DCPMESampVL;//����������״̬�£�PMT������ѹ
extern float REFVOL_PD;//�ο���ǿPD
extern float REFVOL_PMT;//�ο�������PD
extern float OVERALLD;//��һ��ֵ
extern float GASFLOW;//��������,��λ:sccm
extern float SampGasP;//���������ѹ��
extern float CaseGasPRESSURE;//��Ӧ������ѹ��
extern float CaseTEM;//��Ӧ���¶�
extern float CRAteTEM;//ji�����¶�
extern float TECTEM;//TEC�¶�
extern float Change_Lamp_Val; //���Ƶ�ѹ
extern float PMT_DriValage;
//����������
typedef struct {
	              u16 year;u8 month;u8 day;u8 hour; u8 minute; u8 sencond; //ϵͳʱ��
               }readparameter;
extern  readparameter parameter; 
typedef struct     //������ز���
{	
float SO2STARDCON_INPUT ;//��׼Ũ������
float PMTSampVL;//�����������״̬��PMT������ѹ
float PMTSampVL_ZERO;//PMT������ѹ����
float PMTSampVL_STADARD;
float SO2K;//k
float SO2B;//b
float SO2BackGroundValue;
float SteadyNumerical;
float CorrectValue_B;
float Seek_SO2BGV[2];
float Seek_SO2SNV[2];

char SO2BGVLen;
float D_Value;
float COV_ZERO;  //����ϵ�� ���
float COV_STANDARD;//����ϵ�� ������
//float S02KK;
//float S02BB;
}SO2CONCERNRATION;
extern SO2CONCERNRATION SO2CONCERNRATIONCAL;


extern const u16 ADDress2[];
							 
extern const u8 CALREGNUM[];
//������������������ر���
//�������ڼ�״̬����
//typedef struct {
//	               u8 deviceopratestatus; //��������״̬
//	               u8 samplingvalve;      //����/У׼������
//	               u8 Lightsourceshutter; //����
//	               u8 PMTlight;           //PMT��
//	               u8 caseheatingswitch;  //��Ӧ�Ҽ��ȿ���
//               } readdevice;

//extern readdevice readdeviceoperate;
////д��������״̬����
//							 typedef struct {
//	               u8 setdeviceopratestatus; //��������״̬
//	               u8 setsamplingvalve;      //����/У׼������
//	               u8 setLightsourceshutter; //����
//	               u8 setPMTlight;           //PMT��
//	               u8 setcaseheatingswitch;  //��Ӧ�Ҽ��ȿ���
//               } writedevice;

//extern writedevice writedeviceoperate;
////д��������״̬������־
//			 typedef struct {
//	               u8 setdeviceopratestatusFlag; //��������״̬��־
//	               u8 setsamplingvalveFlag;      //����/У׼�����ر�־
//	               u8 setLightsourceshutterFlag; //���ű�־
//	               u8 setPMTlightFlag;           //PMT�Ʊ�־
//	               u8 setcaseheatingswitchFlag;  //��Ӧ�Ҽ��ȿ��ر�־
//               } writedeviceFlag;

//extern writedeviceFlag writedeviceoperateFlag;							 
				 
//����������Ϣ��ر�������							 
typedef struct{
	             u8 warmingstatus_Flag;//����״̬
	             u8 warmingcaseTEM_Flag;//�����¶��쳣����
	             u8 warmingPMTTEM_Flag; //�������¶��쳣
	             u8 warmingcasereactionTEM_Flag;//��Ӧ���¶��쳣
	             u8 warmingSamplingpressure_Flag;//����ѹ���쳣
	             u8 casereactionpressure_Flag;//��Ӧ��ѹ���쳣
	             u8 warmingSamplingflow_Flag;//���������쳣
	             u8 warminglightintensityVolt_Flag;//������ǿ�쳣
	             u8 warmingREFVOL_PD_Flag;//�ο���ǿ�쳣
	             u8 warmingREFVOL_PMT_Flag;//�������쳣
	             u8 warmingChangePD_Flag;//������
              }readwarm;

extern readwarm readwarmingINF;
//��SCREEN0 ,��־��
typedef struct {
                 u8 SCRREN0FLAG;//��Ļ0
	               u8 FristGetPowerFlag;//��һ���ϵ����
	               u8 IntialonceFlag;
	               u8 WarnPara_Flag;
	               u8 SCRREN1FLAG;//��Ļ1
	               u8 SCRREN2FLAG;//��Ļ2
	 u8 SCRREN3FLAG;
	 u8 SCRREN4FLAG;
	 u8 SCRREN5FLAG;
	 u8 SCRREN6FLAG;
	 u8 SCRREN7FLAG;
	 u8 SCRREN8FLAG;
	 u8 SCRREN9FLAG;
	 u8 SCRREN10FLAG;
	 u8 SCRREN11FLAG;
	               u8 ZeroCALFLAG;//���궨��־
	               u8 StadagasCALFLAG;//�����궨��־
	               u8 CACULATEKBFLAG; //У׼K,B��־λ
	               u8 Record_dadaFLAG;//��¼K,B��־
	               u8 Record_SO2BKVFlag;//��¼S02����ֵ��־
	               u8 Record_SO2BKVFinshFlag;//��¼SO2����ֵ�궨��ϡ�
	               u8 SO2STARDCON_INPUT_Flag;
               }	READSCRRENFLAG;

extern READSCRRENFLAG READSCRREN;
typedef struct{
	          float    Min_Ave_Value;
	          float    FiveMin_Ave_Value;
	          float    Min_Total_Value;
            float    Hour_Ave_Value;	
            float    Hour_Total_Value;
            float    Day_Total_Value;
            float    Day_Ave_Value;		
	          float     FiveMin_Aver;//5 ����ƽ��ֵ
              }		average	;
extern average AVG;

/*************************************************************/							 	 
//void readinstruparameter (void);			//������������				 
//void writeinstruparameter (void);
//void readwarmingsianal(void);					//������								 
void readscreen0(void);//����Ļ
int Read_SO2ConCenTration(void);//��SO2noŨ��			 
void ReadCurrentStatus(void);	
void ReadSO2(void);
void ReadPMTSampVL (void);
void Read_Total_Parameter (void);
int Calibration_SO2ConCenTration(void);
void Set_Warn(void);
void Read_SetWarn(void);
void   Record_Data_Task(void);
void PMT_Dri_Valage(void);
void dark_current(void);
u8 yure_jiemian(void);
void CheckWarn (void);
void Display_Sampling_Switch (char x);//ͨ��ģʽ
void Icon_Display (char x);   //��������
void Test_Uint(const char *str);
#endif
