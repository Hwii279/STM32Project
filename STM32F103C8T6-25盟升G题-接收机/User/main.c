#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "LaserReceiver.h"
// 191行
int main(void)
{
    OLED_Init();
    LaserReceiver_Init();

    uint16_t i = 0;

    while (1) {
        OLED_Printf(0, 0, OLED_8X16, "Reveived Byte:");
        for (uint8_t j = 0; j < 4; j++) {
            if (Data_Buff[j] != 0x00) {
                OLED_ShowChar(j * 16, 16, Data_Buff[j], OLED_8X16);
            }
        } 

        OLED_Printf(0, 32, OLED_8X16, "PA0:%d     %05d", !!(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)), i++);
        OLED_Printf(0, 48, OLED_8X16, "Laser_State:%d", Laser_State);
        OLED_Update();
    }
}
