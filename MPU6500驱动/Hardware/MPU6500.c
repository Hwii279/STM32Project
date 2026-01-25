#include "MPU6500_SPI.h"
#include "MPU6500_Reg.h"
#include "System/Delay.h"

int16_t Array_GyroZ[10];
float Variance;

void MPU6500_WriteReg(uint8_t RegAddress, uint8_t Value){
    MPU6500_Start();
    MPU6500_SwapByte(RegAddress & (~0x80));//最高位1 --> R , 0 --> W
    MPU6500_SwapByte(Value);
    MPU6500_Stop();
}

uint8_t MPU6500_ReadReg(uint8_t RegAddress){
    MPU6500_Start();
    MPU6500_SwapByte(RegAddress | 0x80);//最高位1 --> R , 0 --> W
    uint8_t ByteRead = MPU6500_SwapByte(0x2B);//发送垃圾数据
    MPU6500_Stop();
    return ByteRead;
}

void MPU6500_Init(){
    MPU6500_IO_Init();

    MPU6500_WriteReg(MPU6500_PWR_MGMT_1, 0x80);//复位MPU6500
    Delay_ms(100);// see 《register map》page 42 - delay 100ms
    MPU6500_WriteReg(MPU6500_SIGNAL_PATH_RESET, 0x07);//复位GYR+ACC+TEMP
    Delay_ms(100);// see 《register map》page 42 - delay 100ms
    MPU6500_WriteReg(MPU6500_USER_CTRL, 0x11);//SET spi mode+Reset all gyro digital signal
    Delay_ms(100);

    MPU6500_WriteReg(MPU6500_CONFIG, 0x03);//低通滤波 频率41Hz
    Delay_ms(10);
    MPU6500_WriteReg(MPU6500_GYRO_CONFIG, 0x18);//+-2000dps
    Delay_ms(10);
    MPU6500_WriteReg(MPU6500_ACCEL_CONFIG, 0x10);//+-8G
    Delay_ms(10);
    MPU6500_WriteReg(MPU6500_ACCEL_CONFIG_2, 0x03);//使能低通滤波器  设置 Acc 低通滤波
    Delay_ms(10);
    MPU6500_WriteReg(MPU6500_PWR_MGMT_2, 0x00);//加速度与陀螺仪都工作
    Delay_ms(10);

    MPU6500_WriteReg(MPU6500_SMPLRT_DIV, 0x04);//设置采样率为200Hz(1kHz 5分频)
    Delay_ms(10);
}

void MPU6500_ReadData(int16_t* GyroZ){
    uint8_t DataH, DataL;
    DataH = MPU6500_ReadReg(MPU6500_GYRO_ZOUT_H);//读取高8位
    DataL = MPU6500_ReadReg(MPU6500_GYRO_ZOUT_L);//读取低8位
    *GyroZ = (DataH << 8) | DataL;
}

//绝对值函数
int16_t MPU6500_Fabs(int16_t X){
    if(X < 0){
        X = -X;
    }
    return X;
}

float MPU6500_VarianceCalc(int16_t *Sample, int16_t Mean){
    int16_t ErrorSum = 0, i;
    for(i = 0; i < 3; i++){
        ErrorSum += (Sample[i] - Mean) * (Sample[i] - Mean);
    }
    return ErrorSum / 2.0;
}

//均值滤波角度解算
int16_t MPU6500_ReadDirection(){
    int16_t Mean, MeanSum = 0;
    static int16_t Direction;
    static uint8_t Time;
    MPU6500_ReadData(&Array_GyroZ[Time]);//获取一个原始角速度数据
    while(MPU6500_Fabs(Array_GyroZ[Time]) == 256){//异常数据处理256,读取到非256为止
        MPU6500_ReadData(&Array_GyroZ[Time]);
    }
    Time++;
    if(Time == 3){//每10次数据进行滤波
        uint8_t i, n = 3;//循环次数i， 参与均值计算的数据数n
        Time = 0;
        for(i = 0; i < 3; i++){
            //MPU6500_Fabs(Array_GyroZ[i + 1] - Array_GyroZ[i]) >= 30 && MPU6500_Fabs(Array_GyroZ[i - 1] - Array_GyroZ[i]) >= 30
            MeanSum += Array_GyroZ[i];
        }
        Mean = MeanSum / n;//计算均值
        Variance = MPU6500_VarianceCalc(Array_GyroZ, Mean);//计算方差
        if(Variance <= 10){//方差过小，认为没动
            Direction += 0;
        }
        else{
            Direction += (Mean + 15) / 55;
        }
    }   
    return Direction;
}