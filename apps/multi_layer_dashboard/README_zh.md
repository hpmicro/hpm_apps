# HPM68系列:多图层仪表盘方案（Multi-Layer Dashboard）

## 依赖 SDK 1.11.0

## 概述

本方案基于先楫半导体 `HPM6800EVK` 平台，结合 `LVGL`、`FreeRTOS`、`LCDC` 与 `PDMA`，实现了一套数字仪表场景的多图层 Dashboard 演示工程。

与传统“整屏重绘”的界面方案不同，本项目将仪表盘拆分为 **1 个全屏基础层 + 7 个局部硬件叠加层**，总计 **八图层**。其中大面积静态背景与低频元素保留在主层，高频变化元素（如双指针、左右能量条、顶部转向灯、底部告警条、中央档位区）分别独立成层，从而降低无效刷新面积，减轻内存带宽压力，并提升动画流畅度。

本方案重点展示以下内容：
- **八图层仪表盘架构**：基于 LCDC 8 层能力拆分 UI 区域
- **分层渲染策略**：静态内容与高频动态内容分离绘制
- **缓冲区优化设计**：主层与子层采用不同的 buffer 策略
- **PDMA 刷新链路**：利用 PDMA 完成脏区域搬运，降低 CPU 参与度

## 核心特性

### 多图层显示特性
- **八硬件层并行合成**：主背景层配合 7 个 ARGB8888 透明叠加层
- **图层独立刷新**：不同区域按各自更新频率独立输出到 LCDC
- **透明混合显示**：各层使用 `src_over` 混合模式叠加到最终画面
- **局部区域专层承载**：将高频动画控件限制在较小矩形区域内

### 动态 UI 特性
- **双仪表指针动画**：速度表与转速表分别独立刷新
- **左右能量条动画**：左右侧竖向条形区域独立更新
- **告警/指示灯动画**：转向、雾灯、远近光、安全带、手刹等图标独立控制
- **档位滚轮与数值显示**：中心区域单独分层，避免影响整屏
- **时间/温度/续航信息展示**：作为主层静态或低频控件统一显示

### 渲染与性能特性
- **主层 Direct Render**：全屏主显示采用 `LV_DISPLAY_RENDER_MODE_DIRECT`
- **子层 Full Render**：局部层采用 `LV_DISPLAY_RENDER_MODE_FULL`
- **双缓冲切换**：主层和 7 个子层均使用双缓冲
- **PDMA 脏区搬运**：主层 flush 时仅搬运脏区域到 LCDC 扫描缓冲
- **D-Cache 协同**：CPU/DMA 共享内存前后执行 writeback / invalidate

## 硬件要求

### 主控板要求
- **MCU**: `HPM6800EVK`
- **显示输出**: `1920 x 720` 仪表盘界面
- **显示控制器**: 使用片上 `LCDC`
- **DMA 加速**: 使用片上 `PDMA`
- **运行内存**: 需要较大 `SDRAM` 以容纳多组 ARGB8888 帧缓冲

## 设备连接

### 硬件连接示意图
![connect](doc/api/assets/connect.png)


## 创建工程
![generate_project](doc/api/assets/generate_project.png)

### 软件组件
- `LVGL`
- `FreeRTOS`
- `hpm_panel`
- `LCDC` 驱动
- `PDMA` 驱动

## 软件架构

### 系统框架
```
┌──────────────────────────────────────────────┐
│              Dashboard UI Layer              │
│   (背景、指针、档位、告警、能量条等多区域)      │
└────────────────┬─────────────────────────────┘
				 │
┌────────────────┴─────────────────────────────┐
│           LVGL Multi-Display Layer           │
│  (主层 Direct Render + 子层 Full Render)     │
└────────────────┬─────────────────────────────┘
				 │
┌────────────────┴─────────────────────────────┐
│         PDMA + D-Cache Coherency             │
│   (脏区搬运 + cache writeback/invalidate)     │
└────────────────┬─────────────────────────────┘
				 │
┌────────────────┴─────────────────────────────┐
│            LCDC Hardware Compose             │
│   (8层硬件叠加 + Alpha Blend 输出)            │
└──────────────────────────────────────────────┘
```

### 任务结构
- **LVGL 刷新任务**：循环调用 `lv_timer_handler()`，驱动界面与动画
- **显示输出链路**：LCDC 负责主层与 7 个局部层的最终混合输出
- **VSYNC 同步链路**：LCDC 中断用于刷新同步和显示切换确认

## 八图层设计

### 图层设计示意

**背景层示意图**：
![background](doc/api/assets/background.png)

**图层拆分示意图**：
![layer](doc/api/assets/layer.png)

**效果动图**：
![layer](doc/api/assets/screen.gif)

### 图层划分说明

本项目总共使用 8 个硬件显示层，其中 1 个为全屏主层，7 个为局部叠加层。代码中主层在 `user_lvgl_port.c` 中初始化，局部层在 `ui/screens/home_gen.c` 中分别初始化。

| 硬件层 | 软件对象 | 区域尺寸 | 主要内容 | 设计目的 |
|--------|----------|----------|----------|----------|
| Layer 0 | 主显示 `disp` | `1920 x 720` | 背景图、发动机图标、油量/水温图标、日期、时间、温度、续航等 | 承载全屏背景与低频刷新元素 |
| Layer 1 | `home_layer2` | `352 x 350` | 左侧速度表指针 | 将高频旋转指针从主层剥离 |
| Layer 2 | `home_layer3` | `352 x 350` | 右侧转速表指针 | 独立刷新另一支高频指针 |
| Layer 3 | `home_layer4` | `64 x 512` | 左侧竖向能量条 | 只刷新左侧条形区域 |
| Layer 4 | `home_layer5` | `64 x 512` | 右侧竖向能量条 | 只刷新右侧条形区域 |
| Layer 5 | `home_layer6` | `1408 x 100` | 底部告警/灯光图标带 | 集中管理底部状态图标 |
| Layer 6 | `home_layer7` | `640 x 64` | 顶部左右转向灯图标 | 单独实现闪烁动画 |
| Layer 7 | `home_layer8` | `256 x 345` | 中央速度值、档位滚轮、档位标签 | 中央交互区独立刷新 |

### 图层拆分原则
- **高频动画单独成层**：速度指针、转速指针、左右能量条均是高频更新对象
- **相同语义对象集中成层**：底部状态灯集中到一条横向层，顶部转向灯集中到单独层
- **保持主层稳定**：背景、时间、温度等低频元素放在主层，减少全屏重绘次数
- **区域最小化**：每个子层尽量采用包围盒尺寸，降低每层双缓冲成本

## 渲染策略

### 1. 主层采用 Direct Render

在 `software/inc/lv_app_conf.h` 中启用了：

- `LV_USE_HPM_MODE_DIRECT = 1`
- `LV_USE_HPM_PDMA_FLUSH = 1`

主层在 `software/src/user_lvgl_port.c` 中通过：

- `lv_display_set_buffers(disp, user_lvgl_fb0, user_lvgl_fb1, USER_LVGL_FB_SIZE, LV_DISPLAY_RENDER_MODE_DIRECT)`

配置为 **Direct Render 双缓冲模式**。

这样做的意义是：
- LVGL 直接在全屏 draw buffer 上组织最终主层画面
- 当存在多个脏区域时，不必为每个小区域单独维护复杂的局部缓冲布局
- 在仪表盘这种大背景 + 少量动态更新的场景下，便于配合 PDMA 做脏区搬运

### 2. 主层 Flush 使用 PDMA 搬运脏区

主层 flush 回调 `user_lvgl_display_flush_cb()` 的策略为：

- 先累计 LVGL 上报的脏区域
- 对脏区域对应的 draw buffer 数据执行 D-Cache writeback
- 使用 `PDMA` 将脏区域从 `px_map` 拷贝到 `user_lvgl_lcdc_fb`
- 搬运结束后调用 `lv_display_flush_ready()`

这种方式的好处是：
- **减少 CPU 逐像素搬运开销**
- **减少全屏 buffer 直接切换导致的显示抖动风险**
- **兼顾 Direct Render 的开发便利性与 LCDC 扫描缓冲的稳定性**

### 3. 子层采用 Full Render

7 个局部层都通过如下方式创建：

- `lv_display_create(width, height)`
- `lv_display_set_color_format(..., LV_COLOR_FORMAT_ARGB8888)`
- `lv_display_set_buffers(..., buf0, buf1, sizeof(buf0), LV_DISPLAY_RENDER_MODE_FULL)`

选择 `FULL` 模式的原因是：
- 子层尺寸远小于全屏，整层重绘成本可控
- 逻辑更简单，flush 时直接切换该层下一帧 buffer 即可
- 局部层通常对应单一功能区域，天然适合“整层更新”

### 4. LCDC 负责最终多层合成

每个子层都配置为：
- 像素格式：`ARGB8888`
- 背景透明：`lv_obj_set_style_bg_opa(..., LV_OPA_TRANSP, 0)`
- 混合模式：`display_alphablend_mode_src_over`

因此，LCDC 最终会把主层与各局部层按照硬件层次关系实时混合输出，避免软件端反复做整屏合成。

## 缓冲区选择与设计

### 主层缓冲区

主层使用 3 组全屏缓冲：

- `user_lvgl_fb0`：LVGL 绘制缓冲 0
- `user_lvgl_fb1`：LVGL 绘制缓冲 1
- `user_lvgl_lcdc_fb`：LCDC 实际扫描输出缓冲

其中：
- `user_lvgl_fb0 / user_lvgl_fb1` 组成 **双缓冲绘制面**
- `user_lvgl_lcdc_fb` 作为 **稳定前台显示面**
- PDMA 将脏区域从当前绘制缓冲搬运到扫描缓冲，兼顾性能与显示稳定性

### 子层缓冲区

每个子层都各自分配两块独立缓冲，例如：

- `layer2_buf0` / `layer2_buf1`
- `layer3_buf0` / `layer3_buf1`
- ...
- `layer8_buf0` / `layer8_buf1`

这些缓冲统一具备以下特征：
- 放置在 `.framebuffer` 段
- 按 `HPM_L1C_CACHELINE_SIZE` 对齐
- 像素格式统一为 `ARGB8888`
- flush 时通过 `lcdc_layer_set_next_buffer()` 切换到下一帧

### 为什么这样选 buffer

- **主层面积大**：更适合 Direct Render + PDMA 脏区拷贝
- **子层面积小**：更适合 Full Render + 独立双缓冲翻转
- **全层独立**：任一局部动画不会迫使其它层重绘
- **带宽更可控**：高频动画仅占用对应子层带宽
- **结构更清晰**：显示问题更容易按层定位和调试

## 运行现象

### 系统启动后

启动后可看到一个典型数字仪表盘界面，包含：
- 中央速度数字显示
- 左右双圆形仪表指针动画
- 左右竖向能量条动态变化
- 顶部左右转向灯闪烁
- 底部灯光/安全带/手刹等图标状态显示
- 中央档位滚轮和档位标签切换

### 动画表现

- 速度与转速指针可平滑转动
- 顶部转向灯按定时器节奏闪烁
- 底部告警图标可按组显示/隐藏
- 左右能量条独立执行高度和颜色动画

## 方案价值

### 适用场景
- 车载仪表盘
- 智能座舱 HMI
- 需要硬件层叠加优化的显示系统

### 设计收益
- **减少整屏刷新压力**
- **提升动画流畅度**
- **降低无效像素搬运**
- **更适合大分辨率 ARGB8888 显示场景**
- **便于后续扩展更多局部动态控件**

