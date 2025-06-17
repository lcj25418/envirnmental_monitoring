#ifndef __MOTORDRIVE_H
#define __MOTORDRIVE_H

void MotorDrive_Init(void);				//电机初始化函数
void Motor_Speed(uint8_t Compare);		//电机速度控制函数
void Motor_Start(void);					//电机启动函数
void Motor_Stop(void);					//电机停止函数

#endif
