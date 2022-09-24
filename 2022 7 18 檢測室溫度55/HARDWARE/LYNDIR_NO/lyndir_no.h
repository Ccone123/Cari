#ifndef _LYNDIR_NO_H
#define _LYNDIR_NO_H
#include "sys.h"
#pragma pack(push)
#pragma pack(1)                           //按字节对齐
typedef struct
{
	float Span_Old;								//读取量程校准系数   usRegHoldingBuf【0】usRegHoldingBuf[1]
	float Con_New; 								//通标准气体浓度值   usRgHoldingBuf[2]usRegHoldingBuf[3]，写
	float Con_Old; 								//读取当前气体浓度值 usRegHoldingBuf[4]sRegHoldingBuf[5]寥∨ǘ�
	float Span_New;								//写入量程校准系数   usRegHoldingBuf[6]usRegHoldingBuf[7]
	uint16_t  Zero_Cal; 					//写入零点校准指令帧 usRegHoldingBuf[8],
	uint16_t  Zero_CalFinish_Flag;//零点校准完成标志	 usRegHoldingBuf[9]，读，写。读取如果=1代表零点校准完成，写0，代表零点校准接收。读到1后写0.
	uint16_t  Zero_Cal_Flag;      //usRegHoldingBuf[10]开始零点校准写零点校准标志，  校准标志usRegHoldingBuf[10],只写，写，
	uint16_t  Span_Old_Flag;      //usansRegHoldingBuf[11]开始读取量程校准系数
	uint16_t  Span_New_Flag;      //usRegHoldingBuf[12]开始写新的量程校准系数标志 
	uint16_t   LD_CONTROL;	      //usRegHoldingBuf[13] 写光源控制
	float      PD_Vol;            //用于校准usRegHoldingBuf[14],usRegHoldingBuf[15]
	float      LD_TH1_Tem;        //光源温度usRegHoldingBuf[16],usRegHoldingBuf[17]
	float      PD_TH2_Tem;        //PD温度usRegHoldingBuf[18],usRegHoldingBuf[19]
	float      Re_Pressure;       //大气压usRegHoldingBuf[20],usRegHoldingBuf[21]
  float      Humity;             //usRegHoldingBuf[22]usRegHoldingBuf[23]
	uint16_t   FAN_CONTROL;            //风扇usRegHoldingBuf[24]
  uint16_t   FANhot_CONTROL;        //散热风扇usRegHoldingBuf[25]
  uint16_t   PUMP_CONTROL;           //气泵usRegHoldingBuf[26]
  uint16_t   ADDhot_CONTROL;         //冷凝片usRegHoldingBuf[27]
  uint16_t   PPUMP_CONTROL;           //蠕动泵usRegHoldingBuf[28]	
	float      IN_TH3_Tem ;              // 入口烟气温度采集 usRegHoldingBuf[29]	 usRegHoldingBuf[30]
	float      CHAM_TH4_Tem;            ////烟尘气室温度 	 usRegHoldingBuf[31]	usRegHoldingBuf[32]	
	float      PCB_TH5_Tem;              ////机箱内温度     usRegHoldingBuf[33]	usRegHoldingBuf[34]
  float      No_Press;	               //NOm模块压力      usRegHoldingBuf[35]usRegHoldingBuf[36]
	uint16_t   No_Status;                //NOm模块状态     usRegHoldingBuf[37]
	float      Uncorrected_Pressure;    //气压差            usRegHoldingBuf[38] usRegHoldingBuf[39]
	float      New_Humity;              //校准后的湿度       usRegHoldingBuf[40] usRegHoldingBuf[41]
  float      New_Con_Old;             //校准后的浓度       usRegHoldingBuf[42] usRegHoldingBuf[43]
	float      Humity_O2_ADC_V;         //s湿度电压          usRegHoldingBuf[44] usRegHoldingBuf[45]
	uint16_t    Calman_inital;          //卡尔曼标志位usRegHoldingBuf[46]
	float       Battery_Level;          //usRegHoldingBuf[47],usRegHoldingBuf[48]
  uint16_t    Duoji_CONTROL;          //usRegHoldingBuf[49]
} Para_Data,*PPara_Data;
#pragma apop
//extern Para_Data Lyndir_No;
extern PPara_Data Lyndir_No;
 float CALCoefficient_Calulation(float Old);
 void Read_Concentration_Value(void);
 void Read_NOPressure_Value(void);
 void Read_NOStatus_Value(void);
 void Zero_Calibration_Write(void);
 void Read_Zero_CalFinish_Flag(void);
 void Span_CalCoefficientR_Value(void);
 void Span_CalCoefficientW_Value(void);
 float CALCoefficient_Calulation(float Old);
  void Calibrate_Function(void);
#endif
