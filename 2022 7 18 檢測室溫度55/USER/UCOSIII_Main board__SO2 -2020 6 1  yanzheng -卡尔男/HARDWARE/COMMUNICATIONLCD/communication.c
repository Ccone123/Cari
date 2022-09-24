#include "communication.h"
#include "USART3.h"
#include "stdio.h"
#include "processing.h"
//#include "timer.h"
#include "modbus_host.h"
#include "mbcrc.h"
#include "sys.h"
#include "delay.h"
#include "includes.h"
#include "IICEEPROM.h"
#include "rtc.h"
#include "datawr.h"
#include "malloc.h"	
#include "cmd_queue.h"
#include "calibration.h"
#include "experiment.h"
#define PTR2U16(PTR) ((((u8 *)(PTR))[0]<<8)|((u8 *)(PTR))[1])
#define PTR2U32(PTR) ((((u8 *)(PTR))[0]<<24)|(((u8 *)(PTR))[1]<<16)|(((u8 *)(PTR))[2]<<8)|((u8 *)(PTR))[3])  //�ӻ�����ȡ32λ����
const char endbuf[4]={0xFF,0xFC,0xFF,0xFF};
char Menu_Item=0;
char switch_status;
u8 Rx3_String[128] = {0};
u32 Test_Catagry;
input Input; //�����������
int a=0,b=0;
u8  cmd_buffer[CMD_MAX_SIZE]    ;  
extern int TimeMeasCount; 
extern char categroy_change;//ָ���
static u16 current_screen_id = 0;                                                 //��ǰ����ID
//static long int progress_value = 0;                                                     //����������ֵ
//static long inttest_value = 0;                                                         //����ֵ
//static u8 update_en = 0;                                                          //���±��
//static long int meter_flag = 0;                                                         //�Ǳ�ָ��������־λ
//static long int num = 0;                                                                //���߲��������
static int sec = 1;                                                                  //ʱ����
//static long int curves_type = 0;                                                        //���߱�־λ  0Ϊ���Ҳ���1Ϊ��ݲ�                  
//static long int icon_flag = 0;                                                          //ͼ���־λ
static int years,months,weeks,days,hours,minutes;                                    //������ʱ��������
//static u8 Select_H ;                                                              //����ѡ��Сʱ
//static u8 Select_M ;                                                              //����ѡ����� 
//static u8 Last_H ;                                                                //��һ��ѡ��Сʱ
//static u8 Last_M;                                                                 //��һ��ѡ����� 
//static long int second_flag=0;   
extern OS_MUTEX OSUart4Used_Mutex;
extern OS_MUTEX OSI2C1Used_Mutex;
void communication (void)
{
//	unsigned char i;
	u16 size;
	
  	size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);  
 if(size>0)
 {
			ProcessMessage((PCTRL_MSG)cmd_buffer ,size);	
 }    			
	
		
}

void ProcessMessage(PCTRL_MSG msg ,u16 size)
{
	u8 cmd_type = msg->cmd_type;
	u8 ctrl_msg =msg->ctrl_msg ;
	u8 control_type =msg->control_type;
	u8 screen_id =PTR2U16(&msg->screen_id);
	u16 control_id = PTR2U16(&msg->control_id); 
	u32 value = PTR2U32(msg->param);

   switch (cmd_type)
	 {
		 case NOTIFY_TOUCH_PRESS:
		 case NOTIFY_TOUCH_RELEASE:
		         break;		 
		 case NOTIFY_WRITE_FLASH_OK:
			       break;
		 case NOTIFY_WRITE_FLASH_FAILD:
			       break;
		 case NOTIFY_READ_FLASH_OK:
			       break;
		 case NOTIFY_READ_FLASH_FAILD:
			       break;
		 case NOTIFY_READ_RTC:
			    NotifyReadRTC (cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
			       break;
	  
	   case NOTIFY_CONTROL:
		 {
			  if(ctrl_msg==MSG_GET_CURRENT_SCREEN) 
				{
					 NotifyScreen(screen_id);  
					        
				}		
        else
				{
					switch(control_type)
					{
						case kCtrlButton:  //��ť�ؼ�
							  NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;
					  case kCtrlText:    //�ı��ؼ�
							  NotifyText(screen_id,control_id,msg->param);  
        						break;
						case kCtrlProgress:    //�������ؼ�
							    NotifyProgress(screen_id,control_id,value);                        
                    break;   
						case kCtrlSlider:    //����������
							    NotifySlider(screen_id,control_id,value);   
					          break;
						case kCtrlMeter:    //�Ǳ�ؼ�
							    NotifyMeter(screen_id,control_id,value);                           
                    break;  
						case kCtrlMenu:     //�˵��ؼ�
							      NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                            
				  	case kCtrlSelector:  //ѡ��ؼ�
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
            case kCtrlRTC:   //����ʱ�ؼ�
                    NotifyTimer(screen_id,control_id);
                    break;
								
								default :
									break;
					}
				}
      	    break;			
		 }
	    case NOTIFY_HandShake:   //����֪ͨ                                                            //����֪ͨ                                                     
              NOTIFYHandShake();
	             break;
			default:
				     break;
	 	 }
}

/***********************************************************************************
           ��ȡRTC
**************************************************************************************/
void NotifyReadRTC(u8 year,u8 month,u8 week,u8 day,u8 hour,u8 minute,u8 second)
{


	// char send_data[100];
     sec     =(0xff & (second>>4))*10 +(0xf & second);                                    //BCD��תʮ����
    years   =(0xff & (year>>4))*10 +(0xf & year);                                      
    months  =(0xff & (month>>4))*10 +(0xf & month);                                     
    weeks   =(0xff & (week>>4))*10 +(0xf & week);                                      
    days    =(0xff & (day>>4))*10 +(0xf & day);                                      
    hours   =(0xff & (hour>>4))*10 +(0xf & hour);                                       
    minutes =(0xff & (minute>>4))*10 +(0xf & minute);
    	years =2000+years;
    RTC_Set(years,months,days,hours,minutes,sec) ;  //����RTC�

  }
 //�����л������ĺ���
void NotifyScreen(u16 screen_id)
{
//		u8 Buffer_Sender[15]={0};
//	u16 CRCValue;
		// OS_ERR err;
	current_screen_id=screen_id;
	 //2019/8/8����
	  if(current_screen_id==0x00)//screen0 ,Ԥ�Ƚ׶�
		{
			READSCRREN.SCRREN2FLAG=0;//��ʷ��¼
			READSCRREN.SCRREN1FLAG=0;//��ϸ��������
			READSCRREN.ZeroCALFLAG=0;//���У׼
			READSCRREN.StadagasCALFLAG=0;//����У׼
			READSCRREN.CACULATEKBFLAG=0;//K,Bֵ��¼
			READSCRREN.SCRREN0FLAG=1; //������
			TimeMeasCount=0;//TimeMeasCount��0����У׼��
     // 	KuaiMen_Switch(0x00,0x00,0x00,0x02);//�л���������
				
		}
  else if(current_screen_id==0x01)
		{
//			 READSCRREN.SCRREN1FLAG=1;//У׼����
//			 READSCRREN.FristGetPowerFlag=1;
//       READSCRREN.SCRREN0FLAG=0;		
		}
		
    else if(current_screen_id==0x02)
		{
//        READSCRREN.SCRREN2FLAG=1;//�Լ�ػ���
//			  READSCRREN.SCRREN1FLAG=0;   //У׼����ȫ������
//				READSCRREN.ZeroCALFLAG=0;
//				READSCRREN.StadagasCALFLAG=0;
//				READSCRREN.CACULATEKBFLAG=0;
		}
		else if(current_screen_id==0x03)
		{
			
		}
		else if(current_screen_id==0x04)
		{
			
		}
		else if(current_screen_id==0x05)
		{
			
		}
		else if(current_screen_id==0x06)
		{
			
		}
		else if(current_screen_id==0x07)
		{
			   
		}
		else 
		{
			
		}
	
}
  //��ť�ؼ�
void NotifyButton(u16 screen_id , u16 control_id ,u8 state)
{
	u8 i;
  //u8 Buffer_Sender[15]={0};
//	u16 CRCValue;//	OS_ERR err;
    switch (screen_id)
		{
			case 0x00:
								switch(control_id)
								{
								 case 0x1B:    //�л�����ϸ����	
																 if(state==0)	
															{
																 READSCRREN.SCRREN1FLAG=1;			//��ϸ��������	
																 READSCRREN.SCRREN0FLAG=0;    //��������ʾ												
															}																									
														else
                              {
																
															}
															 break;	
								 case 302://void Test_Uint(char *str)//const char string[2][6]={"ppb","ug/m3"};
                             		 if(state==0)	
															{
																 Test_Uint(string[0]);		
                                  categroy_change	=0x01;															
															}	
                                 else
																 {																	 
														     Test_Uint(string[1]);
																 categroy_change=0x02;
																 }
															break;
                   	default:
				                   break; 															
									}	
                       break;						
			case 0x01:			    
							switch(control_id)
							{							
						   	case 0x15:
								       if(state==0)               //��ϸ����
												{
													;	
												}
										   else
											 {
												 READSCRREN.SCRREN1FLAG=1;
											 }
								      break;
											 
						case 0x17:
								 if(state==0)               //У׼����
									{
                     ;
									}
								 else
								 {
									  READSCRREN.SCRREN6FLAG=1;
									//	KuaiMen_Switch(0x00,0x01,0x00,0x02);  //dakai ���ţ������ڣ���PMT��
									 a=1;
								 }
								     break;				 			 
							case 0x18:
                       	  if(state==0)      //���ý���
												{
	                          ;
												}
												
										   else
											 {
												 READSCRREN.SCRREN8FLAG=1;
											 }											 
												break;					
                  default:
				                  break;											 
	         	  }
	                  break;
							
		 case 0x02:		
                switch(control_id)
					       {	
									case 0x12:
										       if(state==0)
													 {
											
													 }
													 else
													 {
														 Delete_RecorD();
												if(Menu_Item==0x01)		 
												{
											     FuckingYou_New_Readdata_5MIN();  //5����
												}
												else if(Menu_Item==0x00)
												{
													FuckingYou_New_Readdata_min();      //1����
												}
												else if (Menu_Item==0x03)
												{
													FuckingYou_New_Readdata_day();      //һ��
												}
												else
												{
													FuckingYou_New_Readdata_hour();     //Сʱ
												}
													
													 }

									case 0x15:
														 if(state==0)               //��ϸ����
															{
															   	;	
															}
															else
														 {
															 READSCRREN.SCRREN1FLAG=1;
														 }
														 break;
										case 0x17:
													 if(state==0)               //У׼����
														{
		
														}
													 else
													 {
														  READSCRREN.SCRREN6FLAG=1; 
													//		KuaiMen_Switch(0x00,0x01,0x00,0x02);  //dakai ���ţ�������
														  a=2;
													 }
										       	break;	
											default:
				                     break;		 
											
								 }		
                         break;								 
			case 0x03:		
                 switch(control_id)
					       {			 
									 case 0x0A:
                             if(state==0)
														 {
															 
												
														 }	
                            else
														{
															FuckingYou_New_Readdata1();  //����ȥ��Ч
														}															
									 
									 
									 
									 
									 case 0x15:
														 if(state==0)               //��ϸ����
															{
															   	;
																
															}
														 else
														 {
															 READSCRREN.SCRREN1FLAG=1;
														 }
														 break;
									case 0x17:
													 if(state==0)               //У׼����
														{
															
															
														}
													 else
													 {
														 READSCRREN.SCRREN6FLAG=1;
													//		KuaiMen_Switch(0x00,0x01,0x00,0x02);  //dakai ���ţ������ڣ���PMT�ƣ�ͬʱ
														  a=3;
													 }
													break;	
                   default:
				                    break;													 
										 }	
                        break;								 
			case 0x04:		
                 switch(control_id)
					       {			 
											case 0x15:
														 if(state==0)               //��ϸ����
															{
															   	;
																
															}
														 else
														 {
															 READSCRREN.SCRREN1FLAG=1;
														 }
														 break;
											case 0x17:
																 if(state==0)               //У׼����
																	{
																		
																		
																	}
																 else
																 {
																	 READSCRREN.SCRREN6FLAG=1;
																//		KuaiMen_Switch(0x00,0x01,0x00,0x02);  //dakai ���ţ������ڣ���PMT�ƣ�ͬʱ
																	  a=4;
																 }
																break;
                         default:
				                          break;																 
															 }									 
											      break;
			case 0x05:		
                 switch(control_id)
					       {			 
											case 0x15:
														 if(state==0)               //��ϸ����
															{
															   	;
																
															}
														 else
														 {
															 READSCRREN.SCRREN1FLAG=1;
														 }
														 break;
										case 0x17:
																	 if(state==0)               //У׼����
																		{
																			
																			
																		}
																	 else
																	 {
																		 READSCRREN.SCRREN6FLAG=1;
																	//		KuaiMen_Switch(0x00,0x01,0x00,0x02);  //dakai ���ţ������ڣ���PMT�ƣ�ͬʱ
																		  a=5;
																	 }
																	break;	
                            default:
				                           break;																	 
								               }	
                         break;								 
			case 0x06:		 //У׼����
                 switch(control_id)
					       {		
									 case 0x05:
										      if(state==0)
													{
														;
													}
													else 
													{
														  READSCRREN.SCRREN1FLAG=1;//��ϸ��������
															 READSCRREN.SCRREN6FLAG=0;
														   READSCRREN.ZeroCALFLAG=0;//���У׼
														   READSCRREN.StadagasCALFLAG=0;//����У׼
													     READSCRREN.CACULATEKBFLAG=0;//K,Bֵ��¼��־
														   TimeMeasCount=0;//TimeMeasCount��0����У׼��
															// KuaiMen_Switch(0x00,0x00,0x01,0x02);//�л���������
													}
                                 break;


									 
											case 0x15:
														 if(state==0)               //��ϸ����
															{
															   	;
																
															}
														 else
														 {
															 READSCRREN.SCRREN1FLAG=1;//��ϸ��������
															 READSCRREN.SCRREN6FLAG=0;
														   READSCRREN.ZeroCALFLAG=0;//���У׼
														   READSCRREN.StadagasCALFLAG=0;//����У׼
													     READSCRREN.CACULATEKBFLAG=0;//K,Bֵ��¼��־
														   TimeMeasCount=0;//TimeMeasCount��0����У׼��
															// KuaiMen_Switch(0x00,0x00,0x01,0x02);//�л���������,ȥ���������л�
														 }
														 break;
										case 0x16:
														 if(state==0)               //��ϸ����
															{
															   	;
																
															}
														 else
														 {
															 READSCRREN.SCRREN3FLAG=1;//��ϸ��������
															 READSCRREN.SCRREN6FLAG=0;
														   READSCRREN.ZeroCALFLAG=0;//���У׼
														   READSCRREN.StadagasCALFLAG=0;//����У׼
													     READSCRREN.CACULATEKBFLAG=0;//K,Bֵ��¼��־
														   TimeMeasCount=0;//TimeMeasCount��0����У׼��
															 //KuaiMen_Switch(0x00,0x00,0x01,0x02);//�л��������ڣ�ȥ���������л�
														 }
														 break;				 
										case 0x18:
														 if(state==0)               //��ϸ����
															{
															   	;
																
															}
														 else
														 {
															 READSCRREN.SCRREN8FLAG=1;//�����������ý���
															 READSCRREN.SCRREN6FLAG=0;
														   READSCRREN.ZeroCALFLAG=0;//���У׼
														   READSCRREN.StadagasCALFLAG=0;//����У׼
													     READSCRREN.CACULATEKBFLAG=0;//K,Bֵ��¼��־
														   TimeMeasCount=0;//TimeMeasCount��0����У׼��
															// KuaiMen_Switch(0x00,0x00,0x01,0x02);//�л���������ȥ���������л�
														 }
														 break;							 	
											case 32:                   //У׼0λ��ʼ��־
												     if(state==0)
														 {
															 READSCRREN.ZeroCALFLAG=1;
														 }
														 else
                                  {}
													break;
																		
									 case 0x21:                      //����У׼��־
															if(state==0)
															{
																READSCRREN.StadagasCALFLAG=1;
															}	
															 break;
												
                 	case 35:
											   if(state==1)
												 {
													 READSCRREN.SCRREN11FLAG=1;
												 }
                         else{;}
													break;


															
									 case 50:  
                                  if(state==1)
														      {
																		KuaiMen_Switch(0x01,0x01,0x01,0x02);switch_status=1;//����У׼��
																			Display_Sampling_Switch(1);
	
																	}		
																	 else
																	 {
																		 KuaiMen_Switch(0x00,0x00,0x00,0x02);switch_status=0;
																		 	Display_Sampling_Switch(0);
	
																	 }
														 break;															 
																	
															
															
															
                    default:
				                       break;


															
										 }									 
										break;	 
											 
											 
							
		 case 0x07:
			          switch(control_id)
								{		
									case 230:
										  if(state==0)
											    {
													Var.AfterCALFlag=3;  //�ر���У׼
 													SaveEpData(360,Var.AfterCALFlag);
													}
										  else
											    {
													Var.AfterCALFlag=1;	
											   	SaveEpData(360,Var.AfterCALFlag);
													} 
									           break;
										case 231:
										  if(state==0)
											    {
												  Var.Add_AfterCALFlag=2;//�ر�offset
 													SaveEpData(2048,Var.Add_AfterCALFlag);//2048+7
													}
										  else
											    {
	                         Var.Add_AfterCALFlag=1;//��У׼����
													 SaveEpData(2048,Var.Add_AfterCALFlag);//2048+7
													} 
									           break;				
													
													
							    case 0x17:
											 if(state==0)               //У׼����
												{
													
													
												}
											 else
											 {
												 READSCRREN.SCRREN6FLAG=1;
													//KuaiMen_Switch(0x00,0x01,0x01,0x02);  //dakai ���ţ������ڣ���PMT�ƣ�ͬʱ
												  a=7;
											 }
											break;		
								 default:
												 break;											 
											}
		               break;
			         
		case 0x08:		
                 switch(control_id)
					       {			 
											case 0x15:
														 if(state==0)               //��ϸ����
															{
															   	;
																
															}
														 else
														 {
															 READSCRREN.SCRREN1FLAG=1;
														 }
														 break;
										 	
									case 0x17:
														 if(state==0)               //У׼����
															{
																
																
															}
														 else
														 {
															 READSCRREN.SCRREN6FLAG=1;
													//			KuaiMen_Switch(0x00,0x01,0x01,0x02);  //dakai ���ţ������ڣ���PMT�ƣ�ͬʱ
															  a=8;
														 }
														break;	
												
    									   default:
				                          break;	 
														 
														 
														 }
                        break;								 
								
		case 0x09:		
                 switch(control_id)
					       {			 
											case 0x15:
														 if(state==0)               //��ϸ����
															{
															   	;
																
															}
														 else
														 {
															 READSCRREN.SCRREN1FLAG=1;
														 }
														 break;
										case 0x17:
														 if(state==0)               //У׼����
															{
																
																
															}
														 else
														 {
															 READSCRREN.SCRREN6FLAG=1;
													//			KuaiMen_Switch(0x00,0x01,0x01,0x02);  //dakai ���ţ������ڣ���PMT�ƣ�ͬʱ
															  a=9;
														 }
														break;	

										case 0x10:
                            if(state==0)
														      {
																		KuaiMen_Switch(0x01,0x01,0x01,0x02);switch_status=1;//����У׼��
																	}		
                             else
														 {
															 KuaiMen_Switch(0x00,0x00,0x00,0x02);switch_status=0;
														 }
														 break;
										case 0x11:
  											 if(state==0)    //����
														      {
																		 KuaiMen_Switch(0x01,0x01,0x01,0x01);
																	}		
                             else
														 {
															  KuaiMen_Switch(0x00,0x00,0x00,0x01);
														 }
														 break;
											case 0x12://PMT��
  											 if(state==0)
														      {
																		KuaiMen_Switch(0x01,0x01,0x01,0x04);
																	}		
                             else
														 {
															 KuaiMen_Switch(0x00,0x00,0x00,0x04);
														 }
														 break;
														 				 
                       default:
				                        break;														 
			 
										 }	
                         break;	
                         

										 
		case 0x0A:		
                 switch(control_id)
					       {			 
											case 0x15:
														 if(state==0)               //��ϸ����
															{
															   	;																
															}
														 else
														 {
															 READSCRREN.SCRREN1FLAG=1;
														 }
														 break;
										default:
				                      break;
										 }	
                   break;	   
	    case 0x0B:		
				          switch (control_id)
									{
										
										case 3:
												if(state==1)               //���У׼����ʼУ׼����
																	{
																		Startup++;
																		
																	   if(Startup==Count_Input)
																		{ 																			
																			AMulti[Startup-1].Startup=1;	
																		}
																	else if(Startup<Count_Input)														     	
																		{
  																		Count_Input--;
																			Startup--;
																		}
																		else 												     	
																		  { 
																			  Startup--;
																			}
																	
																	}
														break;
										case 34:
											   if(state==1)
												 {
													 READSCRREN.SCRREN11FLAG=0;
												 }				
													 else{;}
													 break;	
										case 35:
											   if(state==1)
												 {
													 READSCRREN.SCRREN11FLAG=1;
												 }
												 else{;}
													 break;
										case 140:
															if(state==0)               //ɾ������
																					{
																						AMulti[0].Delete_Flag=1;
																					}
											      
										        break;
										case 141:
											      if(state==0)               //ɾ������
																					{
																						AMulti[1].Delete_Flag=1;
																					}
										        break;
										case 142:
											      if(state==0)               //ɾ������
																					{
																						AMulti[2].Delete_Flag=1;
																					}
										        break;
										case 143:
											      if(state==0)               //ɾ������
																					{
																						AMulti[3].Delete_Flag=1;
																					}
										        break;
										case 144:
											      if(state==0)               //ɾ������
																					{
																						AMulti[4].Delete_Flag=1;
																					}
										        break;
										case 145:
											      if(state==0)               //ɾ������
																					{
																						AMulti[5].Delete_Flag=1;
																					}
										        break;
										case 146:
											      if(state==0)               //ɾ������
																					{
																						AMulti[6].Delete_Flag=1;
																					}
										        break;
										case 147:
											      if(state==0)               //ɾ������
																					{
																						AMulti[7].Delete_Flag=1;
																					}
										        break;	
											 case 50:  
                                  if(state==1)
														      {
																		KuaiMen_Switch(0x01,0x01,0x01,0x02);switch_status=1;//����У׼��
																			Display_Sampling_Switch(1);
	
																	}		
																	 else
																	 {
																		 KuaiMen_Switch(0x00,0x00,0x00,0x02);switch_status=0;
																		 	Display_Sampling_Switch(0);
	
																	 }
														 break;												
									}break;
						case 0x0D:  
                      switch (control_id)			
													{
												case 0x02:
													      if(state==0)
																{
																	for(i=1;i<8;i++)
																	   {
																			 if(AMulti[i].Delete_Flag==1)
																			 {
																			    AMulti[i].Delete_Flag=0; 
																				  
																			 }
																		 }
																		
																}
																			else{;}
													      break;
												case 0x01:
													     if(state==0)
																{
											    						
																	 if(AMulti[0].Delete_Flag==1)
																	 {
																		 AMulti[0].Delete_Flag=0; 
																		 Delete_sort(0);
																	 }
																	 
																	 else{
																					for(i=1;i<8;i++)
																						 {
																							 if(AMulti[i].Delete_Flag==1)
																							 {
																									AMulti[i].Delete_Flag=0; 
																								   if( i<Startup)
																									     Delete_sort(i);
																							 }
																						 } 
																					 }


																}
																else{;}
													      break;
													}	break;											
						
		
		
		
					default:
				         break;
										 
								 					
		}
       
}
 //�ı��ؼ�
void NotifyText(u16 screen_id, u16 control_id, u8 *str)
{
	        float value=0;  	OS_ERR err;
	     char i;char temp;
        sscanf(str,"%f",&value); 
	 switch (screen_id)
	 {
		 case 0x01:
			         switch(control_id)
							      {
												case 0x07:
                                 SO2CONCERNRATIONCAL.SO2STARDCON_INPUT=value;
												         READSCRREN.SO2STARDCON_INPUT_Flag=1; 																									
																	break;												
												case 11:
													    	OSIntEnter(); 
												             SO2CONCERNRATIONCAL.SO2K = value;
												          SaveEpData(0x00,SO2CONCERNRATIONCAL.SO2K*1000); 
												          SO2CONCERNRATIONCAL.SO2K=0;   
													        SO2CONCERNRATIONCAL.SO2K =(float)ReadEpData(0x00)/1000;
																	OSIntExit();  											
													        break;
												
												case 12:
													        	OSIntEnter();
												          SO2CONCERNRATIONCAL.SO2B= value;
												          SaveEpData(0x08,SO2CONCERNRATIONCAL.SO2B*1000); 
												SO2CONCERNRATIONCAL.SO2B=0;
													       SO2CONCERNRATIONCAL.SO2B =(float)ReadEpData(0x08)/1000;
														OSIntExit(); 
													         break;
												
												
								   }
					                break;			

		case 0x02:
			          switch(control_id)
								{
									
									
									case 2:
										      calendar.S_w_year=value;
													OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												  SaveEpData(200,calendar.S_w_year);      //address :200
												  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	  //��������ֵ
									              calendar.S_w_year   = ReadEpData(200);
									              calendar.S_w_month  = ReadEpData(208);
									              calendar.S_w_date   = ReadEpData(216);
									              calendar.S_hour   = ReadEpData(224);
									            RevNameS_D=calendar.S_w_year*10000+calendar.S_w_month*100+calendar.S_w_date; 
									          break;
									case 3:
										     calendar.S_w_month=value;
									     	OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												 SaveEpData(208,calendar.S_w_month);      //address :208
												OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									
									              calendar.S_w_year   = ReadEpData(200);
									              calendar.S_w_month  = ReadEpData(208);
									              calendar.S_w_date   = ReadEpData(216);
									                calendar.S_hour   = ReadEpData(224);
									    RevNameS_D=calendar.S_w_year*10000+calendar.S_w_month*100+calendar.S_w_date; 
									
								            	 break;
									
									case 4:
										     calendar.S_w_date=value;
									      OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												 SaveEpData(216,calendar.S_w_date);      //address :216
												OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									
									                calendar.S_w_year   = ReadEpData(200);
									              calendar.S_w_month  = ReadEpData(208);
									              calendar.S_w_date   = ReadEpData(216);
									                calendar.S_hour   = ReadEpData(224);
									    RevNameS_D=calendar.S_w_year*10000+calendar.S_w_month*100+calendar.S_w_date; 
									           break;
									
									case 5:
										     calendar.S_hour=value;
									       OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												 SaveEpData(224,calendar.S_hour);      //address :224
												 OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);										                 
									                 calendar.S_w_year   = ReadEpData(200);
									                calendar.S_w_month  = ReadEpData(208);
									                calendar.S_w_date   = ReadEpData(216);
									                calendar.S_hour   = ReadEpData(224);
									    RevNameS_D=calendar.S_w_year*10000+calendar.S_w_month*100+calendar.S_w_date; 
									           break;
									case 6:
										     calendar.E_w_year=value;
									     OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												 SaveEpData(232,calendar.E_w_year);      //address :232
												OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									              
                      									calendar.E_w_year=ReadEpData(232);
																				calendar.E_w_month=ReadEpData(240);
																				calendar.E_w_date=ReadEpData(248);
																				calendar.E_hour=ReadEpData(256);
									         RevNameE_D=   calendar.E_w_year*10000+ calendar.E_w_month*100+  calendar.E_w_date;    
									               
									           break;
									
									case 7:
										     calendar.E_w_month=value;
									      OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												 SaveEpData(240,calendar.E_w_month);      //address :240
												OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                          
									                      calendar.E_w_year=ReadEpData(232);
																				calendar.E_w_month=ReadEpData(240);
																				calendar.E_w_date=ReadEpData(248);
																				calendar.E_hour=ReadEpData(256);
									                 RevNameE_D=   calendar.E_w_year*10000+ calendar.E_w_month*100+  calendar.E_w_date;
									          break;
									
									
									case 8:
										     calendar.E_w_date=value;
									       OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												 SaveEpData(248,calendar.E_w_date);      //address :248
												 OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
								                       	calendar.E_w_year=ReadEpData(232);
																				calendar.E_w_month=ReadEpData(240);
																				calendar.E_w_date=ReadEpData(248);
																				calendar.E_hour=ReadEpData(256);
									       RevNameE_D=   calendar.E_w_year*10000+ calendar.E_w_month*100+  calendar.E_w_date;
									             break;
									
									
									case 9:
										   calendar.E_hour=value;
									     OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												 SaveEpData(256,calendar.E_hour);      //address :256
												OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                      calendar.E_w_year=ReadEpData(232);
																				calendar.E_w_month=ReadEpData(240);
																				calendar.E_w_date=ReadEpData(248);
																				calendar.E_hour=ReadEpData(256);
									    RevNameE_D=   calendar.E_w_year*10000+ calendar.E_w_month*100+  calendar.E_w_date;
									           break;
								}
								
		case 0x03:
               switch(control_id)
								{
								 case 2:
									 calendar.S3_w_year=value;
								 					OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												  SaveEpData(264,calendar.S3_w_year);      //address :264
												  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	  //��������ֵ
								                         calendar.S3_w_year=ReadEpData(264);
																			 	 calendar.S3_w_month=ReadEpData(272);
																				 calendar.S3_w_date=ReadEpData(280);
																				 calendar.S3_hour=ReadEpData(288);
								                    RevNameS3_D=   calendar.S3_w_year*10000+ calendar.S3_w_month*100+  calendar.S3_w_date;
								 
								 case 3:
									 calendar.S3_w_month=value;
								         OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												  SaveEpData(272,calendar.S3_w_month);      //address :272
												  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	  //��������ֵ
								                         calendar.S3_w_year=ReadEpData(264);
																			 	 calendar.S3_w_month=ReadEpData(272);
																				 calendar.S3_w_date=ReadEpData(280);
																				 calendar.S3_hour=ReadEpData(288);
								                    RevNameS3_D=   calendar.S3_w_year*10000+ calendar.S3_w_month*100+  calendar.S3_w_date;
								 
								             break;
								 case 4:
									 calendar.S3_w_date=value;
								         OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												  SaveEpData(280,calendar.S3_w_date);      //address :280
												  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	  //��������ֵ
								                         calendar.S3_w_year=ReadEpData(264);
																			 	 calendar.S3_w_month=ReadEpData(272);
																				 calendar.S3_w_date=ReadEpData(280);
																				 calendar.S3_hour=ReadEpData(288);
								                    RevNameS3_D=   calendar.S3_w_year*10000+ calendar.S3_w_month*100+  calendar.S3_w_date;
								       
								             break;
									 
								 case 5:
									 calendar.S3_hour=value;
								  OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												  SaveEpData(288,calendar.S3_hour);      //address :288
												  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	  //��������ֵ
								                         calendar.S3_w_year=ReadEpData(264);
																			 	 calendar.S3_w_month=ReadEpData(272);
																				 calendar.S3_w_date=ReadEpData(280);
																				 calendar.S3_hour=ReadEpData(288);
								                    RevNameS3_D=   calendar.S3_w_year*10000+ calendar.S3_w_month*100+  calendar.S3_w_date;
								 
								              break;
								 case 6:
									 calendar.E3_w_year=value;
								  OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												  SaveEpData(296,calendar.E3_w_year);      //address :296
												  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	  //��������ֵ
								                         calendar.E3_w_year=ReadEpData(296);
																			 	 calendar.E3_w_month=ReadEpData(304);
																				 calendar.E3_w_date=ReadEpData(312);
																				 calendar.E3_hour=ReadEpData(320);
								                    RevNameE3_D=   calendar.E3_w_year*10000+ calendar.E3_w_month*100+  calendar.E3_w_date;
									            break;
								 case 7:
									 calendar.E3_w_month=value;
								  OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												  SaveEpData(304,calendar.E3_w_month);      //address :304
												  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	  //��������ֵ
								                         calendar.E3_w_year=ReadEpData(296);
																			 	 calendar.E3_w_month=ReadEpData(304);
																				 calendar.E3_w_date=ReadEpData(312);
																				 calendar.E3_hour=ReadEpData(320);
								                    RevNameE3_D=   calendar.E3_w_year*10000+ calendar.E3_w_month*100+  calendar.E3_w_date;
									           break;
								 case 8:
									 calendar.E3_w_date=value;
								   OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												  SaveEpData(312,calendar.E3_w_date);      //address :312
												  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	  //��������ֵ
								                         calendar.E3_w_year=ReadEpData(296);
																			 	 calendar.E3_w_month=ReadEpData(304);
																				 calendar.E3_w_date=ReadEpData(312);
																				 calendar.E3_hour=ReadEpData(320);
								                    RevNameE3_D=   calendar.E3_w_year*10000+ calendar.E3_w_month*100+  calendar.E3_w_date;
									           break;
								 case 9:
									  calendar.E3_hour=value;
								     OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
												  SaveEpData(320,calendar.E3_hour);      //address :312
												  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	  //��������ֵ
								                         calendar.E3_w_year=ReadEpData(296);
																			 	 calendar.E3_w_month=ReadEpData(304);
																				 calendar.E3_w_date=ReadEpData(312);
																				 calendar.E3_hour=ReadEpData(320);
								                    RevNameE3_D=   calendar.E3_w_year*10000+ calendar.E3_w_month*100+  calendar.E3_w_date;
									            break;	
		            }

		case 0x06:
									 switch(control_id)
												{
														case 0x07:
																		 SO2CONCERNRATIONCAL.SO2STARDCON_INPUT=value;
																		 READSCRREN.SO2STARDCON_INPUT_Flag=1; 													
																			break;
											 }
															break;		
						 
     case 0x08:							 	
                 switch (control_id)	
								 {
									 case 0x02:
										         OSIntEnter(); 
										          Input.CRAteTEM_LowerLimit=value;//�����¶�������        
									 SaveEpData(30,Input.CRAteTEM_LowerLimit*1000);    //address :30
									     Input.CRAteTEM_LowerLimit  =(float)ReadEpData(30)/1000;
                               OSIntExit(); 									 
									              break;
									 case 0x05:
										          OSIntEnter(); 
										         Input.CRAteTEM_UpperLimit=value;									
									 SaveEpData(38,Input.CRAteTEM_UpperLimit*1000);   //address:38
									 Input.CRAteTEM_UpperLimit=(float)ReadEpData(38)/1000;
									        OSIntExit();
									               break;
									 case 0x09:
										        OSIntEnter(); 
										 Input.TECTEM_LowerLimit=value;//PMT�¶�������
									  SaveEpData(46,Input.TECTEM_LowerLimit*1000);   //address:46
							    	Input.TECTEM_LowerLimit=(float)ReadEpData(46)/1000;
									      OSIntExit();
									               break;
									 case 10 :    
										           OSIntEnter(); 
                              Input.TECTEM_UpperLimit=value;
									    SaveEpData(54,Input.TECTEM_UpperLimit*1000);   //address:54
										  Input.TECTEM_UpperLimit=(float)ReadEpData(54)/1000;
									              OSIntExit();
                                 break;
                   case 26 :  
                    OSIntEnter(); 										 
												  Input.CaseTEM_LowerLimit=value;//��Ӧ���¶�
									 
									 SaveEpData(62,Input.CaseTEM_LowerLimit*1000);   //address:62
									Input.CaseTEM_LowerLimit=(float)ReadEpData(62)/1000;
                  OSIntExit();									 
																 break;
									 case 27:
										        OSIntEnter(); 
										          Input.CaseTEM_UpperLimit=value;
								   	        SaveEpData(70,Input.CaseTEM_UpperLimit*1000);   //address:70
									 
									         Input.CaseTEM_UpperLimit=(float)ReadEpData(70)/1000;
									          OSIntExit();	
										             break;
									 case 17:
										 
										         Input.SampGasP_LowerLimit=value; //����ѹ��
									 	  SaveEpData(78,Input.SampGasP_LowerLimit*1000);
									    Input.SampGasP_LowerLimit=(float)ReadEpData(78)/1000;   //address:78
									
									                break;
									 case 18:
										        Input.SampGasP_UpperLimit=value;      
									 	 OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									 SaveEpData(86,Input.SampGasP_UpperLimit*1000);   //address:86
									 Input.SampGasP_UpperLimit=(float)ReadEpData(86)/1000;
							   	OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);		
									                break;
									 case 13:
									           Input.CaseGasPRESSURE_LowerLimit=value;
									 	 OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									 SaveEpData(94,Input.CaseGasPRESSURE_LowerLimit*1000);   //address:94									 
									  Input.CaseGasPRESSURE_LowerLimit=(float)ReadEpData(94)/1000;
									 OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                break;
									 case 15:
										         Input.CaseGasPRESSURE_UpperLimit=value;
									 	 OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									 SaveEpData(102,Input.CaseGasPRESSURE_UpperLimit*1000);   //address:102
									 Input.CaseGasPRESSURE_UpperLimit=(float)ReadEpData(102)/1000;
									OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									               break;
									 case 31:
										             Input.GASFLOW_LowerLimit=value;
									 	 OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									 SaveEpData(110,Input.GASFLOW_LowerLimit*1000);   //address:110
									 
									 Input.GASFLOW_LowerLimit=(float)ReadEpData(110)/1000;
									 OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                 break;
									 case 32:
										             Input.GASFLOW_UpperLimit=value;
									 	 OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									 
									 SaveEpData(118,Input.GASFLOW_UpperLimit*1000);   //address:118
									 Input.GASFLOW_UpperLimit=(float)ReadEpData(118)/1000;
									OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);		
									                 break;
								
									 case 36:  //��ַ��Ҫ��ҳ
										 OSIntEnter(); 
										               Input.PMTSampVL_LowerLimit=value;
								
									 SaveEpData(128,Input.PMTSampVL_LowerLimit*1000);   //address:128
									 Input.PMTSampVL_LowerLimit=0;
									 Input.PMTSampVL_LowerLimit=(float)ReadEpData(128)/1000;
							      OSIntExit();	
									                  break;
									 case 38:
										         OSIntEnter();
										               Input.PMTSampVL_UpperLimit=value;		
									   SaveEpData(136,Input.PMTSampVL_UpperLimit*1000);   //address:136
									   Input.PMTSampVL_UpperLimit=0;
								  	 Input.PMTSampVL_UpperLimit=(float)ReadEpData(136)/1000;
							    	         OSIntExit();	
									                  break;
									 
									 case 40:       
										               Input.REFVOL_PD_LowerLimit=value;
									 	 OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									   SaveEpData(144,Input.REFVOL_PD_LowerLimit*1000);   //address:144
									 Input.REFVOL_PD_LowerLimit=0;
									     Input.REFVOL_PD_LowerLimit=(float)ReadEpData(144)/1000;
									   OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                    break;
									 case 41:
										                 Input.REFVOL_PD_UpperLimit=value;
									 	 OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									 SaveEpData(152,Input.REFVOL_PD_UpperLimit*1000);   //address:152
									 Input.REFVOL_PD_UpperLimit=0;
									  Input.REFVOL_PD_UpperLimit=(float)ReadEpData(152)/1000;
									 OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                   break;
									 
									 case 43:    
										                  Input.REFVOL_PMT_LowerLimit=value;
									 	OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									  SaveEpData(160,Input.REFVOL_PMT_LowerLimit*1000);   //address:160
									  Input.REFVOL_PMT_LowerLimit=0;
									  Input.REFVOL_PMT_LowerLimit=(float)ReadEpData(160)/1000;
									 OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                 break;
									 case 44:
										                  Input.REFVOL_PMT_UpperLimit=value;
									 	 OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									 SaveEpData(168,Input.REFVOL_PMT_UpperLimit*1000);   //address:168
									 Input.REFVOL_PMT_UpperLimit=0;
									  Input.REFVOL_PMT_UpperLimit=(float)ReadEpData(168)/1000;
									 OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                   break;
									 case 46:   
										                   Input.VEF_LowerLimit=value;
									 	OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									  SaveEpData(176,Input.VEF_LowerLimit*1000);   //address:176
									  Input.VEF_LowerLimit=0;
									  Input.VEF_LowerLimit=(float)ReadEpData(176)/1000;
									  OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                    break;
									 case 47:
										                   Input.VEF_UpperLimit=value;
									 OSMutexPend (&OSI2C1Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
									 SaveEpData(184,Input.VEF_UpperLimit*1000);   //address:184
									 Input.VEF_UpperLimit=0;
									 Input.VEF_UpperLimit=(float)ReadEpData(184)/1000;
									 OSMutexPost(&OSI2C1Used_Mutex,OS_OPT_POST_NONE,&err);	
									                    break; 
								 }
                      break;								 
		 case 11:					
									 switch (control_id)
									 {
										 case 0x02:	
											 
											        AMulti[Count_Input++].SO2Soncentration_Input=value;
										        // temp=Startup;
										        if(Startup<=Count_Input-2)
															{
																Startup=ReadEpData(328);
									              Count_Input=ReadEpData(336);
                                    if( AMulti[Count_Input-1].SO2Soncentration_Input	==0		)
																		{AMulti[0].SO2Soncentration_Input=0;} 			
																		AMulti[0].PMTSampVL=(float)ReadEpData(344)/1000;	
																	 for(i=0;i<Startup-1;i++)
																			 {
																						AMulti[i+1].k =(float)ReadEpData(384+(i)*8*2)/1000;          //kֵ
																						AMulti[i+1].b =(float)ReadEpData(392+(i)*8*2)/1000;  //bֵ 
																				
																					 AMulti[i+1].SO2Soncentration_Input =(float)ReadEpData(512+(i)*8*2)/1000;          //kֵ
																					 AMulti[i+1].PMTSampVL =(float)ReadEpData(520+(i)*8*2)/1000; 												 
																			 }
																		 display();
																			  AMulti[Count_Input++].SO2Soncentration_Input=value;
															}										 
										 if((Count_Input<9)&&(Startup<9))
										 {       
																for(i=0;i<Startup-1;i++)
																{
																	if( AMulti[i].SO2Soncentration_Input== AMulti[Count_Input-1].SO2Soncentration_Input)
																	{
																					Count_Input--;
																					 Cover_Fun(i);
																					AMulti[Count_Input++].SO2Soncentration_Input=value;  
																							break; 																	
																}
															}
														
														}
										    else 
											   	{
													   Count_Input=0;
												      Startup=0;
													}
												
										          break;
										 case 51:
                              Test_Catagry=(u32)value ;		
                           SaveEpData(352, Test_Catagry);										 
										          break;
									 }
									    break;

	 }
	     
	
	
	
	      
}
 //�������ؼ�
void NotifyProgress(u16 screen_id, u16 control_id, u32 value)           
{  
    //TODO: ����û�����   
} 
  //�������ؼ�
void NotifySlider(u16 screen_id, u16 control_id, u32 value)             
{                                                             
    
   
}
 //�Ǳ�ؼ�

void NotifyMeter(u16 screen_id, u16 control_id, u32 value)
{
    //TODO: ����û�����
}
//�˵��ؼ�
void NotifyMenu(u16 screen_id, u16 control_id, u8 item, u8 state)
{
    //TODO: ����û�����
	   if(item==0)
			 Menu_Item=0x00;
     else if(item==1)
			 Menu_Item=0x01;
     else if(item==2)
       Menu_Item=0x02;				 
	   else
		   Menu_Item=0x03;
}
//ѡ��ؼ�
void NotifySelector(u16 screen_id, u16 control_id, u8  item)
{
   
   

}
   //����ʱ�ؼ�
void NotifyTimer(u16 screen_id, u16 control_id)
{
 
}

/*! 
*  \brief  ����֪ͨ
*/
void NOTIFYHandShake(void)
{
   
}





void  KuaiMen_Switch(u8 kuaimen_state ,u8 caiyangjiaozhunfa_state,u8 PMT_state,u8 control_category )
{
	
     	u8 Buffer_Sender[15]={0};	u16 CRCValue;
//			u8 static state=0;
			
								if(control_category&0x01)		    //����
								{
		                    Buffer_Sender[0]=0xFF;
												Buffer_Sender[1]=0x10;	
												Buffer_Sender[2]=0x02;			
												Buffer_Sender[3]=0x39;
												Buffer_Sender[4]=0x00;
												Buffer_Sender[5]=0x01;
												Buffer_Sender[6]=0x02;
												Buffer_Sender[7]=0x00;
												Buffer_Sender[8]=kuaimen_state;							
												CRCValue =usMBCRC16(Buffer_Sender,9);
											 Buffer_Sender[9]=CRCValue&0x00ff;
											 Buffer_Sender[10]=CRCValue>>8;	
									      delay_ms(500);
									      OSIntEnter();
                       RS485_Send_Data(Buffer_Sender,11);	
										    OSIntExit(); 
								}
								if(control_category&0x02)
						     	{     Buffer_Sender[0]=0xFF;
												Buffer_Sender[1]=0x10;	
												Buffer_Sender[2]=0x02;			
												Buffer_Sender[3]=0x37;
												Buffer_Sender[4]=0x00;
												Buffer_Sender[5]=0x01;
												Buffer_Sender[6]=0x02;
												Buffer_Sender[7]=0x00;
												Buffer_Sender[8]=caiyangjiaozhunfa_state;							
												CRCValue =usMBCRC16(Buffer_Sender,9);
											 Buffer_Sender[9]=CRCValue&0x00ff;
											 Buffer_Sender[10]=CRCValue>>8;	
										    delay_ms(500);
										   OSIntEnter();
                       RS485_Send_Data(Buffer_Sender,11);
	                    OSIntExit(); 										
									}	 
						 		
								if(control_category&0x04)
						     	{    
										   Buffer_Sender[0]=0xFF;
												Buffer_Sender[1]=0x10;	
												Buffer_Sender[2]=0x02;			
												Buffer_Sender[3]=0x3B;
												Buffer_Sender[4]=0x00;
												Buffer_Sender[5]=0x01;
												Buffer_Sender[6]=0x02;
												Buffer_Sender[7]=0x00;
												Buffer_Sender[8]=PMT_state;							
												CRCValue =usMBCRC16(Buffer_Sender,9);
											  Buffer_Sender[9]=CRCValue&0x00ff;
											  Buffer_Sender[10]=CRCValue>>8;	
									     	delay_ms(500);
										    OSIntEnter();
                        RS485_Send_Data(Buffer_Sender,11);	
										   OSIntExit(); 	
									}										
}

void  Time_Record (void)
{
	  unsigned char i=0;
//	 unsigned char rs485buf[64];
   OS_ERR err;
	 char len1,len2,len3,len4,len5,len6;
   char FTOCHAR[7][5]; 
       len1=sprintf(FTOCHAR[1],"%d",calendar.S_w_year);      //��ʼ��
    	 len2=sprintf(FTOCHAR[2],"%02d",calendar.S_w_month);     //��ʼ��
  		 len3=sprintf(FTOCHAR[3],"%02d",calendar.S_w_date);       //��ʼ��
  		 len4=sprintf(FTOCHAR[4],"%d",calendar.E_w_year);      //��ֹ��
	     len5=sprintf(FTOCHAR[5],"%02d",calendar.E_w_month);      //��ֹ��
	     len6=sprintf(FTOCHAR[6],"%02d",calendar.E_w_date);      //��ֹ��
	   OSMutexPend (&OSUart4Used_Mutex,0,OS_OPT_PEND_BLOCKING,0,&err);	
	       delay_ms(2); 
  	   USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //��ؼ����͵�ͷ
	     USART3_SendByte(0x00);	USART3_SendByte(0x02);  	                   //����2
	     USART3_SendByte(0x00);	USART3_SendByte(0x02);                      //���غ�2 ����ʼ��
	     USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//�ַ�����
		    for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[1][i]);                //������ʼ������
					}
					
		   	USART3_SendByte(0x00);	USART3_SendByte(0x03);                      //���غ�3 ��ʼ��
			  USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//�ַ�����		
				 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[2][i]);                //������ʼ������
					}	
					
					USART3_SendByte(0x00);	USART3_SendByte(0x04);                      //���غ�4��ʼ��
				  USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//�ַ�����		
				 for(i=0;i<len3;i++)
					{
						USART3_SendByte(FTOCHAR[3][i]);                //������ʼ������
					}		
					
					USART3_SendByte(0x00);	USART3_SendByte(0x06);                      //���غ�6����ֹ��
				  USART3_SendByte(len4>>8);	USART3_SendByte(len4&0xff);	//�ַ�����		
				 for(i=0;i<len4;i++)
					{
						USART3_SendByte(FTOCHAR[4][i]);                //������ֹ��
					}		
					
						USART3_SendByte(0x00);	USART3_SendByte(0x07);                      //���غ�7����ֹyu��
				  USART3_SendByte(len5>>8);	USART3_SendByte(len5&0xff);	//�ַ�����		
				 for(i=0;i<len5;i++)
					{
						USART3_SendByte(FTOCHAR[5][i]);                //������ֹ��
					}			
				USART3_SendByte(0x00);	USART3_SendByte(0x08);                      //���غ�8����ֹ��
				  USART3_SendByte(len6>>8);	USART3_SendByte(len6&0xff);	//�ַ�����		
				 for(i=0;i<len6;i++)
					{
						USART3_SendByte(FTOCHAR[6][i]);                //������ֹ��
					}		
	         END_CMD();	
					 delay_ms(2); 
					
			 len1=sprintf(FTOCHAR[1],"%d",calendar.S3_w_year);      //��ʼ��.����ͼ
    	 len2=sprintf(FTOCHAR[2],"%02d",calendar.S3_w_month);     //��ʼ�£�����ͼ
  		 len3=sprintf(FTOCHAR[3],"%02d",calendar.S3_w_date);       //��ʼ�գ�����ͼ
  		 len4=sprintf(FTOCHAR[4],"%d",calendar.E3_w_year);      //��ֹ�꣬����ͼ
	     len5=sprintf(FTOCHAR[5],"%02d",calendar.E3_w_month);      //��ֹ�£�����ͼ
	     len6=sprintf(FTOCHAR[6],"%02d",calendar.E3_w_date);      //��ֹ�գ�����ͼ	
		 
			 USART3_SendByte(0xEE);USART3_SendByte(0xB1);	USART3_SendByte(0x12); //��ؼ����͵�ͷ	
			 USART3_SendByte(0x00);	USART3_SendByte(0x03);  	                   //����3		
			 USART3_SendByte(0x00);	USART3_SendByte(0x02);                      //���غ�2 ����ʼ��		
			 USART3_SendByte(len1>>8);	USART3_SendByte(len1&0xff);	//�ַ�����
		    for(i=0;i<len1;i++)
					{
						USART3_SendByte(FTOCHAR[1][i]);                //������ʼ������
					}	
			   	USART3_SendByte(0x00);	USART3_SendByte(0x03);                      //���غ�3 ��ʼ��
			   USART3_SendByte(len2>>8);	USART3_SendByte(len2&0xff);	//�ַ�����		
				 for(i=0;i<len2;i++)
					{
						USART3_SendByte(FTOCHAR[2][i]);                //������ʼ������
					}			
						USART3_SendByte(0x00);	USART3_SendByte(0x04);                      //���غ�4��ʼ��
				  USART3_SendByte(len3>>8);	USART3_SendByte(len3&0xff);	//�ַ�����		
				 for(i=0;i<len3;i++)
					{
						USART3_SendByte(FTOCHAR[3][i]);                //������ʼ������
					}		
					
					USART3_SendByte(0x00);	USART3_SendByte(0x06);                      //���غ�6����ֹ��
				  USART3_SendByte(len4>>8);	USART3_SendByte(len4&0xff);	//�ַ�����		
				 for(i=0;i<len4;i++)
					{
						USART3_SendByte(FTOCHAR[4][i]);                //������ֹ��
					}		
					
						USART3_SendByte(0x00);	USART3_SendByte(0x07);                      //���غ�7����ֹ��
				  USART3_SendByte(len5>>8);	USART3_SendByte(len5&0xff);	//�ַ�����		
				 for(i=0;i<len5;i++)
					{
						USART3_SendByte(FTOCHAR[5][i]);                //������ֹ��
					}			
				USART3_SendByte(0x00);	USART3_SendByte(0x08);                      //���غ�8����ֹ��
				  USART3_SendByte(len6>>8);	USART3_SendByte(len6&0xff);	//�ַ�����		
				 for(i=0;i<len6;i++)
					{
						USART3_SendByte(FTOCHAR[6][i]);                //������ֹ��
					}		
	         END_CMD();		
		  OSMutexPost(&OSUart4Used_Mutex,OS_OPT_POST_NONE,&err);
}



void EEprom_Record(void)
{
	     char i;
	                                calendar.S_w_year   = ReadEpData(200);
									                calendar.S_w_month  = ReadEpData(208);
									                calendar.S_w_date   = ReadEpData(216);
									                calendar.S_hour   = ReadEpData(224);
									         RevNameS_D=calendar.S_w_year*10000+calendar.S_w_month*100+calendar.S_w_date; //��ȡ���ݼ�¼ʱ��
	
																calendar.E_w_year=ReadEpData(232);
																calendar.E_w_month=ReadEpData(240);
																calendar.E_w_date=ReadEpData(248);
																calendar.E_hour=ReadEpData(256);
													 RevNameE_D=calendar.E_w_year*10000+ calendar.E_w_month*100+  calendar.E_w_date;
	                                        calendar.S3_w_year=ReadEpData(264);
																			 	 calendar.S3_w_month=ReadEpData(272);
																				 calendar.S3_w_date=ReadEpData(280);
																				 calendar.S3_hour=ReadEpData(288);
								           RevNameS3_D=   calendar.S3_w_year*10000+ calendar.S3_w_month*100+  calendar.S3_w_date;
	                                       calendar.E3_w_year=ReadEpData(296);
																			 	 calendar.E3_w_month=ReadEpData(304);
																				 calendar.E3_w_date=ReadEpData(312);
																				 calendar.E3_hour=ReadEpData(320);
								           RevNameE3_D=   calendar.E3_w_year*10000+ calendar.E3_w_month*100+  calendar.E3_w_date;
                                       
												   		Time_Record	();	 //�ϴε������ݵ�ʱ��									
															//���ݴ�ֵ��Ҫ��ȡ
																SO2CONCERNRATIONCAL.SO2K =  (float)ReadEpData(0x00)/1000;  //K
																SO2CONCERNRATIONCAL.SO2B =  (float)ReadEpData(0x08)/1000;  //B
	//�������ý���
	                Input.CRAteTEM_LowerLimit=(float)ReadEpData(30)/1000;//�����¶�������
	                Input.CRAteTEM_UpperLimit=(float)ReadEpData(38)/1000;
								  Input.TECTEM_LowerLimit=(float)ReadEpData(46)/1000;//PMT�¶�������
								  Input.TECTEM_UpperLimit=(float)ReadEpData(54)/1000;
	                Input.CaseTEM_LowerLimit=(float)ReadEpData(62)/1000;//��Ӧ���¶�
								  Input.CaseTEM_UpperLimit=(float)ReadEpData(70)/1000;
								  Input.SampGasP_LowerLimit=(float)ReadEpData(78)/1000; //����ѹ��
								  Input.SampGasP_UpperLimit=(float)ReadEpData(86)/1000;  
								  Input.CaseGasPRESSURE_LowerLimit=(float)ReadEpData(94)/1000;
								  Input.CaseGasPRESSURE_UpperLimit=(float)ReadEpData(102)/1000;
	                Input.GASFLOW_LowerLimit=(float)ReadEpData(110)/1000;
								  Input.GASFLOW_UpperLimit=(float)ReadEpData(118)/1000;
								  Input.PMTSampVL_LowerLimit=(float)ReadEpData(128)/1000;
								  Input.PMTSampVL_UpperLimit=(float)ReadEpData(136)/1000;
								  Input.REFVOL_PD_LowerLimit=(float)ReadEpData(144)/1000;
								  Input.REFVOL_PD_UpperLimit=(float)ReadEpData(152)/1000;
								  Input.REFVOL_PMT_LowerLimit=(float)ReadEpData(160)/1000;
								  Input.REFVOL_PMT_UpperLimit=(float)ReadEpData(168)/1000;
								  Input.VEF_LowerLimit=(float)ReadEpData(176)/1000;
								  Input.VEF_UpperLimit=(float)ReadEpData(184)/1000;
															 
								   Startup=ReadEpData(328);
									 Count_Input=ReadEpData(336);
                  if(Startup>10)
									  {
											Count_Input=6;Startup=6;
										}
                  										
									 AMulti[0].PMTSampVL=(float)ReadEpData(344)/1000;
								   Test_Catagry=	ReadEpData(352);
                   Var.AfterCALFlag=ReadEpData(360);
                   Var.BackGroundValue=(float)ReadEpData(368)/1000;
								   Var.OffSet=	(float)ReadEpData(376)/1000;
                   Var.Add_AfterCALFlag=	ReadEpData(2048);									
	                 for(i=0;i<Startup-1;i++)
									     {
											    	AMulti[i+1].k =(float)ReadEpData(384+(i)*8*2)/1000;          //kֵ
					                	AMulti[i+1].b =(float)ReadEpData(392+(i)*8*2)/1000;  //bֵ 				
  												 AMulti[i+1].SO2Soncentration_Input =(float)ReadEpData(512+(i)*8*2)/1000;          //kֵ
												   AMulti[i+1].PMTSampVL =(float)ReadEpData(520+(i)*8*2)/1000; 												 
											 }
										  display();											 
											if(Var.AfterCALFlag==2)
										      Auto_Cal_0701(230,1);//���ֿ���λ��������һ��
											else 
											{
												  Var.AfterCALFlag=0;
											    Auto_Cal_0701(230,0);
											}
												 
											if(Var.Add_AfterCALFlag==1)
										      Auto_Cal_0701(231,1);//���ֿ���λ��������һ��
											else 
											{
												Var.Add_AfterCALFlag=0;
											    Auto_Cal_0701(231,0);
											}
}                    
void Delete_RecorD(void)
		{
				USART3_SendByte(0xEE);	
				USART3_SendByte(0xB1);	
			  USART3_SendByte(0x53);	
			  USART3_SendByte(0x00);	
			  USART3_SendByte(0x02);	
			  USART3_SendByte(0x00);	
			  USART3_SendByte(0x01);		
								 END_CMD();	
		}


