# led 效果库

本仓库是 led 库，其原理是通过 pwm 或者 IO 电平控制 led。从而可以显示各种效果。

## 功能特点

1. led 支持共阴极共阳极配置
2. led 支持调节亮度（pwm 控制），开关灯效果
3. 支持闪烁、流水灯、跑马灯、呼吸灯、追逐灯效果

## 如何使用

1. 复制本仓库到 `C:\...\Documents\Arduino\libraries`中
2. 打开 Arduino IDE
3. 在 `文件->示例->led_effect`中打开一个示例
4. 根据初始化代码里所写的 IO 进行配置
5. 编译并烧录

## API 功能参考

### LED 类

`Led` 类用于封装 `LED` 灯的控制，支持 **普通数字控制** 和 **PWM 调光** 两种模式，并兼容 **共阳 (Common Anode)** 与 **共阴 (Common Cathode)** 接法。

普通模式：直接用 `digitalWrite` 控制亮灭。

PWM 模式：通过 `ledcWrite` 控制亮度，支持自定义最大亮度与 PWM 通道。

---

#### 构造函数

```c
Led(uint8_t pin_, bool commonAnode_ = false);
```

- 创建一个 普通数字灯 实例。
- 参数：
  - pin\_：LED 引脚号
  - commonAnode\_：是否为共阳结构（默认 false）

---

```cpp
Led(uint8_t pin_, bool commonAnode_, uint8_t maxBrightness_, uint8_t channel_);
```

- 创建一个 PWM 灯 实例。
- 参数：
  - pin\_：LED 引脚号
  - commonAnode\_：是否为共阳结构
  - maxBrightness\_：最大亮度值（通常为 255）
  - channel\_：PWM 通道号

---

#### 公共方法

```cpp
void setBrightness(uint8_t brightness)
```

- 设置亮度（范围：0 ~ maxBrightness）
- 参数： - brightness：亮度。
  > 普通模式下：brightness > 0 等效于 on()
  > brightness == 0 等效于 off()。

---

```cpp
void on()
```

- 打开 LED
  > 普通模式：点亮
  > PWM 模式：设置为最大亮度

---

```cpp
void off()
```

- 关闭 LED
  > 普通模式：关闭
  > PWM 模式：设置亮度为 0

---

```cpp
uint8_t getBrightness() const
```

- 获取当前亮度值

---

```cpp
bool isPWM() const
```

- 判断当前 LED 是否为 PWM 模式

### LED Strip 类

`LedStrip` 类用于控制一组 LED 灯，实现多种灯效，包括：

- **呼吸灯**（Breathing Light）
- **流水灯**（Running Light）
- **闪烁灯**（Blinking Light）
- **跑马灯**（Marquee Light）
- **追逐灯**（Chase Light）

该类基于 `Led` 类封装，支持丰富的参数配置，例如亮度范围、速度、方向、拖尾效果等，适合开发 LED 特效程序。

---

#### 构造函数

```cpp
LedStrip(uint16_t speed = 100);
```

- 创建一个 LedStrip 实例，并初始化默认参数。
- 参数：
  - speed：基础速度（ms，默认 100）

---

#### 基础功能

```cpp
void addLed(const Led &led)
```

- 添加一个 LED 到灯带中。

```cpp
uint8_t getLedCount() const
```

- 获取当前 LED 数量。

```cpp
void allOff()
```

- 关闭所有 LED。

```cpp
void allOn()
```

- 点亮所有 LED。

```cpp
void setDirection(bool forward)
```

- 设置方向（true=正向，false=反向）。

```cpp
void setBaseSpeed(uint16_t speed)
```

- 设置基础速度（ms）。

---

#### 呼吸灯效果及参数配置

```cpp
void breathingRunningLight()
```

- 启动呼吸灯效果

```cpp
void setBreathPeriod(uint16_t period)
```

- 设置呼吸周期（500-5000ms，默认 2000）。

```cpp
void setBreathBrightnessRange(uint8_t min, uint8_t max)
```

- 设置呼吸灯亮度范围（默认 0-255）。

```cpp
void setBreathReverse(bool reverse)
```

- 设置是否反向呼吸。

---

#### 流水灯效果及参数配置

```cpp
void runningLight()
```

- 启动流水灯效果。

```cpp
void setRunningHoldTime(uint16_t time)
```

- 设置单个 LED 保持时间（默认 100ms）。

```cpp
void setRunningFade(bool enable)
```

- 启用/禁用渐亮渐暗效果。

```cpp
void setRunningBounce(bool enable)
```

- 启用/禁用往返模式。

```cpp
void setRunningTailLength(uint8_t length)
```

- 设置拖尾长度（0-3，默认 0）。

---

#### 闪烁灯效果及参数配置

```cpp
void blinkAll()
```

- 启动闪烁效果。

```cpp
void setBlinkTimes(uint16_t on, uint16_t off)
```

- 设置亮暗时长（默认各 500ms）。

```cpp
void setBlinkMaxCount(uint16_t count)
```

- 设置最大闪烁次数（0=无限）。

```cpp
void setBlinkRandom(bool enable)
```

- 启用/禁用随机间隔闪烁。

---

#### 跑马灯效果及参数配置

```cpp
void stripMarquee()
```

- 启动跑马灯效果。

```cpp
void setMarqueeTailDecay(uint8_t percent)
```

- 设置拖尾亮度衰减百分比（10%-90%，默认 50%）。

```cpp
void setMarqueeTailDuration(uint16_t duration)
```

- 设置拖尾持续时间（ms，默认 200）。

```cpp
void setMarqueeHoldPrevious(bool hold)
```

- 设置是否保持前灯常亮。

```cpp
void setMarqueeBounce(bool enable, uint8_t buffer = 0)
```

- 启用/禁用往返模式及缓冲区。

---

#### 追逐灯效果及参数配置

```cpp
void stripChase()
```

- 启动追逐灯效果。

```cpp
void setChaseBrightness(uint8_t main, uint8_t slave)
```

- 设置主/从 LED 亮度（主：50-255，从：10-主亮度）。

```cpp
void setChaseSpacing(uint8_t spacing)
```

- 设置主从 LED 间距（1-5，默认 1）。

```cpp
void setChaseSlaveCount(uint8_t count)
```

- 设置从 LED 数量（1-3，默认 1）。

```cpp
void setChaseDelay(uint16_t delay)
```

- 设置追逐延迟（ms，默认 100）。

```cpp
void setChaseBounce(bool enable)
```

- 启用/禁用弹跳模式。