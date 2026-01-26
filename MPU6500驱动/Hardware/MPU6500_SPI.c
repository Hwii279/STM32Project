#include "MPU6500_SPI.h"

void MPU6500_W_SCK(uint8_t BitValue)
{
    if (BitValue == 0)
    {
        GPIO_ResetBits(MPU6500_PORT, MPU6500_MPU6500_SCK_PIN);
    }
    else
    {
        GPIO_SetBits(MPU6500_PORT, MPU6500_MPU6500_SCK_PIN);
    }
}

void MPU6500_W_MOSI(uint8_t BitValue)
{
    if (BitValue == 0)
    {
        GPIO_ResetBits(MPU6500_PORT, MPU6500_MPU6500_MOSI_PIN);
    }
    else
    {
        GPIO_SetBits(MPU6500_PORT, MPU6500_MPU6500_MOSI_PIN); 
    }
}

uint8_t MPU6500_R_MISO()
{
    if (GPIO_ReadInputDataBit(MPU6500_PORT, MPU6500_MPU6500_MISO_PIN) > 0)
    {
        return 1;
    }
    return 0;
}

void MPU6500_W_CS(uint8_t BitValue)
{
    if (BitValue == 0)
    {
        GPIO_ResetBits(MPU6500_PORT, MPU6500_MPU6500_CS_PIN);
    }
    else
    {
        GPIO_SetBits(MPU6500_PORT, MPU6500_MPU6500_CS_PIN);
    }
}

void MPU6500_Start()
{
    MPU6500_W_CS(0); // 拉低CS开始通信
}

void MPU6500_Stop()
{
    MPU6500_W_CS(1); // 拉高CS结束通信
}

void MPU6500_IO_Init()
{
    GPIO_InitTypeDef MPU6500_GPIO_InitStruct;
    // SCK MOSI CS 引脚配置为推挽输出
    MPU6500_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    MPU6500_GPIO_InitStruct.GPIO_Pin = MPU6500_MPU6500_SCK_PIN | MPU6500_MPU6500_MOSI_PIN | MPU6500_MPU6500_CS_PIN;
    MPU6500_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MPU6500_PORT, &MPU6500_GPIO_InitStruct);
    // MISO 引脚配置为上拉输入
    MPU6500_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    MPU6500_GPIO_InitStruct.GPIO_Pin = MPU6500_MPU6500_MISO_PIN;
    GPIO_Init(MPU6500_PORT, &MPU6500_GPIO_InitStruct);

    MPU6500_W_SCK(0); // SCK上升沿数据读取，默认低电平
    MPU6500_W_MOSI(1);
    MPU6500_W_CS(1); // 默认高电平不选择从机
}

uint8_t MPU6500_SwapByte(uint8_t ByteSend)
{
    uint8_t i, ByteReceive = 0x00;
    for (i = 0; i < 8; i++)
    {
        MPU6500_W_MOSI(ByteSend & (0x80 >> i)); // 按位从高到低发送数据
        MPU6500_W_SCK(1);                       // 拉高SCK，上升沿从机读取数据
        if (MPU6500_R_MISO())
        {
            ByteReceive |= (0x80 >> i);
        }
        MPU6500_W_SCK(0); // 拉低SCK准备发送下一位
    }
    return ByteReceive;
}
