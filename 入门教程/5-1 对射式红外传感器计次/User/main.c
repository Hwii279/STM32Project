#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "COUNTSENSOR.h"

int main (){
	OLED_Init();
	CountSensorInit();
	OLED_ShowString(1, 1, "count:");
	while(1){
		OLED_ShowNum(1, 7, CountSensor_Get(), 5);
	}
}
