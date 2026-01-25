#ifndef __RP_H
#define __RP_H

#include "stm32f10x.h"

#define RP_PORT      GPIOA

#define RP_1_PIN     GPIO_Pin_0
#define RP_1_CHANNEL ADC_Channel_0

#define RP_2_PIN     GPIO_Pin_1
#define RP_2_CHANNEL ADC_Channel_1

#define RP_3_PIN     GPIO_Pin_4
#define RP_3_CHANNEL ADC_Channel_4

#define RP_4_PIN     GPIO_Pin_5
#define RP_4_CHANNEL ADC_Channel_5

#define RP_ADC       ADC1

void RP_Init(void);

uint16_t RP_GetValue(uint8_t n);

#endif
