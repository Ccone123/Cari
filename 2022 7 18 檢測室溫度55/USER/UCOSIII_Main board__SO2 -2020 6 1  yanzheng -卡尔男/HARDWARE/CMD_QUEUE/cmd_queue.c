#include "cmd_queue.h"


#define CMD_HEAD 0XEE                      //帧头
#define CMD_TAIL 0XFFFCFFFF                //帧尾


typedef struct _QUEUE                                             
{                                                                 
    u16 _head;                              //队列头
    u16 _tail;                              //队列尾
    u8 _data[QUEUE_MAX_SIZE];               //队列数据缓存区
}QUEUE;  

static QUEUE que = {0,0,0};                 //指令队列
static u32 cmd_state = 0;                   //队列帧尾检测状态
static u16 cmd_pos = 0;                     //当前指令指针位置

/*! 
*  \brief  清空指令数据
*/
void queue_reset()
{
    que._head = que._tail = 0;
    cmd_pos = cmd_state = 0;
}
/*! 
* \brief  添加指令数据
* \detial 串口接收的数据，通过此函数放入指令队列 
*  \param  _data 指令数据
*/
void queue_push(u8 _data)
{
	u16 pos=(que._head+1)%QUEUE_MAX_SIZE; 
	if(pos!=que._tail)
	{
		que._data[que._head]=_data;
		que._head=pos;
		
	}
}
//从队列中取一个数据
static void queue_pop(u8 * _data)
{
	if(que._tail!=que._head)
	{
		*_data=que._data[que._tail];
		que._tail=(que._tail+1)%QUEUE_MAX_SIZE;
	}
}
//获取队列中有效数据个数
static u16 queue_size()
{
	return ((que._head+QUEUE_MAX_SIZE-que._tail)%QUEUE_MAX_SIZE);
}
/*! 
*  \brief  从指令队列中取出一条完整的指令
*  \param  cmd 指令接收缓存区
*  \param  buf_len 指令接收缓存区大小
*  \return  指令长度，0表示队列中无完整指令
*/
u16 queue_find_cmd(u8 *buffer,u16 buf_len)
{
	u16 cmd_size = 0;
    u8 _data = 0;
	while (queue_size()>0)
	{
		//取一个数据
		queue_pop(&_data);
	 if(cmd_pos==0&&_data!=CMD_HEAD)//指令第一个字节必须时帧头，否则跳过
        {
				   continue;
				}
	  if(cmd_pos<buf_len) //防止缓冲区溢出
			buffer[cmd_pos++]=_data;
		
		cmd_state=((cmd_state<<8)|_data);//拼接最后4个字节，组成一个32位整数
		 //最后4个字节与帧尾匹配，得到完整帧
		if(cmd_state ==CMD_TAIL)
		{
			cmd_size=cmd_pos;
			cmd_state=0;
			cmd_pos=0;
			return cmd_size;
			
		}
	}
		return 0; //没有形成完整的一帧
}



