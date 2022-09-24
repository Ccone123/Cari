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
//输入寄存器起始地址
#define REG_INPUT_START       0x0000
//输入寄存器数量
#define REG_INPUT_NREGS      100
//保持寄存器起始地址
#define REG_HOLDING_START     0x0000
//保持寄存器数量
#define REG_HOLDING_NREGS    100

//线圈起始地址  
#define REG_COILS_START       0x0000
//线圈数量
#define REG_COILS_SIZE       16

//开关寄存器起始地址   2020 4 21
#define REG_DISCRETE_START    0x0000
//开关寄存器数量
#define REG_DISCRETE_SIZE     16
char usRegHoldingBuf_flag;
char usRegHoldingBuf_Writeflag;
/* Private variables ---------------------------------------------------------*/
//输入寄存器内容
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F};
//寄存器起始地址
uint16_t usRegInputStart = REG_INPUT_START;

//保持寄存器内容
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F};
//保持寄存器起始地址
uint16_t usRegHoldingStart = REG_HOLDING_START;

//线圈状态     2020 4 21
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x00,0x00};
//开关输入状态
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x00,0x00};


/************************************************
//UCOSIII中以下优先级用户程序不能使用，
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
************************************************/
OS_MUTEX OSUart4Used_Mutex;
OS_MUTEX OSI2C1Used_Mutex;
//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define MAIN_TASK_PRIO		4
//任务堆栈大小	
#define MAIN_STK_SIZE 		256
//任务控制块
OS_TCB Main_TaskTCB;
//任务堆栈	
CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *p_arg);

//任务优先级
#define KEYPROCESS_TASK_PRIO 	22
//任务堆栈大小	
#define KEYPROCESS_STK_SIZE 	256
//任务控制块
OS_TCB Keyprocess_TaskTCB;
//任务堆栈	
CPU_STK KEYPROCESS_TASK_STK[KEYPROCESS_STK_SIZE];
//任务函数
void Keyprocess_task(void *p_arg);

//任务优先级
#define MSGDIS_TASK_PRIO	6
//任务堆栈大小
#define MSGDIS_STK_SIZE		128
//任务控制块
OS_TCB	Msgdis_TaskTCB;
//任务堆栈
CPU_STK	MSGDIS_TASK_STK[MSGDIS_STK_SIZE];
//任务函数
void msgdis_task(void *p_arg);

//任务优先级
#define COMMUNICATION_TASK_PRIO 12
//任务堆栈大小
#define COMMUNICATION_STK_SIZE  512
//任务控制块
OS_TCB	COMMUNICATION_TaskTCB;
//任务堆栈
CPU_STK COMMUNICATION_TASK_STK[COMMUNICATION_STK_SIZE];
//任务函数
void communication_task(void *p_arg);    

//任务优先级
#define CALIBRATION_TASK_PRIO 13
//任务堆栈大小
#define CALIBRATION_STK_SIZE 256
//任务控制块
OS_TCB	CALIBRATION_TaskTCB;
//任务堆栈
CPU_STK CALIBRATION_TASK_STK[CALIBRATION_STK_SIZE];
//任务函数
void calibration_task(void *p_arg);

//任务优先级
#define WarnSet_TASK_PRIO 14
//任务退栈大小
#define WarnSet_STK_SIZE 256
//任务控制块
OS_TCB    WarnSet_TaskTCB;
//任务退栈
CPU_STK WarnSet_TASK_STK[WarnSet_STK_SIZE];
//任务函数
void WarnSet_task(void *p_arg);
//任务优先级
#define RecordData_TASK_PRIO 15
//任务堆栈大小
#define RecordData_STK_SIZE 256
//任务控制块
OS_TCB    RecordData_TaskTCB;
//任务堆栈
CPU_STK RecordData_TASK_STK[WarnSet_STK_SIZE];
//任务函数
void RecordData_task(void *p_arg);
////////////////////////消息队列//////////////////////////////
#define KEYMSG_Q_NUM	1	//按键消息队列的数量
#define DATAMSG_Q_NUM	4	//发送数据的消息队列的数量
OS_Q KEY_Msg;				//定义一个消息队列，用于按键消息传递，模拟消息邮箱
OS_Q DATA_Msg;				//定义一个消息队列，用于发送数据					
////////////////////////定时器////////////////////////////////
u8 tmr1sta=0; 	//标记定时器的工作状态
OS_TMR	tmr1;	//定义一个定时器
void tmr1_callback(void *p_tmr,void *p_arg); //定时器1回调函数
int ii,jj;	
u8 c,key;
u8 Migrate;u8 Migrate2;
extern int TimeMeasCount;
extern int TImeMeasControl;
//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	u32 total,free;
	delay_init();  //时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	USART1_Config();  //串口初始化 (0,2)
	USART3_Config();//串口屏通讯3,2
	 //清空串口接收缓冲区                                                          
   queue_reset();                                                                  
  //延时等待串口屏初始化完毕,必须等待300ms                                      
    delay_ms(300); 
	RS485_Initt(57600);//串口4 (2,0)
	eMBInit(MB_RTU, 0x01, 0x01, 9600, MB_PAR_NONE); //初始化 RTU模式 从机地址为1 USART1 9600 无校验         
  eMBEnable(); //启动FreeModbus 
	I2C_EE_Init();
	LED_Init();         //LED初始化	
	KEY_Init();			//按键初始化
	BEEP_Init();		//初始化蜂鸣器
  exfuns_init();	//f为FATFS相关变量申请内存
	mem_init();//初始化内部RAM
		while(SD_Initialize())					//检测SD卡
	{
		delay_ms(200);
		printf("sd init err");
	}	
	exfuns_init();							//为fatfs相关变量申请内存		
  f_mount(fs[0],"0:",1); 					//挂载SD卡 
		while(exf_getfree("0",&total,&free))	//得到SD卡的总容量和剩余容量
	{	  
		printf("FATFS init err");
		delay_ms(200);
	}	
	printf("total=%d, free=%d\r\n",total>>10,free>>10);
	 // delay_ms(5000);
	// delay_ms(5000);
	RTC_Init();//中断优先级 （0 ，0）
	printf("%d-%02d-%02d %02d:%02d:%02d;\r\n",calendar.w_year,calendar.w_month,calendar.w_date, calendar.hour,calendar.min,calendar.sec);
	//数据存值需要读取
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
	  Icon_Display(0);  //消除报警后的绿色通行
	//	delay_ms(5000);//发送切换指令等待两秒
	  while(yure_jiemian())
		{
			Read_Total_Parameter();
		};
    Display_Sampling_Switch(0);//切换成通气口。
		press_button();   //初始化成开机状态
  //切快门去读取暗电流
   KuaiMen_Switch(0x01,0x00,0x00,0x07);switch_status=0;  //关闭快门，采样口，shutdown pmt，同时
	 delay_ms(2000);//发送切换指令等待两秒
	//以上用于切挡板读取暗电流
	OSInit(&err);		    //初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区			 
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
}
//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
		
	OS_CRITICAL_ENTER();	//进入临界区
		//创建一个互斥信号量
	OSMutexCreate((OS_MUTEX*	)&OSUart4Used_Mutex,
				  (CPU_CHAR*	)"OSUart4Used_Mutex",
                  (OS_ERR*		)&err);
						//OSI2C1Used_Mutex			
		OSMutexCreate((OS_MUTEX*	)&OSI2C1Used_Mutex,
				  (CPU_CHAR*	)"OSI2C1Used_Mutex",
                  (OS_ERR*		)&err);							
									
									
	//创建消息队列KEY_Msg
	OSQCreate ((OS_Q*		)&KEY_Msg,	//消息队列
                (CPU_CHAR*	)"KEY Msg",	//消息队列名称
                (OS_MSG_QTY	)KEYMSG_Q_NUM,	//消息队列长度，这里设置为1
                (OS_ERR*	)&err);		//错误码
	//创建消息队列DATA_Msg
	OSQCreate ((OS_Q*		)&DATA_Msg,	
                (CPU_CHAR*	)"DATA Msg",	
                (OS_MSG_QTY	)DATAMSG_Q_NUM,	
                (OS_ERR*	)&err);	
	//创建定时器1
	OSTmrCreate((OS_TMR		*)&tmr1,		//定时器1
                (CPU_CHAR	*)"tmr1",		//定时器名字
                (OS_TICK	 )0,			//0ms
                (OS_TICK	 )50,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//定时器1回调函数
                (void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码
	//创建主任务
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
	//创建按键任务
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
	//创建通信任务						 
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
	//校准任务							 
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
	//报警设置						
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
//数据记录
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
	//创建MSGDIS任务
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
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}
//定时器1的回调函数
void tmr1_callback(void *p_tmr,void *p_arg)
{
	u8 *pbuf;
	static u8 msg_num;
	OS_ERR err;
	pbuf = mymalloc(10);	//申请10个字节
	if(pbuf)	//申请内存成功
	{
		msg_num++;
		sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
		//发送消息
		OSQPost((OS_Q*		)&DATA_Msg,		
				(void*		)pbuf,
				(OS_MSG_SIZE)10,
				(OS_OPT		)OS_OPT_POST_FIFO,
				(OS_ERR*	)&err);
		if(err != OS_ERR_NONE)
		{
			myfree(pbuf);	//释放内存
			OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //停止定时器1
			tmr1sta = !tmr1sta;
		}
	}	
}
//主任务的任务函数
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
			KuaiMen_Switch(0x00,0x00,0x00,0x07);switch_status=0;  //dakai 快门，采样口，shutdown pmt，同时
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
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}		
		OSTimeDlyHMSM(0,0,1,300,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}
}
//按键处理任务的任务函数
void Keyprocess_task(void *p_arg)
{	
	OS_ERR err;
	
	while(1)
	{
			
   // Experiment();
	 OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //延时1s
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
	OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_PERIODIC,&err); //延时200ms
	}
}
void calibration_task(void *p_arg)
{
		OS_ERR err;
	 while(1)
	 {		 
		 Calibration_SO2ConCenTration();
		 Multi_points_Calibration();
		 OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_PERIODIC,&err); //延时200ms
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
			OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_PERIODIC,&err); //延时200ms
		}
	
}
 void RecordData_task(void *p_arg)
	{
		OS_ERR err;
	  while(1)
		{		 
			  Record_Data_Task();
			  ReadValue_InputBuff();
			  OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_PERIODIC,&err); //延时200ms
		}	
	}
//显示消息队列中的消息
void msgdis_task(void *p_arg)
{
	u8 *p;
	OS_MSG_SIZE size;
	OS_ERR err; 
	while(1)
	{
		//请求消息
		p=OSQPend((OS_Q*		)&DATA_Msg,   
				  (OS_TICK		)0,
                  (OS_OPT		)OS_OPT_PEND_BLOCKING,
                  (OS_MSG_SIZE*	)&size,	
                  (CPU_TS*		)0,
                  (OS_ERR*		)&err);
		myfree(p);	//释放内存
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //延时1s
	}
}

/**
  * @brief  输入寄存器处理函数，输入寄存器可读，但不可写。
  * @param  pucRegBuffer  返回数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  * @retval eStatus       寄存器状态
  */
eMBErrorCode 
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int16_t         iRegIndex;
  
  //查询是否在寄存器范围内
  //为了避免警告，修改为有符号整数
  if( ( (int16_t)usAddress >= REG_INPUT_START ) \
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    //获得操作偏移量，本次操作起始地址-输入寄存器的初始地址
    iRegIndex = ( int16_t )( usAddress - usRegInputStart );
    //逐个赋值
    while( usNRegs > 0 )
    {
      //赋值高字节
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] >> 8 );
      //赋值低字节
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] & 0xFF );
      //偏移量增加
      iRegIndex++;
      //被操作寄存器数量递减
      usNRegs--;
    }
  }
  else
  {
    //返回错误状态，无寄存器  
    eStatus = MB_ENOREG;
  }

  return eStatus;
}

/**
  * @brief  保持寄存器处理函数，保持寄存器可读，可读可写
  * @param  pucRegBuffer  读操作时--返回数据指针，写操作时--输入数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  *         eMode         操作方式，读或者写
  * @retval eStatus       寄存器状态
  */
eMBErrorCode 
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //偏移量
  int16_t         iRegIndex;
  
  //判断寄存器是不是在范围内
  if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
     && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    //计算偏移量
    iRegIndex = ( int16_t )( usAddress - usRegHoldingStart );
    
    switch ( eMode )
    {
      //读处理函数  
      case MB_REG_READ:
        while( usNRegs > 0 )
        {
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );
          iRegIndex++;
          usNRegs--;	
        }
				usRegHoldingBuf_flag=0;//z增加读写标志位
        break;

      //写处理函数 
      case MB_REG_WRITE:
        while( usNRegs > 0 )
        {
          usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
          usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
					switch(iRegIndex) //s首地址
										 {
										case 0:
													 WFlag.HoldingregStart=iRegIndex;//获取初始寄存器
													 break;
										}	      
					iRegIndex++;
          usNRegs--;	
				}
				WFlag.iRegIndex=iRegIndex;//获取寄存器个数
				usRegHoldingBuf_flag=1;//增加读写标志位
				usRegHoldingBuf_Writeflag=1;
        break;
     }
  }
  else
  {
    //返回错误状态
    eStatus = MB_ENOREG;
  }
  
  return eStatus;
}


/**
  * @brief  线圈寄存器处理函数，线圈寄存器可读，可读可写
  * @param  pucRegBuffer  读操作---返回数据指针，写操作--返回数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  *         eMode         操作方式，读或者写
  * @retval eStatus       寄存器状态
  */
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //寄存器个数
  int16_t         iNCoils = ( int16_t )usNCoils;
  //寄存器偏移量
  int16_t         usBitOffset;

  //检查寄存器是否在指定范围内
  if( ( (int16_t)usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
  {
    //计算寄存器偏移量
    usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
    switch ( eMode )
    {
      //读操作
      case MB_REG_READ:
        while( iNCoils > 0 )
        {
          *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                          ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
          iNCoils -= 8;
          usBitOffset += 8;
        }
        break;
      //写操作
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
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //操作寄存器个数
  int16_t         iNDiscrete = ( int16_t )usNDiscrete;
  //偏移量
  uint16_t        usBitOffset;

  //判断寄存器时候再制定范围内
  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {
    //获得偏移量
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



