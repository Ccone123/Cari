#ifndef _CMD_QUEUE
#define _CMD_QUEUE
#include "sys.h"
#define FIRMWARE_VER 921          // 保持此固件版本号与真实屏幕一致，确保新增功能可用
#define CRC16_ENABLE 0            // 如果需要CRC16校验功能，修改此宏为1(此时需要在VisualTFT工程中配CRC校验)
#define CMD_MAX_SIZE 64           // 单条指令大小，根据需要调整，尽量设置大一些
#define QUEUE_MAX_SIZE 512        // 指令接收缓冲区大小，根据需要调整，尽量设置大一些

/*! 
*  \brief  清空指令数据
*/
extern void queue_reset(void);

/*! 
* \brief  添加指令数据
* \detial 串口接收的数据，通过此函数放入指令队列 
*  \param  _data 指令数据
*/
extern void queue_push(u8 _data);

/*! 
*  \brief  从指令队列中取出一条完整的指令
*  \param  cmd 指令接收缓存区
*  \param  buf_len 指令接收缓存区大小
*  \return  指令长度，0表示队列中无完整指令
*/
extern u16 queue_find_cmd(u8 *cmd,u16 buf_len);




#endif

