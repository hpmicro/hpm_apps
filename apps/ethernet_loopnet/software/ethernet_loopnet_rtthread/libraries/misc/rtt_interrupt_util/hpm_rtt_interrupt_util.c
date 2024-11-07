/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-24     HPMicro      the first version
 */
#include "hpm_soc.h"
#include "hpm_rtt_interrupt_util.h"
#include <rtthread.h>
#include "rt_hw_stack_frame.h"
#include "rtt_board.h"

#ifdef DEBUG
#define RT_EXCEPTION_TRACE rt_kprintf
#else
#define RT_EXCEPTION_TRACE(...)
#endif

void rt_show_stack_frame(rt_hw_stack_frame_t *stack_frame);


uint32_t hpm_claim_ext_interrupt(void);

volatile uint32_t g_ext_irq_id = HPM_INT_ID_NO_VALID;

hpm_rtt_isr_func_t hpm_rtt_get_int_in_trap = hpm_claim_ext_interrupt;

#ifdef HPM_USING_VECTOR_PREEMPTED_MODE
void enable_rtt_plic_feature(void)
{
    uint32_t plic_feature = 0;
#ifdef HPM_USING_VECTOR_PREEMPTED_MODE
    /* enabled vector mode and preemptive priority interrupt */
    plic_feature |= HPM_PLIC_FEATURE_VECTORED_MODE;
#endif
#if !defined(DISABLE_IRQ_PREEMPTIVE) || (DISABLE_IRQ_PREEMPTIVE == 0)
    /* enabled preemptive priority interrupt */
    plic_feature |= HPM_PLIC_FEATURE_PREEMPTIVE_PRIORITY_IRQ;
#endif
    __plic_set_feature(HPM_PLIC_BASE, plic_feature);
}
#endif

uint32_t hpm_claim_ext_interrupt(void)
{
    return __plic_claim_irq(HPM_PLIC_BASE, HPM_PLIC_TARGET_M_MODE);
}

/*  Get the encoded interrupt identifier */
uint32_t hpm_rtt_interrupt_get(void)
{
    uint32_t interrupt_id = HPM_INT_ID_NO_VALID;
    uint32_t mcause = read_csr(CSR_MCAUSE);
    uint32_t exception_code = mcause & CSR_MCAUSE_EXCEPTION_CODE_MASK;
    if (mcause & CSR_MCAUSE_INTERRUPT_MASK) {
        if (exception_code == IRQ_M_EXT) {
            interrupt_id = __plic_claim_irq(HPM_PLIC_BASE, HPM_PLIC_TARGET_M_MODE);
            if (interrupt_id == 0) {
                interrupt_id = HPM_INT_ID_NO_VALID;
            }
        } else {
            interrupt_id = 0x800 | exception_code;
        }
    } else {
        interrupt_id = 0x1000 | exception_code;
    }
    g_ext_irq_id = interrupt_id;

    /* Workaround for the ISR is triggerred before registering the interrupt hook */
    hpm_rtt_get_int_in_trap = hpm_rtt_get_active_interrupt;

    return interrupt_id;
}

uint32_t hpm_rtt_get_active_interrupt(void)
{
    return g_ext_irq_id;
}

ATTR_WEAK uint32_t hpm_rtt_exception_handler(uint32_t mcause, uint32_t epc)
{
    /* Unhandled Trap */
    uint32_t mdcause = read_csr(CSR_MDCAUSE);
    uint32_t mtval = read_csr(CSR_MTVAL);
    uint32_t cause = mcause & CSR_MCAUSE_EXCEPTION_CODE_MASK;

    rt_uint32_t mscratch = read_csr(0x340);
    rt_hw_stack_frame_t *stack_frame = (rt_hw_stack_frame_t *)mscratch;

    rt_show_stack_frame(stack_frame);

    switch (cause)
    {
    case MCAUSE_INSTR_ADDR_MISALIGNED:
        RT_EXCEPTION_TRACE("exception: instruction address was mis-aligned, mtval=0x%08x\n", mtval);
        break;
    case MCAUSE_INSTR_ACCESS_FAULT:
        RT_EXCEPTION_TRACE("exception: instruction access fault happened, mtval=0x%08x, epc=0x%08x\n", mtval, epc);
        switch (mdcause & 0x07)
        {
        case 1:
            RT_EXCEPTION_TRACE("mdcause: ECC/Parity error\r\n");
            break;
        case 2:
            RT_EXCEPTION_TRACE("mdcause: PMP instruction access violation \r\n");
            break;
        case 3:
            RT_EXCEPTION_TRACE("mdcause: BUS error\r\n");
            break;
        case 4:
            RT_EXCEPTION_TRACE("mdcause: PMP empty hole access \r\n");
            break;
        default:
            RT_EXCEPTION_TRACE("mdcause: reserved \r\n");
            break;
        }
        break;
    case MCAUSE_ILLEGAL_INSTR:
        RT_EXCEPTION_TRACE("exception: illegal instruction was met, mtval=0x%08x\n", mtval);
        switch (mdcause & 0x07)
        {
        case 0:
            RT_EXCEPTION_TRACE("mdcause: the actual faulting instruction is stored in the mtval CSR\r\n");
            break;
        case 1:
            RT_EXCEPTION_TRACE("mdcause: FP disabled exception \r\n");
            break;
        case 2:
            RT_EXCEPTION_TRACE("mdcause: ACE disabled exception \r\n");
            break;
        default:
            RT_EXCEPTION_TRACE("mdcause: reserved \r\n");
            break;
        }
        break;
    case MCAUSE_BREAKPOINT:
        RT_EXCEPTION_TRACE("exception: breakpoint was hit, mtval=0x%08x\n", mtval);
        break;
    case MCAUSE_LOAD_ADDR_MISALIGNED:
        RT_EXCEPTION_TRACE("exception: load address was mis-aligned, mtval=0x%08x\n", mtval);
        break;
    case MCAUSE_LOAD_ACCESS_FAULT:
        RT_EXCEPTION_TRACE("exception: load access fault happened, epc=%08x, mdcause=0x%x\n", epc, mdcause);
        switch (mdcause & 0x07)
        {
        case 1:
            RT_EXCEPTION_TRACE("mdcause: ECC/Parity error\r\n");
            break;
        case 2:
            RT_EXCEPTION_TRACE("mdcause: PMP instruction access violation \r\n");
            break;
        case 3:
            RT_EXCEPTION_TRACE("mdcause: BUS error\r\n");
            break;
        case 4:
            RT_EXCEPTION_TRACE("mdcause: Misaligned access \r\n");
            break;
        case 5:
            RT_EXCEPTION_TRACE("mdcause: PMP empty hole access \r\n");
            break;
        case 6:
            RT_EXCEPTION_TRACE("mdcause: PMA attribute inconsistency\r\n");
            break;
        default:
            RT_EXCEPTION_TRACE("mdcause: reserved \r\n");
            break;
        }
        break;
    case MCAUSE_STORE_AMO_ADDR_MISALIGNED:
        RT_EXCEPTION_TRACE("exception: store amo address was misaligned, epc=%08x\n", epc);
        break;
    case MCAUSE_STORE_AMO_ACCESS_FAULT:
        RT_EXCEPTION_TRACE("exception: store amo access fault happened, epc=%08x\n", epc);
        switch (mdcause & 0x07)
        {
        case 1:
            RT_EXCEPTION_TRACE("mdcause: ECC/Parity error\r\n");
            break;
        case 2:
            RT_EXCEPTION_TRACE("mdcause: PMP instruction access violation \r\n");
            break;
        case 3:
            RT_EXCEPTION_TRACE("mdcause: BUS error\r\n");
            break;
        case 4:
            RT_EXCEPTION_TRACE("mdcause: Misaligned access \r\n");
            break;
        case 5:
            RT_EXCEPTION_TRACE("mdcause: PMP empty hole access \r\n");
            break;
        case 6:
            RT_EXCEPTION_TRACE("mdcause: PMA attribute inconsistency\r\n");
            break;
        case 7:
            RT_EXCEPTION_TRACE("mdcause: PMA NAMO exception \r\n");
        default:
            RT_EXCEPTION_TRACE("mdcause: reserved \r\n");
            break;
        }
        break;
    default:
        RT_EXCEPTION_TRACE("Unknown exception happened, cause=%d\n", cause);
        break;
    }

    rt_kprintf("cause=0x%08x, epc=0x%08x, ra=0x%08x\n", cause, epc, stack_frame->ra);
    while(1) {
    }
}

void rt_show_stack_frame(rt_hw_stack_frame_t *stack_frame)
{
    RT_EXCEPTION_TRACE("Stack frame:\r\n----------------------------------------\r\n");
    RT_EXCEPTION_TRACE("ra      : 0x%08x\r\n", stack_frame->ra);
    RT_EXCEPTION_TRACE("mstatus : 0x%08x\r\n", read_csr(0x300));//mstatus
    RT_EXCEPTION_TRACE("t0      : 0x%08x\r\n", stack_frame->t0);
    RT_EXCEPTION_TRACE("t1      : 0x%08x\r\n", stack_frame->t1);
    RT_EXCEPTION_TRACE("t2      : 0x%08x\r\n", stack_frame->t2);
    RT_EXCEPTION_TRACE("a0      : 0x%08x\r\n", stack_frame->a0);
    RT_EXCEPTION_TRACE("a1      : 0x%08x\r\n", stack_frame->a1);
    RT_EXCEPTION_TRACE("a2      : 0x%08x\r\n", stack_frame->a2);
    RT_EXCEPTION_TRACE("a3      : 0x%08x\r\n", stack_frame->a3);
    RT_EXCEPTION_TRACE("a4      : 0x%08x\r\n", stack_frame->a4);
    RT_EXCEPTION_TRACE("a5      : 0x%08x\r\n", stack_frame->a5);
#ifndef __riscv_32e
    RT_EXCEPTION_TRACE("a6      : 0x%08x\r\n", stack_frame->a6);
    RT_EXCEPTION_TRACE("a7      : 0x%08x\r\n", stack_frame->a7);
    RT_EXCEPTION_TRACE("t3      : 0x%08x\r\n", stack_frame->t3);
    RT_EXCEPTION_TRACE("t4      : 0x%08x\r\n", stack_frame->t4);
    RT_EXCEPTION_TRACE("t5      : 0x%08x\r\n", stack_frame->t5);
    RT_EXCEPTION_TRACE("t6      : 0x%08x\r\n", stack_frame->t6);
#endif
}
