#include "command.h"
#include "usart.h"	
#include <string.h>
#include <ctype.h>			//????
#include <stdlib.h>
#include <stdio.h>
#include "Math.h"
#include "rtc.h"
#include "datawr.h"
#include "processing.h"
#include "communication.h"
	unsigned int checkout(void)
		{
			unsigned char i;
		//	unsigned char rlen;
			char Rx1_String[64] = {0};
			
				if(USARTStructure1.RX_Flag == 1)		  										
				{
						USARTStructure1.RX_Flag = 0;														
				 
						for(i=0; i< USARTStructure1.RX_TMEP_Len; i++)
						{
								Rx1_String[i] = toupper(USARTStructure1.RX_TEMP_BUFF[i]); 
						}
						USARTStructure1.RX_TMEP_Len=0;
						 Rx1_String[i] = '\0';		
							if(RTC_Set_Patameter(Rx1_String) == 0 ){	printf(" Setting Time ok \r\n");    return 0;}	//sensor 
							
				   	else if(strcmp("FUCKING_YOU_READ",Rx1_String) == 0 ){FuckingYou_New_Readdata_min(); return 0;}  
						else if(strcmp("KUAIMEN_OPEN",Rx1_String) == 0 )   { KuaiMen_Switch(0x01,0x01,0x01,0x01);  return 0;}//dakai 快门，采样口，打开PMT灯，同时 ;  	
				    else if(strcmp("KUAIMEN_CLOSE",Rx1_String) == 0 )   { KuaiMen_Switch(0x00,0x00,0x00,0x01);  return 0;}
						else if(strcmp("CALIBRATIONVALVE_OPEN",Rx1_String) == 0 )   { KuaiMen_Switch(0x01,0x01,0x01,0x02); switch_status=1; return 0;}//切到校准口;  	
				    else if(strcmp("SAMPLINGVALVE_OPEN",Rx1_String) == 0 )   { KuaiMen_Switch(0x00,0x00,0x00,0x02);switch_status=0;  return 0;}//切到采样口
						else if(strcmp("PMT_OPEN",Rx1_String) == 0 )   { KuaiMen_Switch(0x01,0x01,0x01,0x04);  return 0;}//PMT	
				    else if(strcmp("PMT_CLOSE",Rx1_String) == 0 )   { KuaiMen_Switch(0x00,0x00,0x00,0x04);  return 0;}//PMT

						else
						{
								printf("COMMAND IS NOT \r\n");
								printf("@_@\r\n");
						}
				}
		    	return 0;
		}


unsigned char RTC_Set_Patameter(char *str)
{
	u8 i;
	u16 _year=0;
	u8 _month=0, _date=0, _hour=0,_minute=0,_second=0;
	if((str[0]=='R')&&(str[1]=='T')&&(str[2]=='C')&&(str[3]=='_')&&(str[4]=='S')&&(str[5]=='E')&&(str[6]=='T')&&(str[7]=='(')  )
	{
		  for(i=8;i<12;i++)
		     {
		      if((str[i]>='0')&&(str[i]<='9'))
		         _year +=(str[i]-'0')*pow(10,11-i);
				  	 else return 1;
			   }
				 
				  //str[12]=','
			for(i=13;i<15;i++)
				 {
					 if((str[i]>='0')&&(str[i]<='9'))
		         _month +=(str[i]-'0')*pow(10,14-i);
					  else return 1;
				 }
		      //	str[15]=','		
		  				for(i=16;i<18;i++)
				 {
					 if((str[i]>='0')&&(str[i]<='9'))
		         _date +=(str[i]-'0')*pow(10,17-i);
					  else return 1;
				 }	
		       //	str[18]=','	
				 				for(i=19;i<21;i++)
				 {
					 if((str[i]>='0')&&(str[i]<='9'))
		         _hour +=(str[i]-'0')*pow(10,20-i);
					  else return 1;
				 }			
				    //	str[21]=','	
				 	  	for(i=22;i<24;i++)
				 {
					 if((str[i]>='0')&&(str[i]<='9'))
		         _minute +=(str[i]-'0')*pow(10,23-i);
					  else return 1;
				 }	 
				    //	str[21]='24'	
				 	  	for(i=25;i<27;i++)
				 {
					 if((str[i]>='0')&&(str[i]<='9'))
		         _second +=(str[i]-'0')*pow(10,26-i);
					  else return 1;
				 }	
				 
      RTC_Set(_year,_month,_date,_hour,_minute,_second) ;      				 
			return 0;	 
	}
	
	return 1;
}

