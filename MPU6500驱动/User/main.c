#include "Delay.h"
#include "MPU6500.h"
#include "OLED.h"
#include "SysTick.h"
#include "stm32f10x.h" // Device header

int main(void)
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* 外设初始化 */
    OLED_Init();
    MPU6500_Init();
    SysTick_Init();
    uint16_t time = 0;

    while (1)
    {
        OLED_Printf(0, 0, OLED_8X16, "%05d", time++);
        OLED_Printf(0, 16, OLED_8X16, "GyroZ:%+04d", GyroZ);
        OLED_Printf(0, 32, OLED_8X16, "V_Yaw:%+05.2f", V_Yaw);
        OLED_Printf(0, 48, OLED_8X16, "Yaw:%+05.2f", Yaw);
        OLED_Update();
    }
}
