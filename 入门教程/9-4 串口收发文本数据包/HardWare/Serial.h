#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

extern uint8_t Serial_RxFlag;
extern char Serial_RxPacket[4];

void Serial_Init(void);
void Serial_SendByte (uint8_t Byte);
void Serial_SendArray (uint8_t* Array, uint16_t Length);
void Serial_SendString (char* String);
uint32_t Serial_Pow (uint32_t X, uint32_t Y);
void Serial_SendNumber (uint32_t Number, uint8_t Length);
int fputc (int ch, FILE *f);
void Serial_Printf(char *format, ...);
void Serial_SendPacket(void);

#endif
