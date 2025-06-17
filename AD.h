#ifndef __AD_H
#define __AD_H

extern uint16_t AD_Value[2];		//存储ADC采样值的数组

void AD_Init(void);					//ADC和DMA初始化函数

uint8_t CalculateLightPercentage(uint16_t adc_value) ;		//根据ADC值测量光照百分比函数
float CalculateTemperature(uint16_t adc_value);				//根据ADC值测量实际温度值，但是实际中函数未使用！！！

#endif
