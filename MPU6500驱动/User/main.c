#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MPU6500.h"

int main(void)
{
	/* 开启时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* 外设初始化 */
	OLED_Init();
	MPU6500_Init();
	int16_t GyroZ;
	uint16_t time = 0;

	while (1)
	{
		MPU6500_ReadData(&GyroZ);
		OLED_Printf(0, 0, OLED_8X16, "GyroZ:%+04d", GyroZ);
		OLED_Update();
	}
}
