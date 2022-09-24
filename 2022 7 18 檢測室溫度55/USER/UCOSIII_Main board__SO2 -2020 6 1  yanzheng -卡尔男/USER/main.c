#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "malloc.h"
#include "beep.h"
#include "includes.h"
#include "rtc.h"
#include "command.h"
#include "MMC_SD.h" 
#include "ff.h"  
#include "exfuns.h"
#include "USART3.h"
#include "communication.h"
#include "modbus_host.h"
#include "IICEEPROM.h"
#include "processing.h"
#include "mb.h"
#include "mbutils.h"
#include "stdio.h"
#include "string.h"
#include "datawr.h"
#include "output.h"
#include "cmd_queue.h"
#include "calibration.h"
#include "experiment.h"
//����Ĵ�����ʼ��ַ
#define REG_INPUT_START       0x0000
//����Ĵ�������
#define REG_INPUT_NREGS      100
//���ּĴ�����ʼ��ַ
#define REG_HOLDING_START     0x0000
//���ּĴ�������
#define REG_HOLDING_NREGS    100

//��Ȧ��ʼ��ַ  
#define REG_COILS_START       0x0000
//��Ȧ����
#define REG_COILS_SIZE       16

//���ؼĴ�����ʼ��ַ   2020 4 21
#define REG_DISCRETE_START    0x0000
//���ؼĴ�������
#define REG_DISCRETE_SIZE     16
char usRegHoldingBuf_flag;
char usRegHoldingBuf_Writeflag;
/* Private variables ---------------------------------------------------------*/
//����Ĵ�������
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F};
//�Ĵ�����ʼ��ַ
uint16_t usRegInputStart = REG_INPUT_START;

//���ּĴ�������
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F};
//���ּĴ�����ʼ��ַ
uint16_t usRegHoldingStart = REG_HOLDING_START;

//��Ȧ״̬     2020 4 21
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x00,0x00};
//��������״̬
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x00,0x00};


/************************************************
//UCOSIII���������ȼ��û�������ʹ�ã�
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
************************************************/
OS_MUTEX OSUart4Used_Mutex;
OS_MUTEX OSI2C1Used_Mutex;
//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define MAIN_TASK_PRIO		4
//�����ջ��С	
#define MAIN_STK_SIZE 		256
//������ƿ�
OS_TCB Main_TaskTCB;
//�����ջ	
CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *p_arg);

//�������ȼ�
#define KEYPROCESS_TASK_PRIO 	22
//�����ջ��С	
#define KEYPROCESS_STK_SIZE 	256
//������ƿ�
OS_TCB Keyprocess_TaskTCB;
//�����ջ	
CPU_STK KEYPROCESS_TASK_STK[KEYPROCESS_STK_SIZE];
//������
void Keyprocess_task(void *p_arg);

//�������ȼ�
#define MSGDIS_TASK_PRIO	6
//�����ջ��С
#define MSGDIS_STK_SIZE		128
//������ƿ�
OS_TCB	Msgdis_TaskTCB;
//�����ջ
CPU_STK	MSGDIS_TASK_STK[MSGDIS_STK_SIZE];
//������
void msgdis_task(void *p_arg);

//�������ȼ�
#define COMMUNICATION_TASK_PRIO 12
//�����ջ��С
#define COMMUNICATION_STK_SIZE  512
//������ƿ�
OS_TCB	COMMUNICATION_TaskTCB;
//�����ջ
CPU_STK COMMUNICATION_TASK_STK[COMMUNICATION_STK_SIZE];
//������
void communication_task(void *p_arg);    

//�������ȼ�
#define CALIBRATION_TASK_PRIO 13
//�����ջ��С
#define CALIBRATION_STK_SIZE 256
//������ƿ�
OS_TCB	CALIBRATION_TaskTCB;
//�����ջ
CPU_STK CALIBRATION_TASK_STK[CALIBRATION_STK_SIZE];
//������
void calibration_task(void *p_arg);

//�������ȼ�
#define WarnSet_TASK_PRIO 14
//������ջ��С
#define WarnSet_STK_SIZE 256
//������ƿ�
OS_TCB    WarnSet_TaskTCB;
//������ջ
CPU_STK WarnSet_TASK_STK[WarnSet_STK_SIZE];
//������
void WarnSet_task(void *p_arg);
//�������ȼ�
#define RecordData_TASK_PRIO 15
//�����ջ��С
#define RecordData_STK_SIZE 256
//������ƿ�
OS_TCB    RecordData_TaskTCB;
//�����ջ
CPU_STK RecordData_TASK_STK[WarnSet_STK_SIZE];
//������
void RecordData_task(void *p_arg);
////////////////////////��Ϣ����//////////////////////////////
#define KEYMSG_Q_NUM	1	//������Ϣ���е�����
#define DATAMSG_Q_NUM	4	//�������ݵ���Ϣ���е�����
OS_Q KEY_Msg;				//����һ����Ϣ���У����ڰ�����Ϣ���ݣ�ģ����Ϣ����
OS_Q DATA_Msg;				//����һ����Ϣ���У����ڷ�������					
////////////////////////��ʱ��////////////////////////////////
u8 tmr1sta=0; 	//��Ƕ�ʱ���Ĺ���״̬
OS_TMR	tmr1;	//����һ����ʱ��
void tmr1_callback(void *p_tmr,void *p_arg); //��ʱ��1�ص�����
int ii,jj;	
u8 c,key;
u8 Migrate;u8 Migrate2;
extern int TimeMeasCount;
extern int TImeMeasControl;
//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	u32 total,free;
	delay_init();  //ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	USART1_Config();  //���ڳ�ʼ�� (0,2)
	USART3_Config();//������ͨѶ3,2
	 //��մ��ڽ��ջ�����                                                          
   queue_reset();                                                                  
  //��ʱ�ȴ���������ʼ�����,����ȴ�300ms                                      
    delay_ms(300); 
	RS485_Initt(57600);//����4 (2,0)
	eMBInit(MB_RTU, 0x01, 0x01, 9600, MB_PAR_NONE); //��ʼ�� RTUģʽ �ӻ���ַΪ1 USART1 9600 ��У��         
  eMBEnable(); //����FreeModbus 
	I2C_EE_Init();
	LED_Init();         //LED��ʼ��	
	KEY_Init();			//������ʼ��
	BEEP_Init();		//��ʼ��������
  exfuns_init();	//fΪFATFS��ر��������ڴ�
	mem_init();//��ʼ���ڲ�RAM
		while(SD_Initialize())					//���SD��
	{
		delay_ms(200);
		printf("sd init err");
	}	
	exfuns_init();							//Ϊfatfs��ر��������ڴ�		
  f_mount(fs[0],"0:",1); 					//����SD�� 
		while(exf_getfree("0",&total,&free))	//�õ�SD������������ʣ������
	{	  
		printf("FATFS init err");
		delay_ms(200);
	}	
	printf("total=%d, free=%d\r\n",total>>10,free>>10);
	 // delay_ms(5000);
	// delay_ms(5000);
	RTC_Init();//�ж����ȼ� ��0 ��0��
	printf("%d-%02d-%02d %02d:%02d:%02d;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec);
	//���ݴ�ֵ��Ҫ��ȡ
      EEprom_Record();
	   delay_ms(5000);
     	Event_Record(0);
	  ReadValue_InputBuff();
	  ReadValue_HoldingBuf();
    KuaiMen_Switch(0x00,0x00,0x00,0x07);switch_status=0;
	for(ii=0;ii<8;ii++)
	 {
		 for( jj=0;jj<120;jj++)
		 {
		  elmResultBuf[ii][jj]=0;
		 }
	 }
	  USART3_SendByte(0xEE);
		USART3_SendByte(0xB1);	
		USART3_SendByte(0x00);	
		USART3_SendByte(0x00);
		USART3_SendByte(0x0A);
	   END_CMD();
	  Icon_Display(0);  //�������������ɫͨ��
	//	delay_ms(5000);//�����л�ָ��ȴ�����
	  while(yure_jiemian())
		{
			Read_Total_Parameter();
		};
    Display_Sampling_Switch(0);//�л���ͨ���ڡ�
		press_button();   //��ʼ���ɿ���״̬
  //�п���ȥ��ȡ������
   KuaiMen_Switch(0x01,0x00,0x00,0x07);switch_status=0;  //�رտ��ţ������ڣ�shutdown pmt��ͬʱ
	 delay_ms(2000);//�����л�ָ��ȴ�����
	//���������е����ȡ������
	OSInit(&err);		    //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���			 
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
}
//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
		
	OS_CRITICAL_ENTER();	//�����ٽ���
		//����һ�������ź���
	OSMutexCreate((OS_MUTEX*	)&OSUart4Used_Mutex,
				  (CPU_CHAR*	)"OSUart4Used_Mutex",
                  (OS_ERR*		)&err);
						//OSI2C1Used_Mutex			
		OSMutexCreate((OS_MUTEX*	)&OSI2C1Used_Mutex,
				  (CPU_CHAR*	)"OSI2C1Used_Mutex",
                  (OS_ERR*		)&err);							
									
									
	//������Ϣ����KEY_Msg
	OSQCreate ((OS_Q*		)&KEY_Msg,	//��Ϣ����
                (CPU_CHAR*	)"KEY Msg",	//��Ϣ��������
                (OS_MSG_QTY	)KEYMSG_Q_NUM,	//��Ϣ���г��ȣ���������Ϊ1
                (OS_ERR*	)&err);		//������
	//������Ϣ����DATA_Msg
	OSQCreate ((OS_Q*		)&DATA_Msg,	
                (CPU_CHAR*	)"DATA Msg",	
                (OS_MSG_QTY	)DATAMSG_Q_NUM,	
                (OS_ERR*	)&err);	
	//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )0,			//0ms
                (OS_TICK	 )50,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����
	//����������
	OSTaskCreate((OS_TCB 	* )&Main_TaskTCB,		
				 (CPU_CHAR	* )"Main task", 		
                 (OS_TASK_PTR )main_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MAIN_TASK_PRIO,     
                 (CPU_STK   * )&MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);			
	//������������
	OSTaskCreate((OS_TCB 	* )&Keyprocess_TaskTCB,		
				 (CPU_CHAR	* )"Keyprocess task", 		
                 (OS_TASK_PTR )Keyprocess_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )KEYPROCESS_TASK_PRIO,     
                 (CPU_STK   * )&KEYPROCESS_TASK_STK[0],	
                 (CPU_STK_SIZE)KEYPROCESS_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEYPROCESS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);			
	//����ͨ������						 
						OSTaskCreate((OS_TCB 	* )&COMMUNICATION_TaskTCB,		
				    (CPU_CHAR	* )"communication task", 		
                 (OS_TASK_PTR )communication_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )COMMUNICATION_TASK_PRIO,     
                 (CPU_STK   * )&COMMUNICATION_TASK_STK[0],	
                 (CPU_STK_SIZE)COMMUNICATION_STK_SIZE/10,	
                 (CPU_STK_SIZE)COMMUNICATION_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				 
	//У׼����							 
					  OSTaskCreate((OS_TCB 	* )&CALIBRATION_TaskTCB,		
				    (CPU_CHAR	* )"calibration task", 		
                 (OS_TASK_PTR )calibration_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )CALIBRATION_TASK_PRIO,     
                 (CPU_STK   * )&CALIBRATION_TASK_STK[0],	
                 (CPU_STK_SIZE)CALIBRATION_STK_SIZE/10,	
                 (CPU_STK_SIZE)CALIBRATION_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
	//��������						
              OSTaskCreate((OS_TCB 	* )&WarnSet_TaskTCB,		
				        (CPU_CHAR	* )"WarnSet task", 		
                 (OS_TASK_PTR )WarnSet_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )WarnSet_TASK_PRIO,  //  WarnSet_TASK_PRIO 
                 (CPU_STK   * )&WarnSet_TASK_STK[0],	//DataRecord_TASK_STK
                 (CPU_STK_SIZE)WarnSet_STK_SIZE/10,	//WarnSet_STK_SIZE
                 (CPU_STK_SIZE)WarnSet_STK_SIZE,		//WarnSet_STK_SIZE
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);			
//���ݼ�¼
							 OSTaskCreate((OS_TCB 	* )&RecordData_TaskTCB,		
				        (CPU_CHAR	* )"RecordData task", 		
                 (OS_TASK_PTR )RecordData_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )RecordData_TASK_PRIO,  //  WarnSet_TASK_PRIO 
                 (CPU_STK   * )&RecordData_TASK_STK[0],	//DataRecord_TASK_STK
                 (CPU_STK_SIZE)RecordData_STK_SIZE/10,	//WarnSet_STK_SIZE
                 (CPU_STK_SIZE)RecordData_STK_SIZE,		//WarnSet_STK_SIZE
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);		 	 								 
	//����MSGDIS����
//	OSTaskCreate((OS_TCB 	* )&Msgdis_TaskTCB,		
//				 (CPU_CHAR	* )"Msgdis task", 		
//                 (OS_TASK_PTR )msgdis_task, 			
//                 (void		* )0,					
//                 (OS_PRIO	  )MSGDIS_TASK_PRIO,     
//                 (CPU_STK   * )&MSGDIS_TASK_STK[0],	
//                 (CPU_STK_SIZE)MSGDIS_STK_SIZE/10,	
//                 (CPU_STK_SIZE)MSGDIS_STK_SIZE,		
//                 (OS_MSG_QTY  )0,					
//                 (OS_TICK	  )0,  					
//                 (void   	* )0,					
//                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
//                 (OS_ERR 	* )&err);	
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}
//��ʱ��1�Ļص�����
void tmr1_callback(void *p_tmr,void *p_arg)
{
	u8 *pbuf;
	static u8 msg_num;
	OS_ERR err;
	pbuf = mymalloc(10);	//����10���ֽ�
	if(pbuf)	//�����ڴ�ɹ�
	{
		msg_num++;
		sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
		//������Ϣ
		OSQPost((OS_Q*		)&DATA_Msg,		
				(void*		)pbuf,
				(OS_MSG_SIZE)10,
				(OS_OPT		)OS_OPT_POST_FIFO,
				(OS_ERR*	)&err);
		if(err != OS_ERR_NONE)
		{
			myfree(pbuf);	//�ͷ��ڴ�
			OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //ֹͣ��ʱ��1
			tmr1sta = !tmr1sta;
		}
	}	
}
//�������������
void main_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		if(Migrate==0)
		{
		  TImeMeasControl=10; //100
	    Migrate=1;Migrate2=1;
		}	
		Read_Total_Parameter();
		dark_current();
	while(!	TImeMeasControl)
	{
		LED0=1;
		if(Migrate2==1)
		{
			KuaiMen_Switch(0x00,0x00,0x00,0x07);switch_status=0;  //dakai ���ţ������ڣ�shutdown pmt��ͬʱ
		  Migrate2=0;
		}	
		Read_Total_Parameter();
		PMT_Dri_Valage();
	  readscreen0();	
  	Read_SO2ConCenTration();
		if(READSCRREN.WarnPara_Flag==0)
		{
		READSCRREN.WarnPara_Flag=1;
		}
		LED0=0;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}		
		OSTimeDlyHMSM(0,0,1,300,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
	}
}
//�������������������
void Keyprocess_task(void *p_arg)
{	
	OS_ERR err;
	
	while(1)
	{
			
   // Experiment();
	 OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //��ʱ1s
	}
}

void communication_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
	eMBPoll();		
	communication();	
	checkout();
	OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_PERIODIC,&err); //��ʱ200ms
	}
}
void calibration_task(void *p_arg)
{
		OS_ERR err;
	 while(1)
	 {		 
		 Calibration_SO2ConCenTration();
		 Multi_points_Calibration();
		 OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_PERIODIC,&err); //��ʱ200ms
	 }
}
void WarnSet_task(void *p_arg)
{
		OS_ERR err;
	OSTimeDlyHMSM(0,1,1,300,OS_OPT_TIME_PERIODIC,&err); 
	  while(1)
		{
			if(READSCRREN.WarnPara_Flag==1)
			{
			 	   Set_Warn();
			     READSCRREN.SCRREN8FLAG=1;
				   CheckWarn();	
			}
				if(usRegHoldingBuf_Writeflag==1)
		{
			
				data_transmitter(WFlag.HoldingregStart,WFlag.iRegIndex);	
			
			  usRegHoldingBuf_Writeflag=0;
		}			
			OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_PERIODIC,&err); //��ʱ200ms
		}
	
}
 void RecordData_task(void *p_arg)
	{
		OS_ERR err;
	  while(1)
		{		 
			  Record_Data_Task();
			  ReadValue_InputBuff();
			  OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_PERIODIC,&err); //��ʱ200ms
		}	
	}
//��ʾ��Ϣ�����е���Ϣ
void msgdis_task(void *p_arg)
{
	u8 *p;
	OS_MSG_SIZE size;
	OS_ERR err; 
	while(1)
	{
		//������Ϣ
		p=OSQPend((OS_Q*		)&DATA_Msg,   
				  (OS_TICK		)0,
                  (OS_OPT		)OS_OPT_PEND_BLOCKING,
                  (OS_MSG_SIZE*	)&size,	
                  (CPU_TS*		)0,
                  (OS_ERR*		)&err);
		myfree(p);	//�ͷ��ڴ�
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //��ʱ1s
	}
}

/**
  * @brief  ����Ĵ���������������Ĵ����ɶ���������д��
  * @param  pucRegBuffer  ��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  * @retval eStatus       �Ĵ���״̬
  */
eMBErrorCode 
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int16_t         iRegIndex;
  
  //��ѯ�Ƿ��ڼĴ�����Χ��
  //Ϊ�˱��⾯�棬�޸�Ϊ�з�������
  if( ( (int16_t)usAddress >= REG_INPUT_START ) \
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    //��ò���ƫ���������β�����ʼ��ַ-����Ĵ����ĳ�ʼ��ַ
    iRegIndex = ( int16_t )( usAddress - usRegInputStart );
    //�����ֵ
    while( usNRegs > 0 )
    {
      //��ֵ���ֽ�
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] >> 8 );
      //��ֵ���ֽ�
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] & 0xFF );
      //ƫ��������
      iRegIndex++;
      //�������Ĵ��������ݼ�
      usNRegs--;
    }
  }
  else
  {
    //���ش���״̬���޼Ĵ���  
    eStatus = MB_ENOREG;
  }

  return eStatus;
}

/**
  * @brief  ���ּĴ��������������ּĴ����ɶ����ɶ���д
  * @param  pucRegBuffer  ������ʱ--��������ָ�룬д����ʱ--��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  *         eMode         ������ʽ��������д
  * @retval eStatus       �Ĵ���״̬
  */
eMBErrorCode 
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //ƫ����
  int16_t         iRegIndex;
  
  //�жϼĴ����ǲ����ڷ�Χ��
  if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
     && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    //����ƫ����
    iRegIndex = ( int16_t )( usAddress - usRegHoldingStart );
    
    switch ( eMode )
    {
      //��������  
      case MB_REG_READ:
        while( usNRegs > 0 )
        {
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );
          iRegIndex++;
          usNRegs--;	
        }
				usRegHoldingBuf_flag=0;//z���Ӷ�д��־λ
        break;

      //д������ 
      case MB_REG_WRITE:
        while( usNRegs > 0 )
        {
          usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
          usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
					switch(iRegIndex) //s�׵�ַ
										 {
										case 0:
													 WFlag.HoldingregStart=iRegIndex;//��ȡ��ʼ�Ĵ���
													 break;
										}	      
					iRegIndex++;
          usNRegs--;	
				}
				WFlag.iRegIndex=iRegIndex;//��ȡ�Ĵ�������
				usRegHoldingBuf_flag=1;//���Ӷ�д��־λ
				usRegHoldingBuf_Writeflag=1;
        break;
     }
  }
  else
  {
    //���ش���״̬
    eStatus = MB_ENOREG;
  }
  
  return eStatus;
}


/**
  * @brief  ��Ȧ�Ĵ�������������Ȧ�Ĵ����ɶ����ɶ���д
  * @param  pucRegBuffer  ������---��������ָ�룬д����--��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  *         eMode         ������ʽ��������д
  * @retval eStatus       �Ĵ���״̬
  */
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //�Ĵ�������
  int16_t         iNCoils = ( int16_t )usNCoils;
  //�Ĵ���ƫ����
  int16_t         usBitOffset;

  //���Ĵ����Ƿ���ָ����Χ��
  if( ( (int16_t)usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
  {
    //����Ĵ���ƫ����
    usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
    switch ( eMode )
    {
      //������
      case MB_REG_READ:
        while( iNCoils > 0 )
        {
          *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                          ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
          iNCoils -= 8;
          usBitOffset += 8;
        }
        break;
      //д����
      case MB_REG_WRITE:
        while( iNCoils > 0 )
        {
          xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
                        ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
                        *pucRegBuffer++ );
          iNCoils -= 8;
        }
        break;
    }
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //�����Ĵ�������
  int16_t         iNDiscrete = ( int16_t )usNDiscrete;
  //ƫ����
  uint16_t        usBitOffset;

  //�жϼĴ���ʱ�����ƶ���Χ��
  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {
    //���ƫ����
    usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );
    
    while( iNDiscrete > 0 )
    {
      *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                                      ( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
      iNDiscrete -= 8;
      usBitOffset += 8;
    }
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}



