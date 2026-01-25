#ifndef __STEPPER_H
#define __STEPPER_H

#include "stm32f10x.h"
#include "Encoder.h"
#include <math.h>

#define STEPPER_HORIZONTAL_ENDIR_PORT GPIOB

#define STEPPER_HORIZONTAL_PIN_EN     GPIO_Pin_13
#define STEPPER_HORIZONTAL_PIN_DIR    GPIO_Pin_15

#define STEPPER_HORIZONTAL_ST_PORT    GPIOA
#define STEPPER_HORIZONTAL_PIN_ST     GPIO_Pin_11

#define STEPPER_VERTICAL_ENDIR_PORT   GPIOB

#define STEPPER_VERTICAL_PIN_EN       GPIO_Pin_12
#define STEPPER_VERTICAL_PIN_DIR      GPIO_Pin_14

#define STEPPER_VERTICAL_ST_PORT      GPIOA
#define STEPPER_VERTICAL_PIN_ST       GPIO_Pin_8

// 位置式 PID
typedef struct
{
    int32_t Target;                   // 目标值待实现脉冲数
    int32_t Atcual;                   // 实际值 0 ~ 512
    int32_t Error1, Error0, ErrorInt; // 本次误差，上次误差，误差积分
    int32_t Out;                      // 输出值
    uint16_t InputDeathZone;          // 输入死区
    uint16_t MaxOut;                  // 输出限幅
    float Kp, Ki, Kd;
} AnglePID_t;

typedef struct
{
    float Target;                   // 目标值速度
    float Atcual;                   // 实际值速度
    float Error1, Error0, ErrorInt; // 本次误差，上次误差，误差积分
    float Out;                      // 输出值周期
    float InputDeathZone;           // 输入死区
    float MaxOut;                   // 输出限幅
    float Kp, Ki, Kd;
} SpeedPID_t;

extern AnglePID_t Hori_Angle_PID, Vert_Angle_PID;
extern SpeedPID_t Hori_Speed_PID, Vert_Speed_PID;

void AnglePID_Realize(AnglePID_t *PID_Struct);
void SpeedPID_Realize(SpeedPID_t *PID_Struct);

void Stepper_Init(void);

#endif
