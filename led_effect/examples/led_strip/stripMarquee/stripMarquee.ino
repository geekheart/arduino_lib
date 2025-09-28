#include <led.h>
#include <led_strip.h>

// 创建一个灯带，使用默认动画延迟(100ms)
LedStrip strip;

void setup()
{
    // 动态添加LED到灯带
    strip.addLed(Led(A0, false, 255, 0)); // PWM灯，引脚A0
    strip.addLed(Led(A1, false, 255, 1)); // PWM灯，引脚A1
    strip.addLed(Led(A2, false, 255, 2)); // 普通数字灯，引脚A2
    strip.addLed(Led(A3, false, 255, 3)); // 普通数字灯，引脚A3
    strip.addLed(Led(A4, false, 255, 4)); // 普通数字灯，引脚A4
    strip.setMarqueeTailDecay(10);        // 设置拖影亮度衰减百分比
    strip.setMarqueeTailDuration(200);    // 设置拖影持续时间
    strip.setMarqueeHoldPrevious(false);  // 设置是否保持前灯常亮
    strip.setMarqueeBounce(true);         // 设置是否弹跳模式及缓冲区
}

void loop()
{
    // 直接调用效果方法即可，无需处理时间逻辑
    strip.stripMarquee();
}