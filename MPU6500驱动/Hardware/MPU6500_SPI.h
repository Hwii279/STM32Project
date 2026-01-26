#ifndef __MPU6500_SPI_H
#define __MPU6500_SPI_H

#include "stm32f10x.h"
#include "Delay.h"

#define MPU6500_PORT                GPIOA
#define MPU6500_MPU6500_SCK_PIN     GPIO_Pin_8
#define MPU6500_MPU6500_MOSI_PIN    GPIO_Pin_9
#define MPU6500_MPU6500_MISO_PIN    GPIO_Pin_10
#define MPU6500_MPU6500_CS_PIN      GPIO_Pin_11

void MPU6500_Start(void);

void MPU6500_Stop(void);

void MPU6500_IO_Init(void);

uint8_t MPU6500_SwapByte(uint8_t ByteSend);

#endif
