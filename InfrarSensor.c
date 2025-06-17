#include "stm32f10x.h"                  // Device header

/**
  *@drief  红外传感器初始化函数
  *@param  无
  *@retval 无
  */
void InfrarSensor_Init(void)
{
	//使能外设GPIOC的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	//GPIO初始化，配置Pin13为上拉输入
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

/**
  *@drief  检测红外传感器是否检测到物体
  *@param  无
  *@retval uint8_t — 返回1表示检测到物体(传感器输出低电平)
					 返回0表示未检测到物体(传感器输出高电平)
  */
uint8_t Infrar_Detected(void)
{
	//读取GPIOC的第13号引脚状态，低电平表示检测到物体
	return (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 0);
}
