/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-24     HPMicro      the first version
 */

#ifndef HPM_RTT_INTERRUPT_UTIL_H
#define HPM_RTT_INTERRUPT_UTIL_H

#include <stdint.h>
#ifdef HPM_USING_VECTOR_PREEMPTED_MODE
#include "cpuport.h"
#include "rtdef.h"
#include "rtthread.h"
#endif

#define MCAUSE_INSTR_ADDR_MISALIGNED (0U)       //!< Instruction Address misaligned
#define MCAUSE_INSTR_ACCESS_FAULT (1U)          //!< Instruction access fault
#define MCAUSE_ILLEGAL_INSTR (2U)               //!< Illegal instruction
#define MCAUSE_BREAKPOINT (3U)                  //!< Breakpoint
#define MCAUSE_LOAD_ADDR_MISALIGNED (4U)        //!< Load address misaligned
#define MCAUSE_LOAD_ACCESS_FAULT (5U)           //!< Load access fault
#define MCAUSE_STORE_AMO_ADDR_MISALIGNED (6U)   //!< Store/AMO address misaligned
#define MCAUSE_STORE_AMO_ACCESS_FAULT (7U)      //!< Store/AMO access fault
#define MCAUSE_ECALL_FROM_USER_MODE (8U)        //!< Environment call from User mode
#define MCAUSE_ECALL_FROM_SUPERVISOR_MODE (9U)  //!< Environment call from Supervisor mode
#define MCAUSE_ECALL_FROM_MACHINE_MODE (11U)    //!< Environment call from machine mode
#define MCAUSE_INSTR_PAGE_FAULT (12U)           //!< Instruction page fault
#define MCAUSE_LOAD_PAGE_FAULT (13)             //!< Load page fault
#define MCAUSE_STORE_AMO_PAGE_FAULT (15U)       //!< Store/AMO page fault

#define IRQ_S_SOFT              1
#define IRQ_H_SOFT              2
#define IRQ_M_SOFT              3
#define IRQ_S_TIMER             5
#define IRQ_H_TIMER             6
#define IRQ_M_TIMER             7
#define IRQ_S_EXT               9
#define IRQ_H_EXT               10
#define IRQ_M_EXT               11


#define HPM_INT_ID_EXT(x) (uint32_t)(x)         /* External interrupt ID */

/* Core Interrupt definitions */
#define HPM_INT_ID_M_SW                         (0x803)
#define HPM_INT_ID_M_TMR                        (0x807)

#define HPM_INT_ID_EXT_INVAL                    (0x3FF)

/* Core Exception definitions */
#define HPM_INT_ID_EXCEPT_INSTR_UNALIGN         (0x1000)
#define HPM_INT_ID_EXCEPT_INSTR_ACC_FAULT       (0x1001)
#define HPM_INT_ID_EXCEPT_ILLEGAL_INSTR         (0x1002)
#define HPM_INT_ID_EXCEPT_BKPT                  (0x1003)
#define HPM_INT_ID_EXCEPT_LOAD_ADDR_UNALIGN     (0x1004)
#define HPM_INT_ID_EXCEPT_LOAD_ACC_FAULT        (0x1005)
#define HPM_INT_ID_EXCEPT_STORE_ADDR_UNALGIN    (0x1006)
#define HPM_INT_ID_EXCEPT_STORE_ACC_FAULT       (0x1007)
#define HPM_INT_ID_EXCEPT_M_ECALL               (0x100B)


typedef uint32_t (*hpm_rtt_isr_func_t)(void);



extern hpm_rtt_isr_func_t hpm_rtt_get_int_in_trap;


#ifdef __cplusplus
extern "C" {
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

/**
 * @note The HPM_GET_INT_ID_IN_HOOK should be called in the rt_interrupt_enter() hook if
 *       the MACRO 'HPM_RTT_CLAIM_INTERRUPT_IN_HOOK' is defined
 * 
 */

#ifdef HPM_RTT_CLAIM_INTERRUPT_IN_HOOK
    #define HPM_GET_INT_ID_IN_HOOK hpm_rtt_interrupt_get
    #define HPM_GET_INT_ID_IN_TRAP hpm_rtt_get_int_in_trap
    #define HPM_INT_ID_NO_VALID (0x1FFF)            /* No active interrupts */
#else
    #define HPM_GET_INT_ID_IN_HOOK() (0)
    #define HPM_GET_INT_ID_IN_TRAP() __plic_claim_irq(HPM_PLIC_BASE, HPM_PLIC_TARGET_M_MODE)
    #define HPM_INT_ID_NO_VALID      (0)            /* No active interrupts */
#endif


/** 
 * @brief Get the encoded interrupt identifier
 *        Claim the PLIC interrupt on demand
 * 
 * @return interrupt id
 */
uint32_t hpm_rtt_interrupt_get(void);

/**
 * @brief Get the active interrupt identifier directly
 * 
 * @return interrupt id
 */
uint32_t hpm_rtt_get_active_interrupt(void);

/**
 * @brief Default exeception handler
 * 
 * @param [in] mcause MCAUSE register value 
 * @param [in] epc EPC register value 
 */
uint32_t hpm_rtt_exception_handler(uint32_t mcause, uint32_t epc);

#ifdef HPM_USING_VECTOR_PREEMPTED_MODE
void enable_rtt_plic_feature(void);
#endif

/* used for vector mode gptmr context switch */
#ifdef HPM_USING_VECTOR_PREEMPTED_MODE
extern rt_uint32_t rt_context_switch_flag;
extern rt_uint32_t sp_before_addr;
#ifdef ARCH_RISCV_FPU
#define RTT_SAVE_FPU_CONTEXT()  { \
    __asm volatile("addi sp, sp, %0" : : "i"(-256) :);\
    __asm volatile("\n\
             c.fswsp ft0,  0*4(sp)\n\
             c.fswsp ft1,  1*4(sp) \n\
             c.fswsp ft2,  2*4(sp) \n\
             c.fswsp ft3,  3*4(sp) \n\
             c.fswsp ft4,  4*4(sp) \n\
             c.fswsp ft5,  5*4(sp) \n\
             c.fswsp ft6,  6*4(sp) \n\
             c.fswsp ft7,  7*4(sp) \n\
             c.fswsp fs0,  8*4(sp) \n\
             c.fswsp fs1,  9*4(sp) \n\
             c.fswsp fa0,  10*4(sp) \n\
             c.fswsp fa1,  11*4(sp) \n\
             c.fswsp fa2,  12*4(sp) \n\
             c.fswsp fa3,  13*4(sp) \n\
             c.fswsp fa4,  14*4(sp) \n\
             c.fswsp fa5,  15*4(sp) \n\
             c.fswsp fa6,  16*4(sp) \n\
             c.fswsp fa7,  17*4(sp) \n\
             c.fswsp fs2,  18*4(sp) \n\
             c.fswsp fs3,  19*4(sp) \n\
             c.fswsp fs4,  20*4(sp)\n\
             c.fswsp fs5,  21*4(sp) \n\
             c.fswsp fs6,  22*4(sp) \n\
             c.fswsp fs7,  23*4(sp) \n\
             c.fswsp fs8,  24*4(sp) \n\
             c.fswsp fs9,  25*4(sp) \n\
             c.fswsp fs10, 26*4(sp) \n\
             c.fswsp fs11, 27*4(sp) \n\
             c.fswsp ft8,  28*4(sp) \n\
             c.fswsp ft9,  29*4(sp) \n\
             c.fswsp ft10, 30*4(sp) \n\
             c.fswsp ft11, 31*4(sp) \n"); \
}

#define RTT_RESTORE_FPU_CONTEXT() { \
    __asm volatile("\n\
             c.flwsp ft0,  0*4(sp)\n\
             c.flwsp ft1,  1*4(sp) \n\
             c.flwsp ft2,  2*4(sp) \n\
             c.flwsp ft3,  3*4(sp) \n\
             c.flwsp ft4,  4*4(sp) \n\
             c.flwsp ft5,  5*4(sp) \n\
             c.flwsp ft6,  6*4(sp) \n\
             c.flwsp ft7,  7*4(sp) \n\
             c.flwsp fs0,  8*4(sp) \n\
             c.flwsp fs1,  9*4(sp) \n\
             c.flwsp fa0,  10*4(sp) \n\
             c.flwsp fa1,  11*4(sp) \n\
             c.flwsp fa2,  12*4(sp) \n\
             c.flwsp fa3,  13*4(sp) \n\
             c.flwsp fa4,  14*4(sp) \n\
             c.flwsp fa5,  15*4(sp) \n\
             c.flwsp fa6,  16*4(sp) \n\
             c.flwsp fa7,  17*4(sp) \n\
             c.flwsp fs2,  18*4(sp) \n\
             c.flwsp fs3,  19*4(sp) \n\
             c.flwsp fs4,  20*4(sp)\n\
             c.flwsp fs5,  21*4(sp) \n\
             c.flwsp fs6,  22*4(sp) \n\
             c.flwsp fs7,  23*4(sp) \n\
             c.flwsp fs8,  24*4(sp) \n\
             c.flwsp fs9,  25*4(sp) \n\
             c.flwsp fs10, 26*4(sp) \n\
             c.flwsp fs11, 27*4(sp) \n\
             c.flwsp ft8,  28*4(sp) \n\
             c.flwsp ft9,  29*4(sp) \n\
             c.flwsp ft10, 30*4(sp) \n\
             c.flwsp ft11, 31*4(sp) \n"); \
    __asm volatile("addi sp, sp, %0" : : "i"(256) :);\
}
#else
#define RTT_SAVE_FPU_CONTEXT()
#define RTT_RESTORE_FPU_CONTEXT()
#endif
#define RTT_SAVE_CALLER_CONTEXT(){\
    __asm volatile("csrci mstatus, 0x8\n"); \
    RTT_SAVE_FPU_CONTEXT(); \
    __asm volatile("addi sp, sp, %0" : : "i"(-128) :);\
    __asm volatile("\n\
                    c.swsp x5,  5*4(sp) \n\
                    c.swsp x1,  1*4(sp) \n\
                    li t0, 0x80 \n\
                    c.swsp t0,  2*4(sp) \n\
                    c.swsp x4,  4*4(sp) \n\
                    c.swsp x6,  6*4(sp) \n\
                    c.swsp x7,  7*4(sp) \n\
                    c.swsp x8,  8*4(sp) \n\
                    c.swsp x9,  9*4(sp) \n\
                    c.swsp x10, 10*4(sp) \n\
                    c.swsp x11, 11*4(sp) \n\
                    c.swsp x12, 12*4(sp) \n\
                    c.swsp x13, 13*4(sp) \n\
                    c.swsp x14, 14*4(sp) \n\
                    c.swsp x15, 15*4(sp) \n\
                    c.swsp x16, 16*4(sp) \n\
                    c.swsp x17, 17*4(sp) \n\
                    c.swsp x18, 18*4(sp) \n\
                    c.swsp x19, 19*4(sp) \n\
                    c.swsp x20, 20*4(sp) \n\
                    c.swsp x21, 21*4(sp) \n\
                    c.swsp x22, 22*4(sp) \n\
                    c.swsp x23, 23*4(sp) \n\
                    c.swsp x24, 24*4(sp) \n\
                    c.swsp x25, 25*4(sp) \n\
                    c.swsp x26, 26*4(sp) \n\
                    c.swsp x27, 27*4(sp) \n\
                    c.swsp x28, 28*4(sp) \n\
                    c.swsp x29, 29*4(sp) \n\
                    c.swsp x30, 30*4(sp) \n\
                    c.swsp x31, 31*4(sp)"); \
    __asm volatile("la t0, rt_interrupt_nest \n\
                    lw t2, 0(t0)\n\
                    bnez t2, 1f \n");\
    __asm volatile("la t0, sp_before_addr \n\
                    sw sp, 0(t0) \n");\
    __asm volatile("csrrw sp,mscratch,sp\n");\
    __asm volatile("1: \n");\
    __asm volatile("la t1, %0\n\t" : : "i" (rt_interrupt_enter) : );\
    __asm volatile("jalr t1\n");\
}

#define RTT_EXIT_CALLER_CONTEXT(){\
    __asm volatile("la t1, %0\n\t" : : "i" (rt_interrupt_leave) : );\
    __asm volatile("jalr t1\n");\
    __asm volatile("la t0, rt_interrupt_nest \n\
                    lw t2, 0(t0)\n\
                    bnez t2, 2f \n");\
    __asm volatile("csrrw sp,mscratch,sp\n");\
    __asm volatile("la t0, rt_context_switch_flag \n\
                    lw t2, 0(t0)\n\
                    beqz t2, 2f \n");\
    __asm volatile("sw zero, 0(t0)\n");\
    __asm volatile("csrr a0, mepc \n\
                    c.swsp a0, 0(sp) \n");\
    __asm volatile("la t0, rt_interrupt_from_thread \n\
                    lw t1, 0(t0) \n\
                    sw sp, 0(t1) \n");\
    __asm volatile("la t0, rt_interrupt_to_thread \n\
                    lw t1, 0(t0) \n\
                    lw sp, 0(t1) \n");\
    __asm volatile("c.lwsp a0, 0(sp) \n\
                    csrw mepc, a0 \n");\
    __asm volatile("2: \n\
                    c.lwsp x1,  1*4(sp) \n\
                    li t0, 0x1800 \n\
                    csrs mstatus, t0 \n\
                    c.lwsp t0, 8(sp) \n\
                    csrs mstatus, t0 \n\
                    c.lwsp x4,  4*4(sp) \n\
                    c.lwsp x5,  5*4(sp) \n\
                    c.lwsp x6,  6*4(sp) \n\
                    c.lwsp x7,  7*4(sp) \n\
                    c.lwsp x8,  8*4(sp) \n\
                    c.lwsp x9,  9*4(sp) \n\
                    c.lwsp x10, 10*4(sp) \n\
                    c.lwsp x11, 11*4(sp) \n\
                    c.lwsp x12, 12*4(sp) \n\
                    c.lwsp x13, 13*4(sp) \n\
                    c.lwsp x14, 14*4(sp) \n\
                    c.lwsp x15, 15*4(sp) \n\
                    c.lwsp x16, 16*4(sp) \n\
                    c.lwsp x17, 17*4(sp) \n\
                    c.lwsp x18, 18*4(sp) \n\
                    c.lwsp x19, 19*4(sp) \n\
                    c.lwsp x20, 20*4(sp) \n\
                    c.lwsp x21, 21*4(sp) \n\
                    c.lwsp x22, 22*4(sp) \n\
                    c.lwsp x23, 23*4(sp) \n\
                    c.lwsp x24, 24*4(sp) \n\
                    c.lwsp x25, 25*4(sp) \n\
                    c.lwsp x26, 26*4(sp) \n\
                    c.lwsp x27, 27*4(sp) \n\
                    c.lwsp x28, 28*4(sp) \n\
                    c.lwsp x29, 29*4(sp) \n\
                    c.lwsp x30, 30*4(sp) \n\
                    c.lwsp x31, 31*4(sp) \n");\
     __asm volatile("addi sp, sp, %0" : : "i"(128) :);\
     RTT_RESTORE_FPU_CONTEXT(); \
}

#define ISR_NAME_M(irq_num) default_isr_##irq_num
#define RTT_DECLARE_EXT_ISR_M(irq_num, isr) \
void isr(void) __attribute__((section(".isr_vector")));\
EXTERN_C void ISR_NAME_M(irq_num)(void) __attribute__((section(".isr_vector")));\
void ISR_NAME_M(irq_num)(void) \
{ \
    RTT_SAVE_CALLER_CONTEXT(); \
    ENTER_NESTED_IRQ_HANDLING_M();\
    __asm volatile("la t1, %0\n\t" : : "i" (isr) : );\
    __asm volatile("jalr t1\n");\
    COMPLETE_IRQ_HANDLING_M(irq_num);\
    EXIT_NESTED_IRQ_HANDLING_M();\
    RTT_EXIT_CALLER_CONTEXT(); \
    __asm volatile("fence io, io");\
    __asm volatile("mret\n");\
}
#else
#define RTT_DECLARE_EXT_ISR_M(irq_num, isr) \
void isr(void) __attribute__((section(".isr_vector")));\
EXTERN_C void ISR_NAME_M(irq_num)(void) __attribute__((section(".isr_vector")));\
void ISR_NAME_M(irq_num)(void) {           \
    isr();                                            \
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* HPM_RTT_INTERRUPT_UTIL_H */
