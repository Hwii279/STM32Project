#include "SysTick.h"
uint16_t SysTick_Count = 0;
void SysTick_Init(void)
{
    /* 配置SysTick定时器，系统时钟为72MHz，设置为1ms中断 */
    // 初始化TIM时基单元
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;               // 1MHz计数频率=  72MHz/72 --> 1MHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;                // 1KHz PWM频率= 1MHz/1000
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 不分频
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            // TIM2没有重复计数器
    TIM_TimeBaseInit(SYSTICK_TIM, &TIM_TimeBaseStructure);

    /* 配置中断 */
    // 配置NVIC中断优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

    NVIC_Init(&NVIC_InitStructure);
    // 清除更新中断标志位并使能更新中断
    TIM_ClearFlag(SYSTICK_TIM, TIM_FLAG_Update);
    TIM_ITConfig(SYSTICK_TIM, TIM_IT_Update, ENABLE);

    // 启动定时器
    TIM_Cmd(SYSTICK_TIM, ENABLE);
}

// 1ms中断函数
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(SYSTICK_TIM, TIM_IT_Update) == SET)
    {
        SysTick_Count++;
        if (SysTick_Count > 100) // 1 <= SysTick_Count <= 100
        {
            SysTick_Count = 1;
        }
        if (SysTick_Count % 20 == 0)// 20ms执行一次
        {
            MPU6500_Tick();
        }
        TIM_ClearITPendingBit(SYSTICK_TIM, TIM_IT_Update);
    }
}
