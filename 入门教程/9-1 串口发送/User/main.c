#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
int main (){
	OLED_Init();
	Serial_Init();
	//Serial_SendString("SBCNMNMSL\r\n");
	//Serial_SendNumber(12345678, 8);
	uint16_t Num = 10;
	//printf ("Num = %d\r\n", Num);
	/*
	char String[100];
	sprintf(String, "Num = %d\r\n", Num);
	Serial_SendString(String);
	*/
	Serial_Printf("傻逼\r\n");
	OLED_ShowString(1, 1, "SBCNMNMSL");
	while(1){
		
	} 
}
