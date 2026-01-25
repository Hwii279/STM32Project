#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "KEY.h"

uint8_t KeyNum;

int main (){
	
	LED_Init();
	Key_Init();
	
	while(1){
		KeyNum=Key_GetNum();
		if(KeyNum == 0){
			LED0_TURN();
		}
		if(KeyNum == 2){
			LED2_TURN();
		}
	}
}
