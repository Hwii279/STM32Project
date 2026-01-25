#include "Serial.h"

uint8_t Serial_RxPacket[4];//数据包格式 FF XX XX XX XX FE
uint8_t Serial_TxPacket[4];
uint8_t Serial_Status;// 0 -> 未接收完成;1 -> 接收完成待收取

void Serial_Init(void)
{
    /* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* GPIO初始化 */
    GPIO_InitTypeDef Serial_GPIO_InitStruct;
    Serial_GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP; // TX -> PA2
    Serial_GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
    Serial_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &Serial_GPIO_InitStruct);

    Serial_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // RX -> PA3, 空闲状态为 1
    Serial_GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3;
    GPIO_Init(GPIOA, &Serial_GPIO_InitStruct);

    /* USART初始化 */
    USART_InitTypeDef Serial_InitStruct;
    Serial_InitStruct.USART_BaudRate            = SERIAL_BAUDRATE;
    Serial_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 不使用硬件流控制
    Serial_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    Serial_InitStruct.USART_Parity              = USART_Parity_No;  // 不使用奇偶校验
    Serial_InitStruct.USART_StopBits            = USART_StopBits_1; // 一位停止位
    Serial_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_Init(USART2, &Serial_InitStruct);

    /*中断输出配置*/
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启串口接收数据的中断

    /* NVIC初始化 */
    NVIC_InitTypeDef Serial_NVIC_InitStruct;
    Serial_NVIC_InitStruct.NVIC_IRQChannel                   = USART2_IRQn;
    Serial_NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
    Serial_NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;// 指定线路抢占优先级为 1
    Serial_NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 1;// 指定线路响应优先级为 1
    NVIC_Init(&Serial_NVIC_InitStruct);

    /* 使能外设 */
    USART_Cmd(USART2, ENABLE);
}

void Serial_SendByte(uint8_t Byte){
    USART_SendData(USART2, Byte);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void Serial_SendPacket(void){
    Serial_SendByte(0xFF);
    for(uint8_t i = 0; i < 4; i++){
        Serial_SendByte(Serial_TxPacket[i]);
    }
    Serial_SendByte(0xFE);
}

uint8_t Serial_GetStatus(void){
    if(Serial_Status == 1){
        Serial_Status = 0;
        return 1;
    }
    return 0;
}

void USART2_IRQHandler(void)
{
    static uint8_t RxStatus = 0;
    static uint8_t p = 0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)		//判断是否是USART2的接收事件触发的中断
	{
        uint8_t Data = USART_ReceiveData(USART2);
        // 状态机 0 -> 空闲             |1 接收数据     |2 接收完成等待包尾
        //            检测到包头 -> 1   |  四位 -> 2   |  包尾 -> 0
        if(RxStatus == 0){
            if(Data == 0xFF){// 包头 
            RxStatus = 1;
            }
        }
        else if(RxStatus == 1){
            Serial_RxPacket[p] = Data;
            p ++;
            if(p > 3){// 四位 -> 2
                RxStatus = 2;
            }
        }
        else if(RxStatus == 2){
            if(Data == 0xFE){// 包尾
                RxStatus = 0;// 回到空闲状态
                p = 0;
                Serial_Status = 1;
            }
        }
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);			//清除USART2的RXNE标志位
																//读取数据寄存器会自动清除此标志位
																//如果已经读取了数据寄存器，也可以不执行此代码
	}
}
