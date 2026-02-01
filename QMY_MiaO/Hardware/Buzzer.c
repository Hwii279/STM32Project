#include "Buzzer.h"

void Buzzer_Init(void)
{
    /* GPIO初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);

    GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
    /* 定时器TIM3_CH1设定为PWM输出模式 */
    // 选择时钟源为TIM3
    TIM_InternalClockConfig(BUZZER_TIM);
    // 初始化TIM时基单元
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;             // 1KHz计数频率=  72MHz/72 00 --> 10KHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;                 // 50Hz PWM频率= 10KHz/200
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 不分频
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            // TIM3没有重复计数器
    TIM_TimeBaseInit(BUZZER_TIM, &TIM_TimeBaseStructure);

    // 初始化TIM3通道1为PWM模式
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);                       // 初始化结构体
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // PWM模式1(默认)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // 输出极性为高
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_Pulse = 500;                          // 初始占空比为50% CCR1 = 100 --> 50%占空比
    TIM_OC1Init(BUZZER_TIM, &TIM_OCInitStructure);                // 初始化TIM3通道1

    /* 外部中断初始化 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;           // TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 子优先级1
    NVIC_Init(&NVIC_InitStructure);

    // 清除更新中断标志位并使能更新中断
    TIM_ClearFlag(BUZZER_TIM, TIM_FLAG_Update);
    TIM_ITConfig(BUZZER_TIM, TIM_IT_Update, ENABLE);

    // 启动定时器
    TIM_Cmd(BUZZER_TIM, ENABLE);
}
