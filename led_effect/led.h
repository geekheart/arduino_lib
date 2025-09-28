/**
 * @file led.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-09-18
 *
 * @copyright Copyright (c) 2025, CorAL. All rights reserved.
 *
 */

#ifndef __LED_H__
#define __LED_H__

/* ==================== [Includes] ========================================== */

#include <Arduino.h>

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

class Led
{
private:
    uint8_t pin;
    bool commonAnode;
    bool usePWM;
    uint8_t channel;
    uint8_t maxBrightness;
    uint8_t currentBrightness;

    void initPWM();
    void writeDigital(bool on);

public:
    // 构造函数重载
    Led(uint8_t pin_, bool commonAnode_ = false);                                   // 普通数字灯
    Led(uint8_t pin_, bool commonAnode_, uint8_t maxBrightness_, uint8_t channel_); // PWM灯

    void setBrightness(uint8_t brightness);
    void on();
    void off();

    uint8_t getBrightness() const { return currentBrightness; }
    bool isPWM() const { return usePWM; }
};

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#endif // __LED_H__
