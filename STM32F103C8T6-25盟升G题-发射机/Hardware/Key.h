#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "OLED.h"
#include "Laser.h"

#define KEY_PORT_01    GPIOA
#define KEY_PIN_0      GPIO_Pin_12
#define KEY_PIN_1      GPIO_Pin_15
#define KEY_PORT_23    GPIOB
#define KEY_PIN_2      GPIO_Pin_3
#define KEY_PIN_3      GPIO_Pin_4
#define KEY_PORT_ENTER GPIOB
#define KEY_PIN_ENTER  GPIO_Pin_0

void Key_Init(void);

#endif
