#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "includes.h"
#include "command.h"
#include "com_press.h"
#include "mb.h"
#include "mbutils.h"
#include "lyndir_no.h"
#include "usart4.h"
#include "adc.h"
#include "iic.h"
#include "ntc.h"
#include "MPL115A2.H"
#include "sm5852.h"
#include "iicssc.h"
#include "PID_Ctrl.h"
#include "pwm.h"
#include "ntc.h"
//输入寄存器起始地址
#define REG_INPUT_START       0x0000
//输入寄存器数量
#define REG_INPUT_NREGS       100
//保持寄存器起始地址
#define REG_HOLDING_START     0x0000
//保持寄存器数量
#define REG_HOLDING_NREGS     100
//线圈起始地址
#define REG_COILS_START       0x0000
//线圈数量
#define REG_COILS_SIZE        16

//开关寄存器起始地址
#define REG_DISCRETE_START    0x0000
//开关寄存器数量
#define REG_DISCRETE_SIZE     16
/* Private variables ---------------------------------------------------------*/
//输入寄存器内容
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0};
//寄存器起始地址
uint16_t usRegInputStart = REG_INPUT_START;
//保持寄存器内容
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0};
//保持寄存器起始地址
uint16_t usRegHoldingStart = REG_HOLDING_START;
//线圈状态
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x00,0x00};
//开关输入状态
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x00,0x00};
/********************************************************/
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
/********************************************************/
//任务优先级
#define MAIN_TASK_PRIO		6
//任务堆栈大小	
#define MAIN_STK_SIZE 		256
//任务控制块
OS_TCB Main_TaskTCB;
//任务堆栈	
CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *p_arg);
/********************************************************/
//任务优先级
#define KEYPROCESS_TASK_PRIO 	9
//任务堆栈大小	
#define KEYPROCESS_STK_SIZE 	128
//任务控制块
OS_TCB Keyprocess_TaskTCB;
//任务堆栈	
CPU_STK KEYPROCESS_TASK_STK[KEYPROCESS_STK_SIZE];
//任务函数
void Keyprocess_task(void *p_arg);
/********************************************************/
//任务优先级
#define MSGDIS_TASK_PRIO	7
//任务堆栈
#define MSGDIS_STK_SIZE		256
//任务控制块
OS_TCB	Msgdis_TaskTCB;
//任务堆栈
CPU_STK	MSGDIS_TASK_STK[MSGDIS_STK_SIZE];
//任务函数
void msgdis_task(void *p_arg);
/********************************************************/
////通讯任务
#define COMMUNICATION_TASK_PRIO	5
//任务堆栈大小
#define COMMUNICATION_STK_SIZE		256
//任务控制块
OS_TCB	Communication_TaskTCB;
//任务堆栈
CPU_STK	COMMUNICATION_TASK_STK[COMMUNICATION_STK_SIZE];
//任务函数
void communication_task(void *p_arg); 
/********************************************************/
//任务优先级
#define CALIBRATION_TASK_PRIO 18
//任务堆栈大小
#define CALIBRATION_STK_SIZE 256
//任务控制块
OS_TCB CALIBRATION_TaskTCB;
//任务堆栈
CPU_STK CALIBRATION_TASK_STK[CALIBRATION_STK_SIZE];
//任务函数
void calibration_task(void *p_arg);
/********************************************************/
////////////////////////消息队列//////////////////////////////
#define KEYMSG_Q_NUM	1	//按键消息队列的数量
#define DATAMSG_Q_NUM	4	//发送数据的消息队列的数量
OS_Q KEY_Msg;				    //定义一个消息队列，用于按键消息传递，模拟消息邮箱
OS_Q DATA_Msg;			  	//定义一个消息队列，用于发送数据
						
////////////////////////定时器////////////////////////////////
u8 tmr1sta=0; 	   //标记定时器的工作状态
OS_TMR	tmr1;	    //定义一个定时器
void tmr1_callback(void *p_tmr,void *p_arg); //定时器1回调函数
OS_MUTEX	TEST_MUTEX;		//定义一个互斥信号量						
//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	delay_init();  //时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	USART1_Config();          //串口初始化 (0,2)        
	RS485_Initt(115200);
  eMBInit(MB_RTU, 0x01, 0x01, 115200, MB_PAR_NONE);//初始化 RTU模式 从机地址为1 USART1 9600 无校验 proit :0,1
	eMBEnable();            //启动FreeModbus 
	Data_Update();        //
	I2C_IInit();         //配置IIC
  ADC_CONFIG();        //配置adc1.
  Base_GPIO_Init();    //GPIO初始化
	PID_Init();
  TIM3_PWM_Init(4000,72);
	
//	TIM_SetCompare1(TIM3,1500);//2500,3500
//	 Lyndir_No->Duoji_CONTROL=0x0004;
	OSInit(&err);		    //初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区		
  // GPIO_ResetBits(GPIOB,GPIO_Pin_6);	
		//创建一个互斥信号量
	OSMutexCreate((OS_MUTEX*	)&TEST_MUTEX,
				  (CPU_CHAR*	)"TEST_MUTEX",
                  (OS_ERR*		)&err);
	
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
//	//创建定时器1
//	OSTmrCreate((OS_TMR		*)&tmr1,		//定时器1
//                (CPU_CHAR	*)"tmr1",		//定时器名字
//                (OS_TICK	 )0,			//0ms
//                (OS_TICK	 )100,          //100*10=500ms
//                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
//                (OS_TMR_CALLBACK_PTR)tmr1_callback,//定时器1回调函数
//                (void	    *)0,			//参数为0
//                (OS_ERR	    *)&err);		//返回的错误码
	//创建主任务
	OSTaskCreate((OS_TCB 	* )&Main_TaskTCB,		
				 (CPU_CHAR	* )"Main task", 		
                 (OS_TASK_PTR )main_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MAIN_TASK_PRIO,     
                 (CPU_STK  *)&MAIN_TASK_STK[0],	
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
	//创建MSGDIS任务
	OSTaskCreate((OS_TCB 	* )&Msgdis_TaskTCB,		
				 (CPU_CHAR	* )"Msgdis task", 		
                 (OS_TASK_PTR )msgdis_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MSGDIS_TASK_PRIO,     
                 (CPU_STK   * )&MSGDIS_TASK_STK[0],	
                 (CPU_STK_SIZE)MSGDIS_STK_SIZE/10,	
                 (CPU_STK_SIZE)MSGDIS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
								 
		//创建通讯任务
        	OSTaskCreate((OS_TCB 	* )&Communication_TaskTCB,		
				 (CPU_CHAR	* )"Communication task", 		
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
						//		 OSTmrStart(&tmr1,&err);								 
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}

//定时器1的回调函数
void tmr1_callback(void *p_tmr,void *p_arg)
{
	
//    OS_ERR err;
//   	LD_COL=1;		  
//		delay_ms(1000);   //延时10ms
//    Lyndir_No->PD_Vol=PD_Signal_Sampling(NTC);//获取PD_Vol电压
//	 	LD_COL=0;		
//    delay_ms(1000); 
	 
}

/***********************************************************
void main_task(void *p_arg)
主要任务，获取各项采样值
************************************************************/
void main_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{   	
		OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);
   	 	Read_Concentration_Value();      //读取N0浓度   
      Calibrate_Function();     //LYNDIR_NO 校准机制
		OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);	
		
//		 TIM_SetCompare1(TIM3,1500);//1.5
//    delay_ms(1000);	
//    
//		 TIM_SetCompare1(TIM3,2500);//2.5
//		delay_ms(1000);	
//		
//		 TIM_SetCompare1(TIM3,3500);//3.5
//		delay_ms(1000);		
		
		
	 	  OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //延时10ms	
		 
		
		
	}
}
/***********************************************************
void Keyprocess_task(void *p_arg)
Keyprocess_task 预留任务
************************************************************/
void Keyprocess_task(void *p_arg)
{	
	OS_ERR err;
	int a;
	while(1)
	{
      Battery_Level_MS(NTC);
	  	OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);
		  Read_NOStatus_Value();
   		Read_NOPressure_Value();
			OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);	
		  OSTimeDlyHMSM(0,0,1,100,OS_OPT_TIME_PERIODIC,&err); 
	}
}
/***********************************************************
void msgdis_task(void *p_arg)
//显示消息队列中的消息 预留
************************************************************/
void msgdis_task(void *p_arg)
{
	u8 reclen=0; 
	OS_ERR err; 
	while(1)
	{
		  Data_ConvertedValue();
			NTC_TEM_Resurt(); //测多个传感器温
	    Electrochemical_Oxygen_Acquisition(NTC);//电化学氧采	  
      Lyndir_No->PD_Vol=PD_Signal_Sampling(NTC);//获取PD_Vol电压
			ReadALL_Parameter(); //读取大气压浓度  
		  delay_ms(5); 
		  SM5852_Air_Pressure_Resurt();   //读取大气压差	
		  	
		  OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //延时1s
	}
}	
/***********************************************************
void communication_task(void *p_arg)
//通讯任务，通讯处理
************************************************************/
	void communication_task(void *p_arg)
{
	 OS_ERR err;
		while(1)
	{		
	
	  Switch_Control_Function(); 
  	eMBPoll();
		checkout();
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //延时10ms
	}
}
/***********************************************************
void calibration_task(void *p_arg)
//校准任务，Lyndir_No;
************************************************************/
void calibration_task(void *p_arg)
{
  	OS_ERR err;
	  float step;
	while(1)
	{	 
	 	
		if((Lyndir_No->CHAM_TH4_Tem>50))
		    {
	  	   PID_Start=1;					
				}	
	 if((Lyndir_No->CHAM_TH4_Tem>50)&&(Lyndir_No->CHAM_TH4_Tem<53))
		    {
	  	   PID_Flag=1;					
				}						
    	if((PID_Flag==1)&&(PID_Start==1)&&(PID_Tem_startup==0))               //PID标志
			 {
		     step= PID_Calc();
			   Tem_Control_Tuning(step);				
			 }
			 if(PID_Tem_startup==1)
			 {
			     FANhot_COL=1;
				  PID_Tem_startup=2;
				  Lyndir_No->ADDhot_CONTROL=4;
			 }
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err); 
	}
}

/***********************************************************
 Wifi convert to Modbus
************************************************************/
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
        break;

      //写处理函数 
      case MB_REG_WRITE:
        while( usNRegs > 0 )
        {
          usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
          usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
          iRegIndex++;
          usNRegs--;
        }
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


