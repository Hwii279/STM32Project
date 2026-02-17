#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY12_PORT  GPIOA
#define KEY1_PIN    GPIO_Pin_3 // TEST
#define KEY2_PIN    GPIO_Pin_10
#define KEY345_PORT GPIOB
#define KEY3_PIN    GPIO_Pin_1 // TEST
#define KEY4_PIN    GPIO_Pin_5
#define KEY5_PIN    GPIO_Pin_8

void Key_Init(void);
void Key_Tick(void);

extern uint8_t Key_Num;

#endif
