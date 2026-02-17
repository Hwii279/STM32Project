#ifndef __MPU6500_H
#define __MPU6500_H

#include "MPU6500_Reg.h"
#include "MPU6500_SPI.h"
#include "Delay.h"

#define MPU6500_GYROZ_SAMPLETIME 20 // ms
#define MPU6500_GYROZ_FULLSCALE 2000 // 满量程配置：+-2000dps
#define MPU6500_GYROZ_SENSITIVITY 0.5f // 2000dps对应的灵敏度：16.4 LSB/(°/s)

extern int16_t Array_GyroZ[10];
extern int16_t GyroZ;
extern float V_Yaw;
extern float Yaw; // z_k

void MPU6500_WriteReg(uint8_t RegAddress, uint8_t Value);

uint8_t MPU6500_ReadReg(uint8_t RegAddress);

void MPU6500_Init(void);

void MPU6500_ReadData(int16_t *GyroZ);

void MPU6500_Tick(void);

#endif
