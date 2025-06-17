#include "stm32f10x.h"                  // Device header

/**
  *@drief  初始化LED相关的GPIO引脚
  *@param  无
  *@retval 无
  */
void LED_Init(void)
{
	//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	//定义GPIO初始化结构体变量
	GPIO_InitTypeDef GPIO_Initstructure;
	//配置为推挽输出模式，适合驱动LED
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//选择GPIOA的3、4、5、6引脚
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	//初始化将所有LED引脚设置为高电平，LED熄灭
	GPIO_SetBits(GPIOA,GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
}

/**
  *@drief  点亮LED3
  *@param  无
  *@retval 无
  */
void LED3_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);	//将GPIOA.3引脚设置为低电平，点亮LED
}

/**
  *@drief 熄灭LED3
  *@param  无
  *@retval 无
  */
void LED3_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);		//将GPIOA.3引脚设置为高电平，熄灭LED
}

/**
  *@drief  翻转LED3的状态
  *@param  无
  *@retval 无
  */
void LED3_Turn(void)
{
	//读取当前GPIOA.3的输出状态，如果为低电平(LED亮)
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_3)==0)
	{
		//设置为高电平，熄灭LED
		GPIO_SetBits(GPIOA,GPIO_Pin_3);
	}
	else
	{
		//否则设置为低电平，点亮LED
		GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	}
}

/**
  *@drief  点亮LED4
  *@param  无
  *@retval 无
  */
void LED4_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);	//将GPIOA.4引脚设置为低电平，点亮LED
}

/**
  *@drief 熄灭LED4
  *@param  无
  *@retval 无
  */
void LED4_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);		//将GPIOA.4引脚设置为高电平，熄灭LED
}

/**
  *@drief  翻转LED4的状态
  *@param  无
  *@retval 无
  */
void LED4_Turn(void)
{
	//读取当前GPIOA.4的输出状态，如果为低电平(LED亮)
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_4)==0)
	{
		//设置为高电平，熄灭LED
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
	}
	else
	{
		//否则设置为低电平，点亮LED
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	}
}

/**
  *@drief  点亮LED5
  *@param  无
  *@retval 无
  */
void LED5_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);	//将GPIOA.5引脚设置为低电平，点亮LED
}

/**
  *@drief 熄灭LED4
  *@param  无
  *@retval 无
  */
void LED5_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_5);		//将GPIOA.5引脚设置为高电平，熄灭LED
}

/**
  *@drief  翻转LED5的状态
  *@param  无
  *@retval 无
  */
void LED5_Turn(void)
{
	//读取当前GPIOA.5的输出状态，如果为低电平(LED亮)
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_5)==0)
	{
		//设置为高电平，熄灭LED
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
	}
	else
	{
		//否则设置为低电平，点亮LED
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	}
}

/**
  *@drief  点亮LED6
  *@param  无
  *@retval 无
  */
void LED6_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//将GPIOA.6引脚设置为低电平，点亮LED
}

/**
  *@drief 熄灭LED4
  *@param  无
  *@retval 无
  */
void LED6_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_6);		//将GPIOA.5引脚设置为高电平，熄灭LED
}

/**
  *@drief  翻转LED6的状态
  *@param  无
  *@retval 无
  */
void LED6_Turn(void)
{
	//读取当前GPIOA.6的输出状态，如果为低电平(LED亮)
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_6)==0)
	{
		//设置为高电平，熄灭LED
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
	}
	else
	{
		//否则设置为低电平，点亮LED
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	}
}
