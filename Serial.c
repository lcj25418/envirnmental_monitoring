#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

//全局变量定义
char Serial_RxPacket[100];		//接收数据包缓冲区
uint8_t Serial_RxFlag;			//接收完成标志位（1：接收完成，2：未完成）

/**
  *@brief	串口初始化函数
  *@param	无
  *@retval	无
**/
void Serial_Init(void)
{
	//使能外设GPIOA、USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	//配置USART1 TX引脚(PA9)为复用推挽输出
	GPIO_InitTypeDef GPIO_InitStructure;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//配置USART1 RX引脚(PA10)为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//配置USART1参数
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;											//波特率9600
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//收发模式
	USART_InitStructure.USART_Parity = USART_Parity_No;									//无校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;								//1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;							//8位数据位
	USART_Init(USART1,&USART_InitStructure);
	
	//使能USART1接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	//配置NVIC中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;				//USART1中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//子优先级1
	NVIC_Init(&NVIC_InitStructure);
	
	//使能USART1
	USART_Cmd(USART1,ENABLE);
}

/**
  *@brief	串口发送一个字节
  *@param	Byte：要发送的字节
  *@retval	无
**/
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);									//发送一个字节
	while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);	//等待发送完成
}

/**
  *@brief	串口发送数组
  *@param	Array：要发送的数组
  *@param	Length：数组长度
  *@retval	无
**/
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);		//逐个字节发送
	}
}

/**
  *@brief	串口发送字符串
  *@param	String：要发送的字符串(以'\0'结尾)
  *@retval	无
  */
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);			//逐个字符发送
	}
}

/**
  *@brief  计算x的y次方
  *@param  x：底数
  *@param  y：指数
  *@retval 计算结果
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;		//累乘计算幂次方
	}
	return Result;
}

/**
  *@drief  串口发送数字
  *@param  Number：要发送的数字
  *@param  Length：数字的位数
  *@retval 无
  */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		//从高位到低位依次发送每一位数字的ASCII码
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  *@brief  重定向printf函数到串口
  *@param  ch：要输出的字符
  *@param  f：文件指针
  *@retval 字符
  */
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);		//发送字符
	return ch;
}

/**
  *@drief  串口格式化输出函数
  *@param  foramt：格式化字符串
  *@param  ...:可变参数列表
  *@retval 无
  */
void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;						
	va_start(arg, format);				//初始化可变参数列表
	vsprintf(String, format, arg);		//格式化字符串
	va_end(arg);						//结束可变参数列表
	Serial_SendString(String);			//发送格式化后的字符串
}

/**
  *@drief  USART1中断函数
  *@param  无
  *@retval 无
  */
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;					//接收状态机状态
	static uint8_t pRxPaxket = 0;				//接收缓冲区指针
	//判断是否接收到数据
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);  //读取接收到的数据
		
		//状态机处理接收数据
		if(RxState == 0)
		{
			//等待帧头'@',且接收标志为0
			if(RxData =='@' && Serial_RxFlag == 0)
			{
				RxState = 1;			//切换到接收数据状态
				pRxPaxket = 0;			//重置缓冲区指针
			}
		}
		else if(RxState ==1)
		{
			//接收数据状态
			if(RxData == '\r')			//遇到回车符
			{
				RxState = 2;			//切换到等待换行符状态
			}
			else
			{
				Serial_RxPacket[pRxPaxket] = RxData;	//存储接收到的数据
				pRxPaxket++;							//缓冲区指针后移
			}
		}
		else if(RxState == 2)
		{
			//等待换行符状态
			if(RxData == '\n')							//遇到换行符
			{
				RxState = 0;							//重置状态机
				Serial_RxPacket[pRxPaxket] = '\0';		//添加字符串结束符
				Serial_RxFlag = 1;						//设置接收完成标志
			}
		}
		//消除中断标志位
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

