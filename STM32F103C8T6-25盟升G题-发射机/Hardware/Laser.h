#ifndef __LASER_H
#define __LASER_H

#include "stm32f10x.h"
#include "OLED.h"

#define LASER_PORT     GPIOA
#define LASER_PIN      GPIO_Pin_6
#define LASER_BIT_TIME 500

void Laser_Init(void);

void Laser_SendByte(uint8_t Byte);

extern uint16_t Laser_State, Loading;

#endif // __LASER_H
