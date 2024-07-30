/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MP_COMMON_H
#define MP_COMMON_H

#include <stdio.h>
#include "hpm_common.h"
#include "hpm_clock_drv.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"
#include "pinmux.h"
#include "mp_config.h"

/**
 * @brief HPM Power common interface;电源通用接口
 * @addtogroup POWER_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

#ifdef HDMA_ADC_CHAIN_DOUBLE_BUF
#ifndef HDMA_ADC_DOUBLE_BUF
#define HDMA_ADC_DOUBLE_BUF
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PWM type(one channel or one pair)
 *        ;PWM类型(单路或一对)
 */
typedef enum
{
    /** @brief one channel  ;单路 */
    MP_PWM_SINGLE_TYPE = 0,
    /** @brief one pair    ;一对 */
    MP_PWM_PAIR_TYPE,
    MP_PWM_TYPE_MAX
}mp_pwm_type;

/**
 * @brief a pair of PWM index number(an abstract MAP sequence number)
 *        ; PWM对序号(MAP抽象序号)
 */
typedef enum
{
    MP_PWM_PAIR_NUM_0 = 0,
    MP_PWM_PAIR_NUM_1,
    MP_PWM_PAIR_NUM_2,
    MP_PWM_PAIR_NUM_3,
    MP_PWM_PAIR_NUM_4,
    MP_PWM_PAIR_NUM_5,
    MP_PWM_PAIR_NUM_6,
    MP_PWM_PAIR_NUM_7,
    MP_PWM_PAIR_NUM_8,
    MP_PWM_PAIR_NUM_9,
    MP_PWM_PAIR_NUM_10,
    MP_PWM_PAIR_NUM_11,
    MP_PWM_PAIR_NUM_12,
    MP_PWM_PAIR_NUM_13,
    MP_PWM_PAIR_NUM_14,
    MP_PWM_PAIR_NUM_15,
    MP_PWM_PAIR_NUM_MAX
} mp_pwm_pair_index;

/**
 * @brief Summation of a pair of PWM sequence numbers
 *        ;PWM对序号总和
 */
#define MP_PWM_PAIR_COUNT    (MP_PWM_PAIR_NUM_MAX)

/**
 * @brief The PWM index number(an abstract MAP sequence number)
 *        ;PWM序号(MAP抽象序号)
 */
typedef enum
{
    MP_PWM_NUM_0 = 0,
    MP_PWM_NUM_1,
    MP_PWM_NUM_2,
    MP_PWM_NUM_3,
    MP_PWM_NUM_4,
    MP_PWM_NUM_5,
    MP_PWM_NUM_6,
    MP_PWM_NUM_7,
    MP_PWM_NUM_MAX
} mp_pwm_index;

/**
 * @brief  ADC index number(an abstract MAP sequence number)
 *         ;ADC序号(MAP抽象序号)
 */
typedef enum
{
    MP_ADC_NUM_0 = 0,
    MP_ADC_NUM_1,
    MP_ADC_NUM_2,
    MP_ADC_NUM_3,
    MP_ADC_NUM_4,
    MP_ADC_NUM_5,
    MP_ADC_NUM_6,
    MP_ADC_NUM_7,
    MP_ADC_NUM_8,
    MP_ADC_NUM_9,
    MP_ADC_NUM_10,
    MP_ADC_NUM_11,
    MP_ADC_NUM_12,
    MP_ADC_NUM_13,
    MP_ADC_NUM_14,
    MP_ADC_NUM_15,
    MP_ADC_NUM_16,
    MP_ADC_NUM_17,
    MP_ADC_NUM_18,
    MP_ADC_NUM_19,
    MP_ADC_NUM_20,
    MP_ADC_NUM_21,
    MP_ADC_NUM_22,
    MP_ADC_NUM_23,
    MP_ADC_NUM_MAX
} mp_adc_index;

/**
 * @brief The type of a pair of PWM(complementary mode, phase center aligned mode, edge aligned mode)
 *        ;PWM对模式类型(互补模式; 相位中心对齐模式; 边沿对齐模式)
 */
typedef enum
{
    /** @brief complementary mode   ;互补模式 */
    MP_PWM_PAIR_MODE_REVERSE = 0,
    /** @brief phase center aligned mode    ;相位中心对齐模式 */
    MP_PWM_PAIR_MODE_CENTRAL,
    /** @brief edge aligned mode    ;边沿对齐模式 */
    MP_PWM_PAIR_MODE_EDGE,
    MP_PWM_PAIR_MODE_MAX
} mp_pwm_pair_mode;

/**
 * @brief The phase Angle of a pair of PWM in phase center aligned mode
 *        ;PWM对相位中心对齐模式相位角度
 */
typedef enum
{
    /** @brief 0 angle  ;0度 */
    MP_PWM_PAIR_PHASE_ANGLE_0 = 0,
    /** @brief 180 angle ;180度 */
    MP_PWM_PAIR_PHASE_ANGLE_180 = 2,
    /** @brief 120 angle ;120度 */
    MP_PWM_PAIR_PHASE_ANGLE_120 = 3,
    /** @brief 90 angle  ;90度 */
    MP_PWM_PAIR_PHASE_ANGLE_90 = 4,
    /** @brief 72 angle  ;72度 */
    MP_PWM_PAIR_PHASE_ANGLE_72 = 5,
    /** @brief 60 angle  ;60度 */
    MP_PWM_PAIR_PHASE_ANGLE_60 = 6,
    /** @brief 45 angle  ;45度 */
    MP_PWM_PAIR_PHASE_ANGLE_45 = 8,
    /** @brief 40 angle  ;40度 */
    MP_PWM_PAIR_PHASE_ANGLE_40 = 9,
    /** @brief 36 angle  ;36度 */
    MP_PWM_PAIR_PHASE_ANGLE_36 = 10,
    /** @brief 30 angle  ;30度 */
    MP_PWM_PAIR_PHASE_ANGLE_30 = 12
} mp_pwm_pair_phase_angle;

/**
 * @brief PWM TRGM mode(Output mode; DMA request mode)
 *        Output mode:TRGM output IO signal
 *        DMA request mode:TRGM output DMA request and then trigger DMA
 *        ;PWM互联触发模式(输出模式;DMA请求模式)
 *        输出模式: 互联输出IO信号
 *        DMA请求模式: 互联输出DMA请求，触发DMA
 */
typedef enum
{
    /** @brief Output mode  ;输出模式 */
    MP_PWM_TRIGGER_MODE_OUTPUT = 0,
    /** @brief DMA request mode   ;DMA请求模式 */
    MP_PWM_TRIGGER_MODE_DMA,
} mp_pwm_trigger_mode;

/**
 * @brief PWM trigger count max   ;PWM互联触发最大总和
 */
#define MP_PWM_TRIGGER_COUNT_MAX    (8)

/**
 * @brief PWM trigger for ADC bit
 *        ;PWM互联触发ADC采样BIT位
 */
#define MP_PWM_TRIGGER_ADC_BIT      (0)

/**
 * @brief ADC MAP data structure 
 *        ;ADC MAP数据结构
 */
typedef struct
{
    /** @brief ADC index number(an abstract MAP sequence number)    
     *         ;ADC序号(MAP抽象序号) */
    uint8_t adc_index;
    /** @brief PIN I/O
     *         ;PIN脚 */
    uint16_t io;
    /** @brief ADC manager base address 
     *         ;ADC管理器基地址 */
    ADC16_Type* adc16;
    /** @brief ADC channel 
     *         ;ADC通道 */
    uint8_t adc_ch;
} mp_adc_map_t;

/**
 * @brief PWM TRGM MAP data structure 
 *        ;PWM 互联触发MAP数据结构
 */
typedef struct
{
    /** @brief mode @ref mp_pwm_trigger_mode
     *        ;模式  @ref mp_pwm_trigger_mode */
    uint8_t mode;
    /** @brief PWM TRGM comparator
     *         ;PWM互联触发比较器 */
    uint8_t trgm_cmp;
    /** @brief PWM TRGM channel
     *         ;PWM互联触发通道 */
    uint8_t trgm_pin;
    /** @brief TRGM base address
     *         ;互联管理器基地址 */
    TRGM_Type* trgm_base;
    /** @brief TRGM input
     *         ;互联触发源信号 */
    uint8_t trgm_input;
    /** @brief TRGM output 
     *         ;互联触发目标信号 */
    uint8_t trgm_output;
} mp_pwm_trigger_map_t;

/**
 * @brief PWM fault protection for MAP data structure 
 *        ;PWM 故障保护MAP数据结构
 */
#pragma pack(1)
typedef struct   
{
    /** @brief 4 internal faults; 2 external faults pwm_fault_source_t 
     *         ;4个内部故障; 2个外部故障 pwm_fault_source_t */
    uint32_t fault_mask;
    union
    {
        /** @brief internal fault data structure 
         *         ;内部故障数据结构 */
        struct in_fault_t
        {
            /** @brief Fault triggered level: 1 for high, 0 for low. Internally, the default is set to 1 for high, configured through TRGM inversion
             *         ;故障触发的电平：1为高，0为低。内部默认为1高，通过互联反转来配置 */
            uint8_t fault_level;
            /** @brief Fault recovery level:1 for high, 0 for low
             *         ;故障恢复的电平：1为高，0为低 */
            uint8_t recover_level;
            /** @brief TRGM base address
             *         ;互联管理器基地址 */
            TRGM_Type* trgm_base;
            /** @brief TRGM input
             *         ;互联触发源信号 */
            uint8_t trgm_input;
            /** @brief TRGM output 
             *        ;互联触发目标信号 */     
            uint8_t trgm_output;
        } in_fault;
        /** @brief External fault data structure
         *         ;外部故障数据结构 */
        struct ex_fault_t
        {
            /** @brief  Fault triggered level: 1 for high, 0 for low
             *          ;故障触发的电平：1为高，0为低 */
            uint8_t fault_level;
            /** @brief Fault recovery level:1 for high, 0 for low
             *         ;故障恢复的电平：1为高，0为低 */
            uint8_t recover_level;
            /** @brief PIN I/O
             *         ;PIN脚 */
            uint16_t pin;
            /** @brief PIN MUX   ;PIN复用功能 */
            uint32_t mux;            
        } ex_fault;
    };
} mp_pwm_fault_map_t;
#pragma pack()

/**
 * @brief Address of a pair of PWM MAP data structure 
 *        ;PWM 对MAP数据结构
 */
typedef struct
{
    /** @brief a pair of PWM index number(an abstract MAP sequence number) 
     *         ;PWM对序号(MAP抽象序号) */
    uint8_t pair_index;
    /** @brief a pair of PWM,two I/O ports
     *         ;一对两路PWM，两路IO口 */
    struct io_p
    {
        /** @brief PIN I/O
         *         ;PIN脚 */
        uint16_t pin;
        /** @brief PIN MUX 
         *         ;PIN复用功能 */
        uint32_t mux;
    } io[2];
    /** @brief PWM manager base address    
     *         ;PWM管理器基地址 */
    PWM_Type* pwm_x;
    /** @brief PWM comparator
     *         ;PWM 比较器 */
    uint8_t cmp;
    /** @brief PWM output channel 
     *         ;PWM 输出通道 */
    uint8_t pin;
    /** @brief  PWM TRGM MAP data structure  @ref mp_pwm_trigger_map_t 
     *          ;PWM 互联触发MAP数据结构 @ref mp_pwm_trigger_map_t */
    mp_pwm_trigger_map_t pwm_trigger_map[MP_PWM_TRIGGER_COUNT_MAX];
    /** @brief  PWM fault protection for MAP data structure  @ref mp_pwm_fault_map_t 
     *          ;PWM 故障保护MAP数据结构 @ref mp_pwm_fault_map_t */
    mp_pwm_fault_map_t pwm_fault_map;
} mp_pwm_pair_map_t;

/**
 * @brief Address of the PWM MAP data structure   ;PWM MAP数据结构
 */
typedef struct
{
    /** @brief The PWM index number(an abstract MAP sequence number) 
     *         ;PWM 序号(MAP抽象序号) */
    uint8_t index;
    struct io_t
    {
        /** @brief PIN I/O
         *         ;PIN脚 */
        uint16_t pin;
        /** @brief PIN MUX 
         *         ;PIN复用功能 */
        uint32_t mux;
    } io;
    /** @brief PWM manager base address 
     *         ;PWM管理器基地址 */
    PWM_Type* pwm_x;
    /** @brief PWM comparator
     *         ;PWM 比较器 */
    uint8_t cmp;
    /** @brief PWM output channel 
     *         ;PWM 输出通道 */
    uint8_t pin;
    /** @brief PWM TRGM MAP data structure @ref mp_pwm_trigger_map_t 
     *          ;互联触发MAP数据结构 @ref mp_pwm_trigger_map_t */
    mp_pwm_trigger_map_t pwm_trigger_map[MP_PWM_TRIGGER_COUNT_MAX];
    /** @brief PWM fault protection for MAP data structure @ref mp_pwm_fault_map_t
     *         ;PWM 故障保护MAP数据结构 @ref mp_pwm_fault_map_t */
    mp_pwm_fault_map_t pwm_fault_map;
} mp_pwm_map_t;

/**
 * @brief ADC callback after sampling ends
 *        ;ADC 采样结束回调
 */
typedef void (*hpm_mp_adc_over_callback)(void* handle);

/**
 * @brief ADC data structure
 *        ;ADC 数据结构
 */
typedef struct
{
    /** @brief ADC index number(an abstract MAP sequence number)
     *        ;ADC 序号(MAP抽象序号) */
    uint8_t adc_index; 
    /** @brief ADC MAP index 
     *         ;ADC MAP数组序号 */
    uint8_t adc_map_index;
    /** @brief ADC Interrupt index(internal use)
     *         ;ADC 中断序号(内部使用) */
    uint8_t adc_irq_index;
    /** @brief ADC preemption trigger(internal use)
     *         ;ADC 抢占触发通道(内部使用) */
    uint8_t adc_trig_ch;
    /** @brief ADC preemption trigger index(internal use)
     *         ;ADC 抢占触发序号(内部使用) */
    uint8_t adc_trig_index;
    /** @brief ADC data 16 bit
     *         ;ADC 采样值 16bit */
    uint16_t adc_data;
    /** @brief ADC BUF address(internal use)
     *         ;ADC BUF地址(内部使用) */ 
    uint32_t adc_buff_addr;
    /** @brief ADC BUF reserve(internal use)
     *         ;ADC BUF扩展(内部使用) */
    uint32_t adc_buff_reserve[2];
} hpm_adc_t;

/**
 * @brief HDMA ADC data structure, using HDMA to transfer ADC sampling values. This method is typically employed for sampling a set of data at a fixed frequency, such as collecting a group of points, for instance, 512/1024/4096.
 *        ;HDMA ADC 数据结构, 使用HDMA搬用ADC采样值，一般应用于固定频率采样一组(例如：512/1024/4096个点)数据
 */
typedef struct
{
    /** @brief Sum total of ADC 
     *         ;ADC 数量总和 */
    uint8_t adc_count;
    /** @brief ADC data structure pointer
     *         ;ADC 数据结构指针 */
    hpm_adc_t* adc_pack;
} hpm_hdma_adc_t;

/**
 * @brief HDMA ADC data transmission and callback completed
 *        ;HDMA ADC数据搬用完成回调
 */
typedef void (*hpm_mp_hdma_over_callback)(hpm_hdma_adc_t* hdma_adc_t);

/**
 * @brief PWM fault protection data structure
 *        ;PWM 故障保护数据结构
 */
typedef struct
{
    /** @brief  When a fault occurs, the PWM signal forced output is: 0- low level; 1- High level; 2- High resistance state
     *          ;故障发生时, PWM强制输出为: 0-低电平; 1-高电平; 2-高阻态; */
    uint8_t fault_level;
    /** @brief 0-Fault clears immediately upon resolution; 1-Fault clears upon the next reload after resolution; 2-Fault clears upon hardware-triggered recovery; 3-Fault clears upon software-triggered recovery
     *         ;0-故障解除立即恢复; 1-故障解除下一个reload恢复; 2-故障解除硬件触发恢复; 3-软件触发恢复; */
    uint8_t recovery_trigger;
} hpm_pwm_fault_t;

/**
 * @brief A pair of PWM data structure
 *        ;PWM 对数据结构
 */
typedef struct
{
    /** @brief A pair of PWM index number(an abstract MAP sequence number)
     *         ;PWM 对序号(MAP抽象序号) */
    uint8_t pwm_pair_index;
    /** @brief A pair of PWM MAP index number
     *         ;PWM 对MAP数组序号 */
    uint8_t pwm_map_index;
    /** @brief A pair of PWM mode @ref mp_pwm_pair_mode
     *         ;PWM 对模式 @ref mp_pwm_pair_mode */
    uint8_t pwm_pair_mode;
    /** @brief A pair of PWM phase angles(used in phase center aligned mode and edge aligned mode) @ref mp_pwm_pair_phase_angle 
     *         ;PWM 对相位角度(中心对齐和边沿对齐时用到) @ref mp_pwm_pair_phase_angle */
    uint8_t phase_angle;
    /** @brief  Starting time for a pair of PWM dead zone insertions. The unit is half of the PWM bus frequency 
     *         ;PWM 对死区插入起始时间,单位为PWM总线的半周期 */
    uint32_t deadzone_start_halfcycle;
    /** @brief  Ending time for a pair of PWM dead zone insertions. The unit is half of the PWM bus frequency 
     *          ;PWM 对死区插入结束时间，单位为PWM总线的半周期 */
    uint32_t deadzone_end_halfcycle;
    /** @brief a pair of PWM frequncy 
     *         ;PWM 对频率 */
    uint32_t pwm_freq;
    /** @brief a pair of PWM bus frequncy(internal use)
     *         ;PWM 对总线频率(内部使用) */
    uint32_t bus_freq;
    /** @brief A pair of PWM reload_count, obtained by 'get default', cannot be changed
     *         ;PWM 对reload_count, get default获得, 不可更改*/               
    uint32_t reload_count;
    /** @brief A pair of PWM duty count, (0..reload_count)
     *         ;PWM 对占空比, (0..reload_count) */
    uint32_t duty_count;
    /** @brief  PWM duty cycle at the moment when the shadow register takes effect, (0..reload_count)
     *          ;PWM 影子寄存器生效时刻占空比, (0..reload_count) */
    uint32_t shadow_dutycount;
    /** @brief Enable fault protection for a pair of PWM(true-enable; false-disable)
     *         ;PWM 对故障保护使能(true-使能; false-禁用) */
    bool fault_enable;
    /** @brief A pair of PWM fault protection data structures @ref hpm_pwm_fault_t
     *         ;PWM 对故障保护数据结构 @ref hpm_pwm_fault_t */
    hpm_pwm_fault_t pwm_fault[2];
    /** @brief The sum total of a pair of PWM TRGM
     *         ;PWM 对互联触发总和 */
    uint8_t trigger_count;
    /** @brief Enable a pair of PWM TRGM(Each bit represents once trigger enable)
     *         ;PWM 对互联触发使能(每个bit代表一个触发使能) */
    uint8_t trigger_enable;
    /** @brief PWM duty cycle at the moment when the ADC triggered, (0..reload_count)
     *         ;PWM 互联触发ADC时刻占空比, (0..reload_count) */
    uint32_t trigger_dutycount[MP_PWM_TRIGGER_COUNT_MAX];
    /** @brief The sum of the ADC sampled by the PWM triggered ADC
     *         ;PWM 触发ADC采样的ADC总和 */
    uint8_t adc_count;
    /** @brief The ADC channel number that enables ADC interrupts after the completion of ADC sampling (regardless of the number of ADC channels sampled, only one interrupt response is required in the end)
     *         ;ADC采样结束后使能ADC中断的ADC序号(不论多少个ADC采样, 最后只需一个中断响应即可) */
    uint8_t adc_intr_index;
    /** @brief ADC data structure @ref hpm_adc_t
     *         ;ADC 数据结构 @ref hpm_adc_t */
    hpm_adc_t* adc_pack;
    /** @brief ADC callback after the end of sampling
     *         ;ADC采样结束后回调 */
    hpm_mp_adc_over_callback adc_over_callback;
    /** @brief ADC handle(internal use)
     *         ;ADC handle(内部使用) */
    void* adc_handle;
} hpm_pwm_pair_t;

/**
 * @brief PWM data structure  
 *        ;PWM 数据结构
 */
typedef struct
{
    /** @brief PWM index number(an abstract MAP sequence number)
     *         ;PWM 序号(MAP抽象序号) */
    uint8_t pwm_index;
    /** @brief PWM MAP index number
     *         ;PWM MAP数组序号 */
    uint8_t pwm_map_index;
    /** @brief PWM frequency
     *         ;PWM 频率 */
    uint32_t pwm_freq;
    /** @brief PWM bus frequncy(internal use)
     *         ;PWM 总线频率(内部使用) */
    uint32_t bus_freq;
    /** @brief PWM reload_count, obtained by 'get default', cannot be changed
     *         ;PWM reload_count, get default获得, 不可更改*/
    uint32_t reload_count;
    /** @brief PWM duty count, (0..reload_count)
     *         ;PWM 占空比, (0..reload_count) */
    uint32_t duty_count;
    /** @brief PWM duty cycle at the moment when the shadow register takes effect, (0..reload_count)
     *         ;PWM 影子寄存器生效时刻占空比, (0..reload_count) */
    uint32_t shadow_dutycount;
    /** @brief Enable fault protection for the PWM(true-enable; false-disable)
     *         ;PWM 故障保护使能(true-使能; false-禁用) */
    bool fault_enable;
    /** @brief PWM fault protection data structures @ref hpm_pwm_fault_t
     *         ;PWM 故障保护数据结构 @ref hpm_pwm_fault_t */
    hpm_pwm_fault_t pwm_fault;
    /** @brief The sum total of the PWM TRGM
     *         ;PWM 互联触发总和 */
    uint8_t trigger_count;
    /** @brief Enable the PWM TRGM(Each bit represents once trigger enable)
     *         ;PWM 互联触发使能(每个bit代表一个触发使能) */
    uint8_t trigger_enable;
    /** @brief  PWM duty cycle at the moment when the ADC triggered, (0..reload_count)
     *          ;PWM 互联触发ADC时刻占空比, (0..reload_count) */
    uint32_t trigger_dutycount[MP_PWM_TRIGGER_COUNT_MAX];
    /** @brief The sum of the ADC sampled by the PWM triggered ADC
     *         ;PWM 触发ADC采样的ADC总和 */
    uint8_t adc_count;
    /** @brief The ADC channel number that enables ADC interrupts after the completion of ADC sampling (regardless of the number of ADC channels sampled, only one interrupt response is required in the end)
     *         ;ADC采样结束后使能ADC中断的ADC序号(不论多少个ADC采样, 最后只需一个中断响应即可) */
    uint8_t adc_intr_index;
    /** @brief  ADC data structure @ref hpm_adc_t
     *          ;ADC 数据结构 @ref hpm_adc_t */
    hpm_adc_t* adc_pack;
    /** @brief ADC callback after the end of sampling
     *         ;ADC采样结束后回调 */
    hpm_mp_adc_over_callback adc_over_callback;
    /** @brief ADC handle(internal use)
     *         ;ADC handle(内部使用) */
    void* adc_handle;
} hpm_pwm_t;

/**
 * @brief Power MP data structure  
 *        ;电源MP数据结构
 */
typedef struct
{
    /** @brief The sum total of the pair of PWM
     *         ;PWM对 总和 */
    uint8_t pwm_pair_count;
    /** @brief Synchronization time of a pair of PWM; ==0- Fully synchronized; >0- phase difference time;
     *         ;PWM对 同步时间; ==0-完全同步; >0-相位差时间; */
    uint32_t pwm_sync_time_us;
    /** @brief A pair of PWM data structure Pointers
     *         ;PWM对 数据结构指针 */
    hpm_pwm_pair_t* pwm_pair;
} hpm_mp_t;

/**
 * @brief High precision timer callback
 *        ;高精定时回调
 */
typedef void (*mp_timer_cb)(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //MP_COMMON_H