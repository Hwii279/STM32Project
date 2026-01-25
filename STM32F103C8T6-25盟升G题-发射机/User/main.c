#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "Stepper.h"
#include "Key.h"
#include "Serial.h"
#include "RP.h"
#include "Laser.h"
#include "Encoder.h"
// 1073行

int main(void)
{
    OLED_Init();
    RP_Init();
    Stepper_Init();
    Key_Init();
    Serial_Init();
    Laser_Init();
    Encoder_Init();

    uint16_t i = 0, Serial_Status;

    Vert_Angle_PID.Target = 150;

    while (1) {
        OLED_ShowNum(0, 16, i++, 5, OLED_6X8);
        OLED_ShowNum(120, 32, Laser_State, 1, OLED_8X16);
        Encoder_ReadAngle(&(Hori_Angle_PID.Atcual), &(Vert_Angle_PID.Atcual));
        Serial_Status = Serial_GetStatus();

        if (Serial_Status == 1) {
            OLED_Printf(0, 32, OLED_8X16, "%02x %02x %02x %02x", Serial_RxPacket[0], Serial_RxPacket[1], Serial_RxPacket[2], Serial_RxPacket[3]);
            for (uint8_t i = 0; i < 4; i++) {
                Serial_TxPacket[i] = Serial_RxPacket[i];
            }
            Serial_SendPacket();
        }

        /* 电位器测试 */
        Hori_Angle_PID.Kp = (float)(RP_GetValue(1)) / 4096.0;
        Hori_Angle_PID.Ki = (float)(RP_GetValue(2)) / 4096.0;
        Hori_Angle_PID.Kd = (float)(RP_GetValue(3)) / 4096.0;
        OLED_Printf(0, 0, OLED_6X8, "Kp:%04.2f Ki:%04.2f", Hori_Angle_PID.Kp, Hori_Angle_PID.Ki);
        OLED_Printf(0, 8, OLED_6X8, "Kd:%04.2f", Hori_Angle_PID.Kd);
        /* PID闭环步进电机测试 */
        // AnglePID_Realize(&Hori_Angle_PID);
        // AnglePID_Realize(&Vert_Angle_PID);
        Hori_Angle_PID.Target = ((float)(RP_GetValue(4)) - 2048.0) * 360 / 4096.0;

        OLED_Printf(48, 8, OLED_6X8, "Out:%+05.2f", Hori_Angle_PID.Out);
        OLED_Printf(48, 16, OLED_6X8, "Target:%+05.2f", Hori_Angle_PID.Target);
        OLED_Printf(48, 24, OLED_6X8, "Actual:%+05.2f", Hori_Angle_PID.Atcual);

        if (Serial_Status == 1) {
            OLED_Printf(0, 32, OLED_8X16, "%02x %02x %02x %02x", Serial_RxPacket[0], Serial_RxPacket[1], Serial_RxPacket[2], Serial_RxPacket[3]);
        }

        OLED_Update();
    }
}
