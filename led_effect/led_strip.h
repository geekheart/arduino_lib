/**
 * @file led_strip.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-09-19
 *
 * @copyright Copyright (c) 2025, CorAL. All rights reserved.
 *
 */

#ifndef __LED_STRIP_H__
#define __LED_STRIP_H__

/* ==================== [Includes] ========================================== */

#include "led.h"
#include <vector>

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

class LedStrip
{
private:
    std::vector<Led> leds;
    unsigned long lastUpdate; // 基础时间戳

    // 通用参数
    uint16_t baseSpeed; // 基础速度（ms）
    bool direction;     // 方向（true=正向）

    // 呼吸灯参数
    struct BreathParams
    {
        uint16_t period;       // 呼吸周期（ms，默认2000）
        uint8_t minBrightness; // 最小亮度（默认0）
        uint8_t maxBrightness; // 最大亮度（默认255）
        bool reverse;          // 是否反向呼吸（默认false）
        uint16_t counter;      // 呼吸计数器
        uint8_t activeLed;     // 当前活动LED
    } breath;

    // 流水灯参数
    struct RunningLightParams
    {
        uint16_t holdTime;       // 单个LED保持时长（ms，默认100）
        bool fade;               // 是否渐亮渐暗（默认false）
        bool bounce;             // 是否往返模式（默认false）
        uint8_t tailLength;      // 拖尾长度（0-3，默认0）
        uint8_t currentIndex;    // 当前索引
        uint8_t lastIndex;       // 上一个索引
        unsigned long tailTimer; // 拖尾计时器
    } running;

    // 闪烁灯参数
    struct BlinkParams
    {
        uint16_t onTime;         // 亮灯时长（ms，默认500）
        uint16_t offTime;        // 暗灯时长（ms，默认500）
        uint16_t maxBlinks;      // 最大闪烁次数（0=无限，默认0）
        bool random;             // 是否随机间隔（默认false）
        bool state;              // 当前状态
        unsigned long lastBlink; // 闪烁时间戳
        uint16_t blinkCount;     // 已闪烁次数
    } blink;

    // 跑马灯参数
    struct MarqueeParams
    {
        uint8_t tailDecay;       // 拖影亮度衰减百分比（10%-90%，默认50%）
        uint16_t tailDuration;   // 拖影持续时间（ms，默认200）
        bool holdPrevious;       // 是否保持前灯常亮（默认false）
        bool bounce;             // 是否往返模式（默认false）
        uint8_t bounceBuffer;    // 弹跳边界缓冲（0-3，默认1）
        uint8_t currentIndex;    // 当前索引
        uint8_t lastIndex;       // 上一个索引
        unsigned long tailTimer; // 拖影计时器
    } marquee;

    // 追逐灯参数
    struct ChaseParams
    {
        uint8_t mainBrightness;           // 主LED亮度（50-255，默认255）
        uint8_t slaveBrightness;          // 从LED亮度（10-主LED亮度，默认50）
        uint8_t spacing;                  // 主从LED间距（1-5，默认1）
        uint8_t slaveCount;               // 从LED数量（1-3，默认1）
        std::vector<uint16_t> slaveTimes; // 从LED点亮时间戳
        uint16_t delay;                   // 追逐延迟（ms，默认100）
        bool bounce;                      // 是否弹跳模式（默认false）
        uint8_t mainIndex;                // 主LED索引
    } chase;

    // 辅助方法
    uint8_t breathCurve(uint16_t t);                                                 // 呼吸曲线计算
    uint8_t fadeValue(uint8_t start, uint8_t end, uint16_t time, uint16_t duration); // 渐变色计算
    uint8_t mapBrightness(uint8_t value, uint8_t min, uint8_t max);                  // 亮度映射

public:
    // 构造函数（初始化默认参数）
    LedStrip(uint16_t speed = 100);

    // 基础功能
    void addLed(const Led &led);
    uint8_t getLedCount() const;
    void allOff();
    void allOn();
    void setDirection(bool forward);
    void setBaseSpeed(uint16_t speed);

    // 呼吸灯效果及参数配置
    void breathingRunningLight();
    void setBreathPeriod(uint16_t period);                   // 设置呼吸周期（500-5000ms）
    void setBreathBrightnessRange(uint8_t min, uint8_t max); // 设置亮度范围
    void setBreathReverse(bool reverse);                     // 设置是否反向呼吸

    // 流水灯效果及参数配置
    void runningLight();
    void setRunningHoldTime(uint16_t time);    // 设置单个LED保持时间
    void setRunningFade(bool enable);          // 启用/禁用渐亮渐暗
    void setRunningBounce(bool enable);        // 启用/禁用往返模式
    void setRunningTailLength(uint8_t length); // 设置拖尾长度（0-3）

    // 闪烁灯效果及参数配置
    void blinkAll();
    void setBlinkTimes(uint16_t on, uint16_t off); // 设置亮暗时长
    void setBlinkMaxCount(uint16_t count);         // 设置最大闪烁次数
    void setBlinkRandom(bool enable);              // 启用/禁用随机间隔

    // 跑马灯效果及参数配置
    void stripMarquee();
    void setMarqueeTailDecay(uint8_t percent);              // 设置拖尾衰
    void setMarqueeTailDuration(uint16_t duration);         // 设置拖尾持续时间
    void setMarqueeHoldPrevious(bool hold);                 // 设置是否保持前灯
    void setMarqueeBounce(bool enable, uint8_t buffer = 0); // 启用/禁用往返模式及缓冲区

    // 追逐灯效果及参数配置
    void stripChase();
    void setChaseBrightness(uint8_t main, uint8_t slave); // 设置主从LED亮度
    void setChaseSpacing(uint8_t spacing);                // 设置主从LED间距
    void setChaseSlaveCount(uint8_t count);               // 设置从LED数量
    void setChaseDelay(uint16_t delay);                   // 设置追逐延迟
    void setChaseBounce(bool enable);                     // 启用/禁用弹跳模式
};

/* ==================== [Macros] ============================================ */

#endif
