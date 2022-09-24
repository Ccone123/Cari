#ifndef __COMMUNICATION_H_	
#define __COMMUNICATION_H_
#include "sys.h"
extern char switch_status;
#define NOTIFY_TOUCH_PRESS         0X01  //����������֪ͨ
#define NOTIFY_TOUCH_RELEASE       0X03  //�������ɿ�֪ͨ
#define NOTIFY_WRITE_FLASH_OK      0X0C  //дFLASH�ɹ�
#define NOTIFY_WRITE_FLASH_FAILD   0X0D  //дFLASHʧ��
#define NOTIFY_READ_FLASH_OK       0X0B  //��FLASH�ɹ�
#define NOTIFY_READ_FLASH_FAILD    0X0F  //��FLASHʧ��
#define NOTIFY_MENU                0X14  //�˵��¼�֪ͨ
#define NOTIFY_TIMER               0X43  //��ʱ����ʱ֪ͨ
#define NOTIFY_CONTROL             0XB1  //�ؼ�����֪ͨ
#define NOTIFY_READ_RTC            0XF7  //��ȡRTCʱ��
#define MSG_GET_CURRENT_SCREEN     0X01  //����ID�仯֪ͨ
#define MSG_GET_DATA               0X11  //�ؼ�����֪ͨ
#define NOTIFY_HandShake           0X55  //����֪ͨ
extern const char endbuf[4];
#define END_CMD() USART3_SendString((char *)endbuf,4)

enum CtrlType
{
    kCtrlUnknown=0x0,
    kCtrlButton=0x10,                     //��ť
    kCtrlText,                            //�ı�
    kCtrlProgress,                        //������
    kCtrlSlider,                          //������
    kCtrlMeter,                            //�Ǳ�
    kCtrlDropList,                        //�����б�
    kCtrlAnimation,                       //����
    kCtrlRTC,                             //ʱ����ʾ
    kCtrlGraph,                           //����ͼ�ؼ�
    kCtrlTable,                           //���ؼ�
    kCtrlMenu,                            //�˵��ؼ�
    kCtrlSelector,                        //ѡ��ؼ�
    kCtrlQRCode,                          //��ά��
};
#pragma pack(push)
#pragma pack(1) 
typedef struct
{
    u8    cmd_head;                    //֡ͷ

    u8    cmd_type;                    //��������(UPDATE_CONTROL)    
    u8    ctrl_msg; 
                                   	//CtrlMsgType-ָʾ��Ϣ������
    u16  screen_id;                   //������Ϣ�Ļ���ID
    u16   control_id;                  //������Ϣ�Ŀؼ�ID
    u8    control_type;                //�ؼ�����

    u8    param[256];                  //�ɱ䳤�Ȳ��������256���ֽ�

    u8  cmd_tail[4];                   //֡β
}CTRL_MSG,*PCTRL_MSG;
#pragma  apop;

 typedef struct
   {
    
		 u16    WARNVRE_Input_Flag; //��׼�����־10
		 
		 float  CRAteTEM_UpperLimit;//�����¶�������
		 float  CRAteTEM_LowerLimit;
		 float  TECTEM_UpperLimit;  //PMT�¶�������
		 float  TECTEM_LowerLimit;
		 float  CaseTEM_UpperLimit; //��Ӧ���¶�������
		 float  CaseTEM_LowerLimit;
		 float  SampGasP_UpperLimit; //����ѹ��
		 float  SampGasP_LowerLimit;
		 float  CaseGasPRESSURE_UpperLimit;//��Ӧ��ѹ��
		 float  CaseGasPRESSURE_LowerLimit;
		 float  GASFLOW_UpperLimit;  //��������
		 float  GASFLOW_LowerLimit;
		 
		 float  PMTSampVL_UpperLimit;//������ǿ
		 float  PMTSampVL_LowerLimit;
		 float  REFVOL_PD_UpperLimit;//�ο���ǿ
		 float  REFVOL_PD_LowerLimit;
		 float  REFVOL_PMT_UpperLimit;//�ο�������
		 float  REFVOL_PMT_LowerLimit;
		 float   VEF_UpperLimit;      //��׼��Դ
		 float   VEF_LowerLimit;
   }input; 
extern input Input;
extern u32 Test_Catagry;


void communication (void);
void ProcessMessage(PCTRL_MSG msg ,u16 size);
void NotifyReadRTC(u8 year,u8 month,u8 week,u8 day,u8 hour,u8 minute,u8 second);
void NotifyScreen(u16 screen_id);
void NotifyButton(u16 screen_id , u16 control_id ,u8 state);
void NotifyText(u16 screen_id, u16 control_id, u8 *str);
void NotifyProgress(u16 screen_id, u16 control_id, u32 value) ;
void NotifySlider(u16 screen_id, u16 control_id, u32 value) ;
void NotifyMeter(u16 screen_id, u16 control_id, u32 value);
void NotifyMenu(u16 screen_id, u16 control_id, u8 item, u8 state);
void NotifySelector(u16 screen_id, u16 control_id, u8  item);
void NotifyTimer(u16 screen_id, u16 control_id);
void NOTIFYHandShake(void);

void  KuaiMen_Switch(u8 kuaimen_state ,u8 caiyangjiaozhunfa_state,u8 PMT_state,u8 control_category );
void EEprom_Record(void);
void  Time_Record (void);
void Delete_RecorD(void);

#endif
