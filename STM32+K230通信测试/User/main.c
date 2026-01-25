#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

int main()
{
    OLED_Init();
    Serial_Init();

    while (1) {
        if (Serial_GetRxFlag()) {
            OLED_ShowHexNum(1, 1, Serial_RxPacket[0], 2);
            OLED_ShowHexNum(1, 4, Serial_RxPacket[1], 2);
            OLED_ShowHexNum(1, 7, Serial_RxPacket[2], 2);
            OLED_ShowHexNum(1, 10, Serial_RxPacket[3], 2);

			Serial_TxPacket[0] = Serial_RxPacket[0];
			Serial_TxPacket[1] = Serial_RxPacket[1];
			Serial_TxPacket[2] = Serial_RxPacket[2];
			Serial_TxPacket[3] = Serial_RxPacket[3];
			Serial_SendPack();
        }
    }
}
