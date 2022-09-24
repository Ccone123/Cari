#ifndef _LYNDIR_NO_H
#define _LYNDIR_NO_H
#include "sys.h"
#pragma pack(push)
#pragma pack(1)                           //���ֽڶ���
typedef struct
{
	float Span_Old;								//��ȡ����У׼ϵ��   usRegHoldingBuf��0��usRegHoldingBuf[1]
	float Con_New; 								//ͨ��׼����Ũ��ֵ   usRgHoldingBuf[2]usRegHoldingBuf[3]��д
	float Con_Old; 								//��ȡ��ǰ����Ũ��ֵ usRegHoldingBuf[4]�usRegHoldingBuf[5]���ȡŨ��
	float Span_New;								//д������У׼ϵ��   usRegHoldingBuf[6]usRegHoldingBuf[7]
	uint16_t  Zero_Cal; 					//д�����У׼ָ��֡ usRegHoldingBuf[8],
	uint16_t  Zero_CalFinish_Flag;//���У׼��ɱ�־	 usRegHoldingBuf[9]������д����ȡ���=1�������У׼��ɣ�д0���������У׼���ա�����1��д0.
	uint16_t  Zero_Cal_Flag;      //usRegHoldingBuf[10]��ʼ���У׼д���У׼��־��  У׼��־usRegHoldingBuf[10],ֻд��д��
	uint16_t  Span_Old_Flag;      //usansRegHoldingBuf[11]��ʼ��ȡ����У׼ϵ��
	uint16_t  Span_New_Flag;      //usRegHoldingBuf[12]��ʼд�µ�����У׼ϵ����־ 
	uint16_t   LD_CONTROL;	      //usRegHoldingBuf[13] д��Դ����
	float      PD_Vol;            //����У׼usRegHoldingBuf[14],usRegHoldingBuf[15]
	float      LD_TH1_Tem;        //��Դ�¶�usRegHoldingBuf[16],usRegHoldingBuf[17]
	float      PD_TH2_Tem;        //PD�¶�usRegHoldingBuf[18],usRegHoldingBuf[19]
	float      Re_Pressure;       //����ѹusRegHoldingBuf[20],usRegHoldingBuf[21]
  float      Humity;             //usRegHoldingBuf[22]usRegHoldingBuf[23]
	uint16_t   FAN_CONTROL;            //����usRegHoldingBuf[24]
  uint16_t   FANhot_CONTROL;        //ɢ�ȷ���usRegHoldingBuf[25]
  uint16_t   PUMP_CONTROL;           //����usRegHoldingBuf[26]
  uint16_t   ADDhot_CONTROL;         //����ƬusRegHoldingBuf[27]
  uint16_t   PPUMP_CONTROL;           //�䶯��usRegHoldingBuf[28]	
	float      IN_TH3_Tem ;              // ��������¶Ȳɼ� usRegHoldingBuf[29]	 usRegHoldingBuf[30]
	float      CHAM_TH4_Tem;            ////�̳������¶� 	 usRegHoldingBuf[31]	usRegHoldingBuf[32]	
	float      PCB_TH5_Tem;              ////�������¶�     usRegHoldingBuf[33]	usRegHoldingBuf[34]
  float      No_Press;	               //NOmģ��ѹ��      usRegHoldingBuf[35]usRegHoldingBuf[36]
	uint16_t   No_Status;                //NOmģ��״̬     usRegHoldingBuf[37]
	float      Uncorrected_Pressure;    //��ѹ��            usRegHoldingBuf[38] usRegHoldingBuf[39]
	float      New_Humity;              //У׼���ʪ��       usRegHoldingBuf[40] usRegHoldingBuf[41]
  float      New_Con_Old;             //У׼���Ũ��       usRegHoldingBuf[42] usRegHoldingBuf[43]
	float      Humity_O2_ADC_V;         //sʪ�ȵ�ѹ          usRegHoldingBuf[44] usRegHoldingBuf[45]
	uint16_t    Calman_inital;          //��������־λusRegHoldingBuf[46]
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
