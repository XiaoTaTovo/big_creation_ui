# CH32H417 独立 UI 自检工程

硬件基线：CH32H417QEU6 / CH32H417QEU-R0-1v1，170×320 ST7789 SPI 屏。复用现有工程的系统时钟、LCD 初始化序列、SPI1、DMA1 Channel3、双局部缓冲和 TIM2 tick。独立于 PD/IPC 应用；V3 只启动 V5，V5 执行显示自检。

## 横竖屏开关

编辑 `V5F/User/ui_config.h`：

```c
#define UI_LANDSCAPE 0  // 竖屏 170×320
// 改为 1：横屏 320×170
```

这是**编译开关**，不是运行时按键。更改后重编译并手动下载新固件。统一切换 LCD MADCTL、窗口偏移、LVGL 分辨率和 10 行缓冲大小，避免只交换 UI 宽高。竖屏沿用 direction=2、X 偏移35；横屏 direction=1、Y 偏移35。横屏硬件方向/裁切须实物验收；此处不宣称已经上板验证。

## 一键构建（已验证）

安装 Python 3 与 MRS3 的 WCH GCC12。可设置 `WCH_GCC_BIN` 指向含 `riscv-wch-elf-gcc.exe` 的目录，或用 `--toolchain` 指定；脚本也尝试 PATH 和本机 MRS3 默认安装位置。

```powershell
python tools/build.py --orientation portrait
python tools/build.py --orientation landscape
```

命令参数覆盖头文件默认方向；全部输出隔离在 `build/portrait` 或 `build/landscape`。两核全部编译链接，V3 镜像放偏移0，V5 放偏移0x10000，生成 `Merge.bin`。下载地址沿用原工程 0x08000000；使用 MRS/WCH 工具由用户手动选择对应方向的合并文件，脚本没有烧录或复位操作。不能把单独的 V5.bin 当合并文件从基址下载。

## MRS 工程

分别打开/导入 `V3F/ST7789_TEST_V3F.wvproj` 和 `V5F/ST7789_TEST_V5F.wvproj`（保留原工程名称）。Common 使用相对目录引用，根目录可整体移动。不要将原项目与本仓库相同名称的工程混在同一工作区。

先构建 V3，再构建 V5；V5 保留合并下载配置，目标是 `V5F/obj/Merge.bin`。确认输出日志的工程物理路径属于本仓库。MRS 桌面导入流程未自动操作验证；遇到 IDE 元数据兼容问题可使用上面的已验证独立脚本。不要搬旧 obj 目录过来。

## 页面与验收

每8秒自动切换，动态更新周期100ms：

- 静态：内缩1像素边框、TL/TR/BL/BR、RGB/白/黑色块、ASCII/数字/电压电流、圆角框、圆点、完整箭头、分辨率。
- 动态：WAITING/READY/--、999/1000/1/12.00 V、运行秒数、往返进度条。短文字覆盖长文字检查残影。
- 在横屏和竖屏下分别观察60秒：四边完整、无镜像和裁切，颜色正确，动态无残影或花屏，自动切页持续运行。重新上电后能重现。
- 板端 DMA 故障会停止后续刷新并通过原有 USART8/PB4 输出错误；SWC/SWD 接线不是串口，未见串口日志不证明没有错误。

主机软件测试（需要本机 GCC/MinGW）：

```powershell
python tools/test_ui.py --cc gcc
```

使用本仓库 LVGL 实际渲染两种尺寸，输出静态/动态 PPM 图片，检查刷新区域边界与重复 start/stop、约69.5秒模拟 tick。它不模拟 LCD SPI、DMA 或真实时间吞吐，不能替代上板。

## 复用最小模块

`V5F/User/ui_selftest.c/.h` 是自检模块，依赖 LVGL 和 `ui_config.h`，仅使用 label/obj/line。外部完成 LCD/LVGL/tick 初始化后调用 `ui_selftest_start()`；继续运行 `lv_timer_handler()` 和显示驱动 poll。`ui_selftest_stop()` 删除模块自身对象和定时器，可重复调用；请在 LVGL 主线程上下文调用，不要从 ISR 调用。

模块从当前 display 获取宽高。移植其他尺寸需重新审查布局；目前验收目标仅为170×320和320×170。模块无 IPC、PD 解析、真实测量或中文字体依赖。Vendor SDK 与 LVGL 的必需源码仍随仓库提供以支持离线编译，并非只有两个文件就能驱动硬件。

详细引脚和修改方法见 [PINOUT.md](PINOUT.md)，构建证据见 [VALIDATION.md](VALIDATION.md)。第三方许可见 [NOTICE.md](NOTICE.md)。
