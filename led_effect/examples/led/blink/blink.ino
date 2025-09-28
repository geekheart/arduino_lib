#include "led.h"

Led led(A0, false);

void setup() {}

void loop()
{
    // 数字灯闪烁
    led.on();
    delay(500);
    led.off();
    delay(500);
}
