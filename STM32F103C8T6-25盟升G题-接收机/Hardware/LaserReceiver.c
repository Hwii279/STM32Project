#include "LaserReceiver.h"

uint8_t Data_Buff[4], DataCNT; // 接收数据缓冲区
uint16_t Laser_State;

void LaserReceiver_Init(void)
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* GPIO初始化 */
    // 有激光为 0, 无激光为 1
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = LASERREVEIVER_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LASERREVEIVER_PORT, &GPIO_InitStructure);

    /* 定时器定时中断初始化 */
    // 时钟源设置
    TIM_InternalClockConfig(TIM2); // 选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
    TIM_TimeBaseInitTypeDef Stepper_Timer_InitStruct;
    Stepper_Timer_InitStruct.TIM_ClockDivision     = TIM_CKD_DIV1;       // 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
    Stepper_Timer_InitStruct.TIM_CounterMode       = TIM_CounterMode_Up; // 向上计数
    Stepper_Timer_InitStruct.TIM_Period            = 1000 - 1;           // 计数周期，即ARR的值
    Stepper_Timer_InitStruct.TIM_Prescaler         = 72 - 1;             // 预分频器，即PSC的值 T = [(PSC + 1) * (ARR + 1)] / 72M
    Stepper_Timer_InitStruct.TIM_RepetitionCounter = 0;                  // 重复计数器，高级定时器才会用到
    TIM_TimeBaseInit(TIM2, &Stepper_Timer_InitStruct);                   // 将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元

    // 中断输出配置
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);      // 清除定时器更新标志位
                                               // TIM_TimeBaseInit函数末尾，手动产生了更新事件
                                               // 若不清除此标志位，则开启中断后，会立刻进入一次中断
                                               // 如果不介意此问题，则不清除此标志位也可
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 开启TIM2的更新中断

    // NVIC中断分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置NVIC为分组2
                                                    // 即抢占优先级范围：0~3，响应优先级范围：0~3
                                                    // 此分组配置在整个工程中仅需调用一次
                                                    // 若有多个中断，可以把此代码放在main函数内，while循环之前
                                                    // 若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置

    // NVIC配置
    NVIC_InitTypeDef Stepper_NVIC_InitStructure;                              // 定义结构体变量
    Stepper_NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn; // 选择配置NVIC的TIM2线
    Stepper_NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;    // 指定NVIC线路使能
    Stepper_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;         // 指定NVIC线路的抢占优先级为2
    Stepper_NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;         // 指定NVIC线路的响应优先级为1
    NVIC_Init(&Stepper_NVIC_InitStructure);                                   // 将结构体变量交给NVIC_Init，配置NVIC外设

    // TIM使能
    TIM_Cmd(TIM2, ENABLE); // 使能TIM2，定时器开始运行
}

// 1ms 一次定时中断
void TIM2_IRQHandler(void)
{
    static uint16_t Laser_Count = 0;
    // static uint16_t Laser_State = 0;
    static int16_t pData = 0;
    // 状态机: 空闲 -> 起始信号(快闪两次) -> 数据接收存储 -> 空闲
    // 0:未接收到第一次信号
    //   接收到激光 -> 1
    // 1:已经接收到第一次信号
    //   在LASER_BIT_TIME * 40 / 100 <= Δt <= LASER_BIT_TIME * 60 / 100 接收到第二次激光 -> 2
    // 2:已接收完成起始信号位，准备开始接收信息
    //   等待1个周期后 -> 3
    // 3:正在接受信息, 每隔LASER_BIT_TIME采集一次引脚电平信息
    //   pData == 0(8位信息接收完成) -> 4
    // 4:接收完成, 重置接收器准备下一字节接收 pData = 7, 接收字节存入缓存数组，接受字节重新变为0x00
    //  LASER_BIT_TIME * 2的时间内持续为无激光照射 -> 0
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        Laser_Count++;
        if (Laser_State == 0) {
            // 第一次激光
            if (LASER_1) {
                Laser_Count = 0;
                Laser_State = 1;
            }
        }
        if (Laser_State == 1) {
            if ((Laser_Count >= (LASER_BIT_TIME * 40 / 100)) && (Laser_Count <= (LASER_BIT_TIME * 60 / 100))) {
                // 第二次激光
                if (LASER_1) {
                    Laser_State = 2;
                    Laser_Count = 0;
                }
            }
        }
        if (Laser_State == 2) {
            if (DataCNT > 3) {
                // 数组超界
                DataCNT = 0;
                // 清空接收缓存数组
                Data_Buff[0] = 0;
                Data_Buff[1] = 0;
                Data_Buff[2] = 0;
                Data_Buff[3] = 0;
            }
            // 等待的一个 T 已经包含在3状态的第一次等待中了
            if (Laser_Count == LASER_BIT_TIME / 2) {
                Laser_State = 3;
                Laser_Count = 0;
            }
        }
        if (Laser_State == 3) {
            // 每LASER_BIT_TIME采集一次引脚电平
            if (Laser_Count == LASER_BIT_TIME) {
                Laser_Count = 0;
                if (LASER_1) {
                    Data_Buff[DataCNT] |= (0x01 << pData);
                }
                pData++;
            }
            // OLED_Printf(64, 32, OLED_8X16, " ");
            //  接收完成，准备进入状态4
            if (pData == 8 && Laser_Count == LASER_BIT_TIME / 2) {
                Laser_Count = 0;
                Laser_State = 4;
            }
        }
        if (Laser_State == 4) {
            pData = 0; // 重置接收位指针
            // LASER_BIT_TIME * 2 时间内持续检测无激光照射 Laser_State 置0空闲状态, 准备接收下一字节信息
            if (Laser_Count >= LASER_BIT_TIME * 2) {
                Laser_Count = 0;
                // 一字节信息接收完成, DataCNT ++
                DataCNT++;
                Laser_State = 0;
            }
        }

        // 防卡死, 空闲 >= 2T 重置
        if ((Laser_State != 0) && (Laser_Count == LASER_BIT_TIME * 2)) {
            Laser_Count = 0;
            Laser_State = 0;
            // 不要这次数据了, 直接下一个 
            DataCNT++;
        }

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
