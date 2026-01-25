#ifndef __SERIAL_H
#define __SERIAL_H

#define SERIAL_BAUTRATE 115200

extern uint8_t Serial_TxPacket[4];
extern uint8_t Serial_RxPacket[4];
extern uint8_t Serial_RxFlag;

void Serial_Init(void);

uint8_t Serial_GetRxFlag();
void Serial_SendPack();

#endif