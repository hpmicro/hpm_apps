#include "hpm_csr_drv.h"
#include "hpm_ticktime.h"

#define FREQ_1MHz (1000000UL)
#define FREQ_1KHz (1000UL)

extern uint32_t hpm_core_clock;

uint64_t clock_get_now_tick_us(void)
{
    return (uint64_t)(FREQ_1MHz * hpm_csr_get_core_cycle() / hpm_core_clock);
}

uint64_t clock_get_now_tick_ms(void)
{
    return (uint64_t)(FREQ_1KHz * hpm_csr_get_core_cycle() / hpm_core_clock);
}

uint64_t clock_get_now_tick_s(void)
{
    return (uint64_t)(hpm_csr_get_core_cycle() / hpm_core_clock);
}