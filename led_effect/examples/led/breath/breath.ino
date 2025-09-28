#include "led.h"

Led led(A0, false, 255, 0); // 最大亮度255，PWM通道0

void setup() {}

void loop()
{
    // PWM 灯呼吸
    for (int i = 0; i <= 255; i++)
    {
        led.setBrightness(i);
        delay(5);
    }
    for (int i = 255; i >= 0; i--)
    {
        led.setBrightness(i);
        delay(5);
    }
}
