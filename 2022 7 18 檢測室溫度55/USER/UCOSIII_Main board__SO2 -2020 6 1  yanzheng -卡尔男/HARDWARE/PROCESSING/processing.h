#ifndef _PROCESSING_H
#define _PROCESSING_H
#include "sys.h"
extern float elmResultBuf[8][101] ;
extern u8 reqErrCnt[8];
extern float FiveMin_AVG_dis;
extern const char string[2][6];
extern const u16 ADDress3[];
extern float SO2ConCenTration;		

extern float SO2concentration;//S02的浓度,du
//"so2 浓度的一个建立"
extern float SO2ConCenTration;//S02浓度测试
extern float SO2ConCenTration_AfterCal;//滑动平均以后的浓度
extern float SO2SD;//S02标准差
extern float DCPMESampVL;//暗电流采样状态下，PMT采样电压
extern float REFVOL_PD;//参考光强PD
extern float REFVOL_PMT;//参考暗电流PD
extern float OVERALLD;//归一化值
extern float GASFLOW;//气体流量,单位:sccm
extern float SampGasP;//采样气体的压力
extern float CaseGasPRESSURE;//反应室气体压力
extern float CaseTEM;//反应室温度
extern float CRAteTEM;//ji机箱温度
extern float TECTEM;//TEC温度
extern float Change_Lamp_Val; //换灯电压
extern float PMT_DriValage;
//读仪器参数
typedef struct {
	              u16 year;u8 month;u8 day;u8 hour; u8 minute; u8 sencond; //系统时间
               }readparameter;
extern  readparameter parameter; 
typedef struct     //待测相关参数
{	
float SO2STARDCON_INPUT ;//标准浓度输入
float PMTSampVL;//待测气体采样状态下PMT采样电压
float PMTSampVL_ZERO;//PMT采样电压更新
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
float COV_ZERO;  //变异系数 零点
float COV_STANDARD;//变异系数 标气点
//float S02KK;
//float S02BB;
}SO2CONCERNRATION;
extern SO2CONCERNRATION SO2CONCERNRATIONCAL;


extern const u16 ADDress2[];
							 
extern const u8 CALREGNUM[];
//仪器器件操作流程相关变量
//读仪器期间状态变量
//typedef struct {
//	               u8 deviceopratestatus; //器件操作状态
//	               u8 samplingvalve;      //采样/校准乏开关
//	               u8 Lightsourceshutter; //快门
//	               u8 PMTlight;           //PMT灯
//	               u8 caseheatingswitch;  //反应室加热开关
//               } readdevice;

//extern readdevice readdeviceoperate;
////写仪器器件状态变量
//							 typedef struct {
//	               u8 setdeviceopratestatus; //器件操作状态
//	               u8 setsamplingvalve;      //采样/校准乏开关
//	               u8 setLightsourceshutter; //快门
//	               u8 setPMTlight;           //PMT灯
//	               u8 setcaseheatingswitch;  //反应室加热开关
//               } writedevice;

//extern writedevice writedeviceoperate;
////写仪器器件状态变量标志
//			 typedef struct {
//	               u8 setdeviceopratestatusFlag; //器件操作状态标志
//	               u8 setsamplingvalveFlag;      //采样/校准乏开关标志
//	               u8 setLightsourceshutterFlag; //快门标志
//	               u8 setPMTlightFlag;           //PMT灯标志
//	               u8 setcaseheatingswitchFlag;  //反应室加热开关标志
//               } writedeviceFlag;

//extern writedeviceFlag writedeviceoperateFlag;							 
				 
//器件报警信息相关变量详述							 
typedef struct{
	             u8 warmingstatus_Flag;//报警状态
	             u8 warmingcaseTEM_Flag;//机箱温度异常报警
	             u8 warmingPMTTEM_Flag; //传感器温度异常
	             u8 warmingcasereactionTEM_Flag;//反应室温度异常
	             u8 warmingSamplingpressure_Flag;//采样压力异常
	             u8 casereactionpressure_Flag;//反应室压力异常
	             u8 warmingSamplingflow_Flag;//采样流量异常
	             u8 warminglightintensityVolt_Flag;//测量光强异常
	             u8 warmingREFVOL_PD_Flag;//参考光强异常
	             u8 warmingREFVOL_PMT_Flag;//暗电流异常
	             u8 warmingChangePD_Flag;//更换灯
              }readwarm;

extern readwarm readwarmingINF;
//读SCREEN0 ,标志。
typedef struct {
                 u8 SCRREN0FLAG;//屏幕0
	               u8 FristGetPowerFlag;//第一次上电读屏
	               u8 IntialonceFlag;
	               u8 WarnPara_Flag;
	               u8 SCRREN1FLAG;//屏幕1
	               u8 SCRREN2FLAG;//屏幕2
	 u8 SCRREN3FLAG;
	 u8 SCRREN4FLAG;
	 u8 SCRREN5FLAG;
	 u8 SCRREN6FLAG;
	 u8 SCRREN7FLAG;
	 u8 SCRREN8FLAG;
	 u8 SCRREN9FLAG;
	 u8 SCRREN10FLAG;
	 u8 SCRREN11FLAG;
	               u8 ZeroCALFLAG;//零点标定标志
	               u8 StadagasCALFLAG;//标气标定标志
	               u8 CACULATEKBFLAG; //校准K,B标志位
	               u8 Record_dadaFLAG;//记录K,B标志
	               u8 Record_SO2BKVFlag;//记录S02本地值标志
	               u8 Record_SO2BKVFinshFlag;//记录SO2本底值标定完毕。
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
	          float     FiveMin_Aver;//5 分钟平均值
              }		average	;
extern average AVG;

/*************************************************************/							 	 
//void readinstruparameter (void);			//仪器参数设置				 
//void writeinstruparameter (void);
//void readwarmingsianal(void);					//读报警								 
void readscreen0(void);//读屏幕
int Read_SO2ConCenTration(void);//读SO2no浓度			 
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
void Display_Sampling_Switch (char x);//通气模式
void Icon_Display (char x);   //报警内容
void Test_Uint(const char *str);
#endif
