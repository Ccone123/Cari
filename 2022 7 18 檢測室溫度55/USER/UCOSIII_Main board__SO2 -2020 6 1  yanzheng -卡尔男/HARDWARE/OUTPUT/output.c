#include "output.h"
#include "processing.h"
#include "mbcrc.h"
#include "rtc.h"
#include "IICEEPROM.h"

flag WFlag;
extern uint16_t usRegInputBuf[100];
extern uint16_t usRegHoldingBuf[100];



//int **t;
//int a[3][4];
void poiters (void)
{
	//t=a;
}




//写相关参数
typedef union 
           {
						 float exdataFLOAT;
						  u16  tempdata[2];
					 }DataGet;
				
	DataGet DataGet_HoldingBuf;				 
					 
void ReadValue_InputBuff(void)
{
	  u16 *P[50];
	 char i;
   P[0] = (u16 *) &SO2CONCERNRATIONCAL.PMTSampVL;
	 P[1] = ((u16 *)&SO2CONCERNRATIONCAL.PMTSampVL)+1; //测量光强
	
   P[2] = (u16 *) &DCPMESampVL;
	 P[3] = ((u16 *)&DCPMESampVL)+1;//测量暗电流
	
	 P[4] = (u16 *) &REFVOL_PD;
	 P[5] = ((u16 *)&REFVOL_PD)+1; //参考光强
	
	 P[6] = (u16 *) &REFVOL_PMT;
	 P[7] = ((u16 *)&REFVOL_PMT)+1;//参考暗电流
	
	 P[8] = (u16 *) &GASFLOW;
	 P[9] = ((u16 *)&GASFLOW)+1;//气体流量
	
	 P[10] = (u16 *) &SampGasP;
	 P[11] = ((u16 *)&SampGasP)+1; //采样气体的压力	
	
	 P[12] = (u16 *) &CaseGasPRESSURE;
	 P[13] = ((u16 *)&CaseGasPRESSURE)+1;	//反应室气体压力	 
	
	 P[14] = (u16 *) &CaseTEM;
	 P[15] = ((u16 *)&CaseTEM)+1; 	//反应室温度
	 
   P[16] = (u16 *) &CRAteTEM;
	 P[17] = ((u16 *)&CRAteTEM)+1; 	//ji机箱温度
	 
	 P[18] = (u16 *) &TECTEM;
	 P[19] = ((u16 *)&TECTEM)+1;//PMT温度		
	 
	 P[20] = (u16 *) &SO2ConCenTration_AfterCal;
	 P[21] = ((u16 *)&SO2ConCenTration_AfterCal)+1; //PMT驱动电压

	 P[22] = (u16 *) &Change_Lamp_Val;
	 P[23] = ((u16 *)&Change_Lamp_Val)+1;//替换灯
	 
	 P[24] = (u16 *) &PMT_DriValage;
	 P[25] = ((u16 *)&PMT_DriValage)+1; //PMT驱动电压
	 for(i=0;i<26;i++)
	 {
	  usRegInputBuf[i] = *P[i];   //发送到输入寄存器
	 }
	 usRegInputBuf[50]=readwarmingINF.warmingcaseTEM_Flag||readwarmingINF.warmingPMTTEM_Flag||readwarmingINF.warmingcasereactionTEM_Flag||readwarmingINF.warmingSamplingpressure_Flag||readwarmingINF.casereactionpressure_Flag||readwarmingINF.warmingSamplingflow_Flag||readwarmingINF.warminglightintensityVolt_Flag||readwarmingINF.warmingREFVOL_PD_Flag||readwarmingINF.warmingREFVOL_PMT_Flag||readwarmingINF.warmingChangePD_Flag;
	 usRegInputBuf[51]=readwarmingINF.warmingcaseTEM_Flag;
	 usRegInputBuf[52]=readwarmingINF.warmingPMTTEM_Flag;
	 usRegInputBuf[53]=readwarmingINF.warmingcasereactionTEM_Flag;
	 usRegInputBuf[54]=readwarmingINF.warmingSamplingpressure_Flag;
	 usRegInputBuf[55]=readwarmingINF.casereactionpressure_Flag;
	 usRegInputBuf[56]=readwarmingINF.warmingSamplingflow_Flag;
	 usRegInputBuf[57]=readwarmingINF.warminglightintensityVolt_Flag;
	 usRegInputBuf[58]=readwarmingINF.warmingREFVOL_PD_Flag;
	 usRegInputBuf[59]=readwarmingINF.warmingREFVOL_PMT_Flag;	 
	 usRegInputBuf[60]=readwarmingINF.warmingChangePD_Flag;
}
void ReadValue_HoldingBuf(void)
{
	int i;
	for(i=0;i<6;i++)
	{
		usRegHoldingBuf[i]=ReadEpData(1024+i*8);
	}
//	usRegHoldingBuf[22]=0x28f6;
//	usRegHoldingBuf[23]=0x420e;
}

void WriteValue_HoldingBuf(char StartRegx,char IndexNumx)
{
	int i;
		if((WFlag.HoldingregStart==0)&&(WFlag.iRegIndex)>=6)
		{
	              calendar._year = usRegHoldingBuf[0];
	              calendar._month = usRegHoldingBuf[1];
                calendar._date = usRegHoldingBuf[2];
	              calendar._hour  =usRegHoldingBuf[3];
	              calendar._min=usRegHoldingBuf[4];
                calendar._sec=usRegHoldingBuf[5];
			 RTC_Set(calendar._year,calendar._month,calendar._date,calendar._hour,calendar._min,calendar._sec);  
			   for(i=0;i<WFlag.iRegIndex;i++)       //写保持寄存器的值
			    {
						SaveEpData(1024+i*8,usRegHoldingBuf[i]);  
					}
			 
		}  
   
		
//	    DataGet_HoldingBuf.tempdata[0]= usRegHoldingBuf[22];
//	    DataGet_HoldingBuf.tempdata[1]= usRegHoldingBuf[23];
//	    PMT_DriValage =DataGet_HoldingBuf.exdataFLOAT;
}

void data_transmitter(char StartReg,char IndexNum)
{
	
	WriteValue_HoldingBuf(WFlag.HoldingregStart,WFlag.iRegIndex);
	
	 
}





