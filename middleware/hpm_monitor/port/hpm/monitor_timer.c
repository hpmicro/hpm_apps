#include "monitor_kconfig.h"
#include "monitor_log.h"
#include "hpm_gptmr_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "monitor_timer.h"

#define MONITOR_TIMER_CTX &MONITOR_PRIVATE_VAR_NAME(monitor_timer);
typedef struct
{
    uint8_t is_loop;
    uint8_t is_running;
    uint8_t dma_ch;
    sample_result_cb monitor_sample_cb;
    montiro_report_data_t *monitor_data;
} monitor_timer_t;

static monitor_timer_t MONITOR_PRIVATE_VAR_NAME(monitor_timer) = {0};

ATTR_RAMFUNC
#ifdef HPM_SOC_HAS_HPMSDK_DMAV2
static void monitor_isr_dma(DMAV2_Type *dma_base)
#else
static void monitor_isr_dma(DMA_Type *dma_base)
#endif
{
    monitor_timer_t *ctx = MONITOR_TIMER_CTX;
    volatile hpm_stat_t stat;
    uint8_t index;
    index = ctx->monitor_data->sample.mem_cur_index;
    stat = dma_check_transfer_status(dma_base, ctx->dma_ch);
    if ((stat & DMA_CHANNEL_STATUS_TC))
    {
        ctx->is_running = 0;
        if (ctx->is_loop)
        {
            if (ctx->monitor_data->sample.mem_cur_index >= ctx->monitor_data->sample.mem_count - 1)
            {
                ctx->monitor_data->sample.mem_cur_index = 0;
            }
            else
            {
                ctx->monitor_data->sample.mem_cur_index++;
            }
            if (monitor_report_addr_is_released(ctx->monitor_data->mem_addr[ctx->monitor_data->channel.pkt_count + ctx->monitor_data->sample.mem_cur_index]))
            {
                monitor_sample_dma_config();
                monitor_sample_dma_start();
            }
        }
        else
        {
            monitor_timer_stop();
        }
        if (ctx->monitor_sample_cb != NULL)
        {
            ctx->monitor_sample_cb(0, index);
        }
    }
    else if (stat & DMA_CHANNEL_STATUS_ERROR)
    {
        ctx->is_running = 0;
        monitor_timer_stop();
        MONITOR_LOG_ERR("dma error!\r\n");
        if (ctx->monitor_sample_cb != NULL)
        {
            ctx->monitor_sample_cb(-2, index); //-1 is lost
        }
    }
    else if (stat & DMA_CHANNEL_STATUS_ABORT)
    {
        ctx->is_running = 0;
        monitor_timer_stop();
        MONITOR_LOG_ERR("dma abort!\r\n");
        if (ctx->monitor_sample_cb != NULL)
        {
            ctx->monitor_sample_cb(-3, index);
        }
    }
}

#if (defined(MONITOR_SAMPLE_DMA_1_BASE) && (MONITOR_SAMPLE_DMA_1_BASE == HPM_XDMA_BASE)) || defined(MONITOR_SAMPLE_DMA_2_BASE) && (MONITOR_SAMPLE_DMA_2_BASE == HPM_XDMA_BASE)
static void monitor_isr_xdma(void)
{
    monitor_isr_dma(HPM_XDMA);
}
#endif
#if (defined(MONITOR_SAMPLE_DMA_1_BASE) && (MONITOR_SAMPLE_DMA_1_BASE == HPM_HDMA_BASE)) || defined(MONITOR_SAMPLE_DMA_2_BASE) && (MONITOR_SAMPLE_DMA_2_BASE == HPM_HDMA_BASE)
static void monitor_isr_hdma(void)
{
    monitor_isr_dma(HPM_HDMA);
}
#endif

#if defined(MONITOR_SAMPLE_DMA_1_BASE) && (MONITOR_SAMPLE_DMA_1_BASE == HPM_XDMA_BASE)
SDK_DECLARE_EXT_ISR_M(IRQn_XDMA, monitor_isr_xdma)
#elif defined(MONITOR_SAMPLE_DMA_1_BASE) && (MONITOR_SAMPLE_DMA_1_BASE == HPM_HDMA_BASE)
SDK_DECLARE_EXT_ISR_M(IRQn_HDMA, monitor_isr_hdma)
#endif

#if defined(MONITOR_SAMPLE_DMA_2_BASE) && (MONITOR_SAMPLE_DMA_2_BASE == HPM_XDMA_BASE)
SDK_DECLARE_EXT_ISR_M(IRQn_XDMA, monitor_isr_xdma)
#elif defined(MONITOR_SAMPLE_DMA_2_BASE) && (MONITOR_SAMPLE_DMA_2_BASE == HPM_HDMA_BASE)
SDK_DECLARE_EXT_ISR_M(IRQn_HDMA, monitor_isr_hdma)
#endif

#define MONITOR_GET_GPTMR_SRC(base, i) \
    ((base) == HPM_GPTMR0 ? (HPM_DMA_SRC_GPTMR0_0 + (i)) : ((base) == HPM_GPTMR1 ? (HPM_DMA_SRC_GPTMR1_0 + (i)) : ((base) == HPM_GPTMR2 ? (HPM_DMA_SRC_GPTMR2_0 + (i)) : ((base) == HPM_GPTMR3 ? (HPM_DMA_SRC_GPTMR3_0 + (i)) : 0))))

static uint32_t monitor_get_dmamux_trgm_src(uint8_t ch)
{
    if (ch >= MONITOR_STREAM_BUFFER_MAXCOUNT)
    {
        return 0;
    }
#ifdef MONITOR_SAMPLE_GPTMR_1_BASE
    if (ch < 4)
    {
        return MONITOR_GET_GPTMR_SRC(MONITOR_SAMPLE_GPTMR_1_BASE, ch);
    }
#endif
#ifdef MONITOR_SAMPLE_GPTMR_2_BASE
    else if (ch < 8)
    {
        return MONITOR_GET_GPTMR_SRC(MONITOR_SAMPLE_GPTMR_2_BASE, ch - 4);
    }
#endif
#ifdef MONITOR_SAMPLE_GPTMR_3_BASE
    else if (ch < 12)
    {
        return MONITOR_GET_GPTMR_SRC(MONITOR_SAMPLE_GPTMR_3_BASE, ch - 8);
    }
#endif
#ifdef MONITOR_SAMPLE_GPTMR_4_BASE
    else if (ch < 16)
    {
        return MONITOR_GET_GPTMR_SRC(MONITOR_SAMPLE_GPTMR_4_BASE, ch - 12);
    }
#endif
    return 0;
}

void monitor_timer_config(uint32_t freq_hz, uint32_t count)
{
    gptmr_channel_config_t config;
    uint32_t gptmr_freq, i;
    uint32_t cur_count = 0;
#ifdef MONITOR_SAMPLE_GPTMR_1_BASE
    gptmr_channel_get_default_config(MONITOR_SAMPLE_GPTMR_1_BASE, &config);
    gptmr_freq = clock_get_frequency(MONITOR_SAMPLE_GPTMR_1_CLOCK);
    config.reload = gptmr_freq / freq_hz;
    config.cmp_initial_polarity_high = false;
    config.dma_request_event = gptmr_dma_request_on_reload;
    MONITOR_LOG_INFO("monitor tmr1 freq:%dHz, reload:%d\r\n", freq_hz, config.reload);
    for (i = 0; i < 4; i++)
    {
        gptmr_disable_irq(MONITOR_SAMPLE_GPTMR_1_BASE, GPTMR_CH_RLD_IRQ_MASK(i));
        gptmr_channel_config(MONITOR_SAMPLE_GPTMR_1_BASE, i, &config, false);
        gptmr_channel_enable_dma_request(MONITOR_SAMPLE_GPTMR_1_BASE, i, true);
        gptmr_channel_reset_count(MONITOR_SAMPLE_GPTMR_1_BASE, i);
        cur_count++;
        if (cur_count >= count)
        {
            return;
        }
    }
#endif

#ifdef MONITOR_SAMPLE_GPTMR_2_BASE
    gptmr_channel_get_default_config(MONITOR_SAMPLE_GPTMR_2_BASE, &config);
    gptmr_freq = clock_get_frequency(MONITOR_SAMPLE_GPTMR_2_CLOCK);
    config.reload = gptmr_freq / freq_hz;
    config.cmp_initial_polarity_high = false;
    config.dma_request_event = gptmr_dma_request_on_reload;
    MONITOR_LOG_INFO("monitor tmr2 freq:%dHz, reload:%d\r\n", freq_hz, config.reload);
    for (i = 0; i < 4; i++)
    {
        gptmr_disable_irq(MONITOR_SAMPLE_GPTMR_2_BASE, GPTMR_CH_RLD_IRQ_MASK(i));
        gptmr_channel_config(MONITOR_SAMPLE_GPTMR_2_BASE, i, &config, false);
        gptmr_channel_enable_dma_request(MONITOR_SAMPLE_GPTMR_2_BASE, i, true);
        gptmr_channel_reset_count(MONITOR_SAMPLE_GPTMR_2_BASE, i);
        cur_count++;
        if (cur_count >= count)
        {
            return;
        }
    }
#endif

#ifdef MONITOR_SAMPLE_GPTMR_3_BASE
    gptmr_channel_get_default_config(MONITOR_SAMPLE_GPTMR_3_BASE, &config);
    gptmr_freq = clock_get_frequency(MONITOR_SAMPLE_GPTMR_3_CLOCK);
    config.reload = gptmr_freq / freq_hz;
    config.cmp_initial_polarity_high = false;
    config.dma_request_event = gptmr_dma_request_on_reload;
    MONITOR_LOG_INFO("monitor tmr3 freq:%dHz, reload:%d\r\n", freq_hz, config.reload);
    for (i = 0; i < 4; i++)
    {
        gptmr_disable_irq(MONITOR_SAMPLE_GPTMR_3_BASE, GPTMR_CH_RLD_IRQ_MASK(i));
        gptmr_channel_config(MONITOR_SAMPLE_GPTMR_3_BASE, i, &config, false);
        gptmr_channel_enable_dma_request(MONITOR_SAMPLE_GPTMR_3_BASE, i, true);
        gptmr_channel_reset_count(MONITOR_SAMPLE_GPTMR_3_BASE, i);
        cur_count++;
        if (cur_count >= count)
        {
            return;
        }
    }
#endif

#ifdef MONITOR_SAMPLE_GPTMR_4_BASE
    gptmr_channel_get_default_config(MONITOR_SAMPLE_GPTMR_4_BASE, &config);
    gptmr_freq = clock_get_frequency(MONITOR_SAMPLE_GPTMR_4_CLOCK);
    config.reload = gptmr_freq / freq_hz;
    config.cmp_initial_polarity_high = false;
    config.dma_request_event = gptmr_dma_request_on_reload;
    MONITOR_LOG_INFO("monitor tmr4 freq:%dHz, reload:%d\r\n", freq_hz, config.reload);
    for (i = 0; i < 4; i++)
    {
        gptmr_disable_irq(MONITOR_SAMPLE_GPTMR_4_BASE, GPTMR_CH_RLD_IRQ_MASK(i));
        gptmr_channel_config(MONITOR_SAMPLE_GPTMR_4_BASE, i, &config, false);
        gptmr_channel_enable_dma_request(MONITOR_SAMPLE_GPTMR_4_BASE, i, true);
        gptmr_channel_reset_count(MONITOR_SAMPLE_GPTMR_4_BASE, i);
        cur_count++;
        if (cur_count >= count)
        {
            return;
        }
    }
#endif
    if (cur_count < count)
    {
        MONITOR_LOG_ERR("BAD, monitor timer not enough for sample!\r\n");
    }
}

void monitor_timer_stop(void)
{
    int i;
#ifdef MONITOR_SAMPLE_GPTMR_1_BASE
    for (i = 0; i < 4; i++)
    {
        gptmr_stop_counter(MONITOR_SAMPLE_GPTMR_1_BASE, i);
    }
#endif
#ifdef MONITOR_SAMPLE_GPTMR_2_BASE
    for (i = 0; i < 4; i++)
    {
        gptmr_stop_counter(MONITOR_SAMPLE_GPTMR_2_BASE, i);
    }
#endif
#ifdef MONITOR_SAMPLE_GPTMR_3_BASE
    for (i = 0; i < 4; i++)
    {
        gptmr_stop_counter(MONITOR_SAMPLE_GPTMR_3_BASE, i);
    }
#endif
#ifdef MONITOR_SAMPLE_GPTMR_4_BASE
    for (i = 0; i < 4; i++)
    {
        gptmr_stop_counter(MONITOR_SAMPLE_GPTMR_4_BASE, i);
    }
#endif
}

void monitor_timer_start(void)
{
    monitor_timer_t *ctx = MONITOR_TIMER_CTX;
    int i;
    uint32_t cur_count = 0, count;
    if(ctx->monitor_data == NULL)
        return;
    count = ctx->monitor_data->sample.pkt_count;
#ifdef MONITOR_SAMPLE_GPTMR_1_BASE
    for (i = 0; i < 4; i++)
    {
        gptmr_channel_reset_count(MONITOR_SAMPLE_GPTMR_1_BASE, i);
        gptmr_start_counter(MONITOR_SAMPLE_GPTMR_1_BASE, i);
        cur_count++;
    }
    gptmr_trigger_channel_software_sync(MONITOR_SAMPLE_GPTMR_1_BASE, GPTMR_CH_GCR_SWSYNCT_MASK(0) | GPTMR_CH_GCR_SWSYNCT_MASK(1) | GPTMR_CH_GCR_SWSYNCT_MASK(2) | GPTMR_CH_GCR_SWSYNCT_MASK(3));
    if (cur_count >= count)
        return;
#endif

#ifdef MONITOR_SAMPLE_GPTMR_2_BASE
    for (i = 0; i < 4; i++)
    {
        gptmr_channel_reset_count(MONITOR_SAMPLE_GPTMR_2_BASE, i);
        gptmr_start_counter(MONITOR_SAMPLE_GPTMR_2_BASE, i);
        cur_count++;
    }
    gptmr_trigger_channel_software_sync(MONITOR_SAMPLE_GPTMR_2_BASE, GPTMR_CH_GCR_SWSYNCT_MASK(0) | GPTMR_CH_GCR_SWSYNCT_MASK(1) | GPTMR_CH_GCR_SWSYNCT_MASK(2) | GPTMR_CH_GCR_SWSYNCT_MASK(3));
    if (cur_count >= count)
        return;
#endif

#ifdef MONITOR_SAMPLE_GPTMR_3_BASE
    for (i = 0; i < 4; i++)
    {
        gptmr_channel_reset_count(MONITOR_SAMPLE_GPTMR_3_BASE, i);
        gptmr_start_counter(MONITOR_SAMPLE_GPTMR_3_BASE, i);
        cur_count++;
    }
    gptmr_trigger_channel_software_sync(MONITOR_SAMPLE_GPTMR_3_BASE, GPTMR_CH_GCR_SWSYNCT_MASK(0) | GPTMR_CH_GCR_SWSYNCT_MASK(1) | GPTMR_CH_GCR_SWSYNCT_MASK(2) | GPTMR_CH_GCR_SWSYNCT_MASK(3));
    if (cur_count >= count)
        return;
#endif

#ifdef MONITOR_SAMPLE_GPTMR_4_BASE
    for (i = 0; i < 4; i++)
    {
        gptmr_channel_reset_count(MONITOR_SAMPLE_GPTMR_4_BASE, i);
        gptmr_start_counter(MONITOR_SAMPLE_GPTMR_4_BASE, i);
        cur_count++;
    }
    gptmr_trigger_channel_software_sync(MONITOR_SAMPLE_GPTMR_4_BASE, GPTMR_CH_GCR_SWSYNCT_MASK(0) | GPTMR_CH_GCR_SWSYNCT_MASK(1) | GPTMR_CH_GCR_SWSYNCT_MASK(2) | GPTMR_CH_GCR_SWSYNCT_MASK(3));
    if (cur_count >= count)
        return;
#endif
}

int monitor_sample_dma_config(void)
{
    monitor_timer_t *ctx = MONITOR_TIMER_CTX;
    int type_byte;
    uint16_t i;
    montiro_report_data_t *data = ctx->monitor_data;
    dma_channel_config_t dma_ch_config;
    if (data == NULL || data->sample.pkt_count == 0)
    {
        return -1;
    }

#ifndef MONITOR_SAMPLE_DMA_1_BASE
    return -2;
#else
    for (i = 0; i < data->sample.pkt_count; i++)
    {
        if (i < DMA_SOC_CHANNEL_NUM)
        {
            dma_default_channel_config(MONITOR_SAMPLE_DMA_1, &dma_ch_config);
        }
        #ifdef MONITOR_SAMPLE_DMA_2_BASE
        else
        {
            dma_default_channel_config(MONITOR_SAMPLE_DMA_2, &dma_ch_config);
        }
        #endif
        type_byte = monitor_type_convert_byte(data->data[data->channel.pkt_count + i].type);
        if (type_byte <= 0)
        {
            MONITOR_LOG_ERR("BAD, dma type no support!\r\n");
            return -2;
        }
        dma_ch_config.size_in_byte = data->sample.data_count * type_byte;
        dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)data->data[data->channel.pkt_count + i].addr_or_ch);
        dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0,
                                 (uint32_t)(data->mem_addr[data->channel.pkt_count + data->sample.mem_cur_index] + data->sample.mem_payload_offset[i] + MONITOR_PAYLOAD_DATA_OFFSET));
        if (type_byte == 1)
        {
            dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
            dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
        }
        else if (type_byte == 2)
        {
            dma_ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
            dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
        }
        else if (type_byte == 4)
        {
            dma_ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
            dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
        }
        else if (type_byte == 8)
        {
            dma_ch_config.src_width = DMA_TRANSFER_WIDTH_DOUBLE_WORD;
            dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_DOUBLE_WORD;
        }
        else
        {
            MONITOR_LOG_ERR("BAD, dma type error!\r\n");
            return -3;
        }
        dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
        dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
        dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
        dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
        dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
        dma_ch_config.handshake_opt = DMA_HANDSHAKE_OPT_ONE_BURST;
#endif
        dma_ch_config.priority = DMA_CHANNEL_PRIORITY_HIGH;
        dma_ch_config.interrupt_mask = DMA_INTERRUPT_MASK_ALL;
        dma_ch_config.linked_ptr = 0;
        if (i == data->sample.pkt_count - 1)
        {
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
            dma_ch_config.interrupt_mask = DMA_INTERRUPT_MASK_HALF_TC;
#else
            dma_ch_config.interrupt_mask = DMA_INTERRUPT_MASK_NONE;
#endif
        }

        if (i < DMA_SOC_CHANNEL_NUM)
        {
            dmamux_config(MONITOR_SAMPLE_DMAMUX_BASE,
                          DMA_SOC_CHN_TO_DMAMUX_CHN(MONITOR_SAMPLE_DMA_1, i),
                          monitor_get_dmamux_trgm_src(i), true);
            if (dma_ch_config.interrupt_mask != DMA_INTERRUPT_MASK_ALL)
            {
                ctx->dma_ch = i;
#if defined(MONITOR_SAMPLE_DMA_1_BASE) && (MONITOR_SAMPLE_DMA_1_BASE == HPM_XDMA_BASE)
                intc_m_enable_irq_with_priority(IRQn_XDMA, 1);
#elif defined(MONITOR_SAMPLE_DMA_1_BASE) && (MONITOR_SAMPLE_DMA_1_BASE == HPM_HDMA_BASE)
                intc_m_enable_irq_with_priority(IRQn_HDMA, 1);
#endif
            }
            if (status_success != dma_setup_channel(MONITOR_SAMPLE_DMA_1, i, &dma_ch_config, true))
            {
                MONITOR_LOG_ERR(" dma setup channel failed 0\n");
                return -4;
            }
        }
#ifdef MONITOR_SAMPLE_DMA_2_BASE
        else
        {
            dmamux_config(MONITOR_SAMPLE_DMAMUX_BASE,
                          DMA_SOC_CHN_TO_DMAMUX_CHN(MONITOR_SAMPLE_DMA_2, i),
                          monitor_get_dmamux_trgm_src(i), true);
            if (dma_ch_config.interrupt_mask != DMA_INTERRUPT_MASK_ALL)
            {
                ctx->dma_ch = i - DMA_SOC_CHANNEL_NUM;
#if defined(MONITOR_SAMPLE_DMA_2_BASE) && (MONITOR_SAMPLE_DMA_2_BASE == HPM_XDMA_BASE)
                intc_m_enable_irq_with_priority(IRQn_XDMA, 1);
#elif defined(MONITOR_SAMPLE_DMA_2_BASE) && (MONITOR_SAMPLE_DMA_2_BASE == HPM_HDMA_BASE)
                intc_m_enable_irq_with_priority(IRQn_HDMA, 1);
#endif
            }
            if (status_success != dma_setup_channel(MONITOR_SAMPLE_DMA_2, i - DMA_SOC_CHANNEL_NUM, &dma_ch_config, false))
            {
                MONITOR_LOG_ERR(" dma setup channel failed 0\n");
                return -4;
            }
        }
#else
        else
        {
            MONITOR_LOG_ERR("BAD, no support more dma channel!\r\n");
            return -5;
        }
#endif
    }
    return 0;
#endif
}

void monitor_sample_dma_start(void)
{
    uint32_t i;
    monitor_timer_t *ctx = MONITOR_TIMER_CTX;
    if(ctx->monitor_data == NULL)
        return;

#ifndef MONITOR_SAMPLE_DMA_1_BASE
    return;
#else
    for (i = 0; i < ctx->monitor_data->sample.pkt_count; i++)
    {
        if (i < DMA_SOC_CHANNEL_NUM)
        {
            dma_enable_channel(MONITOR_SAMPLE_DMA_1, i);
        }
#ifdef MONITOR_SAMPLE_DMA_2_BASE
        else
        {
            dma_enable_channel(MONITOR_SAMPLE_DMA_2, i - DMA_SOC_CHANNEL_NUM);
        }
#else
        else
        {
            MONITOR_LOG_ERR("BAD, no support more dma channel!\r\n");
            return;
        }
#endif
    }
#endif
}

bool monitor_timer_sample_is_running(void)
{
    monitor_timer_t *ctx = MONITOR_TIMER_CTX;
    return ctx->is_running;
}

int monitor_timer_sample_data_config(montiro_report_data_t *data, sample_result_cb cb, uint8_t is_loop, bool enable)
{
    monitor_timer_t *ctx = MONITOR_TIMER_CTX;
    if (data == NULL || cb == NULL || data->sample.pkt_count<= 0)
    {
        return -1;
    }
    monitor_timer_sample_destroy();
    ctx->monitor_sample_cb = cb;
    ctx->monitor_data = data;
    ctx->is_loop = is_loop;

    monitor_timer_config(data->sample.data_freq, data->sample.pkt_count);

    if (monitor_sample_dma_config() != 0)
    {
        return -3;
    }
    if (enable)
    {
        monitor_sample_dma_start();
        monitor_timer_start();
    }
    return 0;
}

void monitor_timer_sample_restart(void)
{
    monitor_sample_dma_config();
    monitor_sample_dma_start();
    monitor_timer_start();
}

void monitor_timer_sample_stop(void)
{
    monitor_timer_stop();
}

void monitor_timer_sample_destroy(void)
{
    monitor_timer_t *ctx = MONITOR_TIMER_CTX;
    monitor_timer_stop();
    ctx->is_loop = 0;
    ctx->is_running = 0;
    ctx->dma_ch = 0;
    ctx->monitor_sample_cb = NULL;
    ctx->monitor_data = NULL;
}

void monitor_timer_init(void)
{
    monitor_timer_t *ctx = MONITOR_TIMER_CTX;
    ctx->is_loop = 0;
    ctx->is_running = 0;
    ctx->dma_ch = 0;
    ctx->monitor_sample_cb = NULL;
    ctx->monitor_data = NULL;
#ifdef MONITOR_SAMPLE_GPTMR_1_BASE
    clock_add_to_group(MONITOR_SAMPLE_GPTMR_1_CLOCK, 0);
#endif
#ifdef MONITOR_SAMPLE_GPTMR_2_BASE
    clock_add_to_group(MONITOR_SAMPLE_GPTMR_2_CLOCK, 0);
#endif
#ifdef MONITOR_SAMPLE_GPTMR_3_BASE
    clock_add_to_group(MONITOR_SAMPLE_GPTMR_3_CLOCK, 0);
#endif
#ifdef MONITOR_SAMPLE_GPTMR_4_BASE
    clock_add_to_group(MONITOR_SAMPLE_GPTMR_4_CLOCK, 0);
#endif
}