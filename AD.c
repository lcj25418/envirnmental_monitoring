#include "stm32f10x.h"                  // Device header
#include <math.h>

uint16_t AD_Value[2];		//存储两个通道ADC转换结果的数组

/**
  *@brief  初始化ADC及DMA，配置ADC1的通道0和通道1进行连续扫描转换
  *@param  无
  *@retval 无
  */
void AD_Init(void)
{
	//使能ADC1、GPIOA、DMA1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	//配置ADC时钟为PCLK2的6分频，ADC最大时钟为14MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//配置GPIOA的引脚0和1为模拟输入模式
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//配置ADC规则通道
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_55Cycles5);
	
	//配置ADC工作模式
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						//连续转换模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;							//扫描模式(多通道)
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		//软件触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					//数据右对齐
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						//独立模式(单ADC)
	ADC_InitStructure.ADC_NbrOfChannel = 2;									//转换通道数目
	ADC_Init(ADC1,&ADC_InitStructure);
	
	//配置DMA传输
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;					//ADC数据寄存器地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;		//半字(16位)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设地址不变
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;						//内存地址(存储ADC结果)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;				//半字(16位)
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							//内存地址递增
	DMA_InitStructure.DMA_BufferSize = 2;											//缓冲区大小(两个通道)
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;								//从外设到内存
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;									//非内存到内存模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;									//循环模式(持续转换)
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;								//高优先级
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	
	//使能ADC、ADC的DMA请求和DMA通道
	ADC_Cmd(ADC1,ENABLE);		
	ADC_DMACmd(ADC1,ENABLE);		
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	//执行ADC校准
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);		//等待校准复位完成
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);			//等待校准完成
	
	//启动ADC软件转换
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

/**
  *@drief  根据ADC值计算光照百分比
  *@param  adc_value：ADC采样值(0~4095)
  *@retval 光照百分比(0~100%)
  */
uint8_t CalculateLightPercentage(uint16_t adc_value) 
{
	
    uint16_t dark_adc = 3000;		//暗环境下ADC的值
    uint16_t bright_adc = 300;		//亮环境下ADC的值
    
	//超出范围处理
    if (adc_value >= dark_adc) return 0;		//比暗环境海暗，返回0%
    if (adc_value <= bright_adc) return 100;	//比亮环境还亮，返回100%
    return (uint8_t)((adc_value - dark_adc) * 100.0f / (bright_adc - dark_adc));	//线性映射计算光照百分比
}

/**
  *@drief  根据ADC值计算实际温度值
  *@param  adc_value：ADC采样值(0~4095)
  *@retval 测量实际温度值
  */
//由于热敏电阻传感器温度测量不准，这个暂时不使用！！！
//float CalculateTemperature(uint16_t adc_value) 
//	{
////    float voltage = adc_value * 3.3f / 4096.0f;
////    float Rt = 10000.0f * (3.3f / voltage - 1.0f);
////    
////    float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
////    float log_Rt = log(Rt);
////    float T_K = 1.0f / (A + B*log_Rt + C*pow(log_Rt, 3));
////    return T_K - 273.15f;
//	const float m = 0.112;    // ??
//    const float b = -40.5;    // ??
//    
//    return (float)adc_value * m + b;
//}
