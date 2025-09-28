#include <led.h>
#include <led_strip.h>

// 创建一个灯带，使用默认动画延迟(100ms)
LedStrip strip;

void setup()
{
    // 动态添加LED到灯带
    strip.addLed(Led(A0, false, 255, 0));    // PWM灯，引脚A0
    strip.addLed(Led(A1, false, 255, 1));    // PWM灯，引脚A1
    strip.addLed(Led(A2));                   // 普通数字灯，引脚A2
    strip.addLed(Led(A3));                   // 普通数字灯，引脚A3
    strip.addLed(Led(A4));                   // 普通数字灯，引脚A4
    strip.setBreathPeriod(2000);             // 设置呼吸周期为2000ms
    strip.setBreathBrightnessRange(50, 255); // 设置亮度范围为
}

void loop()
{
    // 普通数字灯无法呼吸
    // 直接调用效果方法即可，无需处理时间逻辑
    strip.breathingRunningLight();
}