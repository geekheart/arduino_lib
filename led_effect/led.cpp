/**
 * @file led.cpp
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-09-18
 *
 * @copyright Copyright (c) 2025, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "led.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */
// ---- 普通数字灯 ----
Led::Led(uint8_t pin_, bool commonAnode_)
    : pin(pin_), commonAnode(commonAnode_), usePWM(false),
      channel(0), maxBrightness(0), currentBrightness(0)
{
    pinMode(pin, OUTPUT);
    off();
}

// ---- PWM 灯 ----
Led::Led(uint8_t pin_, bool commonAnode_, uint8_t maxBrightness_, uint8_t channel_)
    : pin(pin_), commonAnode(commonAnode_), usePWM(true),
      channel(channel_), maxBrightness(maxBrightness_), currentBrightness(0)
{
    pinMode(pin, OUTPUT);
    initPWM();
    off();
}

void Led::initPWM()
{
    // ESP32 PWM 初始化：8位分辨率，5kHz
    ledcSetup(channel, 5000, 8);
    ledcAttachPin(pin, channel);
}

// 数字开关写法（共阴/共阳自动处理）
void Led::writeDigital(bool on)
{
    if (commonAnode)
        digitalWrite(pin, on ? LOW : HIGH);
    else
        digitalWrite(pin, on ? HIGH : LOW);
}

// 写亮度
void Led::setBrightness(uint8_t brightness)
{
    currentBrightness = brightness;
    if (usePWM)
    {
        uint8_t val = commonAnode ? maxBrightness - brightness : brightness;
        ledcWrite(channel, val);
    }
    else
    {
        writeDigital(brightness > 0);
    }
}

void Led::on()
{
    if (usePWM)
        setBrightness(maxBrightness);
    else
        writeDigital(true);
}

void Led::off()
{
    if (usePWM)
        setBrightness(0);
    else
        writeDigital(false);
}
/* ==================== [Static Functions] ================================== */
