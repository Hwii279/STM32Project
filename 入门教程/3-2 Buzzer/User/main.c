#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "BUZZER.h"
#include "LIGHTSENSOR.h"

uint8_t KeyNum;

int main (){
	BUZZER_Init();
	LightSensor_Init();
	while(1){
		if(LightSensor_Get()){
			BUZZER_ON();
		}
		else{
			BUZZER_OFF();
		}
	}
}
