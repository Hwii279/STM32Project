#ifndef __MPU6500_H
#define __MPU6500_H

#include "MPU6500_Reg.h"
#include "MPU6500_SPI.h"
#include "Delay.h"

extern int16_t Array_GyroZ[10];

void MPU6500_WriteReg(uint8_t RegAddress, uint8_t Value);

uint8_t MPU6500_ReadReg(uint8_t RegAddress);

void MPU6500_Init(void);

void MPU6500_ReadData(int16_t *GyroZ);

int16_t MPU6500_ReadDirection(void);

#endif
