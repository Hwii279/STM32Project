#include "Laser.h"

uint8_t Byte_Send = 0x00;

// Loading : 进度条(10)
uint16_t Laser_State, Loading;

void Laser_Init(void)
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* GPIO初始化 */
    GPIO_InitTypeDef Laster_GPIO_InitStruct;
    Laster_GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_OD;
    Laster_GPIO_InitStruct.GPIO_Pin   = LASER_PIN;
    Laster_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LASER_PORT, &Laster_GPIO_InitStruct);

    GPIO_SetBits(LASER_PORT, LASER_PIN); // 初始化关闭激光器

    /* 定时器定时中断初始化 */
    // 时钟源设置
    TIM_InternalClockConfig(TIM4); // 选择TIM4为内部时钟，若不调用此函数，TIM默认也为内部时钟
    TIM_TimeBaseInitTypeDef Stepper_Timer_InitStruct;
    Stepper_Timer_InitStruct.TIM_ClockDivision     = TIM_CKD_DIV1;       // 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
    Stepper_Timer_InitStruct.TIM_CounterMode       = TIM_CounterMode_Up; // 向上计数
    Stepper_Timer_InitStruct.TIM_Period            = 1000 - 1;           // 计数周期，即ARR的值
    Stepper_Timer_InitStruct.TIM_Prescaler         = 72 - 1;             // 预分频器，即PSC的值 T = [(PSC + 1) * (ARR + 1)] / 72M
    Stepper_Timer_InitStruct.TIM_RepetitionCounter = 0;                  // 重复计数器，高级定时器才会用到
    TIM_TimeBaseInit(TIM4, &Stepper_Timer_InitStruct);                   // 将结构体变量交给TIM_TimeBaseInit，配置TIM4的时基单元

    // 中断输出配置
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);      // 清除定时器更新标志位
                                               // TIM_TimeBaseInit函数末尾，手动产生了更新事件
                                               // 若不清除此标志位，则开启中断后，会立刻进入一次中断
                                               // 如果不介意此问题，则不清除此标志位也可
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); // 开启TIM4的更新中断

    // NVIC中断分组,已在Stepper_Init中配置，此处可省略

    // NVIC配置
    NVIC_InitTypeDef Stepper_NVIC_InitStructure;                              // 定义结构体变量
    Stepper_NVIC_InitStructure.NVIC_IRQChannel                   = TIM4_IRQn; // 选择配置NVIC的TIM4线
    Stepper_NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;    // 指定NVIC线路使能
    Stepper_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;         // 指定NVIC线路的抢占优先级为2
    Stepper_NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;         // 指定NVIC线路的响应优先级为1
    NVIC_Init(&Stepper_NVIC_InitStructure);                                   // 将结构体变量交给NVIC_Init，配置NVIC外设

    // TIM使能
    TIM_Cmd(TIM4, ENABLE); // 使能TIM4，定时器开始运行
}

void Laser_SendByte(uint8_t Byte)
{
    Byte_Send = Byte;
}

// 1ms 一次定时中断
void TIM4_IRQHandler(void)
{
    static uint16_t Laser_Count = 0, Start_Count = 0, pByte = 0;
    /*
    发射状态机：
    0 : 空闲
        检测到 Byte_Send != 0x00 有信息需要发送 Start_Count = Laser_Count , -> 1
    1 : 正在发送起始位
        0 <= Δt <= T/4, 发送 1
        T/4 <= Δt <= 2T/4, 发送 0
        2T/4 <= Δt <= 3T/4 , 发送 1
        3T/4 <= Δt <= 4T/4 , 发送 0
        Δt >= T, 起始位信号发送完成, -> 2
    2 : 正在发送数据位, LSB先行, 每次发送最后一位 Byte_Send & 0x01 后Byte_Send右移一位, pByte++
        Byte_Send == 0 -> 3
    3 : 1字节信息发送完成
        等待 2T 空闲时间后 -> 0
    */
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {
        Laser_Count++;
        Loading ++;
        if (Laser_State == 0) {
            Loading = 0;
            if (Byte_Send != 0x00) {
                // 有信息需要发送
                Start_Count = Laser_Count;
                Laser_State = 1;
            }
        }
        if (Laser_State == 1) {
            // 正在发送起始位
            if ((Laser_Count - Start_Count) <= (LASER_BIT_TIME * 25 / 100)) { // 起始时刻, 发送第一次起始信号
                GPIO_ResetBits(LASER_PORT, LASER_PIN);                        // 打开激光器
            } else if ((Laser_Count - Start_Count) <= (LASER_BIT_TIME * 50 / 100)) {
                GPIO_SetBits(LASER_PORT, LASER_PIN); // 关闭激光器
            } else if ((Laser_Count - Start_Count) <= (LASER_BIT_TIME * 75 / 100)) {
                GPIO_ResetBits(LASER_PORT, LASER_PIN); // 打开激光器
            } else if ((Laser_Count - Start_Count) < (LASER_BIT_TIME)) {
                GPIO_SetBits(LASER_PORT, LASER_PIN); // 关闭激光器
            }
            else if((Laser_Count - Start_Count) == (LASER_BIT_TIME)){
                // 起始信号发送完成
                Laser_Count = 0;
                Laser_State = 2;
            }
        }
        if (Laser_State == 2) {
            // 正在发送数据位, LSB先行, 每次发送最后一位 Byte_Send & 0x01 后Byte_Send右移一位
            if (Byte_Send & 0x01) { // LSB为 1
                GPIO_ResetBits(LASER_PORT, LASER_PIN); // 打开激光器
                if (Laser_Count >= (LASER_BIT_TIME)) {
                    Laser_Count = 0;
                    Byte_Send >>= 1; // 1 bit 发送完成
                    pByte ++;
                    Loading ++;
                }
            } else { // LSB为 0
                GPIO_SetBits(LASER_PORT, LASER_PIN); // 关闭激光器
                if (Laser_Count >= (LASER_BIT_TIME)) {
                    Laser_Count = 0;
                    Byte_Send >>= 1; // 1 bit 发送完成
                    pByte ++;
                    Loading ++;
                }
            }

            if(pByte > 7){
                // 1字节信息发送完成
                Laser_Count = 0;
                Laser_State = 3;
            }
        }
        if(Laser_State == 3){
            GPIO_SetBits(LASER_PORT, LASER_PIN); // 关闭激光器
            if(Laser_Count >= 2 * LASER_BIT_TIME){
                Laser_State = 0;
                pByte = 0;
                Loading = 0;
            }
        }   
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}
