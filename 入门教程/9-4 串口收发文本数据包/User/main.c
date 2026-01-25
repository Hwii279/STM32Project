#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "LED.h"
#include <string.h>

int main (){
	OLED_Init();
	Serial_Init();
	LED_Init();
	
	while(1){
		
		if(Serial_RxFlag == 1){
			//OLED_ShowString(4, 1, Serial_RxPacket);
			
			if(strcmp(Serial_RxPacket, "LED_ON") == 0){
				LED1_ON();
				OLED_ShowString(1, 1, "                ");
				OLED_ShowString(1, 1, "LED_ON OK");
			}
			else if(strcmp(Serial_RxPacket, "LED_OFF") == 0){
				LED1_OFF();
				OLED_ShowString(1, 1, "                ");
				OLED_ShowString(1, 1, "LED_OFF OK");
			}
			else{
				OLED_ShowString(1, 1, "                ");
				OLED_ShowString(1, 1, "Err Cmd!");
			}
			Serial_SendString(Serial_RxPacket);
			Serial_RxFlag = 0;
		}
	} 
}
