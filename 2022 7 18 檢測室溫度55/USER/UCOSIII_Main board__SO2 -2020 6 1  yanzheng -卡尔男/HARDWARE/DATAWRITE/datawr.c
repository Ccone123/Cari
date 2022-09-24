#include "datawr.h"
#include "malloc.h"	 
#include "rtc.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <ctype.h>
#include "exfuns.h"
#include "fattester.h" 
#include "includes.h" 
#include "delay.h"
#include "communication.h"
#include "USART3.h"
#include "experiment.h"
extern char Start_Up_Flag;
extern char *FN_SaveCatalog[];
extern u16 CatalogLen;
char *Pname=NULL;
u8 CreatFileFlag;
u32  RevNameS_D=1;
u32  RevNameE_D=30000000;
u32  RevNameS3_D=1;
u32  RevNameE3_D=30000000;
void FuckingYou_New_Pathname(void) //创建新文件夹
{
	u8 Len;
	Pname=(char*)mymalloc(50);
	RTC_Get();
//	if((calendar.hour==4)&&(CreatFileFlag==1))
//	{
		 CreatFileFlag=0;
		 Len=sprintf (Pname,"0:DATA/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);
     mf_open((u8 *)Pname,FA_OPEN_ALWAYS);//打开文件，如果文件不存在，则创建一个新文件
		 mf_close();
		 Len=sprintf (Pname,"0:DATA2/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);
		  mf_open((u8 *)Pname,FA_OPEN_ALWAYS);//打开文件，如果文件不存在，则创建一个新文件
	  	mf_close();
	    Len=sprintf (Pname,"0:DATA3/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);//5分钟存储，2020 3 2
		  mf_open((u8 *)Pname,FA_OPEN_ALWAYS);//打开文件，如果文件不存在，则创建一个新文件
	  	mf_close();
	    Len=sprintf (Pname,"0:DATA4/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);//保存流量等参数
		  mf_open((u8 *)Pname,FA_OPEN_ALWAYS);//打开文件，如果文件不存在，则创建一个新文件
	  	mf_close();
	    Len=sprintf (Pname,"0:DATA5/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);//保存流量等参数
		  mf_open((u8 *)Pname,FA_OPEN_ALWAYS);//打开文件，如果文件不存在，则创建一个新文件
	  	mf_close();
		 printf("\r\n%d-%02d-%02d\r\n",  calendar.w_year,calendar.w_month,calendar.w_date);
     myfree(Pname);
//	}
	if(calendar.hour==3)
		  CreatFileFlag=1;
}
void FuckingYou_New_Writedata_Min(float data)
{
	u8 Len;
	char *P;
	P=(char *)mymalloc(50);
	Pname=(char*)mymalloc(50);
	RTC_Get();
	printf("%d",calendar.w_year);//xinzeng 2020 2 26
	Len=sprintf (Pname,"0:DATA/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);
	 mf_open((unsigned char*)Pname,FA_READ|FA_WRITE); //打开文件，可读可写。
	Len=sprintf (P,"%d-%02d-%02d %02d:%02d:%02d;%0.3f;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec,data);
	 my_lseek(0); 
	mf_write((u8*)P,Len);
	 mf_close(); 
	 myfree(P);
	 myfree(Pname);
	 printf("%d-%02d-%02d %02d:%02d:%02d;%0.3f;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec,data);
}
void FuckingYou_New_Writedata_5MIN(float data)
{
	u8 Len;
	char *P;
	P=(char *)mymalloc(50);
	Pname=(char*)mymalloc(50);
	RTC_Get();
	Len=sprintf (Pname,"0:DATA3/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);
	 mf_open((unsigned char*)Pname,FA_READ|FA_WRITE); //打开文件，可读可写。
	Len=sprintf (P,"%d-%02d-%02d %02d:%02d:%02d;%0.3f;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec,data);
	 my_lseek(0); 
	mf_write((u8*)P,Len);
	 mf_close(); 
	 myfree(P);
	 myfree(Pname);
	 printf("%d-%02d-%02d %02d:%02d:%02d;%0.3f;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec,data);
}
void FuckingYou_New_Writedata_Hour(float data)
{
	u8 Len;
	char *P;
	P=(char *)mymalloc(50);
	Pname=(char*)mymalloc(50);
	RTC_Get();
	Len=sprintf (Pname,"0:DATA2/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);
	 mf_open((unsigned char*)Pname,FA_READ|FA_WRITE); //打开文件，可读可写。
	Len=sprintf (P,"%d-%02d-%02d %02d:%02d:%02d;%0.3f;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec,data);
	 my_lseek(0); 
	mf_write((u8*)P,Len);
	 mf_close(); 
	 myfree(P);
	 myfree(Pname);
	 printf("%d-%02d-%02d %02d:%02d:%02d;%0.3f;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec,data);
}
void FuckingYou_New_Writedata_day(float data)
{
	u8 Len;
	char *P;
	P=(char *)mymalloc(50);
	Pname=(char *)mymalloc(50);
	RTC_Get();
	Len=sprintf(Pname,"0:DATA5/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);
	mf_open((unsigned char*)Pname,FA_READ|FA_WRITE); //打开文件，可读可写。
	Len=sprintf (P,"%d-%02d-%02d %02d:%02d:%02d;%0.3f;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec,data);
		 my_lseek(0); //寻找文件尾部
		mf_write((u8*)P,Len);
	 mf_close(); 
	 myfree(P);
		 myfree(Pname);
	 printf("%d-%02d-%02d %02d:%02d:%02d;%0.3f;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec,data);
}
void FuckingYou_New_Writedata_Parament(float _data1,float _data2,float _data3,float _data4,float _data5,float _data6,float _data7,float _data8)
{
	u8 Len;
	char *P;
	P=(char *)mymalloc(100);
	Pname=(char*)mymalloc(50);
	RTC_Get();
	Len=sprintf (Pname,"0:DATA4/%d-%02d-%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date);
	 mf_open((unsigned char*)Pname,FA_READ|FA_WRITE); //打开文件，可读可写。
	Len=sprintf (P,"%d-%02d-%02d %02d:%02d:%02d;%0.3f;%0.3f;%0.3f;%0.3f;%0.3f;%0.3f;%0.3f;%0.3f;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec,_data1,_data2,_data3,_data4,_data5,_data6,_data7,_data8);
	 my_lseek(0); 
	mf_write((u8*)P,Len);
	 mf_close(); 
	 myfree(P);
	 myfree(Pname);
}

void FuckingYou_New_Readdata_min(void) //单分钟
{
	 int len1,Ex_SendLen;
	 u32 i,j,k;
	 char str[30]={0};
	u32 File_Size;
	u16 index,total;
	char *FN_Catalog=NULL;
		OSIntEnter();
	mf_scan_files("0:DATA");
 printf("%d-%d\r\n",RevNameS_D,RevNameE_D);//打印出时间段     
		for(i=0;i<CatalogLen;i++)
	  {
		FN_Catalog=(char *)mymalloc(30);
		strcpy (FN_Catalog,"0:DATA/");
		 strcat (FN_Catalog,FN_SaveCatalog[i]);
			 printf("%s\r\n",FN_Catalog);	
   mf_open((u8 *)FN_Catalog,0x03); //打开文件，可读可写。
    mf_read(65535);
	  mf_close();
   myfree(FN_Catalog);			
		}	
		Button_Control(0x02,0x12,0);//数据导入液晶屏结束
		 printf("\r\nfinish%d\r\n",CatalogLen);//打印出时间段     
		CatalogLen=0;
		OSIntExit(); 
}
void FuckingYou_New_Readdata_5MIN(void) //5分钟
{
	 int len1,Ex_SendLen;
	 u32 i,j,k;
	 char str[30]={0};
	u32 File_Size;
	u16 index,total;
	char *FN_Catalog=NULL;
		OSIntEnter();
	mf_scan_files("0:DATA3");
 printf("%d-%d\r\n",RevNameS_D,RevNameE_D);//打印出时间段     
		for(i=0;i<CatalogLen;i++)
	  {
		FN_Catalog=(char *)mymalloc(30);
		strcpy (FN_Catalog,"0:DATA3/");
		 strcat (FN_Catalog,FN_SaveCatalog[i]);
			 printf("%s\r\n",FN_Catalog);	
   mf_open((u8 *)FN_Catalog,0x03); //打开文件，可读可写。
    mf_read(65535);
	  mf_close();
   myfree(FN_Catalog);			
		}	
	  Button_Control(0x02,0x12,0);//数据导入液晶屏结束
		 printf("\r\nfinish%d\r\n",CatalogLen);//打印出时间段     
		CatalogLen=0;
		OSIntExit(); 
}
void FuckingYou_New_Readdata_hour(void) //1xiaoshi
{
	 int len1,Ex_SendLen;
	 u32 i,j,k;
	 char str[30]={0};
	u32 File_Size;
	u16 index,total;
	char *FN_Catalog=NULL;
		OSIntEnter();
	mf_scan_files("0:DATA2");
 printf("%d-%d\r\n",RevNameS_D,RevNameE_D);//打印出时间段     
		for(i=0;i<CatalogLen;i++)
	  {
		FN_Catalog=(char *)mymalloc(30);
		strcpy (FN_Catalog,"0:DATA2/");
		 strcat (FN_Catalog,FN_SaveCatalog[i]);
			 printf("%s\r\n",FN_Catalog);	
   mf_open((u8 *)FN_Catalog,0x03); //打开文件，可读可写。
    mf_read(65535);
	  mf_close();
   myfree(FN_Catalog);			
		}	
	  Button_Control(0x02,0x12,0);//数据导入液晶屏结束
		 printf("\r\nfinish%d\r\n",CatalogLen);//打印出时间段     
		CatalogLen=0;
		OSIntExit(); 
}
void FuckingYou_New_Readdata_day(void)
{
	int len1,Ex_SendLen;
	u32 i,j,k;
	char str[30]={0};
	u32 File_Size;
	u16 index,total;
	char *FN_Catalog=NULL;
	OSIntEnter();
		mf_scan_files("0:DATA5");
	 printf("%d-%d\r\n",RevNameS_D,RevNameE_D);//打印出时间段   
		for(i=0;i<CatalogLen;i++)
	  {
				FN_Catalog=(char *)mymalloc(30);
				strcpy (FN_Catalog,"0:DATA5/");
		    strcat (FN_Catalog,FN_SaveCatalog[i]);
			  printf("%s\r\n",FN_Catalog);	
        mf_open((u8 *)FN_Catalog,0x03); //打开文件，可读可写。
        mf_read(65535);
	      mf_close();
        myfree(FN_Catalog);	
		}
		 Button_Control(0x02,0x12,0);//数据导入液晶屏结束
		 printf("\r\nfinish%d\r\n",CatalogLen);//打印出时间段     
		 CatalogLen=0;
		 OSIntExit();
}

void FuckingYou_New_Readdata1(void) //曲线图
{
	 int len1,Ex_SendLen;
	 u32 i,j,k;
	 char str[30]={0};
	u32 File_Size;
	u16 index,total;
	char *FN_Catalog=NULL;
		OSIntEnter();
	 Start_Up_Flag=1;
	 
	mf_scan_files("0:DATA");
	 
  Start_Up_Flag=0;
 printf("%d-%d\r\n",RevNameS_D,RevNameE_D);//打印出时间段     
		for(i=0;i<CatalogLen;i++)
	  {
		FN_Catalog=(char *)mymalloc(30);
		strcpy (FN_Catalog,"0:DATA/");
		 strcat (FN_Catalog,FN_SaveCatalog[i]);
			 printf("%s\r\n",FN_Catalog);	
      mf_open((u8 *)FN_Catalog,0x03); //打开文件，可读可写。
      mf_read1(65535);
	  mf_close();
      myfree(FN_Catalog);			
		}	
	  Button_Control(0x03,0x0A,0);//数据导入液晶屏结束
		 printf("\r\nfinish%d\r\n",CatalogLen);//打印出时间段     
		CatalogLen=0;
		OSIntExit(); 
}
void FuckingYou_New_Readdata1_Hour(void) //曲线图
{
	 int len1,Ex_SendLen;
	 u32 i,j,k;
	 char str[30]={0};
	u32 File_Size;
	u16 index,total;
	char *FN_Catalog=NULL;
		OSIntEnter();
	 Start_Up_Flag=1;
	 
	mf_scan_files("0:DATA2");
	 
  Start_Up_Flag=0;
 printf("%d-%d\r\n",RevNameS_D,RevNameE_D);//打印出时间段     
		for(i=0;i<CatalogLen;i++)
	  {
		FN_Catalog=(char *)mymalloc(30);
		strcpy (FN_Catalog,"0:DATA2/");
		 strcat (FN_Catalog,FN_SaveCatalog[i]);
			 printf("%s\r\n",FN_Catalog);	
   mf_open((u8 *)FN_Catalog,0x03); //打开文件，可读可写。
    mf_read1(65535);
	  mf_close();
   myfree(FN_Catalog);			
		}	
		 printf("\r\nfinish%d\r\n",CatalogLen);//打印出时间段     
		CatalogLen=0;
		OSIntExit(); 
}


int GetStringNum( char *cmd)
{
	uint8_t k=0;
	int b[4];
	char buf[40];
	char buf1[40];
	b[0]=0;
	while (cmd[k] !='\0') { 
		if(isdigit(cmd[k])){
			buf[k] =cmd[k]; 	    
		}else{
	buf[k] = ' '; 
		}
		++k; 
	} 
	buf[k]='\0';
strRemov(buf1,buf,' ');
	
	
	
	//printf("%s\r\n", buf1);
	sscanf(buf1,"%u", &b[0]);
	return b[0];
}

char *strRemov(char* dst,  char* src, char ch )
{
    int i = -1, j = 0;
    while (src[++i])
        if (src[i] != ch)
            dst[j++] = src[i];
    dst[j] = '\0';
    return dst;
}

void Event_Record(char event_type)
{
	u8 Len,i;
	char *PL;
	
		
	PL=(char *)mymalloc(50) ;
	RTC_Get();
	if(event_type==0) //开机时间
		
	{
		 printf("%d-%02d-%02d %02d:%02d:%02d;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec);
	   Len=sprintf (PL,"%d-%02d-%02d %02d:%02d:%02d;USER;PowerON;00;",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	  
	}
	else if(event_type==1)//校准时间
	{
		printf("%d-%02d-%02d %02d:%02d:%02d;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec);
  Len=sprintf (PL,"%d-%02d-%02d %02d:%02d:%02d;USER;CALibration;00;",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	}
   else{;}
	         
		        delay_ms(2);
		      	OSIntEnter();	
		         USART3_SendByte(0xEE);
						 USART3_SendByte(0xB1);
						 USART3_SendByte(0x59);//数据记录头
						 USART3_SendByte(0x00);
						 USART3_SendByte(0x04);//screen2
						 USART3_SendByte(0x00);
						 USART3_SendByte(0x01);//jinakong
						 USART3_SendByte(0x00);
						 USART3_SendByte(0x00);
	       	for(i=0;i<Len;i++)
						{
						 USART3_SendByte(*(PL+i));
						}
            END_CMD();
						OSIntExit(); 	
	         myfree(PL);
}

