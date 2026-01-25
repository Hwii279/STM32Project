#include "stm32f10x.h"
#include "Serial.h"

uint8_t Serial_TxPacket[4];
uint8_t Serial_RxPacket[4];

uint8_t Serial_RxFlag = 0;

void Serial_Init(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //GPIO初始化
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;              //PA9 : TX
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;              //PA10 : RX
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    //USART初始化
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = SERIAL_BAUTRATE;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;            //奇偶校验，不需要
    USART_InitStruct.USART_StopBits = USART_StopBits_1; 
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStruct);
    
    /*中断输出配置*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//开启串口接收数据的中断

    /*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//配置NVIC为分组2

    //NVIC初始化,初始化串口接收中断
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

    /*USART使能*/
	USART_Cmd(USART1, ENABLE);								//使能USART1，串口开始运行
}

void Serial_SendByte(uint8_t Byte){
    USART_SendData(USART1, Byte);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length){
    uint16_t i;
    for(i = 0; i < Length; i++){
        Serial_SendByte(Array[i]);
    }
}

//数据包格式 FF XX XX XX XX FE
void Serial_SendPack(){
    Serial_SendByte(0xFF);
    Serial_SendArray(Serial_TxPacket, 4);
    Serial_SendByte(0xFE);
}

uint8_t Serial_GetRxFlag(){
    if(Serial_RxFlag == 1){         // 为 1 表示已经接受了一组数据包
        Serial_RxFlag = 0;          // 清零标志位，准备进行下次接收
        return 1;
    }
    return 0;
}

void USART1_IRQHandler(){
    static uint8_t RxState = 0;		// 定义表示当前状态机状态的静态变量  0 -> 空闲; 1 -> 接收到包头，正在接受数据; 2 -> 接受到包尾且 p > 3，准备进入空闲状态
	static uint8_t pRxPacket = 0;	// 定义表示当前接收数据位置的静态变量

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
        uint8_t Data = USART_ReceiveData(USART1);
        //处理数据包的不同部分
        if(RxState == 0){                                   // 状态 0
            if(Data == 0xFF){                               // 检测到包头，进入状态 1 
                RxState = 1;
                pRxPacket = 0;
            }
        }
        else if(RxState == 1){                              // 状态 1
            if(pRxPacket <= 3){                             
                Serial_RxPacket[pRxPacket] = Data;
                pRxPacket ++;
            }
            else{
                RxState = 2;
            }
        }
        else if(RxState == 2){                              // 状态 2
            if(Data == 0xFE){                               // 检测包尾
                RxState = 0;                                // 重新进入空闲状态 0
                Serial_RxFlag = 1;                          // 接收标志位置 1，表示已接受完成一组数据包
            }
        }
        //USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}