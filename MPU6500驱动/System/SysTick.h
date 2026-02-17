#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include "MPU6500.h"

#define SYSTICK_TIM TIM2

void SysTick_Init(void);

#endif
