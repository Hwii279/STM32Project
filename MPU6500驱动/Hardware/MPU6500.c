#include "MPU6500.h"

int16_t Array_GyroZ[10];
int16_t GyroZ;
float V_Yaw;
float Yaw = 0; // z_k

void MPU6500_WriteReg(uint8_t RegAddress, uint8_t Value)
{
    MPU6500_Start();
    MPU6500_SwapByte(RegAddress & (~0x80)); // 最高位1 --> R , 0 --> W
    MPU6500_SwapByte(Value);
    MPU6500_Stop();
}

uint8_t MPU6500_ReadReg(uint8_t RegAddress)
{
    MPU6500_Start();
    MPU6500_SwapByte(RegAddress | 0x80);       // 最高位1 --> R , 0 --> W
    uint8_t ByteRead = MPU6500_SwapByte(0x2B); // 发送垃圾数据
    MPU6500_Stop();
    return ByteRead;
}

void MPU6500_Init(void)
{
    MPU6500_IO_Init();

    MPU6500_WriteReg(MPU6500_PWR_MGMT_1, 0x80);        // 复位MPU6500
    Delay_ms(100);                                     // see 《register map》page 42 - delay 100ms
    MPU6500_WriteReg(MPU6500_SIGNAL_PATH_RESET, 0x07); // 复位GYR+ACC+TEMP
    Delay_ms(100);                                     // see 《register map》page 42 - delay 100ms
    MPU6500_WriteReg(MPU6500_USER_CTRL, 0x11);         // SET spi mode+Reset all gyro digital signal
    Delay_ms(100);

    MPU6500_WriteReg(MPU6500_CONFIG, 0x03); // 低通滤波 频率41Hz
    Delay_ms(10);
    MPU6500_WriteReg(MPU6500_GYRO_CONFIG, 0x18); //+-2000dps
    Delay_ms(10);
    MPU6500_WriteReg(MPU6500_ACCEL_CONFIG, 0x10); //+-8G
    Delay_ms(10);
    MPU6500_WriteReg(MPU6500_ACCEL_CONFIG_2, 0x03); // 使能低通滤波器  设置 Acc 低通滤波
    Delay_ms(10);
    MPU6500_WriteReg(MPU6500_PWR_MGMT_2, 0x00); // 加速度与陀螺仪都工作
    Delay_ms(10);

    MPU6500_WriteReg(MPU6500_SMPLRT_DIV, 0x04); // 设置采样率为200Hz(1kHz 5分频)
    Delay_ms(10);
}

void MPU6500_ReadData(int16_t *GyroZ)
{
    uint8_t DataH, DataL;
    DataH = MPU6500_ReadReg(MPU6500_GYRO_ZOUT_H); // 读取高8位
    DataL = MPU6500_ReadReg(MPU6500_GYRO_ZOUT_L); // 读取低8位
    *GyroZ = (DataH << 8) | DataL;
}

// 内部函数 mpu_abs 取绝对值
float mpu_abs(float x)
{
    return x < 0 ? -x : x;
}

// 每 MPU6500_GYROZ_SAMPLETIME ms 调用一次
void MPU6500_Tick(void)
{
    MPU6500_ReadData(&GyroZ);
    V_Yaw = GyroZ * MPU6500_GYROZ_FULLSCALE / 32768.0f; // 单位：度 / s
    // 积分得到角度
    // 设置积分阈值，规避零漂
    if (mpu_abs(V_Yaw) >= MPU6500_GYROZ_SENSITIVITY)
    {
        Yaw += V_Yaw * MPU6500_GYROZ_SAMPLETIME / 1000.0f; // 单位：度
    }

    Yaw %= 360.0f; // 限制在0-360度范围内
}
