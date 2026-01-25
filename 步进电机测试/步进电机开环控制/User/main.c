#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Stepper.h"
#include "ADC.h"
uint16_t AD_Value;

int main (){
	OLED_Init();
	Stepper_Init();
	AD_Init();
	int32_t Angle = 30, Speed = -250;

	GPIO_SetBits(GPIOA, STEPPER_DIR_PIN);
	GPIO_SetBits(GPIOA, STEPPER_ENA_PIN);

	Stepper_SetSpeed(Speed);

	while(1){
		AD_Value = AD_GetValue();
		OLED_Printf(0, 0, OLED_8X16, "AD_Value:%04d", AD_Value);
		OLED_Update();
		//Delay_ms(100); 
	}
}
