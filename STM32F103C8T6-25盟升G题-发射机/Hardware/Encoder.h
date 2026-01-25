#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"

#define ENCODER_PORT     GPIOB

#define ENCODER_HORI_I2C I2C1
#define ENCODER_HORI_SCL GPIO_Pin_6
#define ENCODER_HORI_SDA GPIO_Pin_7

#define ENCODER_VERT_I2C I2C2
#define ENCODER_VERT_SCL GPIO_Pin_10
#define ENCODER_VERT_SDA GPIO_Pin_11

#define AS5600_ADDRESS   (0x36 << 1) // 7位地址左移1位，作为8位地址使用

#define AS5600_ANGLE_L   0x0F // ANGLE[7:0]
#define AS5600_ANGLE_H   0x0E // ANGLE[11:8]

void Encoder_Init(void);

void Encoder_ReadAngle(int32_t *Hori_Angle, int32_t *Vert_Angle);

#endif
