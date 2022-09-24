#include "command.h"
#include "usart.h"	
#include <string.h>
#include <ctype.h>			//????
#include <stdlib.h>
#include <stdio.h>
#include <Math.h>
#include "lyndir_no.h"
#include "iicssc.h"
	unsigned int checkout(void)
		{
			unsigned char i;
//			int tempValue=0;	
		//	unsigned char rlen;
			 float tempValue=0;	
			char Rx1_String[128] = {0};
			
				if(USARTStructure1.RX_Flag == 1)		  										
				{
						USARTStructure1.RX_Flag = 0;														
				 
						for(i=0; i< USARTStructure1.RX_TMEP_Len; i++)
						{
							//	Rx1_String[i] = toupper(USARTStructure1.RX_TEMP_BUFF[i]);
               Rx1_String[i] = USARTStructure1.RX_TEMP_BUFF[i]; 							
						}
					 	USARTStructure1.RX_TMEP_Len=0;
						 Rx1_String[i] = '\0';							
			      if(strcmp("",Rx1_String) == 0 ){ ;printf("@_@\r\n");return 0;}  
						else if(strcmp("READ_Concentration_Value",Rx1_String) == 0 )   {Read_Concentration_Value(); printf("@_@\r\n");return 0;}					
				    else if(strcmp("Zero_Calibration_Write",Rx1_String) == 0 )   { Zero_Calibration_Write();printf("@_@\r\n");return 0;}	
					  else if(strcmp("Read_Zero_CalFinish_Flag",Rx1_String) == 0 )   {Read_Zero_CalFinish_Flag(); printf("@_@\r\n");return 0;}	
				    else if(strcmp("Span_CalCoefficientR_Value",Rx1_String) == 0 )   { Span_CalCoefficientR_Value();printf("@_@\r\n");return 0;}          
            else if(strcmp("Span_CalCoefficientW_Value",Rx1_String) == 0 )   {Span_CalCoefficientW_Value(); printf("@_@\r\n");return 0;}	
            else if(strcmp("DEVICEID",Rx1_String) == 0 )	                     { Change_DeviceID();}
						else if(strcmp("DEVICEID1",Rx1_String) == 0 )	                     { Change_DeviceID1();}
              else if(strncmp("Con_",Rx1_String,4)==0)                    {tempValue=GetStringNum(Rx1_String); Lyndir_No->Con_New=tempValue;   printf("tempValue=%f\r\n",tempValue);printf("@_@\r\n");return 0;}						                                            //sscanf(str,"%f",&value); 
				                                                              
						else 
						{
								printf("COMMAND IS NOT EXIST \r\n");
								printf("@_@\r\n");
						}
				}
		    	return 0;
		}
 int GetStringNum(char *ptr)
 {
	 uint8_t k=0;
	int b[4];
	char buf[40];
	b[0]=0;
	while (ptr[k] !='\0') { 
		if(isdigit(ptr[k])){
			buf[k] =ptr[k]; 	    
		}else{
			buf[k] = ' '; 
		}
		++k; 
	} 
	buf[k]='\0';
	sscanf(buf,"%u", &b[0]);// 10 jinzhi shu
	return b[0];            //get configuration value
 }



