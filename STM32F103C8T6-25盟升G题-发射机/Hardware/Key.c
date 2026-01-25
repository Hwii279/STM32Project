#include "Key.h"
#include "Delay.h"
// 非阻塞式按键程序

uint8_t ByteSend = 0x0000;
int8_t p_Byte    = 0;

void Key_Init(void)
{
    /* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* GPIO初始化 */
    // 解决JTAG引脚复用问题
    // 启用 AFIO 时钟（必须！）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // 禁用 JTAG，但保留 SWD（推荐，还能调试）
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    // 编码器输出低两位
    GPIO_InitTypeDef Key_GPIO_InitStruct;
    Key_GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    Key_GPIO_InitStruct.GPIO_Pin   = KEY_PIN_0 | KEY_PIN_1;
    Key_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_PORT_01, &Key_GPIO_InitStruct);
    // 编码器输出高两位
    Key_GPIO_InitStruct.GPIO_Pin = KEY_PIN_2 | KEY_PIN_3;
    GPIO_Init(KEY_PORT_23, &Key_GPIO_InitStruct);
    // Enter按键
    Key_GPIO_InitStruct.GPIO_Pin = KEY_PIN_ENTER;
    GPIO_Init(KEY_PORT_ENTER, &Key_GPIO_InitStruct);

    /* 定时器定时中断初始化 */
    // 时钟源设置
    TIM_InternalClockConfig(TIM3); // 选择TIM3为内部时钟，若不调用此函数，TIM默认也为内部时钟

    TIM_TimeBaseInitTypeDef Key_Timer_InitStruct;
    Key_Timer_InitStruct.TIM_ClockDivision     = TIM_CKD_DIV1;       // 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
    Key_Timer_InitStruct.TIM_CounterMode       = TIM_CounterMode_Up; // 向上计数
    Key_Timer_InitStruct.TIM_Period            = 1000 - 1;           // 计数周期，即ARR的值
    Key_Timer_InitStruct.TIM_Prescaler         = 72 - 1;             // 预分频器，即PSC的值 T = [(PSC + 1) * (ARR + 1)] / 72M
    Key_Timer_InitStruct.TIM_RepetitionCounter = 0;                  // 重复计数器，高级定时器才会用到
    TIM_TimeBaseInit(TIM3, &Key_Timer_InitStruct);                   // 将结构体变量交给TIM_TimeBaseInit，配置TIM3的时基单元

    // 中断输出配置
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);      // 清除定时器更新标志位
                                               // TIM_TimeBaseInit函数末尾，手动产生了更新事件
                                               // 若不清除此标志位，则开启中断后，会立刻进入一次中断
                                               // 如果不介意此问题，则不清除此标志位也可
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // 开启TIM3的更新中断

    // NVIC中断分组 --> 已在Stepper_Init()中配置完成

    // NVIC配置
    NVIC_InitTypeDef Key_NVIC_InitStructure;                              // 定义结构体变量
    Key_NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn; // 选择配置NVIC的TIM3线
    Key_NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;    // 指定NVIC线路使能
    Key_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;         // 指定NVIC线路的抢占优先级为2
    Key_NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;         // 指定NVIC线路的响应优先级为1
    NVIC_Init(&Key_NVIC_InitStructure);                                   // 将结构体变量交给NVIC_Init，配置NVIC外设

    // TIM使能
    TIM_Cmd(TIM3, ENABLE); // 使能TIM3，定时器开始运行
}

// 获取编码器数值转化为8进制数
uint8_t Key_GetState(void)
{
    uint8_t State = 0x00;
    State |= ((!(GPIO_ReadInputDataBit(KEY_PORT_23, KEY_PIN_3))) ? 0x08 : 0x00); // | 0000 1000
    State |= ((!(GPIO_ReadInputDataBit(KEY_PORT_23, KEY_PIN_2))) ? 0x04 : 0x00); // | 0000 0100
    State |= ((!(GPIO_ReadInputDataBit(KEY_PORT_01, KEY_PIN_1))) ? 0x02 : 0x00); // | 0000 0010
    State |= ((!(GPIO_ReadInputDataBit(KEY_PORT_01, KEY_PIN_0))) ? 0x01 : 0x00); // | 0000 0001
    State = ~State;                                                              // 0000 0000 -> 1111 1111
    State &= 0x0F;                                                               // 1111 1111 -> 0000 1111
    // 空闲状态 : 00 , 0->01 1->02...7->08 del->09
    return 0x0F - State;
}

void Key_PassNum(uint8_t Byte)
{
    // 清空指针位, 数据位
    ByteSend = 0x00;
    p_Byte   = 0;
    // 发送区清屏
    OLED_Printf(0, 32, OLED_8X16, "    ");
    OLED_Printf(0, 48, OLED_6X8, "Sending:0x%02X aka.'%c'", Byte, Byte);
    Laser_SendByte(Byte);
}

// 1ms 一次轮询中断
void TIM3_IRQHandler(void)
{
    static uint8_t LastNum = 0x00, CurrNum = 0x00, Enter_Last = 0, Enter_Now = 0;
    static uint16_t CNT = 0;
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) {
        CNT++;
        LastNum = CurrNum;
        CurrNum = Key_GetState();
        // 按键按下瞬间, 处理 ByteSend, 输入3位8进制数MSB --> LSB
        if ((LastNum == 0x00) && (CurrNum != 0x00)) {
            // 0x09 -> Del
            if (CurrNum == 0x09) {
                // 上一位清零
                ByteSend &= ~(0x07 << (3 * (3 - p_Byte))); // 0x07 --> 0000 0111
                // OLED清除一位
                OLED_Printf(p_Byte * 8, 32, OLED_8X16, "  ");
            }
            // 非DEL
            else {
                if (p_Byte < 3) {
                    ByteSend |= ((CurrNum - 1) << (3 * (2 - p_Byte))); // XX00 0000 2位(0) | 00XX X000 1位(1) | 0000 0XXX 0位(2) (p_Byte)
                }
            }
        }
        // 按键松开瞬间, 处理 p_Byte
        else if ((LastNum != 0x00) && (CurrNum == 0x00)) {
            // DEL
            if (LastNum == 0x09) {
                p_Byte--;
                if (p_Byte < 0) {
                    p_Byte = 0;
                }
            }
            // 非DEL
            else {
                if (p_Byte < 3) {
                    OLED_Printf(p_Byte * 8, 32, OLED_8X16, "%d", LastNum - 1);
                }
                p_Byte++;
                if (p_Byte > 3) {
                    p_Byte = 3;
                }
            }
        }
        // 500ms光标闪烁一次
        if (CNT < 400) {
            OLED_Printf(p_Byte * 8, 32, OLED_8X16, "  ");
        }
        if (CNT >= 400) {
            OLED_DrawLine(p_Byte * 8, 32, p_Byte * 8, 47);
        }
        if (CNT >= 800) {
            CNT = 0;
        }
        // 数据传递
        Enter_Last                  = Enter_Now;
        Enter_Now                   = !(GPIO_ReadInputDataBit(KEY_PORT_ENTER, KEY_PIN_ENTER));
        static uint8_t temp         = 0x00;
        static uint8_t LoadingState = 0;
        // 进度条状态机
        if (LoadingState == 0) {
            // 0 : 空闲
            // Enter_Last == 0 && Enter_Now != 0 发送数据开始读条
            if (Enter_Last == 0 && Enter_Now != 0) {
                if (ByteSend != 0x00) {
                    temp = ByteSend;
                    OLED_Printf(0, 48, OLED_8X16, "                  ");
                    Key_PassNum(ByteSend);
                    LoadingState = 1;
                }
            }
        }
        if (LoadingState == 1) {
            // 1 : 读条
            OLED_DrawLine(Loading * 12 / LASER_BIT_TIME, 55, Loading * 12 / LASER_BIT_TIME, 63);
            if (Loading * 12 / LASER_BIT_TIME > 127) {
                // 读条完成
                LoadingState = 2;
            }
        }
        if (LoadingState == 2) {
            OLED_Printf(0, 48, OLED_8X16, "Byte '%c' Send OK!", temp);
            LoadingState = 0;
        }

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
