#ifndef __MPU6500_SPI_H
#define __MPU6500_SPI_H

#include "stm32f10x.h"
#include "System/Delay.h"

void MPU6500_Start();

void MPU6500_Stop();

void MPU6500_IO_Init();

uint8_t MPU6500_SwapByte(uint8_t ByteSend);

#endif
