/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_soc.h"
#include "hpm_rtt_interrupt_util.h"


void trap_entry(void);

typedef void (*isr_func_t)(void);


__attribute((weak)) void mchtmr_isr(void)
{
}

__attribute__((weak)) void mswi_isr(void)
{
}

__attribute__((weak)) void syscall_handler(uint32_t n, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
}

void trap_entry(void)
{
    uint32_t mcause = read_csr(CSR_MCAUSE);
    uint32_t mepc = read_csr(CSR_MEPC);
    uint32_t mstatus = read_csr(CSR_MSTATUS);

#if SUPPORT_PFT_ARCH
    uint32_t mxstatus = read_csr(CSR_MXSTATUS);
#endif
#ifdef __riscv_dsp
    int ucode = read_csr(CSR_UCODE);
#endif
#ifdef __riscv_flen
    int fcsr = read_fcsr();
#endif

    /* clobbers list for ecall */
#ifdef __riscv_32e
    __asm volatile("" : : :"t0", "a0", "a1", "a2", "a3");
#else
    __asm volatile("" : : :"a7", "a0", "a1", "a2", "a3");
#endif

    /* Do your trap handling */
    uint32_t cause_type = mcause & CSR_MCAUSE_EXCEPTION_CODE_MASK;
    uint32_t irq_index;
    if (mcause & CSR_MCAUSE_INTERRUPT_MASK)
    {
        switch (cause_type)
        {
        /* Machine timer interrupt */
        case IRQ_M_TIMER:
            mchtmr_isr();
            break;
            /* Machine EXT interrupt */
        case IRQ_M_EXT:
            /* Claim interrupt */
            irq_index = HPM_GET_INT_ID_IN_TRAP();
            /* Execute EXT interrupt handler */
            if (irq_index != HPM_INT_ID_NO_VALID)
            {
                ((isr_func_t) __vector_table[irq_index])();
                /* Complete interrupt */
                __plic_complete_irq(HPM_PLIC_BASE, HPM_PLIC_TARGET_M_MODE, irq_index);
            }
            break;
            /* Machine SWI interrupt */
        case IRQ_M_SOFT:
            mswi_isr();
            intc_m_complete_swi();
            break;
        }
    }
    else if (cause_type == MCAUSE_ECALL_FROM_MACHINE_MODE)
    {
        /* Machine Syscal call */
        __asm volatile(
                "mv a4, a3\n"
                "mv a3, a2\n"
                "mv a2, a1\n"
                "mv a1, a0\n"
#ifdef __riscv_32e
                "mv a0, t0\n"
#else
                "mv a0, a7\n"
#endif
                "call syscall_handler\n"
                : : : "a4"
        );
        mepc += 4;
    }
    else
    {
        mepc = hpm_rtt_exception_handler(mcause, mepc);
    }

    /* Restore CSR */
    write_csr(CSR_MSTATUS, mstatus);
    write_csr(CSR_MEPC, mepc);
#if SUPPORT_PFT_ARCH
    write_csr(CSR_MXSTATUS, mxstatus);
#endif
#ifdef __riscv_dsp
    write_csr(CSR_UCODE, ucode);
#endif
#ifdef __riscv_flen
    write_fcsr(fcsr);
#endif
}
