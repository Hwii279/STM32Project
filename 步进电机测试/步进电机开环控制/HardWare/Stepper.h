#ifndef __STEPPER_H
#define __STEPPER_H

#define STEPPER_DIR_PIN     GPIO_Pin_1
#define STEPPER_ENA_PIN     GPIO_Pin_0

void Stepper_Init();

void Stepper_SetSpeed(int32_t Speed);

#endif
