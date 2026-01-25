#include "RP.h"

void RP_Init(void)
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* GPIO初始化 */
    GPIO_InitTypeDef RP_GPIO_InitStruct;
    RP_GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;
    RP_GPIO_InitStruct.GPIO_Pin   = RP_1_PIN | RP_2_PIN | RP_3_PIN | RP_4_PIN;
    RP_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RP_PORT, &RP_GPIO_InitStruct);

    /* ADC初始化 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADC时钟选择6分频，ADCCLK = 72MHz / 6 = 12MHz

    ADC_InitTypeDef RP_ADC_InitStruct;
    RP_ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;                   // 连续转换，失能，每转换一次规则组序列后停止
    RP_ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;       // 数据对齐，选择右对齐
    RP_ADC_InitStruct.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None; // 是否使用外部触发，使用软件触发，不需要外部触发
    RP_ADC_InitStruct.ADC_Mode               = ADC_Mode_Independent;      // 模式，选择独立模式，即单独使用ADC1
    RP_ADC_InitStruct.ADC_NbrOfChannel       = 1;                         // 通道数，为1，仅在扫描模式下，才需要指定大于1的数，在非扫描模式下，只能是1
    RP_ADC_InitStruct.ADC_ScanConvMode       = DISABLE;                   // 扫描模式，失能，只转换规则组的序列1这一个位置
    ADC_Init(RP_ADC, &RP_ADC_InitStruct);

    // ADC使能
    ADC_Cmd(RP_ADC, ENABLE); // 使能ADC1，ADC开始运行

    // ADC校准
    ADC_ResetCalibration(RP_ADC); // 固定流程，内部有电路会自动执行校准
    while (ADC_GetResetCalibrationStatus(RP_ADC) == SET);
    ADC_StartCalibration(RP_ADC);
    while (ADC_GetCalibrationStatus(RP_ADC) == SET);
}

uint16_t RP_GetValue(uint8_t n)
{
    if (n == 1) // 指定读取RP1
    {
        /*配置规则组为通道2（PA0）*/
        ADC_RegularChannelConfig(RP_ADC, RP_1_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    } else if (n == 2) // 指定读取RP2
    {
        /*配置规则组为通道3（PA1）*/
        ADC_RegularChannelConfig(RP_ADC, RP_2_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    } else if (n == 3) // 指定读取RP3
    {
        /*配置规则组为通道4（PA4）*/
        ADC_RegularChannelConfig(RP_ADC, RP_3_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    } else if (n == 4) // 指定读取RP4
    {
        /*配置规则组为通道5（PA5）*/
        ADC_RegularChannelConfig(RP_ADC, RP_4_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    }

    ADC_SoftwareStartConvCmd(RP_ADC, ENABLE);                 // 软件触发AD转换一次
    while (ADC_GetFlagStatus(RP_ADC, ADC_FLAG_EOC) == RESET); // 等待EOC标志位，即等待AD转换结束
    return ADC_GetConversionValue(RP_ADC);                    // 读数据寄存器，得到AD转换的结果
}
