#ifndef __RTC_H
#define __RTC_H	 
#include "sys.h"
//ʱ��ṹ��
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//������������
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;
//
	vu8 S_hour;  //���ݼ�¼ʹ��		
	vu16 S_w_year;
	vu8  S_w_month;
	vu8  S_w_date;

	vu8 E_hour;  //���ݼ�¼ʹ��		
	vu16 E_w_year;
	vu8  E_w_month;
	vu8  E_w_date;
//
	vu8  S3_hour;  //���ݼ�¼ʹ��		
	vu16 S3_w_year;
	vu8  S3_w_month;
	vu8  S3_w_date;

	vu8  E3_hour;  //���ݼ�¼ʹ��		
	vu16 E3_w_year;
	vu8  E3_w_month;
	vu8  E3_w_date;
  
	vu8 _hour;
	vu8 _min;
	vu8 _sec;			
	//������������
	vu16 _year;
	vu8  _month;
	vu8  _date;
	vu8  _week;
	
}_calendar_obj;					 
extern _calendar_obj calendar;	//�����ṹ��
extern u8 const mon_table[12];	//�·��������ݱ�
void Disp_Time(u8 x,u8 y,u8 size);//���ƶ�λ�ÿ�ʼ��ʾʱ��
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);//��ָ��λ����ʾ����
u8 RTC_Init(void);        //��ʼ��RTC,����0,ʧ��;1,�ɹ�;
u8 Is_Leap_Year(u16 year);//ƽ��,�����ж�
u8 RTC_Get(void);         //����ʱ��   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//����ʱ��			 



#endif
