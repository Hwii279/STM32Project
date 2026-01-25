#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"

int main(void)
{
	OLED_Init();
	while (1)
	{
		OLED_Printf(0, 0, OLED_8X16, "Hello, World!");
	}
}
