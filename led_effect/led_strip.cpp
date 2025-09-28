/**
 * @file led_strip.cpp
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-09-19
 *
 * @copyright Copyright (c) 2025, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "led_strip.h"
#include <cmath>

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

// 构造函数：初始化所有效果的默认参数
LedStrip::LedStrip(uint16_t baseSpeed) : baseSpeed(baseSpeed), direction(true), lastUpdate(0)
{
    // 呼吸灯参数
    breath.period = 2000;
    breath.minBrightness = 0;
    breath.maxBrightness = 255;
    breath.reverse = false;
    breath.counter = 0;
    breath.activeLed = 0;

    // 流水灯参数
    running.holdTime = 100;
    running.fade = false;
    running.bounce = false;
    running.tailLength = 0;
    running.currentIndex = 0;
    running.lastIndex = 0;
    running.tailTimer = 0;

    // 闪烁灯参数
    blink.onTime = 500;
    blink.offTime = 500;
    blink.maxBlinks = 0;
    blink.random = false;
    blink.state = false;
    blink.lastBlink = 0;
    blink.blinkCount = 0;

    // 跑马灯参数（新增）
    marquee.tailDecay = 50;       // 拖影亮度衰减百分比（50%=每级拖影亮度减半）
    marquee.tailDuration = 200;   // 拖影持续时间
    marquee.holdPrevious = false; // 是否保持前灯常亮
    marquee.bounce = false;       // 是否弹跳模式
    marquee.bounceBuffer = 1;     // 弹跳边界缓冲（提前反转方向的LED数量）
    marquee.currentIndex = 0;
    marquee.lastIndex = 0;
    marquee.tailTimer = 0;

    // 追逐灯参数（新增）
    chase.mainBrightness = 255;  // 主LED亮度
    chase.slaveBrightness = 100; // 从LED亮度
    chase.spacing = 1;           // 主从间距
    chase.slaveCount = 1;        // 从LED数量
    chase.delay = 50;            // 从LED追赶延迟
    chase.bounce = false;        // 是否弹跳模式
    chase.mainIndex = 0;
    chase.slaveTimes.resize(chase.slaveCount, 0);
}

// 基础功能实现
void LedStrip::addLed(const Led &led) { leds.push_back(led); }
uint8_t LedStrip::getLedCount() const { return leds.size(); }
void LedStrip::allOff()
{
    for (auto &led : leds)
        led.off();
}
void LedStrip::allOn()
{
    for (auto &led : leds)
        led.on();
}
void LedStrip::setDirection(bool forward) { direction = forward; }
void LedStrip::setBaseSpeed(uint16_t speed) { baseSpeed = constrain(speed, 50, 2000); }

// 辅助方法：亮度映射
uint8_t LedStrip::mapBrightness(uint8_t value, uint8_t min, uint8_t max)
{
    return min + (value * (max - min)) / 255;
}

// ---------------- 呼吸灯实现 ----------------
uint8_t LedStrip::breathCurve(uint16_t t)
{
    float phase = (t % breath.period) / (float)breath.period;
    float value = sin(phase * 2 * M_PI - M_PI / 2) + 1;
    return breath.minBrightness + (breath.maxBrightness - breath.minBrightness) * (value * 0.5);
}

void LedStrip::setBreathPeriod(uint16_t period) { breath.period = constrain(period, 500, 5000); }
void LedStrip::setBreathBrightnessRange(uint8_t min, uint8_t max)
{
    breath.minBrightness = constrain(min, 0, 255);
    breath.maxBrightness = constrain(max, breath.minBrightness, 255);
}
void LedStrip::setBreathReverse(bool reverse) { breath.reverse = reverse; }

void LedStrip::breathingRunningLight()
{
    if (leds.empty())
        return;
    unsigned long now = millis();
    if (now - lastUpdate >= 20)
    {
        lastUpdate = now;
        breath.counter += breath.reverse ? -20 : 20;
        if (breath.counter < 0)
            breath.counter = breath.period;
        if (breath.counter >= breath.period)
            breath.counter = 0;

        uint8_t brightness = breathCurve(breath.counter);
        if (breath.counter == 0)
        {
            breath.activeLed = direction ? (breath.activeLed + 1) % leds.size() : (breath.activeLed - 1 + leds.size()) % leds.size();
            allOff();
        }

        if (leds[breath.activeLed].isPWM())
        {
            leds[breath.activeLed].setBrightness(brightness);
        }
        else
        {
            brightness > (breath.minBrightness + breath.maxBrightness) / 2 ? leds[breath.activeLed].on() : leds[breath.activeLed].off();
        }
    }
}

// ---------------- 流水灯实现 ----------------
void LedStrip::setRunningHoldTime(uint16_t time) { running.holdTime = constrain(time, 50, 1000); }
void LedStrip::setRunningFade(bool enable) { running.fade = enable; }
void LedStrip::setRunningBounce(bool enable) { running.bounce = enable; }
void LedStrip::setRunningTailLength(uint8_t length) { running.tailLength = constrain(length, 0, 3); }

uint8_t LedStrip::fadeValue(uint8_t start, uint8_t end, uint16_t time, uint16_t duration)
{
    return duration == 0 ? end : start + (end - start) * constrain((float)time / duration, 0, 1);
}

void LedStrip::runningLight()
{
    if (leds.empty())
        return;
    unsigned long now = millis();
    if (now - lastUpdate >= baseSpeed)
    {
        lastUpdate = now;
        running.lastIndex = running.currentIndex;

        if (running.bounce)
        {
            if (running.currentIndex >= leds.size() - 1)
                direction = false;
            if (running.currentIndex <= 0)
                direction = true;
        }

        running.currentIndex = direction ? (running.currentIndex + 1) % leds.size() : (running.currentIndex - 1 + leds.size()) % leds.size();
        running.tailTimer = now;
    }

    if (running.tailLength == 0 && now - lastUpdate > 50)
    {
        leds[running.lastIndex].off();
    }

    if (running.tailLength > 0)
    {
        for (uint8_t i = 1; i <= running.tailLength; i++)
        {
            int8_t tailIndex = (int8_t)running.currentIndex - i;
            if (tailIndex < 0)
                tailIndex += leds.size();

            if (now - running.tailTimer < baseSpeed)
            {
                uint8_t brightness = 255 - (255 / (running.tailLength + 1)) * i;
                if (running.fade)
                    brightness = fadeValue(brightness, 0, now - running.tailTimer, baseSpeed);
                if (leds[tailIndex].isPWM())
                {
                    leds[tailIndex].setBrightness(brightness);
                }
                else
                {
                    leds[tailIndex].on();
                }
            }
            else
            {
                leds[tailIndex].off();
            }
        }
    }

    if (running.fade)
    {
        uint8_t brightness = fadeValue(0, 255, now - lastUpdate, baseSpeed / 2);
        if (leds[running.currentIndex].isPWM())
        {
            leds[running.currentIndex].setBrightness(brightness);
        }
        else
        {
            leds[running.currentIndex].on();
        }
    }
    else
    {
        leds[running.currentIndex].on();
    }
}

// ---------------- 闪烁灯实现 ----------------
void LedStrip::setBlinkTimes(uint16_t on, uint16_t off)
{
    blink.onTime = constrain(on, 100, 2000);
    blink.offTime = constrain(off, 100, 2000);
}
void LedStrip::setBlinkMaxCount(uint16_t count) { blink.maxBlinks = count; }
void LedStrip::setBlinkRandom(bool enable) { blink.random = enable; }

void LedStrip::blinkAll()
{
    if (leds.empty())
        return;
    if (blink.maxBlinks > 0 && blink.blinkCount >= blink.maxBlinks * 2)
    {
        allOff();
        return;
    }

    unsigned long now = millis();
    uint16_t interval = blink.state ? blink.onTime : blink.offTime;
    if (blink.random && blink.lastBlink == 0)
        interval = random(interval / 2, interval * 2);

    if (now - blink.lastBlink >= interval)
    {
        blink.lastBlink = now;
        blink.state = !blink.state;
        blink.blinkCount++;
        blink.state ? allOn() : allOff();

        if (blink.random)
        {
            interval = blink.state ? blink.onTime : blink.offTime;
            interval = random(interval / 2, interval * 2);
        }
    }
}

// ---------------- 跑马灯实现 ----------------
// 跑马灯参数配置方法
void LedStrip::setMarqueeTailDecay(uint8_t percent)
{
    marquee.tailDecay = constrain(percent, 10, 90); // 10%-90%的衰减率
}
void LedStrip::setMarqueeTailDuration(uint16_t duration)
{
    marquee.tailDuration = constrain(duration, 50, 1000);
}
void LedStrip::setMarqueeHoldPrevious(bool hold)
{
    marquee.holdPrevious = hold;
}
void LedStrip::setMarqueeBounce(bool enable, uint8_t buffer)
{
    marquee.bounce = enable;
    marquee.bounceBuffer = constrain(buffer, 0, 3);
}

// 跑马灯核心逻辑
void LedStrip::stripMarquee()
{
    if (leds.empty())
        return;
    unsigned long now = millis();

    // 主LED移动控制
    if (now - lastUpdate >= baseSpeed)
    {
        lastUpdate = now;
        marquee.lastIndex = marquee.currentIndex;

        // 弹跳模式处理（带边界缓冲）
        if (marquee.bounce)
        {
            if (marquee.currentIndex >= leds.size() - 1 - marquee.bounceBuffer)
            {
                direction = false;
            }
            if (marquee.currentIndex <= marquee.bounceBuffer)
            {
                direction = true;
            }
        }

        // 更新主LED索引
        marquee.currentIndex = direction ? (marquee.currentIndex + 1) % leds.size() : (marquee.currentIndex - 1 + leds.size()) % leds.size();

        marquee.tailTimer = now;
        leds[marquee.currentIndex].on(); // 点亮当前主LED
    }

    // 拖影效果处理
    if (now - marquee.tailTimer < marquee.tailDuration)
    {
        uint8_t tailBrightness = 255;
        int8_t tailIndex = marquee.lastIndex;

        // 计算拖影亮度（按衰减率递减）
        tailBrightness = (tailBrightness * marquee.tailDecay) / 100;

        if (leds[tailIndex].isPWM())
        {
            leds[tailIndex].setBrightness(tailBrightness);
        }
        else
        {
            leds[tailIndex].on(); // 非PWM灯保持常亮作为拖影
        }
    }
    else if (!marquee.holdPrevious)
    {
        // 拖影结束且不保持前灯时，熄灭上一个LED
        leds[marquee.lastIndex].off();
    }
}

// ---------------- 追逐灯实现 ----------------
// 追逐灯参数配置方法
void LedStrip::setChaseBrightness(uint8_t main, uint8_t slave)
{
    chase.mainBrightness = constrain(main, 50, 255);
    chase.slaveBrightness = constrain(slave, 10, chase.mainBrightness);
}
void LedStrip::setChaseSpacing(uint8_t spacing)
{
    chase.spacing = constrain(spacing, 1, 5); // 最大间距5个LED
}
void LedStrip::setChaseSlaveCount(uint8_t count)
{
    chase.slaveCount = constrain(count, 1, 3); // 最多3个从LED
    chase.slaveTimes.resize(chase.slaveCount, 0);
}
void LedStrip::setChaseDelay(uint16_t delay)
{
    chase.delay = constrain(delay, 10, baseSpeed); // 不超过基础速度
}
void LedStrip::setChaseBounce(bool enable)
{
    chase.bounce = enable;
}

// 追逐灯核心逻辑
void LedStrip::stripChase()
{
    if (leds.size() < chase.spacing + 1)
        return; // 确保至少能容纳主从LED
    unsigned long now = millis();

    // 主LED移动控制
    if (now - lastUpdate >= baseSpeed)
    {
        lastUpdate = now;

        // 熄灭上一个主LED
        leds[chase.mainIndex].off();

        // 弹跳模式处理
        if (chase.bounce)
        {
            if (chase.mainIndex >= leds.size() - 1)
                direction = false;
            if (chase.mainIndex <= 0)
                direction = true;
        }

        // 更新主LED索引
        chase.mainIndex = direction ? (chase.mainIndex + 1) % leds.size() : (chase.mainIndex - 1 + leds.size()) % leds.size();

        // 点亮当前主LED（设置亮度）
        if (leds[chase.mainIndex].isPWM())
        {
            leds[chase.mainIndex].setBrightness(chase.mainBrightness);
        }
        else
        {
            leds[chase.mainIndex].on();
        }

        // 重置从LED计时器
        for (uint8_t i = 0; i < chase.slaveCount; i++)
        {
            chase.slaveTimes[i] = now;
        }
    }

    // 从LED追赶逻辑
    for (uint8_t i = 0; i < chase.slaveCount; i++)
    {
        // 计算从LED位置（主LED后方，间距×(i+1)）
        uint8_t slaveIndex = (chase.mainIndex - chase.spacing * (i + 1) + leds.size()) % leds.size();

        // 从LED延迟点亮（模拟追赶）
        if (now - chase.slaveTimes[i] >= chase.delay)
        {
            // 设置从LED亮度（比主LED暗）
            if (leds[slaveIndex].isPWM())
            {
                leds[slaveIndex].setBrightness(chase.slaveBrightness);
            }
            else
            {
                leds[slaveIndex].on();
            }

            // 从LED提前熄灭（避免堆积）
            if (now - chase.slaveTimes[i] >= chase.delay + baseSpeed / 2)
            {
                leds[slaveIndex].off();
            }
        }
    }
}

/* ==================== [Static Functions] ================================== */
