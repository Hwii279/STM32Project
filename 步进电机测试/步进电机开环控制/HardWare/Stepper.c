#include "stm32f10x.h"
#include "Stepper.h"

void Stepper_IO_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Stepper_PWM_Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
    /*配置时钟源*/
    TIM_InternalClockConfig(TIM2); // 选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟

    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    // 默认1ms周期 (1000Hz)             f = 72M / [(PSC + 1) * (ARR + 1)]
    TIM_InitStructure.TIM_Prescaler         = 72 - 1;   // PSC = 71
    TIM_InitStructure.TIM_Period            = 1000 - 1; // ARR = 99
    TIM_InitStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_InitStructure);

    /*输出比较初始化*/
    TIM_OCInitTypeDef TIM_OCInitStructure;  // 定义结构体变量
    TIM_OCStructInit(&TIM_OCInitStructure); // 结构体初始化，若结构体没有完整赋值
                                            // 则最好执行此函数，给结构体所有成员都赋一个默认值
                                            // 避免结构体初值不确定的问题
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;        // 输出比较模式，选择PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;    // 输出极性，选择为高，若选择极性为低，则输出高低电平取反
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
    TIM_OCInitStructure.TIM_Pulse       = 500;                     // 初始的CCR值
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                      // 将结构体变量交给TIM_OC3Init，配置TIM2的输出比较通道3

    /*TIM使能*/
    TIM_Cmd(TIM2, ENABLE); // 使能TIM2，定时器开始运行
}

void Stepper_Init()
{
    Stepper_IO_Init();
    Stepper_PWM_Init();
}

//Speed : Hz
void Stepper_SetSpeed(int32_t Speed){
    if (Speed == 0)
    {
        TIM_SetCompare3(TIM2, 0);
    }
    else if (Speed < 0)
    {
        GPIO_ResetBits(GPIOA, STEPPER_DIR_PIN);
        TIM_SetAutoreload(TIM2, 1000000 / -Speed);
        TIM_SetCompare3(TIM2, (1000000 / -Speed) / 2);
    }
    else{
        GPIO_SetBits(GPIOA, STEPPER_DIR_PIN);
        TIM_SetAutoreload(TIM2, 1000000 / Speed);
        TIM_SetCompare3(TIM2, (1000000 / Speed) / 2);
    }
}

int32_t PID_Realize(){
    return 0;
}