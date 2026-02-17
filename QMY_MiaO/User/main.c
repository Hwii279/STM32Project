#include "Buzzer.h"
#include "Delay.h"
#include "Key.h"
#include "OLED.h"
#include "SysTick.h"
#include "stm32f10x.h" // Device header

int main(void)
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // PWM输出
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // SysTick时钟

    /* NVIC中断优先级设置 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* 外设初始化 */
    // Buzzer_Init();
    OLED_Init();
    Key_Init();
    SysTick_Init();

    uint16_t i = 0;

    while (1)
    {
        i++;
        OLED_Printf(0, 0, OLED_8X16, "%05d", i);
        OLED_Printf(0, 16, OLED_8X16, "KeyNum:%d", Key_Num);
        OLED_Update();
    }
}
