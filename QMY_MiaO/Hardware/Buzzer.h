#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f10x.h"

#define BUZZER_PORT GPIOA
#define BUZZER_PIN  GPIO_Pin_6
#define BUZZER_TIM  TIM3

void Buzzer_Init(void);

#endif
