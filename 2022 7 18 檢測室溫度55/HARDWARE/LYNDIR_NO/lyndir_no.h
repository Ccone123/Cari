#ifndef _LYNDIR_NO_H
#define _LYNDIR_NO_H
#include "sys.h"
#pragma pack(push)
#pragma pack(1)                           //°´×Ö½Ú¶ÔÆë
typedef struct
{
	float Span_Old;								//¶ÁÈ¡Á¿³ÌĞ£×¼ÏµÊı   usRegHoldingBuf¡¾0¡¿usRegHoldingBuf[1]
	float Con_New; 								//Í¨±ê×¼ÆøÌåÅ¨¶ÈÖµ   usRgHoldingBuf[2]usRegHoldingBuf[3]£¬Ğ´
	float Con_Old; 								//¶ÁÈ¡µ±Ç°ÆøÌåÅ¨¶ÈÖµ usRegHoldingBuf[4]£usRegHoldingBuf[5]¬¶ÁÈ¡Å¨¶È
	float Span_New;								//Ğ´ÈëÁ¿³ÌĞ£×¼ÏµÊı   usRegHoldingBuf[6]usRegHoldingBuf[7]
	uint16_t  Zero_Cal; 					//Ğ´ÈëÁãµãĞ£×¼Ö¸ÁîÖ¡ usRegHoldingBuf[8],
	uint16_t  Zero_CalFinish_Flag;//ÁãµãĞ£×¼Íê³É±êÖ¾	 usRegHoldingBuf[9]£¬¶Á£¬Ğ´¡£¶ÁÈ¡Èç¹û=1´ú±íÁãµãĞ£×¼Íê³É£¬Ğ´0£¬´ú±íÁãµãĞ£×¼½ÓÊÕ¡£¶Áµ½1ºóĞ´0.
	uint16_t  Zero_Cal_Flag;      //usRegHoldingBuf[10]¿ªÊ¼ÁãµãĞ£×¼Ğ´ÁãµãĞ£×¼±êÖ¾£¬  Ğ£×¼±êÖ¾usRegHoldingBuf[10],Ö»Ğ´£¬Ğ´£¬
	uint16_t  Span_Old_Flag;      //usansRegHoldingBuf[11]¿ªÊ¼¶ÁÈ¡Á¿³ÌĞ£×¼ÏµÊı
	uint16_t  Span_New_Flag;      //usRegHoldingBuf[12]¿ªÊ¼Ğ´ĞÂµÄÁ¿³ÌĞ£×¼ÏµÊı±êÖ¾ 
	uint16_t   LD_CONTROL;	      //usRegHoldingBuf[13] Ğ´¹âÔ´¿ØÖÆ
	float      PD_Vol;            //ÓÃÓÚĞ£×¼usRegHoldingBuf[14],usRegHoldingBuf[15]
	float      LD_TH1_Tem;        //¹âÔ´ÎÂ¶ÈusRegHoldingBuf[16],usRegHoldingBuf[17]
	float      PD_TH2_Tem;        //PDÎÂ¶ÈusRegHoldingBuf[18],usRegHoldingBuf[19]
	float      Re_Pressure;       //´óÆøÑ¹usRegHoldingBuf[20],usRegHoldingBuf[21]
  float      Humity;             //usRegHoldingBuf[22]usRegHoldingBuf[23]
	uint16_t   FAN_CONTROL;            //·çÉÈusRegHoldingBuf[24]
  uint16_t   FANhot_CONTROL;        //É¢ÈÈ·çÉÈusRegHoldingBuf[25]
  uint16_t   PUMP_CONTROL;           //Æø±ÃusRegHoldingBuf[26]
  uint16_t   ADDhot_CONTROL;         //ÀäÄıÆ¬usRegHoldingBuf[27]
  uint16_t   PPUMP_CONTROL;           //Èä¶¯±ÃusRegHoldingBuf[28]	
	float      IN_TH3_Tem ;              // Èë¿ÚÑÌÆøÎÂ¶È²É¼¯ usRegHoldingBuf[29]	 usRegHoldingBuf[30]
	float      CHAM_TH4_Tem;            ////ÑÌ³¾ÆøÊÒÎÂ¶È 	 usRegHoldingBuf[31]	usRegHoldingBuf[32]	
	float      PCB_TH5_Tem;              ////»úÏäÄÚÎÂ¶È     usRegHoldingBuf[33]	usRegHoldingBuf[34]
  float      No_Press;	               //NOmÄ£¿éÑ¹Á¦      usRegHoldingBuf[35]usRegHoldingBuf[36]
	uint16_t   No_Status;                //NOmÄ£¿é×´Ì¬     usRegHoldingBuf[37]
	float      Uncorrected_Pressure;    //ÆøÑ¹²î            usRegHoldingBuf[38] usRegHoldingBuf[39]
	float      New_Humity;              //Ğ£×¼ºóµÄÊª¶È       usRegHoldingBuf[40] usRegHoldingBuf[41]
  float      New_Con_Old;             //Ğ£×¼ºóµÄÅ¨¶È       usRegHoldingBuf[42] usRegHoldingBuf[43]
	float      Humity_O2_ADC_V;         //sÊª¶ÈµçÑ¹          usRegHoldingBuf[44] usRegHoldingBuf[45]
	uint16_t    Calman_inital;          //¿¨¶ûÂü±êÖ¾Î»usRegHoldingBuf[46]
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
