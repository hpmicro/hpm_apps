#ifndef __TIMER_LED_H
#define __TIMER_LED_H

#include "hpm_soc.h"
#include "hpm_clock_drv.h"

/**
 * @brief LED SCREEN Driver interface; LED 大屏驱动接口
 * @addtogroup LED_API
 * @{
 *
 */

/**
 * @brief RGB UCS1903 Waveform freq
 *        ;RGB UCS1903 波形频率
 */
#define UCS1903_PWM_FREQ (800000) // 1250ns, 800khz //782150

/**
 * @brief RGB UCS1903 Waveform segments count
 *        ;RGB UCS1903 波形分段总数
 */
#define UCS1903_SPLIT_COUNT (5) // 1250/5 = 250ns

/**
 * @brief RGB UCS1903 Driver Timer freq
 *        ;RGB UCS1903 驱动定时器频率
 */
#define UCS1903_TIMER_FREQ (UCS1903_PWM_FREQ * UCS1903_SPLIT_COUNT)

/**
 * @brief RGB UCS1903 one led bit count
 *        ;RGB UCS1903 每个LED BIT 数
 */
#define USC1903_ONELED_BIT (24)

/**
 * @brief RGB UCS1903 channel count， max 32
 *        ;RGB UCS1903 通道总数，最大 32 路
 */
#define USC1903_CHANNEL_COUNT (8) // maxsize 32

#define UCS1903_LED_COUNT (720)

/**
 * @brief RGB UCS1903 data count
 *        ;RGB UCS1903 数据总数
 */
#define UCS1903_DATA_COUNT (UCS1903_SPLIT_COUNT * USC1903_ONELED_BIT * UCS1903_LED_COUNT)

#define DATA_PLACEHOLDER_COUNT (1)

/**
 * @brief RGB UCS1903 1 data of hige level segments count
 *        ;RGB UCS1903 1 数据 高电平段数总数
 */
#define UCS1903_1_HIGH_COUNT (4)

/**
 * @brief RGB UCS1903 0 data of hige level segments count
 *        ;RGB UCS1903 0 数据 高电平段数总数
 */
#define USC1903_0_HIGH_COUNT (2)

/**
 * @brief RGB UCS1903 usb gptmr
 *        ;RGB UCS1903 使用的GPTMR
 */
#define USC1903_USE_GPTIMER (HPM_GPTMR0)

/**
 * @brief RGB UCS1903 usb gptmr channel
 *        ;RGB UCS1903 使用的gptmr 通道
 */
#define USC1903_USE_GPTIMER_CH (1)

/**
 * @brief RGB UCS1903 usb gptmr irq
 *        ;RGB UCS1903 使用的gptmr 中断
 */
#define USC1903_USE_GPTIMER_IRQ IRQn_GPTMR0

/**
 * @brief RGB UCS1903 usb gptmr irq clock name
 *        ;RGB UCS1903 使用的gptmr 中断时钟名称
 */
#define USC1903_USE_GPTIMER_CLK_NAME clock_gptmr0

/**
 * @brief RGB UCS1903 usb dma base
 *        ;RGB UCS1903 使用的DMA
 */
#define USC1903_USE_DMA (HPM_HDMA)

/**
 * @brief RGB UCS1903 usb dma channel
 *        ;RGB UCS1903 使用的DMA 通道
 */
#define USC1903_USE_DMA_CHANN (0)

/**
 * @brief RGB UCS1903 usb dma irq
 *        ;RGB UCS1903 使用的DMA 中断
 */
#define USC1903_USE_DMA_IRQ IRQn_HDMA

/**
 * @brief RGB UCS1903 usb dmamux
 *        ;RGB UCS1903 使用的DMAMUX
 */
#define USC1903_USE_DMAMUX (HPM_DMAMUX)

/**
 * @brief RGB UCS1903 usb dmamux channel
 *        ;RGB UCS1903 使用的DMAMUX 通道
 */
#define USC1903_USE_DMAMUX_CHANN (DMAMUX_MUXCFG_HDMA_MUX0)

/**
 * @brief RGB UCS1903 usb dmamux source
 *        ;RGB UCS1903 使用的DMAMUX 源
 */
#define USC1903_USE_DMAMUX_DEST (HPM_DMA_SRC_GPTMR0_1)

/**
 * @brief RGB UCS1903 usb gpio control
 *        ;RGB UCS1903 使用的GPIO控制器
 */
#define USC1903_USE_GPIO (HPM_GPIO0)

/**
 * @brief ucs1903 gpio pinmux init,  max 32 pin
 *        ;ucs1903 使用的管脚PINMUX初始化, 最大使用32个管脚
 *
 * @param [none]; 无
 *
 * @retval none; 无
 */
void ucs1903_init_gpio_pins(void);

/**
 * @brief ucs1903 gpio init
 *        ;ucs1903 使用的管脚GPIO初始化
 *
 * @param [none]; 无
 *
 * @retval none; 无
 */
void ucs1903_gpio_init(void);

/**
 * @brief ucs1903 gptmr config
 *        ;ucs1903 使用的gptmr定时器配置
 *
 * @param [none]; 无
 *
 * @retval none; 无
 */
void ucs1903_timer_config(void);

/**
 * @brief ucs1903 dma config
 *        ;ucs1903 使用的DMA配置
 *
 * @param [none]; 无
 *
 * @retval none; 无
 */
void ucs1903_dma_config(void);

/**
 * @brief ucs1903 RGB data convert to Waveform data
 *        ;ucs1903 RGB 数据转换成发波的数据
 *
 * @param [in] count Count for the rgb data ;RGB数据总数
 * @param [in] data  rgb data point; RGB数据指针
 *
 * @retval none; 无
 */
void ucs1903_rgb_convert(uint32_t count, const uint8_t data[][3]);

/**
 * @brief ucs1903 dma start transfer
 *        ;ucs1903 dma启动发波
 *
 * @param [none] 无
 *
 * @retval none; 无
 */
void ucs1903_dma_transfer(void);

/**
 * @brief ucs1903 dma send data
 *        ;ucs1903 dma 发送数据
 *
 * @param [in] r RGB R data ;RGB 红色数值
 * @param [in] g  RGB G data;RGB 绿色数值
 * @param [in] b  RGB B data;RGB 蓝色数值
 * @param [in] bright  LED bright data; LED亮度数值
 * @param [in] lenght  LED channel count; LED灯珠个数，default 720
 * @param [in] flag  test data flag; 测试数据标识
 *
 * @retval none; 无
 */
void ucs1903_dma_send(int r, int g, int b, unsigned char bright, int length, unsigned char flag);

/** @} */

#endif //__TIMER_LED_H