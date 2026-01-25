#include "Encoder.h"

// 硬件I2C读取AS5600磁编码器

void Encoder_WaitEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT)
{
    uint16_t Timeout = 1000;
    while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS) {
        if(--Timeout == 0) {
            break;
        }
    }
}

void Encoder_WriteReg(I2C_TypeDef *I2Cx, uint8_t RegAddress, uint8_t Data)
{
    I2C_GenerateSTART(I2Cx, ENABLE);                       // S
    Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT); // EV5

    I2C_Send7bitAddress(I2Cx, AS5600_ADDRESS, I2C_Direction_Transmitter); // 发送从机7位地址, 方向为发送
    Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);  // EV6
    // Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING);          // EV8

    I2C_SendData(I2Cx, RegAddress);                             // 发送寄存器地址
    Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED); // EV8_2

    I2C_GenerateSTOP(I2Cx, ENABLE); // P
}

void Encoder_ReadReg(I2C_TypeDef *I2Cx, uint8_t RegAddress, uint8_t *DataOut)
{
    // 先写一字节寄存器地址
    I2C_GenerateSTART(I2Cx, ENABLE);                       // S
    Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2Cx, AS5600_ADDRESS, I2C_Direction_Transmitter); // 发送从机7位地址, 方向为发送
    Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);  // EV6(发送)
    // Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING);          // EV8

    I2C_SendData(I2Cx, RegAddress);                             // 发送寄存器地址
    Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED); // EV8_2
    // 再读一字节寄存器值
    I2C_GenerateSTART(I2Cx, ENABLE);                       // S
    Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT); // EV5

    I2C_Send7bitAddress(I2Cx, AS5600_ADDRESS, I2C_Direction_Receiver);   // 发送从机7位地址, 方向为接收
    Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED); // EV6(接收)
    // 只适于接收1个字节的情况。恰好在EV6之后(即清除了ADDR之后)，要清除响应和停止条件的产生位。 
    I2C_AcknowledgeConfig(I2Cx, DISABLE); // 在接收最后一个字节之前提前将应答失能
    I2C_GenerateSTOP(I2Cx, ENABLE);       // 在接收最后一个字节之前提前申请停止条件

    Encoder_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED); // 等待EV7
    *DataOut = I2C_ReceiveData(I2Cx);                            // 接收数据寄存器

    I2C_AcknowledgeConfig(I2Cx, ENABLE); // 将应答恢复为使能，为了不影响后续可能产生的读取多字节操作
}

void Encoder_ReadAngle(int32_t *Hori_Angle, int32_t *Vert_Angle){
    static int32_t lastH = 0, lastV = 0;
    uint8_t Angle_H, Angle_L;
    
    // Hori_Angle
    Encoder_ReadReg(ENCODER_HORI_I2C, AS5600_ANGLE_H, &Angle_H);
    Encoder_ReadReg(ENCODER_HORI_I2C, AS5600_ANGLE_L, &Angle_L);
    lastH = (Angle_H << 8) | Angle_L;
    *Hori_Angle = lastH;
    // Vert_Angle 
    Encoder_ReadReg(ENCODER_VERT_I2C, AS5600_ANGLE_H, &Angle_H);
    Encoder_ReadReg(ENCODER_VERT_I2C, AS5600_ANGLE_L, &Angle_L);
    lastV = (Angle_H << 8) | Angle_L;
    *Vert_Angle = lastV;
}

void Encoder_Init(void)
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    /* GPIO初始化 */
    GPIO_InitTypeDef Encoder_GPIOInitStruct;
    // SCL SDA 复用开漏输出
    Encoder_GPIOInitStruct.GPIO_Mode  = GPIO_Mode_AF_OD;
    Encoder_GPIOInitStruct.GPIO_Pin   = ENCODER_HORI_SCL | ENCODER_VERT_SCL | ENCODER_HORI_SDA | ENCODER_VERT_SDA;
    Encoder_GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ENCODER_PORT, &Encoder_GPIOInitStruct);

    GPIO_SetBits(ENCODER_PORT, ENCODER_HORI_SCL | ENCODER_VERT_SCL | ENCODER_HORI_SDA | ENCODER_VERT_SDA); // 默认释放I2C总线

    /* I2C初始化 */
    I2C_InitTypeDef Encoder_I2CInitStruct;
    Encoder_I2CInitStruct.I2C_Ack                 = I2C_Ack_Enable;               // 使能ACK位
    Encoder_I2CInitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 应答地址，选择 7位，从机模式下才有效
    Encoder_I2CInitStruct.I2C_ClockSpeed          = 50000;                        // 恢复较低速率 50KHz
    Encoder_I2CInitStruct.I2C_DutyCycle           = I2C_DutyCycle_2;              // 时钟占空比，选择Tlow/Thigh = 2
    Encoder_I2CInitStruct.I2C_Mode                = I2C_Mode_I2C;                 // 模式，选择为I2C模式
    Encoder_I2CInitStruct.I2C_OwnAddress1         = 0x00;                         // 自身地址，从机模式下才有效
    I2C_Init(I2C2, &Encoder_I2CInitStruct);
    I2C_Cmd(I2C2, ENABLE); // 使能I2C2，开始运行
    I2C_Init(I2C1, &Encoder_I2CInitStruct);
    I2C_Cmd(I2C1, ENABLE); // 使能I2C1，开始运行

    /* AS5600寄存器初始化 */ 
}
