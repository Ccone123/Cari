#ifndef _CMD_QUEUE
#define _CMD_QUEUE
#include "sys.h"
#define FIRMWARE_VER 921          // ���ִ˹̼��汾������ʵ��Ļһ�£�ȷ���������ܿ���
#define CRC16_ENABLE 0            // �����ҪCRC16У�鹦�ܣ��޸Ĵ˺�Ϊ1(��ʱ��Ҫ��VisualTFT��������CRCУ��)
#define CMD_MAX_SIZE 64           // ����ָ���С��������Ҫ�������������ô�һЩ
#define QUEUE_MAX_SIZE 512        // ָ����ջ�������С��������Ҫ�������������ô�һЩ

/*! 
*  \brief  ���ָ������
*/
extern void queue_reset(void);

/*! 
* \brief  ���ָ������
* \detial ���ڽ��յ����ݣ�ͨ���˺�������ָ����� 
*  \param  _data ָ������
*/
extern void queue_push(u8 _data);

/*! 
*  \brief  ��ָ�������ȡ��һ��������ָ��
*  \param  cmd ָ����ջ�����
*  \param  buf_len ָ����ջ�������С
*  \return  ָ��ȣ�0��ʾ������������ָ��
*/
extern u16 queue_find_cmd(u8 *cmd,u16 buf_len);




#endif

