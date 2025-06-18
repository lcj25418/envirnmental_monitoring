#include "stm32f10x.h"                  // Device header

/**
  *@brief	电机驱动初始化函数
  *@param	无
  *@retval	无
  */
void MotorDrive_Init(void)
{
	//使能GPIOA、GPIOB和TIM1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	//GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	//配置GPIOA Pin8为复用推挽输出模式
	//PWM信号将通过此引脚输出到电机驱动电路
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//配置GPIOB Pin13和Pin14为通用推挽输出模式(控制电机方向)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//定时器TIM1初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseInitStruct.TIM_Period = 100-1;						//自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 720-1;					//预分频值
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				//重复计数器
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	
	//PWM输出配置
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);							//重置PWM配置结构体为默认值
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//PWM模式1：CNT<CCR时输出有效电平
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//高电平有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始化占空比为0（电机停止）
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM1,ENABLE);					//使能TIM1计数器
	TIM_CtrlPWMOutputs(TIM1,ENABLE);		//使能高级定时器的主输出
}
/**
  *@brief	设置电机转速
  *@param	Compare (0~99),对应占空比0%~99%
  *@retval	无
  */
void Motor_Speed(uint8_t Compare)
{
	TIM_SetCompare1(TIM1,Compare);			//设置TIM1通道1的比较值，决定PWM占空比
}

/**
  *@brief	启动电机并设置为正向旋转
  *@param	无
  *@retval	无
**/
void Motor_Start(void)
{
	//设置电机方向控制引脚：正向旋转
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
}

/**
  *@brief	停止电机
  *@param 	无
  *@retval	无
**/
void Motor_Stop(void)
{
	//设置电机方向控制引脚：停止状态
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
}
