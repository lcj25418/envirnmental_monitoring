#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

//全局变量声明
extern char Serial_RxPacket[];		//接收数据包缓冲区
extern uint8_t Serial_RxFlag;       //接收完成标志位（1：接收完成，2：未完成）

void Serial_Init(void);										//串口初始化
void Serial_SendByte(uint8_t Byte);							//发送单个字节
void Serial_SendArray(uint8_t *Array, uint16_t Length);		//发送字节数组
void Serial_SendString(char *String);						//发送字符串
uint32_t Serial_Pow(uint32_t X, uint32_t Y);				//计算x的y次方
void Serial_SendNumber(uint32_t Number, uint8_t Length);	//发送数字

int fputc(int ch, FILE *f);					//重定向printf函数
void Serial_Printf(char *format, ...);		//格式化输出函数
void USART1_IRQHandler(void);				//USART1中断服务函数

#endif
