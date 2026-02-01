#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "Buzzer.h"

int main(void)
{
	/* 开启时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* 外设初始化 */
	Buzzer_Init();

    while (1)
    {
    }
}
