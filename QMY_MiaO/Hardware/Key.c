#include "Key.h"
uint8_t Key_Num;

void Key_Init(void)
{
    /* GPIO初始化 */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = KEY1_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY12_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = KEY3_PIN;
    GPIO_Init(KEY345_PORT, &GPIO_InitStruct);
}

uint8_t Key_GetNum(void)
{
    if (GPIO_ReadInputDataBit(KEY12_PORT, KEY1_PIN) == RESET)
    {
        return 1;
    }
    /* else if (GPIO_ReadInputDataBit(KEY12_PORT, KEY2_PIN) == RESET)
    {
        return 2;
    } */
    else if (GPIO_ReadInputDataBit(KEY345_PORT, KEY3_PIN) == RESET)
    {
        return 3;
    }
    /* else if (GPIO_ReadInputDataBit(KEY345_PORT, KEY4_PIN) == RESET)
    {
        return 4;
    }
    else if (GPIO_ReadInputDataBit(KEY345_PORT, KEY5_PIN) == RESET)
    {
        return 5;
    } */
    else
    {
        return 0;
    }
}

void Key_Tick(void)
{
    static uint8_t Last = 0, Now = 0;
    Last = Now;
    Now = Key_GetNum();
    if (Last == 0 && Now == 1) // 1按下瞬间
    {
        Key_Num = 1;
    }
    else if (Last == 0 && Now == 2) // 2按下瞬间
    {
        Key_Num = 2;
    }
    else if (Last == 0 && Now == 3) // 3按下瞬间
    {
        Key_Num = 3;
    }
    else if (Last == 0 && Now == 4) // 4按下瞬间
    {
        Key_Num = 4;
    }
    else if (Last == 0 && Now == 5) // 5按下瞬间
    {
        Key_Num = 5;
    }
    else if (Last == 0 && Now == 0)
    {
        Key_Num = 0;
    }

    if (Last != 0 && Now == 0) // 松开瞬间
    {
        Key_Num = 0;
    }
}
