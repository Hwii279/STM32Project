#ifndef __LASER_RECEIVER_H
#define __LASER_RECEIVER_H

#include "stm32f10x.h"
#include "OLED.h"

#define LASER_BIT_TIME     500

#define LASERREVEIVER_PORT GPIOA
#define LASERREVEIVER_PIN  GPIO_Pin_0

#define LASER_0 (!!(GPIO_ReadInputDataBit(LASERREVEIVER_PORT, LASERREVEIVER_PIN)))
#define LASER_1 (!(GPIO_ReadInputDataBit(LASERREVEIVER_PORT, LASERREVEIVER_PIN)))

extern uint8_t Data_Buff[4], DataCNT; // 接收数据缓冲区
extern uint16_t Laser_State;

void LaserReceiver_Init(void);

#endif
