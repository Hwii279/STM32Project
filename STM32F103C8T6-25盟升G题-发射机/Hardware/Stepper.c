#include "Stepper.h"

int32_t TargetStep;

AnglePID_t Hori_Angle_PID, Vert_Angle_PID;
SpeedPID_t Hori_Speed_PID, Vert_Speed_PID;

void Stepper_Init(void)
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* GPIO初始化 */
    GPIO_InitTypeDef Stepper_GPIO_InitStruct;
    // Horizontal EN & DIR
    Stepper_GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    Stepper_GPIO_InitStruct.GPIO_Pin   = STEPPER_HORIZONTAL_PIN_EN | STEPPER_HORIZONTAL_PIN_DIR;
    Stepper_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(STEPPER_HORIZONTAL_ENDIR_PORT, &Stepper_GPIO_InitStruct);
    // Horizontal ST
    Stepper_GPIO_InitStruct.GPIO_Pin = STEPPER_HORIZONTAL_PIN_ST;
    GPIO_Init(STEPPER_HORIZONTAL_ST_PORT, &Stepper_GPIO_InitStruct);

    // Vertical EN & DIR
    Stepper_GPIO_InitStruct.GPIO_Pin = STEPPER_VERTICAL_PIN_EN | STEPPER_VERTICAL_PIN_DIR;
    GPIO_Init(STEPPER_VERTICAL_ENDIR_PORT, &Stepper_GPIO_InitStruct);
    // Vertical ST
    Stepper_GPIO_InitStruct.GPIO_Pin = STEPPER_VERTICAL_PIN_ST;
    GPIO_Init(STEPPER_VERTICAL_ST_PORT, &Stepper_GPIO_InitStruct);

    GPIO_SetBits(STEPPER_HORIZONTAL_ENDIR_PORT, STEPPER_HORIZONTAL_PIN_EN); // 使能高
    GPIO_SetBits(STEPPER_HORIZONTAL_ENDIR_PORT, STEPPER_HORIZONTAL_PIN_DIR);
    GPIO_SetBits(STEPPER_VERTICAL_ENDIR_PORT, STEPPER_VERTICAL_PIN_EN); // 使能高
    GPIO_SetBits(STEPPER_VERTICAL_ENDIR_PORT, STEPPER_VERTICAL_PIN_DIR);

    /* 定时器定时中断初始化 */
    // 时钟源设置
    TIM_InternalClockConfig(TIM2); // 选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
    TIM_TimeBaseInitTypeDef Stepper_Timer_InitStruct;
    Stepper_Timer_InitStruct.TIM_ClockDivision     = TIM_CKD_DIV1;       // 时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
    Stepper_Timer_InitStruct.TIM_CounterMode       = TIM_CounterMode_Up; // 向上计数
    Stepper_Timer_InitStruct.TIM_Period            = 100 - 1;            // 计数周期，即ARR的值
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

    /* PID参数初始化 */
    // Hori_Angle
    Hori_Angle_PID.Error0         = 0;
    Hori_Angle_PID.Error1         = 0;
    Hori_Angle_PID.ErrorInt       = 0;
    Hori_Angle_PID.Out            = 0;
    Hori_Angle_PID.Target         = 0;
    Hori_Angle_PID.InputDeathZone = 3;
    Hori_Angle_PID.Kp             = 0.4;
    Hori_Angle_PID.Ki             = 0;
    Hori_Angle_PID.Kd             = 0;
    Hori_Angle_PID.MaxOut         = 1000;
    // Hori_Speed
    Hori_Speed_PID.Error0         = 0;
    Hori_Speed_PID.Error1         = 0;
    Hori_Speed_PID.ErrorInt       = 0;
    Hori_Speed_PID.Out            = 0;
    Hori_Speed_PID.Target         = 0;
    Hori_Speed_PID.InputDeathZone = 3;
    Hori_Speed_PID.Kp             = 0.7;
    Hori_Speed_PID.Ki             = 0.2;
    Hori_Speed_PID.Kd             = 0;
    Hori_Speed_PID.MaxOut         = 100;
    // Vert_Angle
    Vert_Angle_PID.Error0         = 0;
    Vert_Angle_PID.Error1         = 0;
    Vert_Angle_PID.ErrorInt       = 0;
    Vert_Angle_PID.Out            = 0;
    Vert_Angle_PID.Target         = 0;
    Vert_Angle_PID.InputDeathZone = 3;
    Vert_Angle_PID.Kp             = 0.4;
    Vert_Angle_PID.Ki             = 0;
    Vert_Angle_PID.Kd             = 0;
    Vert_Angle_PID.MaxOut         = 1000;
    // Vert_Speed
    Vert_Speed_PID.Error0         = 0;
    Vert_Speed_PID.Error1         = 0;
    Vert_Speed_PID.ErrorInt       = 0;
    Vert_Speed_PID.Out            = 0;
    Vert_Speed_PID.Target         = 0;
    Vert_Speed_PID.InputDeathZone = 3;
    Vert_Speed_PID.Kp             = 0.4;
    Vert_Speed_PID.Ki             = 0;
    Vert_Speed_PID.Kd             = 0;
    Vert_Speed_PID.MaxOut         = 1000;
}

void AnglePID_Realize(AnglePID_t *PID_Struct)
{
    // 误差值传递
    (PID_Struct->Error1) = (PID_Struct->Error0);
    (PID_Struct->Error0) = (PID_Struct->Target) - (PID_Struct->Atcual);
    // 输入死区
    if (fabs(PID_Struct->Error0) <= PID_Struct->InputDeathZone) {
        PID_Struct->Error0 = 0;
    }
    // 误差积分计算
    if (fabs(PID_Struct->Error0) <= 30) {
        // 积分分离
        (PID_Struct->ErrorInt) += (PID_Struct->Error0);
    } else {
        (PID_Struct->ErrorInt) = 0;
    }

    // 输出值为速度目标值
    (PID_Struct->Out) = (PID_Struct->Kp) * (PID_Struct->Error0) +
                        (PID_Struct->Ki) * (PID_Struct->ErrorInt) +
                        (PID_Struct->Kd) * ((PID_Struct->Error0) - (PID_Struct->Error1));
    // 输出限幅
    if (PID_Struct->Out > PID_Struct->MaxOut) {
        PID_Struct->Out = PID_Struct->MaxOut;
    } else if (PID_Struct->Out < -(PID_Struct->MaxOut)) {
        PID_Struct->Out = -(PID_Struct->MaxOut);
    }
}

void SpeedPID_Realize(SpeedPID_t *PID_Struct)
{
    // 误差值传递
    (PID_Struct->Error1) = (PID_Struct->Error0);
    (PID_Struct->Error0) = (PID_Struct->Target) - (PID_Struct->Atcual);
    // 输入死区
    if (fabs(PID_Struct->Error0) <= PID_Struct->InputDeathZone) {
        PID_Struct->Error0 = 0;
    }
    // 误差积分计算
    if (fabs(PID_Struct->Error0) <= 50) {
        // 积分分离
        (PID_Struct->ErrorInt) += (PID_Struct->Error0);
    } else {
        (PID_Struct->ErrorInt) = 0;
    }

    // 输出值为目标周期,
    (PID_Struct->Out) = (((PID_Struct->Kp) * (PID_Struct->Error0) +
                          (PID_Struct->Ki) * (PID_Struct->ErrorInt) +
                          (PID_Struct->Kd) * ((PID_Struct->Error0) - (PID_Struct->Error1))));
    // 输出限幅
    if (PID_Struct->Out > PID_Struct->MaxOut) {
        PID_Struct->Out = PID_Struct->MaxOut;
    } else if (PID_Struct->Out < -(PID_Struct->MaxOut)) {
        PID_Struct->Out = -(PID_Struct->MaxOut);
    }
}

// 非阻塞式定速步进 0.1ms一次
void TIM2_IRQHandler(void)
{
    static uint16_t LastHoriAngle/*, LastVertAngle*/;
    static uint16_t AngleCount = 0, SpeedCount = 0;
    static float    ElapsedMsSinceAngle = 5.0f; // 自上次角度变化后的累计时间
    static float    PulseTimer = 0.0f;      // ms 累加器
    static uint8_t  PulseHold  = 0;         // 脉宽保持标志
    static uint8_t  HoldTicks  = 0;         // 脉宽保持计数(以0.1ms为单位)

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        AngleCount++;
        SpeedCount++;

        // 角度分支：不要改LastHoriAngle，否则会把速度增量清零
        if (AngleCount >= 200) {             // 每20ms
            AngleCount = 0;
            // LastVertAngle = Vert_Angle_PID.Atcual; // 如需垂直轴再启用
        }

        if (SpeedCount >= 50) {              // 每5ms
            SpeedCount = 0;
            ElapsedMsSinceAngle += 5.0f;

            uint8_t hasNew = 0;
            float   newSpeed = 0.0f;
            if (LastHoriAngle >= 3900 && Hori_Angle_PID.Atcual <= 100) {
                int32_t diff = (int32_t)(Hori_Angle_PID.Atcual + 4096 - LastHoriAngle);
                // 按真实Δt计算速度
                newSpeed = ((float)diff) * (360.0f / 4096.0f) * (1000.0f / (ElapsedMsSinceAngle > 0.1f ? ElapsedMsSinceAngle : 0.1f));
                LastHoriAngle = Hori_Angle_PID.Atcual;
                ElapsedMsSinceAngle = 0.0f;
                hasNew = 1;
            } else if (LastHoriAngle <= 100 && Hori_Angle_PID.Atcual >= 3900) {
                int32_t diff = (int32_t)(Hori_Angle_PID.Atcual - (LastHoriAngle + 4096));
                newSpeed = ((float)diff) * (360.0f / 4096.0f) * (1000.0f / (ElapsedMsSinceAngle > 0.1f ? ElapsedMsSinceAngle : 0.1f));
                LastHoriAngle = Hori_Angle_PID.Atcual;
                ElapsedMsSinceAngle = 0.0f;
                hasNew = 1;
            } else {
                int32_t diff = (int32_t)Hori_Angle_PID.Atcual - (int32_t)LastHoriAngle;
                if (diff != 0) {
                    newSpeed = ((float)diff) * (360.0f / 4096.0f) * (1000.0f / (ElapsedMsSinceAngle > 0.1f ? ElapsedMsSinceAngle : 0.1f));
                    LastHoriAngle = Hori_Angle_PID.Atcual;
                    ElapsedMsSinceAngle = 0.0f;
                    hasNew = 1;
                }
            }

            // 速度一阶低通：仅在有新样本时更新（撤回尖峰抑制）
            static uint8_t speedInit = 0;
            static float   speedFilt = 0.0f;
            if (hasNew) {
                if (!speedInit) { speedFilt = newSpeed; speedInit = 1; }
                else            { speedFilt = 0.85f * speedFilt + 0.15f * newSpeed; }
            }

            Hori_Speed_PID.Atcual = speedFilt;
            SpeedPID_Realize(&Hori_Speed_PID);
        }

        // 撤回：恢复为直接用 Out 作为“速度(deg/s)”，按 112.5/|Out| 计算周期，方向由 Out 符号决定
        float out = Hori_Speed_PID.Out; // deg/s
        if (out < 0) GPIO_ResetBits(STEPPER_HORIZONTAL_ENDIR_PORT, STEPPER_HORIZONTAL_PIN_DIR);
        else         GPIO_SetBits  (STEPPER_HORIZONTAL_ENDIR_PORT, STEPPER_HORIZONTAL_PIN_DIR);

        float absOut = fabsf(out);
        float periodMs = 0.0f;
        if (absOut > 1e-3f) {
            // 16*200步/圈：period(ms)=112.5/|deg/s|
            periodMs = 112.5f / absOut;
        } else {
            periodMs = 0.0f; // 视为不运动
        }

        // 先维护脉宽保持（单个中断节拍脉宽）
        if (PulseHold) {
            if (HoldTicks > 0 && --HoldTicks == 0) {
                GPIO_ResetBits(STEPPER_HORIZONTAL_ST_PORT, STEPPER_HORIZONTAL_PIN_ST);
                PulseHold = 0;
            }
        }

        // 按节拍累加并触发新脉冲（若有速度指令）
        PulseTimer += 0.1f; // 中断周期=0.1ms
        if (!PulseHold && periodMs > 0.0f && PulseTimer >= periodMs) {
            PulseTimer -= periodMs;
            GPIO_SetBits(STEPPER_HORIZONTAL_ST_PORT, STEPPER_HORIZONTAL_PIN_ST);
            PulseHold = 1;
            HoldTicks = 1; // 0.1ms 脉宽
        }

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
