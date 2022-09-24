#include "delay.h" 			 
#include "usart.h" 			 
#include "usart3.h" 			 
#include "hc05.h" 
#include "led.h" 
#include "string.h"	 
#include "math.h"

u8 HC05_Init(void)
{
	u8 retry=10,t;
	u8 temp=1;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化A15
	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA4

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	HC05_KEY=1;
	HC05_LED=1; 
	usart3_init(9600);	//初始化串口2为:9600,波特率.
	while(retry--)
	{
		HC05_KEY=1;
		delay_ms(10);
		u3_printf("AT\r\n");		//发送AT测试指令
		HC05_KEY=0;		
			for(t=0;t<20;t++) 			//最长等待100ms,来接收HC05模块的回应
		{
			if(USART3_RX_STA&0X8000)break;
			delay_ms(5);
		}	
		if(USART3_RX_STA&0X8000)
		{
			temp=USART3_RX_STA&0X7FFF;	//得到数据长度
			USART3_RX_STA=0;		
		  		if(temp==4&&USART3_RX_BUF[0]=='O'&&USART3_RX_BUF[1]=='K')//接收到正确的应答了
			{			
				temp=0;
				break;			 
			}
		}
	}
	if(retry==0)temp=1;	//检测失败
	return temp;	
}
//h获取ATK-HC05模块的角色
//返回值：0，从机；1，主机；0XFF,获取失败
u8 HC05_Get_Role(void)
{	
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY=1;
		delay_ms(10);
		u3_printf("AT+ROLE?\r\n");
		for(t=0;t<20;t++)
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)break;
		}
		HC05_KEY=0; //KEYl拉低，推出AT模式
		if(USART3_RX_STA&0X8000)//接收到一次数据了
		{
			temp=USART3_RX_STA&0x7FFF;
			USART3_RX_STA=0;
		if(temp==13&&USART3_RX_BUF[0]=='+')
		{
			temp=USART3_RX_BUF[6]-'0';//得到主从模式值
			break;
		}			
		}	
	}
	if(retry==0)temp=0xff;//查询失败
	    return temp;
}
//ATK-HC05设置命令。
//此函数用于设置ATK-HC05,适用于仅返回OK应答的ZT指令
//atstr:AT指令串.比如:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"等字符串
//返回值:0,设置成功;其他,设置失败.	
u8 HC05_Set_Cmd(u8 * atstr)
{
	u8 retry=0x0f;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY=1;
		delay_ms(10);
		u3_printf("%s\r\n",atstr);
		HC05_KEY=0;
		for(t=0;t<20;t++)
		{
			if(USART3_RX_STA&0x8000)break;
			delay_ms(5);
		}
			if(USART3_RX_STA&0X8000)	//接收到一次数据了
		{
			temp=USART3_RX_STA&0X7FFF;// 得到数据长度
			USART3_RX_STA=0;
			if(temp==4&&USART3_RX_BUF[0]=='O')//接收到正确的应答了
			{			
				temp=0;
				break;			 
			}
		}
	}
		if(retry==0)temp=0XFF;//设置失败.
	     return temp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//通过该函数，可以利用USMART,调试接在串口3上的ATK-HC05m模块
//str:命令串.(这里注意不再需要再输入回车符)
void HC05_CFG_CMD(u8 *str)
{
	u8 temp;
	u8 t;
	HC05_KEY=1;//KEY置高
		delay_ms(10);
	u3_printf("%s\r\n",(char*)str); //发送指令
	for(t=0;t<50;t++)
	 {
		 if(USART3_RX_STA&0X8000)break;
		 delay_ms(10);
	 }
	 HC05_KEY=0;
	 if(USART3_RX_STA&0X8000)//j接受到一次数据了
	 {
		 temp=USART3_RX_STA&0X7FFF;
		 USART3_RX_STA=0;
		 USART3_RX_BUF[temp]=0;//加结束符
		 printf("\r\n%s",USART3_RX_BUF);
	 }
	
}

