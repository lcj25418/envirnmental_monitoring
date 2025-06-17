#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Buzzer.h"
#include "MotorDrive.h"
#include "Serial.h"
#include "InfrarSensor.h"
#include "AD.h"
#include "LED.h"
#include "Key.h"
#include "DS18B20.h"
#include <string.h>

#define TEMP_THRESHOLD 28.0f     			//温度报警阈值
#define LIGHT_THRESHOLD 30       			//光照报警阈值
//#define STORE_INTERVAL 30000     			
#define DISPLAY_MODE_REAL_TIME 0    		//显示模式：实时数据

float temp_threshold = TEMP_THRESHOLD;       		//可动态调整的温度阈值
uint8_t light_threshold = LIGHT_THRESHOLD;   		//可动态调整的光照阈值
uint8_t Display_mode = DISPLAY_MODE_REAL_TIME;  	//当前显示模式

/*
**根据温度值动态调整电机转动速度
** temp：当前温度
*/
void AdjustMotorSpeed(float temp)
{
	uint8_t speed = 0;					//根据温度范围设置不同风扇转速  
	if(temp>28.6)
	{
		speed = 100;
	}									//高温区域：全速运转
	else if(temp >temp_threshold)
	{
		speed = 70;	
	}									//阈值以上：中速运转	
	else 
	{
		speed = 0;
	}									//正常温度：停止运转
	
	Motor_Speed(speed);					//设置PWM占空比
	if(speed>0)
	{
		Motor_Start();					//启动电机
	}
	else
	{
		Motor_Stop();					//停止电机
	}
}


/*
*主函数
*/

int main(void)
{
	//初始化外设
	OLED_Init();				//初始化OLED显示屏			
	Buzzer_Init();				//初始化蜂鸣器
	MotorDrive_Init();			//初始化电机驱动
	Serial_Init();				//初始化串口通信
	InfrarSensor_Init();		//初始化红外传感器
	AD_Init();					//初始化模数转换器
	LED_Init();					//初始化LED指示灯
	Key_Init(); 				//初始化按键
	
	while(1)
	{
		//这个暂时不使用！！！
//		uint16_t ADC_Temp = AD_Value[0];						//获取温度ADC值					
//		float Temp = CalculateTemperature(ADC_Temp);			//转换为实际温度值
		
		//获取温度传感器数据
		float Temp = DS18B20_GetTemperture();
		
		//获取光照传感器数据
		uint16_t ADC_Light = AD_Value[1];						//获取光照ADC值
		uint8_t Light = CalculateLightPercentage(ADC_Light);	//转换为光照百分比
		
		//获取红外传感器数据
		uint8_t Object_Detected = Infrar_Detected();			//监测是否有物体靠近
		
		//温度超限处理
		if(Temp >temp_threshold)
		{
			AdjustMotorSpeed(Temp);						//启动柜散热风扇
			Buzzer_ON();								//蜂鸣器报警
			LED6_ON();									//六号引脚lED灯亮
			OLED_ShowString(1,1,"COOLING  ");			//显示散热状态
		}
		else 										//温度正常
		{
			Buzzer_OFF();							//蜂鸣器关闭
			LED6_OFF();								//六号引脚LED灯灭
			Motor_Stop();							//停止电机
			OLED_ShowString(1,1,"NORMAL  ");		//显示正常状态
		}
		//光照不足处理
		if(Light<light_threshold)
		{
			LED4_ON();								//四号引脚LED灯亮
		}
		else
		{
			LED4_OFF();								//四号引脚LED灯灭
		}
		//物体接近处理
		if(Object_Detected)
		{
			LED3_ON();								//三号引脚LED灯亮
			OLED_ShowString(2,1,"Wutikaojin");		//显示有物体靠近
		}
		else
		{
			LED3_OFF();								//三号引脚LED灯灭
			OLED_ShowString(2,1,"          ");		//清除显示
		}
		//显示实时数据
		if(Display_mode == DISPLAY_MODE_REAL_TIME)
		{
			OLED_ShowString(3,1,"Tem:");		//在OLED显示屏幕上第一列显示温度标识
			if(Temp<0)
			{
				OLED_ShowString(3,5,"-");		//温度为负数，显示负号
				Temp = -Temp;					//取绝对值
			}
			else
			{
				OLED_ShowString(3,5,"+");		//温度为正数，显示正号
			}
					
			OLED_ShowNum(3,6,Temp,2);  		//显示温度整数部分
			OLED_ShowString(3,8,".");		//显示小数点
			OLED_ShowNum(3,9,(unsigned long)(Temp*10)%10,1);	//显示温度小数部分  
			
			OLED_ShowString(4,1,"Light:");		//在OLED显示屏第四行第一列显示光照标识
			OLED_ShowNum(4,7,Light,3);			//显示光照百分比
			OLED_ShowString(4,10,"%");			//显示百分号
		}
		else
		{
			OLED_ShowString(3,1,"History Mode");	//非实时显示模式，显示历史模式信息
		}
		
		if(Serial_RxFlag == 1)		//检查串口接收标志位，判断是否接收到数据
		{			
			if(strcmp(Serial_RxPacket,"LED5_ON") == 0)		//接收到数据包为"LED5_ON"
			{
				LED5_ON();									//五号引脚LED灯亮
				Serial_SendString("LED5_ON_OK\r\n");		//向串口发送确认信息
			}
			else if(strcmp(Serial_RxPacket,"LED5_OFF") == 0)	//接收到数据包为"LED5_OFF"
			{
				LED5_OFF();									//五号引脚LED灯灭
				Serial_SendString("LED5_OFF_OK\r\n");		//向串口发送确认信息
			}
			else
			{
				Serial_SendString("ERROR_COMMAND\r\n");		//接收到的命令无效，向串口发送错误信息
			}
				Serial_RxFlag = 0;							//清除串口接收标志位
		}
		
		AdjustMotorSpeed(Temp);					//动态控制电机速度
		
		Delay_ms(100);							//延时100ms，控制循环频率
	}
}
