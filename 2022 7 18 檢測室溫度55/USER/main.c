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
//����Ĵ�����ʼ��ַ
#define REG_INPUT_START       0x0000
//����Ĵ�������
#define REG_INPUT_NREGS       100
//���ּĴ�����ʼ��ַ
#define REG_HOLDING_START     0x0000
//���ּĴ�������
#define REG_HOLDING_NREGS     100
//��Ȧ��ʼ��ַ
#define REG_COILS_START       0x0000
//��Ȧ����
#define REG_COILS_SIZE        16

//���ؼĴ�����ʼ��ַ
#define REG_DISCRETE_START    0x0000
//���ؼĴ�������
#define REG_DISCRETE_SIZE     16
/* Private variables ---------------------------------------------------------*/
//����Ĵ�������
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0};
//�Ĵ�����ʼ��ַ
uint16_t usRegInputStart = REG_INPUT_START;
//���ּĴ�������
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0};
//���ּĴ�����ʼ��ַ
uint16_t usRegHoldingStart = REG_HOLDING_START;
//��Ȧ״̬
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x00,0x00};
//��������״̬
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x00,0x00};
/********************************************************/
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
/********************************************************/
//�������ȼ�
#define MAIN_TASK_PRIO		6
//�����ջ��С	
#define MAIN_STK_SIZE 		256
//������ƿ�
OS_TCB Main_TaskTCB;
//�����ջ	
CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *p_arg);
/********************************************************/
//�������ȼ�
#define KEYPROCESS_TASK_PRIO 	9
//�����ջ��С	
#define KEYPROCESS_STK_SIZE 	128
//������ƿ�
OS_TCB Keyprocess_TaskTCB;
//�����ջ	
CPU_STK KEYPROCESS_TASK_STK[KEYPROCESS_STK_SIZE];
//������
void Keyprocess_task(void *p_arg);
/********************************************************/
//�������ȼ�
#define MSGDIS_TASK_PRIO	7
//�����ջ
#define MSGDIS_STK_SIZE		256
//������ƿ�
OS_TCB	Msgdis_TaskTCB;
//�����ջ
CPU_STK	MSGDIS_TASK_STK[MSGDIS_STK_SIZE];
//������
void msgdis_task(void *p_arg);
/********************************************************/
////ͨѶ����
#define COMMUNICATION_TASK_PRIO	5
//�����ջ��С
#define COMMUNICATION_STK_SIZE		256
//������ƿ�
OS_TCB	Communication_TaskTCB;
//�����ջ
CPU_STK	COMMUNICATION_TASK_STK[COMMUNICATION_STK_SIZE];
//������
void communication_task(void *p_arg); 
/********************************************************/
//�������ȼ�
#define CALIBRATION_TASK_PRIO 18
//�����ջ��С
#define CALIBRATION_STK_SIZE 256
//������ƿ�
OS_TCB CALIBRATION_TaskTCB;
//�����ջ
CPU_STK CALIBRATION_TASK_STK[CALIBRATION_STK_SIZE];
//������
void calibration_task(void *p_arg);
/********************************************************/
////////////////////////��Ϣ����//////////////////////////////
#define KEYMSG_Q_NUM	1	//������Ϣ���е�����
#define DATAMSG_Q_NUM	4	//�������ݵ���Ϣ���е�����
OS_Q KEY_Msg;				    //����һ����Ϣ���У����ڰ�����Ϣ���ݣ�ģ����Ϣ����
OS_Q DATA_Msg;			  	//����һ����Ϣ���У����ڷ�������
						
////////////////////////��ʱ��////////////////////////////////
u8 tmr1sta=0; 	   //��Ƕ�ʱ���Ĺ���״̬
OS_TMR	tmr1;	    //����һ����ʱ��
void tmr1_callback(void *p_tmr,void *p_arg); //��ʱ��1�ص�����
OS_MUTEX	TEST_MUTEX;		//����һ�������ź���						
//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	delay_init();  //ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	USART1_Config();          //���ڳ�ʼ�� (0,2)        
	RS485_Initt(115200);
  eMBInit(MB_RTU, 0x01, 0x01, 115200, MB_PAR_NONE);//��ʼ�� RTUģʽ �ӻ���ַΪ1 USART1 9600 ��У�� proit :0,1
	eMBEnable();            //����FreeModbus 
	Data_Update();        //
	I2C_IInit();         //����IIC
  ADC_CONFIG();        //����adc1.
  Base_GPIO_Init();    //GPIO��ʼ��
	PID_Init();
  TIM3_PWM_Init(4000,72);
	
//	TIM_SetCompare1(TIM3,1500);//2500,3500
//	 Lyndir_No->Duoji_CONTROL=0x0004;
	OSInit(&err);		    //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���		
  // GPIO_ResetBits(GPIOB,GPIO_Pin_6);	
		//����һ�������ź���
	OSMutexCreate((OS_MUTEX*	)&TEST_MUTEX,
				  (CPU_CHAR*	)"TEST_MUTEX",
                  (OS_ERR*		)&err);
	
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
//	//������ʱ��1
//	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
//                (CPU_CHAR	*)"tmr1",		//��ʱ������
//                (OS_TICK	 )0,			//0ms
//                (OS_TICK	 )100,          //100*10=500ms
//                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
//                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
//                (void	    *)0,			//����Ϊ0
//                (OS_ERR	    *)&err);		//���صĴ�����
	//����������
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
	//����MSGDIS����
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
								 
		//����ͨѶ����
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
						//		 OSTmrStart(&tmr1,&err);								 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}

//��ʱ��1�Ļص�����
void tmr1_callback(void *p_tmr,void *p_arg)
{
	
//    OS_ERR err;
//   	LD_COL=1;		  
//		delay_ms(1000);   //��ʱ10ms
//    Lyndir_No->PD_Vol=PD_Signal_Sampling(NTC);//��ȡPD_Vol��ѹ
//	 	LD_COL=0;		
//    delay_ms(1000); 
	 
}

/***********************************************************
void main_task(void *p_arg)
��Ҫ���񣬻�ȡ�������ֵ
************************************************************/
void main_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{   	
		OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);
   	 	Read_Concentration_Value();      //��ȡN0Ũ��   
      Calibrate_Function();     //LYNDIR_NO У׼����
		OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);	
		
//		 TIM_SetCompare1(TIM3,1500);//1.5
//    delay_ms(1000);	
//    
//		 TIM_SetCompare1(TIM3,2500);//2.5
//		delay_ms(1000);	
//		
//		 TIM_SetCompare1(TIM3,3500);//3.5
//		delay_ms(1000);		
		
		
	 	  OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms	
		 
		
		
	}
}
/***********************************************************
void Keyprocess_task(void *p_arg)
Keyprocess_task Ԥ������
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
//��ʾ��Ϣ�����е���Ϣ Ԥ��
************************************************************/
void msgdis_task(void *p_arg)
{
	u8 reclen=0; 
	OS_ERR err; 
	while(1)
	{
		  Data_ConvertedValue();
			NTC_TEM_Resurt(); //������������
	    Electrochemical_Oxygen_Acquisition(NTC);//�绯ѧ����	  
      Lyndir_No->PD_Vol=PD_Signal_Sampling(NTC);//��ȡPD_Vol��ѹ
			ReadALL_Parameter(); //��ȡ����ѹŨ��  
		  delay_ms(5); 
		  SM5852_Air_Pressure_Resurt();   //��ȡ����ѹ��	
		  	
		  OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //��ʱ1s
	}
}	
/***********************************************************
void communication_task(void *p_arg)
//ͨѶ����ͨѶ����
************************************************************/
	void communication_task(void *p_arg)
{
	 OS_ERR err;
		while(1)
	{		
	
	  Switch_Control_Function(); 
  	eMBPoll();
		checkout();
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err); //��ʱ10ms
	}
}
/***********************************************************
void calibration_task(void *p_arg)
//У׼����Lyndir_No;
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
    	if((PID_Flag==1)&&(PID_Start==1)&&(PID_Tem_startup==0))               //PID��־
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
        break;

      //д������ 
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


