#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "ADC.h"
int main (){
	AD_Init();
	OLED_Init();
	
	OLED_ShowString(1, 1, "AD1:");
	
	while(1){
		AD_GetValue();
		OLED_ShowNum(1, 5, AD_Value[0], 4);
		Delay_ms(100);
	}
}
