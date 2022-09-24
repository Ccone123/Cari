#ifndef __COMMUNICATION_H_	
#define __COMMUNICATION_H_
#include "sys.h"
extern char switch_status;
#define NOTIFY_TOUCH_PRESS         0X01  //触摸屏按下通知
#define NOTIFY_TOUCH_RELEASE       0X03  //触摸屏松开通知
#define NOTIFY_WRITE_FLASH_OK      0X0C  //写FLASH成功
#define NOTIFY_WRITE_FLASH_FAILD   0X0D  //写FLASH失败
#define NOTIFY_READ_FLASH_OK       0X0B  //读FLASH成功
#define NOTIFY_READ_FLASH_FAILD    0X0F  //读FLASH失败
#define NOTIFY_MENU                0X14  //菜单事件通知
#define NOTIFY_TIMER               0X43  //定时器超时通知
#define NOTIFY_CONTROL             0XB1  //控件更新通知
#define NOTIFY_READ_RTC            0XF7  //读取RTC时间
#define MSG_GET_CURRENT_SCREEN     0X01  //画面ID变化通知
#define MSG_GET_DATA               0X11  //控件数据通知
#define NOTIFY_HandShake           0X55  //握手通知
extern const char endbuf[4];
#define END_CMD() USART3_SendString((char *)endbuf,4)

enum CtrlType
{
    kCtrlUnknown=0x0,
    kCtrlButton=0x10,                     //按钮
    kCtrlText,                            //文本
    kCtrlProgress,                        //进度条
    kCtrlSlider,                          //滑动条
    kCtrlMeter,                            //仪表
    kCtrlDropList,                        //下拉列表
    kCtrlAnimation,                       //动画
    kCtrlRTC,                             //时间显示
    kCtrlGraph,                           //曲线图控件
    kCtrlTable,                           //表格控件
    kCtrlMenu,                            //菜单控件
    kCtrlSelector,                        //选择控件
    kCtrlQRCode,                          //二维码
};
#pragma pack(push)
#pragma pack(1) 
typedef struct
{
    u8    cmd_head;                    //帧头

    u8    cmd_type;                    //命令类型(UPDATE_CONTROL)    
    u8    ctrl_msg; 
                                   	//CtrlMsgType-指示消息的类型
    u16  screen_id;                   //产生消息的画面ID
    u16   control_id;                  //产生消息的控件ID
    u8    control_type;                //控件类型

    u8    param[256];                  //可变长度参数，最多256个字节

    u8  cmd_tail[4];                   //帧尾
}CTRL_MSG,*PCTRL_MSG;
#pragma  apop;

 typedef struct
   {
    
		 u16    WARNVRE_Input_Flag; //基准输入标志10
		 
		 float  CRAteTEM_UpperLimit;//机箱温度上下限
		 float  CRAteTEM_LowerLimit;
		 float  TECTEM_UpperLimit;  //PMT温度上下限
		 float  TECTEM_LowerLimit;
		 float  CaseTEM_UpperLimit; //反应室温度上下限
		 float  CaseTEM_LowerLimit;
		 float  SampGasP_UpperLimit; //采样压力
		 float  SampGasP_LowerLimit;
		 float  CaseGasPRESSURE_UpperLimit;//反应室压力
		 float  CaseGasPRESSURE_LowerLimit;
		 float  GASFLOW_UpperLimit;  //气体流量
		 float  GASFLOW_LowerLimit;
		 
		 float  PMTSampVL_UpperLimit;//测量光强
		 float  PMTSampVL_LowerLimit;
		 float  REFVOL_PD_UpperLimit;//参考光强
		 float  REFVOL_PD_LowerLimit;
		 float  REFVOL_PMT_UpperLimit;//参考暗电流
		 float  REFVOL_PMT_LowerLimit;
		 float   VEF_UpperLimit;      //基准光源
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
