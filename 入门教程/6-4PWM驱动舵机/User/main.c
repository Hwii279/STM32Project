#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "KEY.h"
uint16_t Deg, n = 500;
int main (){
	OLED_Init();
	PWM_Init();
	Key_Init();
	OLED_ShowString(1, 1, "Degree:");
	while(1){
		if(Key_GetNum() == 1){
			if(Deg == 180){
				Deg = 0;
				n = 500;
				goto A;
			}
			Deg += 45;
			n += 500;
			A:
			PWM_SetCompare2(n);
		}
		OLED_ShowNum(1, 8, Deg, 3);
	}
}
