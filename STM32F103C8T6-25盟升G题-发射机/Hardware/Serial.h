#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"

#define SERIAL_BAUDRATE 115200

extern uint8_t Serial_RxPacket[4];
extern uint8_t Serial_TxPacket[4];

void Serial_Init(void);

void Serial_SendPacket(void);
uint8_t Serial_GetStatus(void);

#endif
