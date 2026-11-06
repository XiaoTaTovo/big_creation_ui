# 引脚与驱动资源

以下来自本仓库复用的现有源码配置，未重新测量实物排线。MCU 管脚名称不等于 FPC 排线序号；请按你已验证的转接板对应关系接线。

| LCD 信号 | MCU 管脚 | 配置/功能 |
|---|---|---|
| SCK / CLK | PA5 | SPI1 AF5，LCD_SCK_* |
| MOSI / SDA / DIN | PA7 | SPI1 AF5，LCD_MOSI_* |
| CS | PB7 | 普通 GPIO 输出，低有效 |
| DC / RS | PB6 | 普通 GPIO，命令低、数据高 |
| RESET / RES | PB5 | 普通 GPIO，低复位 |
| BL / LED | PD8 | 现有配置高电平开背光 |
| GND | 板卡 GND | 共地 |
| VCC | 沿用已验证的屏模组电源 | 代码不能确认模块额定电压，勿据此把VCC接到任意GPIO或5V |

不需要 MISO。PB8/PB9 是当前使用的 SWC/SWD 调试接线，不是本项目 LCD 或调试 UART 的引脚。V5 printf 沿用 USART8/PB4 TX、115200；普通自检无需接收串口。实际引脚复用合法性需查本芯片手册，不可照其他 MCU 的 AF 表修改。

## 改引脚的位置

统一在 `V5F/User/config.h`：

- CS/DC/RESET/BL：同时修改对应 `*_RCC`、`*_PORT`、`*_PIN`，LCD_GPIOInit 会据此使能时钟与配置GPIO。
- SCK/MOSI：同时修改 `*_RCC`、`*_PORT`、`*_PIN`、`*_SOURCE`、`*_AF`。spi.c 已用这些宏，检查新引脚确实支持 SPI1 对应信号。
- 本轮可改的是 SPI1 的合法引脚映射，不是任意 SPI 实例。若改为 SPI2 等，必须额外修改外设时钟、DMA/DMAMUX请求、DMA通道、中断号及ISR，不能只改引脚宏。

## 复用资源

SPI1：单线只发送、8bit、Mode0、MSB first；SPI_BaudRatePrescaler_Mode2 保持原配置。DMA1 Channel3、DMAMUX 请求63；TIM2 提供1ms LVGL tick。RGB565 高字节先发，两个10行绘制缓冲加一块DMA转换缓冲。横屏每块6400B，竖屏每块3400B，所以横屏总缓冲比竖屏多9000B。

DMA使用原有异步提交/poll完成机制；LVGL等待缓冲复用时仍可能等待，不能将本工程称为严格无阻塞。屏幕方向的唯一入口是 ui_config.h，不要单独在应用里调用 LCD_direction 改方向。
