#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"

int16_t AX, AY, AZ, GX, GY, GZ;

int main ()
{
	OLED_Init();
	MPU6050_Init();
	OLED_ShowString(1, 1, "MPU6050 ID:");
	OLED_ShowHexNum(1, 12, MPU6050_GetID(), 2);
	OLED_ShowString(2, 1, "X:");
	OLED_ShowString(3, 1, "Y:");
	OLED_ShowString(4, 1, "Z:");
	while(1){
		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
		OLED_ShowSignedNum(2, 3, AX, 5);// (AX / 32768) * 16 --> g(SI)
		OLED_ShowSignedNum(3, 3, AY, 5);
		OLED_ShowSignedNum(4, 3, AZ, 5);
		OLED_ShowSignedNum(2, 11, GX, 5);
		OLED_ShowSignedNum(3, 11, GY, 5);
		OLED_ShowSignedNum(4, 11, GZ, 5);
	}
}
